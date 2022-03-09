CXX = g++
CXXFLAGS = -std=c++11
SERVER_NAME = server 
CLIENT_NAME = client 

hitandblow: src/server/main.cpp src/server/RPCServer.cpp src/utility/HitAndBlow.cpp src/client/main.cpp src/client/RPCClient.cpp src/utility/HitAndBlow.cpp
	$(CXX) $(CXXFLAGS) -pthread -o $(SERVER_NAME) src/server/main.cpp src/server/RPCServer.cpp src/utility/HitAndBlow.cpp
	$(CXX) $(CXXFLAGS) -o $(CLIENT_NAME) src/client/main.cpp src/client/RPCClient.cpp src/utility/HitAndBlow.cpp