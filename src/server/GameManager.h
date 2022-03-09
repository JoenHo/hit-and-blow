#ifndef HITANDBLOW_GAMEMANAGER_H
#define HITANDBLOW_GAMEMANAGER_H

#include "../utility/HitAndBlow.h"

using  namespace std;

/**
 * GameManager class is responsible for managing games that currently active.
 * The shared resources are protected by mutex lock to avoid access collision by clients.
 */
class GameManager {
private:
    mutex mtx;
public:
    unordered_map<int, HitAndBlow*> activeGameList; // <client#, gameObj>

    /**
     * Creat a new single play game and assign it to the specified client.
     * @param clientNum
     * @param clientName
     */
    void assignSinglePlayGame(int clientNum, string clientName){
        mtx.lock(); // Lock mutex

        HitAndBlow* game = new HitAndBlow(SINGLEPLAY);
        game->addPlayer(clientNum, clientName);
        activeGameList[clientNum] = game;

        mtx.unlock(); // Unlock mutex
    }

    /**
     * Find a two-player game (or create a new two-player game if not available)
     * and assign it to the specified client.
     * @param clientNum
     * @param clientName
     */
    void assignMultiPlayGame(int clientNum, string clientName){
        mtx.lock(); // Lock mutex

        bool isAssigned = false;
        for(auto &game : activeGameList){
            // find an existing game to assign if available
            if(game.second->getGameMode()==MULTIPLAY && game.second->getPlayers().size() < 2){
                cout << "JOINED A TWO-PLAYER GAME\n";
                game.second->addPlayer(clientNum, clientName);
                activeGameList[clientNum] = game.second;
                isAssigned = true;
                break;
            }
        }
        // create new game to assign if couldn't find existing game
        if(!isAssigned){
            // create new multiplayer game to assign
            cout << "START A TWO-PLAYER GAME\n";
            HitAndBlow* game = new HitAndBlow(MULTIPLAY);
            set<int> players;
            players.insert(clientNum);
            game->addPlayer(clientNum, clientName);
            game->setNextPlayer(clientNum);
            activeGameList[clientNum] = game;
        }

        mtx.unlock(); // Unlock mutex
    }

    /**
     * Delete game object that was assigned to the specified client
     * and remove it from the active game list.
     * @param clientNum
     */
    void leaveGame(int clientNum){
        mtx.lock(); // Lock mutex

        if(activeGameList[clientNum]->getGameMode()==SINGLEPLAY){
            delete activeGameList[clientNum];   // free memory
            activeGameList[clientNum] = nullptr;
        }else if(activeGameList[clientNum]->getGameMode()==MULTIPLAY){
            activeGameList[clientNum]->removePlayer(clientNum);
            // if there is no player
            if(activeGameList[clientNum]->getPlayers().size() == 0){
                delete activeGameList[clientNum];   // free memory
                activeGameList[clientNum] = nullptr;
            }
        }
        // remove from the active list
        activeGameList.erase(clientNum);

        mtx.unlock(); // Unlock mutex
    }
};
#endif //HITANDBLOW_GAMEMANAGER_H
