#ifndef HITANDBLOW_RPCCLIENT_H
#define HITANDBLOW_RPCCLIENT_H

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <exception>
#include "../utility/ConsoleColor.h"
#define SINGLEPLAY 1
#define MULTIPLAY 2

using namespace std;

/**
 * RPCClient class is responsible for making remote procedure calls to the server
 */
class RPCClient {
private:
    int sock;

    void parseTokens(char *buffer, vector<string> &tokens);
    vector<string> splitStr(string str, string delim);

public:
    RPCClient(){}
    virtual ~RPCClient(){}
    void connectToServer(string ipAddress, int port);
    void loginToServer();
    void talkToServer(string msg, string &res);
    bool connectRPC(string userName, string password);
    void disconnectRPC();
    void selectModeRPC(int mode, string &anotherPlayer);
    void guessRPC(vector<int> pegs, int& hits, int& blows);
    bool isMyTurn(vector<int>& prevGuess, int& hits, int& blows);
    void endGameRPC(vector<int>& answer, vector<string> &endMsg);
};


#endif //HITANDBLOW_RPCCLIENT_H
