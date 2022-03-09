#include <iostream>
#include <pthread.h>
#include <thread>
#include <mutex>
#include <exception>
#include "RPCServer.h"
#include "Client.h"
#include "GameManager.h"

#define PORT 8080

//--- Global Variables --
int cliNum = 0;
RPCServer rpc;
vector<thread> threads;
mutex mtx;
//-----------------------

/**
 * Creates a socket and binds to the server address
 * @param port
 * @param sock
 * @throw error message if error occurs
 */
void createConnection(int port, int &sock){

    int domain = AF_INET;
    int type = SOCK_STREAM;
    int protocol = 0;
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    // Create a socket
    if((sock = socket(domain, type, protocol))<0){
        throw string("Failed to create a socket");
    }
    cout << "... CREATED SOCKET ...\n" << endl;

    // Bind to an address
    if (::bind(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        throw string("Failed to bind to the address");
    }
    cout << "... BOUND TO ADDRESS ...\n" << endl;
}

/**
 * Listens on the port for a client's connection to be established.
 * @param sock
 * @throw error message if error occurs
 */
void listenForClient(int sock){
    // Listen on a port, and wait for a connection to be established
    if (listen(sock, 3) < 0){
        throw string("Failed to listen");
    }
    cout << "... LISTENING ...\n" << endl;
}

/**
 * Accepts the connection from a client
 * @param sock
 * @throw error message if error occurs
 */
Client acceptConnection(int sock){
    Client cliObj;
    if((cliObj.sock = accept(sock, (struct sockaddr *)&cliObj.cli_addr,(socklen_t*)&cliObj.cli_addr)) < 0){
        throw string("Failed to accept a connection from client");
    }
    cout << "... NEW CONNECTION ACCEPTED [Sock#" + to_string(cliObj.sock) +  "] ...\n" << endl;
    return cliObj;
}

/**
 * Start interacting with client
 * @param cliNum
 * @return
 */
void startRPC(int cliNum){
    cout << "... START THREAD FOR CLIENT #" << cliNum << " ..." << endl << endl;

    // Start interacting with client
    rpc.talkToClient(cliNum);

    cout << "... END THREAD FOR CLIENT #" << cliNum << " ..." << endl << endl;
}

/**
 * This is main function for server program.
 * If the server program is called with Port number,
 * it connects to the server program at specified IP address and Port.
 * If not, it uses localhost ip address and 8080 as default.
 * @param argc number of arguments
 * @param argv  argv[0] program name
 *              argv[1] port
 */
int main(int argc, char const* argv[]) {

    try{
        //---------------------------------
        // Check Input Arguments
        //---------------------------------
        int port = PORT;
        if(argc == 2){
            // use specified port
            port = stoi(argv[1]);
        }

        //---------------------------------
        // Establish Connection for Server
        //---------------------------------
        int sock;
        createConnection(port, sock);

        //------------------------------------
        // Listen to Clients & Create Threads
        //------------------------------------
        while(true){
            try{
                // Lock mutex
                mtx.lock();

                // Listening on a port
                listenForClient(sock);

                // Accept the connection from a client
                Client newClient = acceptConnection(sock);

                // create a thread for the new client (pass in clientNum)
                thread newThread(startRPC, cliNum);

                // store new client to rpc hashmap
                rpc.addClients(cliNum, newClient);

                // store new thread to vector
                threads.push_back(move(newThread));

                // increment client number
                cliNum++;

                // Unlock mutex
                mtx.unlock();

            }catch(string &msg){
                cerr << msg << endl;
            }catch(invalid_argument & e){
                cerr << "INVALID ARGUMENT ERROR:" << e.what() << endl;
            }catch(exception &e){
                cerr << "ERROR:" << e.what() << endl;
            }
        }
    }catch(std::invalid_argument & e){
        cerr << "INVALID ARGUMENT ERROR:" << e.what() << endl;
        return -1;
    }catch(exception &e){
        cerr << "ERROR:"<< e.what() << endl;
        return -1;
    }
}
