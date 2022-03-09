#include "RPCClient.h"

#define DELIMITER ";"
#define SUCCESS "0"
#define FAIL "-1"
// --- RPC list ---
#define RPC_CONNECT 1
#define RPC_DISCONNECT 2
#define RPC_SELECTMODE 3
#define RPC_GUESS 4
#define RPC_ISMYTURN 5
#define RPC_ENDGAME 6
// ----------------

/**
 * Creates a socket and connects the socket to the server
 * @param ipAddress server ip address
 * @param port port
 * @throw error message if error occurs
 */
void RPCClient::connectToServer(string ipAddress, int port){

    int domain = AF_INET;
    int type = SOCK_STREAM;
    int protocol = 0;
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Create a socket
    if((this->sock = socket(domain, type, protocol)) < 0){
        throw string("Failed to create a socket");
    }

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, ipAddress.c_str(), &serv_addr.sin_addr) <= 0){
        throw string("Invalid address/ Address not supported");
    }

    // Connect the socket to the address of the server
    if(connect(this->sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        throw string("Failed to connect to the server");
    }
}

/**
 * Obtain username and password from user input, then call connectRPC for login to the server
 */
void RPCClient::loginToServer() {
    cout << "\n********************************************\n";
    cout << BOLDYELLOW << "              LOGIN TO SERVER" << RESETTEXT << endl;
    cout << "********************************************\n";
    cout << " * username: 2-10 letters, contains only letters and digits\n";
    cout << " * password: 4-10 letters, contains at least one letter, one digit,\n"
            "             and one special character(!@#$%^&)\n\n";
    while(true){
        // Ask user to enter username and password
        string userName, password;
        cout << FOREGRN << ">>>" << RESETTEXT << " USER NAME: ";
        getline(cin, userName);
        cout << FOREGRN << ">>>" << RESETTEXT << " PASSWORD: ";
        getline(cin, password);
        cout << endl;

        // Login to the server program
        if(true==connectRPC(userName, password)){
            // login successful
            break;
        }
        cout << "PLEASE RE-ENTER USERNAME AND PASSWORD\n" << endl;
    }
}

/**
 * Communicat with the server by sending and receiving a message via socket
 * @param msg message to send to the server
 * @param res response message from the server
 * @throw error message if error occurs
 */
void RPCClient::talkToServer(string msg, string &res){

    char buffer[1024] = {0};

    // Send a message to the server
    if((send(this->sock , msg.c_str(), sizeof (msg), 0) < 0)){
        throw string("Failed to send a request to socket");
    }

    // Receive response from server
    if((read(this->sock, buffer, sizeof(buffer)) < 0)){
        throw string("Failed to read from socket");
    }
    res = buffer;
}

/**
 * Connects to the server with username and password
 * @param userName username
 * @param password password
 * @return true if login successful; false if login unsuccessful;
 */
bool RPCClient::connectRPC(string userName, string password){

    string msg = to_string(RPC_CONNECT) + ";" + userName + ";" + password + ";";

    // talk to server
    string res;
    talkToServer(msg, res);

    // parse tokens
    vector<string> tokens;
    parseTokens(const_cast<char*>(res.c_str()), tokens);

    // Print the message from server
    cout << FOREGRN << tokens[1] << RESETTEXT << endl << endl;

    bool isSuccessful = false;
    if(tokens[0] == SUCCESS){
        isSuccessful = true;
    }else if(tokens[0] == FAIL){
        isSuccessful = false;
    }
    return isSuccessful;
}

/**
 * Disconnect from the server
 * @throw error message if error occurs
 */
void RPCClient::disconnectRPC(){
    string msg = to_string(RPC_DISCONNECT);

    // talk to server
    string res;
    talkToServer(msg, res);

    // parse tokens
    vector<string> tokens;
    parseTokens(const_cast<char*>(res.c_str()), tokens);

    // Print the message from server
    cout << FOREGRN << tokens[1] << RESETTEXT << endl << endl;

    // close socket
    if((close(sock)) < 0){
        throw string("Failed to close the socket");
    }
}

