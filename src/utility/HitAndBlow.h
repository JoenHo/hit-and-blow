#ifndef HITANDBLOW_HITANDBLOW_H
#define HITANDBLOW_HITANDBLOW_H

#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <exception>
#include <random>
#include <ctime>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include "ConsoleColor.h"

//-- Peg Colors --
#define PEGRED 1 // "red"
#define PEGYEL 2 //"yellow"
#define PEGCYN 3 //"cyan"
#define PEGGRN 4 //"green"
#define PEGMAG 5 //"magenta"
#define PEGWHT 6 //"blue"
#define HOME 0
#define SINGLEPLAY 1
#define MULTIPLAY 2
#define ENDGAME 3

using namespace std;

/**
 * HitAndBlow class is responsible for providing the logic of Hit&Blow game
 */
class HitAndBlow {
private:
    int gameMode = 0; // 0: Home 1: Single-Player Mode, 2: 2-Player Mode, 3: End game
    unordered_map<int, int> answer; // <color number, position>
    unordered_map<int, string> players; // <playerNo, PlayerName>
    vector< vector<int> > attempts;
    int nextPlayer = -1; // playerNo (only used for 2-Player mode)
    bool isSolved = false;

private:
    vector<int> processInput(string strPegs, string delim);
    vector<string> splitStr(string str, string delim);

public:
    // constructors & destructor
    HitAndBlow();
    HitAndBlow(int gameMode);
    virtual ~HitAndBlow();

    // functions for server program
    void setPegs();
    void submitGuess(vector<int> guess);
    void countHitsBlows(vector<int> pegs, int& hits, int&blows);
    void addPlayer(int playerNo, string playerName);
    void removePlayer(int playerNo);
    int getAnotherPlayer(int playerNo);

    // functions for client program
    void gameHome();
    vector<int> guess();
    void printPegOptions();
    void printPeg(int pegNum);
    void printPegs(vector<int> pegs);
    void printHitsBlows(int hits, int blows);
    void printAttemptNo(int num);
    void endGame(vector<string> msgs);
    void resetGame();

    // getters & setters
    int getGameMode() const;
    void setGameMode(int gameMode);
    vector<int> getAnswer();
    void setAnswer(const vector<int> &answer);
    const vector< vector<int> > &getAttempts() const;
    bool getIsSolved() const;
    void setIsSolved(bool isSolved);
    const unordered_map<int, string> &getPlayers() const;
    int getNextPlayer() const;
    void setNextPlayer(int nextPlayer);

};

#endif //HITANDBLOW_HITANDBLOW_H
