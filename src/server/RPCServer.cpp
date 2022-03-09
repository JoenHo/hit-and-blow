#include "RPCServer.h"

#define PORT 8080
#define DELIMITER ";"
// --- RPC list ---
#define RPC_CONNECT 1
#define RPC_DISCONNECT 2
#define RPC_SELECTMODE 3
#define RPC_GUESS 4
#define RPC_ISMYTURN 5
#define RPC_ENDGAME 6
// ----------------
#define SUCCESS "0"
#define FAIL "-1"

 /**
  * Continue communicating with client until client disconnects
  * @param cliNum No. of client
  * @throw error message if error occurs
  */
void RPCServer::talkToClient(int cliNum){

    bool continueLoop = true;
    while(continueLoop){
        int res;
        char buffer[1024] = {0};
        // receive the data on the socket and store it in a buffer
        if ((res = recv(clients[cliNum].sock, buffer, sizeof(buffer), 0)) <= 0) {
            if(res==0){
                // res = 0 if the connection is already closed
                cout << "... CLIENT " << to_string(cliNum) << " [Sock#" << to_string(clients[cliNum].sock) << "] DISCONNECTED ...\n" << endl;
                // delete the child socket
                close(clients[cliNum].sock);
                break;
            }else{
                // res = -1 if unsuccessful
                throw string("Failed to read from socket");
            }
        }else{
            // parse tokens
            vector<string> tokens;
            parseTokens(buffer, tokens);

            // first token is the RPC specifier
            string strRPC = tokens[0];
            int rpcNum = stoi(strRPC);

            // process RPC
            switch (rpcNum) {
                case RPC_CONNECT:
                    connectRPC(cliNum, tokens);
                    break;
                case RPC_DISCONNECT:
                    disconnectRPC(cliNum);
                    continueLoop = false; // exit while loop
                    break;
                case RPC_SELECTMODE:
                    selectModeRPC(cliNum, tokens);
                    break;
                case RPC_GUESS:
                    guessRPC(cliNum, tokens);
                    break;
                case RPC_ISMYTURN:
                    isMyTurnRPC(cliNum, tokens);
                    break;
                case RPC_ENDGAME:
                    endGameRPC(cliNum);
                    break;
                default:
                    break;
            }
        }
    }
}

/**
 * Send a message to the client
 * @param cliNum No. of client who requested this rpc
 * @param msg message to send
 * @throw error message if error occurs
 */
void RPCServer::sendToClient(int cliNum, string msg){

    if((send(clients[cliNum].sock , msg.c_str() , msg.length() , 0 ) < 0)){
        throw string("Failed to send the message to client");
    }
}

/**
 * [RPC] connectRPC: validate username and password for a client to login and send the response to client
 * @param cliNum No. of client who requested this rpc
 * @param tokens tokens received from the client
 * @throw error message if error occurs
 */
void RPCServer::connectRPC(int cliNum, vector<string> &tokens){

    cout << "-------------------------------------------------" << endl;
    cout << FOREGRN;
    cout << "[CLIENT #" << cliNum << "]" << " LOGIN\n\n";

    // regular expression for username (2 to 10 letters in "a-zA-Z0-9" )
    regex regUsername ("[a-zA-Z0-9]{2,10}");
    // regular expression for password (4-10 letters, contains at lease one letter, digit(0-9), special character (!@#$%^&))
    regex regPassword ("^(?=.*[0-9])(?=.*[a-zA-Z])(?=.*[!@#$%^&]).{4,10}$");

    if(tokens.size() < 3){
        throw string("Invalid size of tokens");
    }

    // print the username and password received from client
    cout << "USER NAME: " << tokens[1] << " PASSWORD: " << tokens[2] << endl;

    // validate username and password
    if(!regex_match(tokens[1], regUsername)){
        string msg = (string)FAIL + ";LOGIN DENIED: INVALID USERNAME";
        cout << "LOGIN DENIED: INVALID USERNAME" << endl;
        // Send a message to the client
        sendToClient(cliNum, msg);
    }else if(!regex_match(tokens[2], regPassword)){
        string msg = (string)FAIL + ";LOGIN DENIED: INVALID PASSWORD";
        cout << "LOGIN DENIED: INVALID PASSWORD" << endl;
        // Send a message to the client
        sendToClient(cliNum, msg);
    }else{
        // username and password are valid
        clients[cliNum].setUsername(tokens[1]);
        clients[cliNum].setPassword(tokens[2]);
        // Send a message to the client
        string msg =  (string)SUCCESS + ";[YOU ARE LOGGED IN] HELLO " + clients[cliNum].getUsername();
        cout << "LOGIN SUCCESSFUL" << endl;
        sendToClient(cliNum, msg);
    }
    cout << RESETTEXT;
    cout << "-------------------------------------------------" << endl << endl;
}

