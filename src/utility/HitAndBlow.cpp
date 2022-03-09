#include "HitAndBlow.h"

/**
 * Constructor for HitAndBlow class
 * @param gameMode
 */
HitAndBlow::HitAndBlow(){}

/**
 * Constructor for HitAndBlow class that takes gameMode as input
 * @param gameMode
 */
HitAndBlow::HitAndBlow(int gameMode) : gameMode(gameMode) {
    // set random pegs for the game
    this->setPegs();
    cout << "ANSWER PEGS ARE SET TO: ";
    for(int peg : this->getAnswer()){
        this->printPeg(peg);
        cout << " ";
    }
    cout << endl;
}

/**
 * Destructor for HitAndBlow class
 * @param gameMode
 */
HitAndBlow::~HitAndBlow() {}

/**
 * Hit&Blow game home that prompt user to select the options from below:
 * 1. SINGLE-PLAY MODE
 * 2. TWO-PLAYER MODE
 * 3. EXIT GAME
 */
void HitAndBlow::gameHome() {
    cout << "\n********************************************\n";
    cout << BOLDYELLOW << "                 HIT & BLOW" << RESETTEXT;
    cout << "\n********************************************\n";
    cout << BOLDTEXT << "▼ RULES\n" << RESETTEXT;
    cout << "  Your goal is to guess the correct sequence of 4 colored pegs.\n"
                 "  You have 8 attempts to guess. For each attempt, \n"
                 "  you will get feedback with the number of HITs and BLOWs\n\n  "
              << BackRED("HIT") << "  : right color at right position\n  "
              << BackBLU("BLOW") << " : right color but wrong position\n\n";
    cout << BOLDTEXT << "▼ SELECT OPTION" << RESETTEXT << endl;
    cout << "  1. SINGLE-PLAYER MODE\n";
    cout << "  2. TWO-PLAYER MODE\n";
    cout << "  3. EXIT GAME\n\n";
    while(true){
        cout << FOREGRN << ">>> " << RESETTEXT;
        string input;
        getline(cin, input);
        cout << endl;
        try{
            int num = stoi(input);
            if(num == SINGLEPLAY || num == MULTIPLAY || num == ENDGAME){
                this->gameMode = num;
                break;
            }
        }catch(exception &e){ }
        cout<< "INVALID INPUT: ENTER 1 OR 2 OR 3" << endl;
    }
}

/**
 * Ask user to guess the sequence of colored pegs
 * @return vector<int> number list of user's guess
 */
vector<int> HitAndBlow::guess() {
    // print the attempt no
    printAttemptNo(getAttempts().size() + 1);
    cout << "\nENTER 4 NUMBERS SEPARATED BY A SPACE\n";
    cout << "PEG OPTIONS ▶︎ ";
    printPegOptions();

    vector<int> attempt;
    while(true){
        cout << FOREGRN << "\n>>> " << RESETTEXT;
        string guess;
        getline(cin, guess);
        attempt = processInput(guess, " ");

        if(attempt.size() != 4){
            cout << "INVALID INPUT\n ";
        }else{
            cout << "CONFIRM? (Y/N) ";
            printPegs(attempt);
            cout << FOREGRN << ">>> " << RESETTEXT;
            string res;
            getline(cin, res);
            cout << endl;
            if(res == "y" || res == "Y"){ break; }
        }
    }
    submitGuess(attempt);
    return attempt;
}

/**
 * Print the ending message and ask user to select from continue and end game
 * @param msgs ending message
 */
void HitAndBlow::endGame(vector<string> msgs) {
    string msg1, msg2;
    if(msgs.size() == 2){
        msg1 = msgs[0];
        msg2 = msgs[1];
        if(gameMode == MULTIPLAY){
            msg2 += " & " + players[0];
        }
    }
    cout << endl << endl;
    cout << "********************************************\n";
    cout << "********************************************\n\n";
    cout << FOREYEL << BOLDTEXT << BLINK << "              " << msg1 << RESETTEXT << endl << endl;
    cout << "      " << msg2 << endl << endl << endl;
    cout << "            ANSWER IS ";
    vector<int> ans = this->getAnswer();
    printPegs(ans);
    cout << "********************************************\n";
    cout << "********************************************\n\n";
    cout << "CONTINUE? (Y/N)" << endl;
    cout << FOREGRN << ">>> " << RESETTEXT;
    string res;
    getline(cin, res);
    cout << endl;
    if(res == "y" || res == "Y"){
        // reset the game
        resetGame();
    }else{
        gameMode = ENDGAME;
    }
}

/**
 * Count the number of hits and blows (Set solved flag to true if hits == 4)
 * @param pegs number list for pegs
 * @param hits number of hits
 * @param blows nmber of blows
 */
void HitAndBlow::countHitsBlows(vector<int> pegs, int &hits, int &blows) {

    unordered_set<int> colors;

    for(int i = 0; i < pegs.size(); i ++) {
        if(this->answer.find(pegs[i]) != this->answer.end()) {
            if(this->answer[pegs[i]] == i){
                if(colors.count(pegs[i]) == 1) {
                    hits++;
                    blows--;
                }
                else{
                    hits++;
                }
            }else{
                if(colors.count(pegs[i]) == 1) {
                    continue;
                }
                else{
                    blows++;
                }
            }
        }
        colors.insert(pegs[i]);
    }
    if(hits==4){
        setIsSolved(true);
    }

}

/**
 * Set the 4 colored pegs in a random order (no duplicate colors)
 */
