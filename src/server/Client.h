#ifndef HITANDBLOW_CLIENT_H
#define HITANDBLOW_CLIENT_H

#include <string>
#include <netinet/in.h>

using namespace std;

/**
 * Client class is responsible for storing client information
 */
class Client {
public:
    int sock;
    struct sockaddr_in cli_addr;
private:
    string username;
    string password;

public:
    Client() {}
    virtual ~Client() {}

    const string &getUsername() const {
        return username;
    }

    void setUsername(const string &username) {
        Client::username = username;
    }

    const string &getPassword() const {
        return password;
    }

    void setPassword(const string &password) {
        Client::password = password;
    }
};

#endif //HITANDBLOW_CLIENT_H