/**
 * [RPC] disconnectRPC: disconnect client from server and send the response to client
 * @param cliNum No. of client who requested this rpc
 */
void RPCServer::disconnectRPC(int cliNum){

    cout << "-------------------------------------------------" << endl;
    cout << FOREGRN;
    cout << "[CLIENT #" << cliNum << " : " << clients[cliNum].getUsername() << "]" << " DISCONNECT\n";

    // Send response to the client
    string msg =  (string)SUCCESS + ";[YOU ARE LOGGED OUT] BYE " + clients[cliNum].getUsername();
    sendToClient(cliNum, msg);

    // delete the child socket
    close(clients[cliNum].sock);

    // remove client from lists
    clients.erase(cliNum);

    cout << RESETTEXT;
    cout << "-------------------------------------------------" << endl << endl;
}

/**
 * [RPC] selectModeRPC: set the selected game mode and send the response to client
 * @param cliNum No. of client who requested this rpc
 * @param tokens tokens received from the client
 */
void RPCServer::selectModeRPC(int cliNum, vector<string> &tokens) {

    cout << "-------------------------------------------------" << endl;
    cout << FOREGRN;
    cout << "[CLIENT #" << cliNum << " : " << clients[cliNum].getUsername() << "]" << " SELECT GAME MODE\n\n";

    if(tokens.size() < 2){
        throw string("Invalid size of tokens");
    }
    cout << "GAME MODE: " << stoi(tokens[1]) << endl;

    string msg =  (string)SUCCESS + ";";
    if(stoi(tokens[1])==SINGLEPLAY){
        // assign a single-player game to client
        gameMgr.assignSinglePlayGame(cliNum, clients[cliNum].getUsername());
    }else if(stoi(tokens[1])==MULTIPLAY){
        // assign a two-player game to client
        gameMgr.assignMultiPlayGame(cliNum, clients[cliNum].getUsername());

        // wait until the second player to join
        while(gameMgr.activeGameList[cliNum]->getPlayers().size() != 2){}

        // set the response message
        for(auto player : gameMgr.activeGameList[cliNum]->getPlayers()){
            if(player.first != cliNum){
                msg += player.second;
                break;
            }
        }
    }
    // Send response to the client
    sendToClient(cliNum, msg);

    cout << RESETTEXT;
    cout << "-------------------------------------------------" << endl << endl;
}

/**
 * [RPC] guessRPC: set the guess for client and send the response to client
 * @param cliNum No. of client who requested this rpc
 * @param tokens tokens received from the client
 */
void RPCServer::guessRPC(int cliNum, vector<string> &tokens) {

    cout << "-------------------------------------------------" << endl;
    cout << FOREGRN;
    cout << "[CLIENT #" << cliNum << " : " << clients[cliNum].getUsername() << "]" << " GUESS PEGS\n\n";

    if(tokens.size() < 2){
        cout << "INVALID SIZE OF TOKENS";
        throw string("Invalid size of tokens");
    }
    cout << "GUESS: " + tokens[1] << endl;

    // submit client's guess
    vector<string> strs = splitStr(tokens[1], " ");
    vector<int> guess;
    for(string str : strs){
        guess.push_back(stoi(str));
    }
    gameMgr.activeGameList[cliNum]->submitGuess(guess);

    // count hits and blows
    int hits = 0;
    int blows = 0;
    gameMgr.activeGameList[cliNum]->countHitsBlows(guess, hits, blows);
    cout << "HITS: " << to_string(hits) << " BLOWS: " << to_string(blows) << endl;

    // if two-player mode, set the next player
    if(gameMgr.activeGameList[cliNum]->getGameMode() == MULTIPLAY){
        gameMgr.activeGameList[cliNum]->setNextPlayer(gameMgr.activeGameList[cliNum]->getAnotherPlayer(cliNum));
    }

    // Send response to client
    string msg =  (string)SUCCESS + ";" + to_string(hits) + " " + to_string(blows);
    sendToClient(cliNum, msg);

    cout << RESETTEXT;
    cout << "-------------------------------------------------" << endl << endl;
}