void HitAndBlow::setPegs() {
    int pos = 0;
    srand(time(nullptr));  // initialize random seed
    while(this->answer.size() < 4){
        int colorNum = 1 + rand() % 6;
        if(this->answer.find(colorNum) == this->answer.end()){
            this->answer[colorNum] = pos++;
        }
    }
}

/**
 * Split a string of integers by space and store them into integer vector.
 * @param strPegs string to be split into integers
 * @param delim delimiter
 * @return integer vector
 */
vector<int> HitAndBlow::processInput(string strPegs, string delim) {
    vector<string> strs = splitStr(strPegs, delim);
    vector<int> pegs;
    try{
        for(auto s : strs){
            int num = stoi(s);
            if(num >= 1 && num <=6){
                pegs.push_back(num);
            }
        }
    }catch (exception &e){}
    return pegs;
}

/**
 * Split a string by specified delimiter
 * @param str
 * @param delim
 * @return a vector of strings
 */
vector<string> HitAndBlow::splitStr(string str, string delim){
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

/**
 * Print the specified peg
 * @param pegNum number of a peg
 */
void HitAndBlow::printPeg(int pegNum) {
    switch(pegNum){
        case PEGRED:
            cout << FORERED "❶" << RESETTEXT;
            break;
        case PEGYEL:
            cout << FOREYEL "❷" << RESETTEXT;
            break;
        case PEGCYN:
            cout << FORECYN "❸" << RESETTEXT;
            break;
        case PEGGRN:
            cout << FOREGRN "❹" << RESETTEXT;
            break;
        case PEGMAG:
            cout << FOREMAG "❺" << RESETTEXT;
            break;
        case PEGWHT:
            cout << FOREWHT "❻️" << RESETTEXT;
            break;
    }
}

/**
 * Print the specified pegs
 * @param pegs list of pegs
 */
void HitAndBlow::printPegs(vector<int> pegs) {
    for(auto p : pegs){
        printPeg(p);
        cout << " ";
    }
    cout << endl;
}

/**
 * Print peg options
 */
void HitAndBlow::printPegOptions() {
    for(int i = 1; i <=6 ;i++){
        cout << i << ":";
        printPeg(i);
        cout << "  ";
    }
    cout << endl;
}

/**
 * Print the number of attempts
 * @param num
 */
void HitAndBlow::printAttemptNo(int num) {
    cout << BACKWHT << BOLDBLACK << "                 ATTEMPT #" << num << "                 " << RESETTEXT;
}

/**
 * Store the guess into attempts list
 * @param guess
 */
void HitAndBlow::submitGuess(vector<int> guess){
    attempts.push_back(guess);
}

/**
 * Reset the game to get ready for a new game
 */
void HitAndBlow::resetGame() {
    gameMode = HOME;
    answer.clear();
    players.clear();
    attempts.clear();
    nextPlayer = -1;
    isSolved = false;
}

/**
 * Print the number of hits and blows
 * @param hits
 * @param blows
 */
void HitAndBlow::printHitsBlows(int hits, int blows){
    cout << "             " << BACKRED  << "HIT: " << hits << RESETTEXT << "   "
    << BACKBLU << "BLOW: " << blows << RESETTEXT << endl << endl;;
}

/**
 * Getter for answer pegs
 * @return
 */
vector<int> HitAndBlow::getAnswer() {
    vector<int> ans(4);
    for(auto it : this->answer){
        ans[(it).second] = (it).first;
    }
    return ans;
}

/**
 * Setter for answer pegs
 * @param answer
 */
void HitAndBlow::setAnswer(const vector<int> &answer) {
    for(int i = 0; i < answer.size(); i++){
        this->answer[answer[i]] = i;
    }
}

/**
 * Getter for isSolved
 * @return
 */
bool HitAndBlow::getIsSolved() const {
    return isSolved;
}

/**
 * Setter for isSolved
 * @param isSolved
 */
void HitAndBlow::setIsSolved(bool isSolved) {
    HitAndBlow::isSolved = isSolved;
}

/**
 * Getter for gameMode
 * @return
 */
int HitAndBlow::getGameMode() const {
    return gameMode;
}

/**
 * Setter for setMode
 * @param gameMode
 */
void HitAndBlow::setGameMode(int gameMode) {
    HitAndBlow::gameMode = gameMode;
}

/**
 * Getter for attemps
 * @return
 */
const vector< vector<int> > &HitAndBlow::getAttempts() const {
    return attempts;
}

/**
 * Getter for players
 * @return
 */
const unordered_map<int, string> &HitAndBlow::getPlayers() const {
    return players;
}

/**
 * Getter for nextPlayer (used only in 2-player mode)
 * @return
 */
int HitAndBlow::getNextPlayer() const {
    return nextPlayer;
}

/**
 * Setter for nextPlayer (used only in 2-player mode)
 * @param nextPlayer
 */
void HitAndBlow::setNextPlayer(int nextPlayer) {
    HitAndBlow::nextPlayer = nextPlayer;
}

/**
 * Add a player to the game (used only in 2-player mode)
 * @param playerNo
 * @param playerName
 */
void HitAndBlow::addPlayer(int playerNo, string playerName) {
    players.insert(make_pair(playerNo, playerName));
}

/**
 * Remove a player from the game (used only in 2-player mode)
 * @param playerNo
 */
void HitAndBlow::removePlayer(int playerNo) {
    players.erase(playerNo);
}

/**
 * Get the player number for another player in the game (used only in 2-player mode)
 * @param playerNo
 * @return
 */
int HitAndBlow::getAnotherPlayer(int playerNo){
    int anotherPlayer = -1;
    for(auto player : players){
        if(player.first != playerNo){
            anotherPlayer =  player.first;
        }
    }
    return  anotherPlayer;
}
