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
#include "Archive.h"
#include "Errors.h"
#include "CommManager.h"
#include "CommReceiver.h"
#include "RemoteMessage.h"
#include "EventProcessor.h"
#include "RemoteMacros.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <strings.h>
#include <errno.h>

#include <iostream>
#include <sstream>
#include <string>


using namespace std;


CommReceiverImp::CommReceiverImp(const int _socketFd, HostAddressPort _source):
	socketFd(_socketFd) {
	source.swap(_source);
}

CommReceiverImp::~CommReceiverImp() {
	//close the connection
	Close();
}

//close the socket
int CommReceiverImp::Close() {
	if (socketFd != -1) {
		close(socketFd);
		socketFd = -1;

		return 0;
	}

	return -1;
}


int CommReceiverImp::ProduceMessage(void) {
	//read messages from the socket until the generator is killed from the exterior
	NetworkMessageHeader header;

	// first read the header from the stream
	size_t sizeHd = recv(socketFd, (void*)(&header), sizeof(NetworkMessageHeader), 0);
	//if there is any error, eliminate receiver from CommManager and kill receiver
	if (sizeHd != sizeof(NetworkMessageHeader)) {
		WARNING("ERROR reading header from socket %d (%s, %d).",
			socketFd, source.address.c_str(), source.port);

		CommManager& manager = CommManager::GetManager();
		manager.EliminateReceiver(source);

		return -1;
	}

	// now we know the size of the message so we read it for real
	string sBuffer;
	IN_PLACE_STRING_CONSTRUCTOR(sBuffer, header.size, buffer);

	size_t sizeBf = header.size;
	char *cPos = buffer;
	// we expect at least sometimes to read a message fragment by fragment
	while (sizeBf > 0){
		size_t cSize = recv(socketFd, cPos, sizeBf, 0);
		if ((cSize < 0) || (cSize > sizeBf)) {
			WARNING("ERROR reading from socket %d (%s, %d).",
				socketFd, source.address.c_str(), source.port);

			CommManager& manager = CommManager::GetManager();
			manager.EliminateReceiver(source);

			return -1;
		}

		cPos += cSize;
		sizeBf -= cSize;
	}

	// find the destination event processor based on the service in message header
	CommManager& manager = CommManager::GetManager();
	EventProcessor destination;
	int rez = manager.GetEventProcessor(header.service, destination);
	WARNINGIF(rez != 0, "ERROR event processor from socket %d (%s, %d).",
		socketFd, source.address.c_str(), source.port);

	// Create a message container
	if (rez == 0)
		RemoteMessageContainer_Factory(destination, source.address, sBuffer, header);

	return 0;
}
