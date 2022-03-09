#ifndef HITANDBLOW_RPCSERVER_H
#define HITANDBLOW_RPCSERVER_H

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <regex>
#include <pthread.h>
#include <mutex>
#include <thread>
#include "Client.h"
#include "GameManager.h"

using namespace std;

/**
 * RPCServer class is responsible for responding to the remote procedure calls from clients.
 * The shared resources are protected by mutex lock to avoid access collision by clients.
 */
class RPCServer {

private:
    mutex mtx;
    unordered_map<int, Client> clients; // <clientNum, clientObj>
    GameManager gameMgr;

    void connectRPC(int cliNum, vector<string> &tokens);
    void disconnectRPC(int cliNum);
    void selectModeRPC(int cliNum, vector<string> &tokens);
    void guessRPC(int cliNum, vector<string> &tokens);
    void isMyTurnRPC(int cliNum, vector<string> &tokens);
    void endGameRPC(int cliNum);
    void sendToClient(int cliNum, string msg);
    void parseTokens(char *buffer, vector<string> &tokens);
    vector<string> splitStr(string str, string delim);

public:
    RPCServer(){}
    virtual ~RPCServer(){}
    void talkToClient(int cliNum);
    void addClients(int cliNum, Client cliObj);
};

#endif //HITANDBLOW_RPCSERVER_H