/**
 * Send game mode to server
 * When it is two-player mode, receive name of another player
 * @param mode game mode to send to server
 * @param anotherPlayer name of another player (only in weo-player mode)
 */
void RPCClient::selectModeRPC(int mode, string &anotherPlayer){

    if(mode==MULTIPLAY){
        cout << endl << "... FINDING ANOTHER PLAYER ..." << endl;
    }
    string msg = to_string(RPC_SELECTMODE) + ";" + to_string(mode);

    // talk to server
    string res;
    talkToServer(msg, res);

    // parse tokens
    vector<string> tokens;
    parseTokens(const_cast<char*>(res.c_str()), tokens);

    if(mode==MULTIPLAY){
        if(tokens.size() == 2 && tokens[0] == SUCCESS){
            cout << endl << FOREGRN << tokens[1] << " JOINED" << RESETTEXT << endl << endl;
            anotherPlayer = tokens[1];
        }
    }
}

/**
 * Send client's guess to server and receive number of hits and blows
 * @param pegs number list of pegs
 * @param hits number of hits
 * @param blows number of blows
 */
void RPCClient::guessRPC(vector<int> pegs, int &hits, int &blows) {
    string peglist;
    for(int p : pegs){
        peglist += to_string(p) + " ";
    }
    string msg = to_string(RPC_GUESS) + ";" + peglist;

    // talk to server
    string res;
    talkToServer(msg, res);

    // parse tokens
    vector<string> tokens;
    parseTokens(const_cast<char*>(res.c_str()), tokens);

    if(tokens[0] == SUCCESS){
        vector<string> list = splitStr(tokens[1], " ");
        hits = stoi(list[0]);
        blows = stoi(list[1]);
    }
}

/**
 * Ask server whose turn it is (used only in two-player mode)
 * If it is this player's turn: return true and receive result of previous guess by another player
 * It it is another player's turn: return false and receive nothing
 * @param prevGuess
 * @param hits
 * @param blows
 * @return true if it is this player's turn; false if it is another player's turn
 */
bool RPCClient::isMyTurn(vector<int> &prevGuess, int &hits, int &blows) {
    string msg = to_string(RPC_ISMYTURN);

    // talk to server
    string res;
    talkToServer(msg, res);

    // parse tokens
    vector<string> tokens;
    parseTokens(const_cast<char*>(res.c_str()), tokens);

    if(tokens[0] == SUCCESS){  // YES
        if(tokens.size() == 4){
            vector<string> list = splitStr(tokens[1], " ");
            for(string s : list){
                prevGuess.push_back(stoi(s));
            }
            hits = stoi(tokens[2]);
            blows = stoi(tokens[3]);
        }
        return true;
    }
    return false;
}

/**
 * Ask server to end the game and receive the correct answer and the ending message from server
 * @param answer
 * @param endMsg
 */
void RPCClient::endGameRPC(vector<int> &answer, vector<string> &endMsg) {
    string msg = to_string(RPC_ENDGAME);

    // talk to server
    string res;
    talkToServer(msg, res);

    // parse tokens
    vector<string> tokens;
    parseTokens(const_cast<char*>(res.c_str()), tokens);

    if(tokens[0] == SUCCESS){
        vector<string> list = splitStr(tokens[1], " ");
        for(string s : list){
            answer.push_back(stoi(s));
        }
        endMsg.push_back(tokens[2]);
        endMsg.push_back(tokens[3]);
    }
}

/**
 * Populates a string vector with tokens extracted from the message
 * @param buffer message(input)
 * @param tokens tokens extracted from the buffer (output)
*/
void RPCClient::parseTokens(char *buffer, vector<string> &tokens){
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
vector<string> RPCClient::splitStr(string str, string delim){
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



