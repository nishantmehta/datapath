//
//  Copyright 2012 Alin Dobra and Christopher Jermaine
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
#include "CommListener.h"
#include "CommManager.h"
#include "CommunicationFramework.h"
#include "Errors.h"
#include "RemoteMacros.h"
#include "CommConfig.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <strings.h>

#include <iostream>
#include <sstream>
#include <string>


using namespace std;


CommListenerImp::CommListenerImp(string _machineName, int _port):
	machineName(_machineName), port(_port) , socketFd (-1) {

	//open socket for the listener (IPv4, TCP/IP)
	socketFd = socket(AF_INET, SOCK_STREAM, 0);
	FATALIF(socketFd < 0, "ERROR opening socket.");

	//bind the socket to the listening port
	struct sockaddr_in serv_addr;
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);
	FATALIF(bind(socketFd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0, "ERROR on binding.");

	//mark the socket as ready to accept connections
	listen(socketFd, LISTENER_MAX_OUT_CONNECTIONS);
}


CommListenerImp::~CommListenerImp() {
	//close client connection
	close(socketFd);
}


int CommListenerImp::ProduceMessage(void){
	struct sockaddr_in peerAddr;
	socklen_t peerAddrLen = sizeof(peerAddr);

	//accept new connections
	int peerSocketFd = accept(socketFd, (struct sockaddr *)&peerAddr, &peerAddrLen);
	if (peerSocketFd < 0) {
		WARNING("ERROR Listener on accept. Exit.");

		CommManager& manager = CommManager::GetManager();
		manager.StopListener();

		return -1;
	}

	//extract the address of the connecting peer in application format
	HostAddressPort peer;
	IN_PLACE_STRING_CONSTRUCTOR(peer.address, INET_ADDRSTRLEN, buffer);

	if (inet_ntop(AF_INET, &peerAddr.sin_addr, buffer, INET_ADDRSTRLEN) == NULL) {
		WARNING("ERROR Listener peer address. Exit.");

		CommManager& manager = CommManager::GetManager();
		manager.StopListener();

		return -1;
	}
	peer.port = (int)ntohs(peerAddr.sin_port);

	//tell the comm manager to register the new communication receiver
	CommManager& manager = CommManager::GetManager();
	int res = manager.RegisterReceiver(peerSocketFd, peer);
	WARNINGIF(res != 0,
		"ERROR receiver already registered for (%s, %d).",
		peer.address.c_str(), peer.port);

	return 0;
}