/**
 * [RPC] isMyTurnRPC: check whose turn is it next and send the response to client
 * @param cliNum No. of client who requested this rpc
 * @param tokens tokens received from the client
 */
void RPCServer::isMyTurnRPC(int cliNum, vector<string> &tokens) {

    cout << "-------------------------------------------------" << endl;
    cout << FOREGRN;
    cout << "[CLIENT #" << cliNum << " : " << clients[cliNum].getUsername() << "]" << " CHECKING WHOSE TURN\n\n";
    string msg;

    if(gameMgr.activeGameList[cliNum]->getNextPlayer() == cliNum){
        cout << clients[cliNum].getUsername() << "'S TURN\n";
        msg += (string)SUCCESS + ";";
        if(gameMgr.activeGameList[cliNum]->getAttempts().size() != 0){
            int hits = 0;
            int blows = 0;
            // add the result of previous attempt to msg
            vector<int> preAttempt = gameMgr.activeGameList[cliNum]->getAttempts().back();
            gameMgr.activeGameList[cliNum]->countHitsBlows(preAttempt, hits, blows);
            for(auto p : preAttempt){
                msg += to_string(p) + " ";
            }
            msg += ";" + to_string(hits) + ";" + to_string(blows);
        }
    }else{
        cout << clients[gameMgr.activeGameList[cliNum]->getAnotherPlayer(cliNum)].getUsername() << "'S TURN\n";
        msg += (string)FAIL;
    }

    // Send response to the client
    sendToClient(cliNum, msg);

    cout << RESETTEXT;
    cout << "-------------------------------------------------" << endl << endl;
}

/**
 * [RPC] isMyTurnRPC: end the game for specified client and send the response to client
 * @param cliNum No. of client who requested this rpc
 */
void RPCServer::endGameRPC(int cliNum) {

    cout << "-------------------------------------------------" << endl;
    cout << FOREGRN;
    cout << "[CLIENT #" << cliNum << " : " << clients[cliNum].getUsername() << "]" << " END GAME\n";

    string msg =  (string)SUCCESS + ";";
    for(int peg : gameMgr.activeGameList[cliNum]->getAnswer()){
        msg += to_string(peg) + " ";
    }

    string players = clients[cliNum].getUsername();
    if(gameMgr.activeGameList[cliNum]->getIsSolved()){
        msg += ";CONGRATULATIONS!;";
        msg += "        GOOD JOB " + players;
    }else{
        msg += ";   GAME OVER!;";
        msg += "BETTER LUCK NEXT TIME " + players;
    }
    // Send response to client
    sendToClient(cliNum, msg);
    // Remove client from list
    gameMgr.leaveGame(cliNum);

    cout << RESETTEXT;
    cout << "-------------------------------------------------" << endl << endl;
}

/**
 * add client to the client list
 * @param cliNum No. of client
 * @param cliObj Client object
 */
void RPCServer::addClients(int cliNum, Client cliObj) {

    mtx.lock(); // Lock mutex
    clients[cliNum] = cliObj;
    mtx.unlock(); // Unlock mutex
}

/**
 * Populates a string vector with tokens extracted from the message the client sent
 * @param buffer message the client sent (input)
 * @param tokens tokens extracted from the buffer (output)
 * Note: fist token is always a RPC specifier
 * Example buffer: "1;username;password;"
*/
void RPCServer::parseTokens(char *buffer, vector<string> &tokens){

    char* token;
    char* rest = (char *) buffer;

    while ((token = strtok_r(rest, DELIMITER, &rest))){
        tokens.push_back(token);
    }
}

/**
 * Split a string by specified delimiter
 * @param str
 * @param delim
 * @return a vector of strings
 */
vector<string> RPCServer::splitStr(string str, string delim){

    vector<string> list;
    size_t pos = 0;
    while((pos = str.find(delim)) != string::npos){
        list.push_back(str.substr(0, pos));
        str.erase(0, pos + delim.length());
    }
    if(str.length()!=0){
        list.push_back(str);
    }
    return list;
}
