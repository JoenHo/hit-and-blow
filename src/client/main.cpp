#include <chrono>
#include <thread>
#include "RPCClient.h"
#include "../utility/HitAndBlow.h"

#define LOCALHOST "127.0.0.1"
#define DEFAULT_PORT 8080

/**
 * Sleeps for specified seconds
 * @param sec
 */
void sleep(int sec){
    srand(time(0)); // Initialize random number generator with system clock
    this_thread::sleep_for(chrono::seconds (sec) );
}

/**
 * This is main function for client program.
 * If the client program is called with server IP address and Port number,
 * it connects to the server program at specified IP address and Port.
 * If not, it uses localhost ip address and 8080 as default.
 * @param argc number of arguments
 * @param argv  argv[0] program name
 *              argv[1] server ip address
 *              argv[2] port
 */
int main(int argc, char const* argv[]) {

    try{
        //---------------------------------
        // Check Input Arguments
        //---------------------------------
        string ipAddress = LOCALHOST;
        int port = DEFAULT_PORT;
        if(argc == 3){
            // use specified ip address and port
            ipAddress = argv[1];
            port = stoi(argv[2]);
        }

        //-----------------------
        // Connect to the server
        //-----------------------
        RPCClient clientRPC;
        clientRPC.connectToServer(ipAddress, port);

        //-----------------------------
        // Login to the server program
        //-----------------------------
        clientRPC.loginToServer();

        //------------
        // Start game
        //------------
        HitAndBlow game;
        while(game.getGameMode() != ENDGAME){
            game.gameHome();
            vector<int> ans;
            vector<string> endMsg;
            vector<int> guess;
            string anotherPlayer;
            int hits = 0;
            int blows = 0;
            switch (game.getGameMode()) {
                case SINGLEPLAY: // Single-Player Mode
                    // RPC: SELECTMODE
                    clientRPC.selectModeRPC(SINGLEPLAY, anotherPlayer);

                    // guess until problem solved or all attempts are used
                    while(game.getIsSolved() == false && game.getAttempts().size() < 8){
                        guess.clear();
                        guess = game.guess();
                        // RPC: GUESS
                        clientRPC.guessRPC(guess, hits, blows);
                        if(hits == 4){
                            game.setIsSolved(true);
                        }
                        game.printHitsBlows(hits, blows);
                    }
                    // RPC: ENDGAME
                    clientRPC.endGameRPC(ans, endMsg);
                    game.setAnswer(ans);
                    game.endGame(endMsg);
                    break;

                case MULTIPLAY: // Two-Player Mode
                    // RPC: SELECTMODE
                    clientRPC.selectModeRPC(MULTIPLAY, anotherPlayer);
                    game.addPlayer(0, anotherPlayer);

                    // guess until problem solved or all attempts are used
                    while(game.getIsSolved() == false && game.getAttempts().size() < 8) {

                        // RPC: ISMYTURN
                        guess.clear();
                        if(!clientRPC.isMyTurn(guess, hits, blows)){
                            // print the attempt no
                            game.printAttemptNo(game.getAttempts().size() + 1);
                            cout << "\n... " << anotherPlayer << " IS GUESSING ...\n\n";
                            // sleep for 5 sec before asking again
                            sleep(5);
                        }
                        while (!clientRPC.isMyTurn(guess, hits, blows)) {
                            // sleep for 5 sec before asking again
                            sleep(5);
                        }

                        // if there is previous guess, show the previous guess
                        if (guess.size() != 0) {
                            game.submitGuess(guess);
                            // print previous guess, and the number of hits & blows
                            //game.printAttemptNo(game.getAttempts().size());
                            cout << "\nGUESS BY " << anotherPlayer << ": ";
                            game.printPegs(guess);
                            cout << endl << endl;
                            game.printHitsBlows(hits, blows);
                            if(hits == 4){
                                game.setIsSolved(true);
                            }
                        }

                        // guess
                        if(game.getIsSolved() == false && game.getAttempts().size() < 8){
                            guess.clear();
                            guess = game.guess();
                            // RPC: GUESS
                            clientRPC.guessRPC(guess, hits, blows);
                            if(hits == 4){
                                game.setIsSolved(true);
                            }
                            game.printHitsBlows(hits, blows);
                        }
                    }
                    // RPC: ENDGAME
                    clientRPC.endGameRPC(ans, endMsg);
                    game.setAnswer(ans);
                    game.endGame(endMsg);
                    break;

                case ENDGAME: // End the Game
                    game.setGameMode(ENDGAME);
                    break;
            }
        }
        //----------------------------
        // Disconnect from the server
        //----------------------------
        clientRPC.disconnectRPC();

    }catch(string &msg){
        cerr << msg << endl;
        return -1;
    }catch(invalid_argument & e){
        cerr << "INVALID ARGUMENT ERROR:" << e.what() << endl;
        return -1;
    }catch(exception &e){
        cerr << "ERROR:" << e.what() << endl;
        return -1;
    }
    return 0;
}
