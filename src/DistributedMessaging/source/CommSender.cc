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
#include "RemoteMessage.h"
#include "CommSender.h"
#include "CommConfig.h"
#include "Archive.h"
#include "Errors.h"
#include "CommManager.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <iostream>
#include <sstream>
#include <string>


using namespace std;


CommSenderImp::CommSenderImp(string _remoteMachine):
	remoteMachine(_remoteMachine), isActive(false)
#ifdef DEBUG_EVPROC
	, EventProcessorImp(true, "CommSender") // comment to remove debug
#endif
{
	struct sockaddr_in serv_addr;
	struct hostent *server;

	socketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketFd < 0) {
		WARNING("ERROR opening socket.");
		return;
	}

	server = gethostbyname(remoteMachine.c_str());
	if (server == NULL) {
		WARNING("ERROR host %s cannot be found.", remoteMachine.c_str());
		Close();
		return;
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr,
				(char *)&serv_addr.sin_addr.s_addr,
				server->h_length);
	serv_addr.sin_port = htons(LISTEN_PORT);
	if (connect(socketFd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
		WARNING("ERROR connecting to %s on port %d.", remoteMachine.c_str(), LISTEN_PORT);
		Close();
		return;
	}

	isActive = true;

	RegisterMessageProcessor(RemoteMessage::type, &ProcessRemoteMessage, 1 /*priority*/);
}

MESSAGE_HANDLER_DEFINITION_BEGIN(CommSenderImp, ProcessRemoteMessage, RemoteMessage){
	ostringstream ss(ios::binary);
	BINARY_ARCHIVE_OUTPUT oa(ss);

	RemoteMessage *tMsg = &msg;
	oa << tMsg;

	// send first the newtork header
	NetworkMessageHeader header;
	msg.SetRemote();
	header.type = msg.Type();
	header.service = msg.GetService();

	string str = ss.str();
	header.size = str.length();

	size_t n = send(evProc.socketFd, (void*)(&header), sizeof(NetworkMessageHeader), MSG_NOSIGNAL);
	if (n != sizeof(NetworkMessageHeader)) {
		WARNING("ERROR writing header to socket %d.", evProc.socketFd);

		CommManager& manager = CommManager::GetManager();
		manager.EliminateSender(evProc.remoteMachine);

		return;
	}

	// now we send the payload
	// NOTE: we assume that the object is unithreaded and only one object sends to
	// a given machine => NO MESSAGE IN BETWEEN THESE MESSAGES
	n = send(evProc.socketFd, str.data(), str.length(), MSG_NOSIGNAL);
	if (n != str.length()) {
		WARNING("ERROR writing to socket %d.", evProc.socketFd);

		CommManager& manager = CommManager::GetManager();
		manager.EliminateSender(evProc.remoteMachine);

		return;
	}

	// QUESTION: is it the case that that we always have the full message sent
	// Might need to put a while loop around "send" to send all pieces
}MESSAGE_HANDLER_DEFINITION_END

CommSenderImp::~CommSenderImp() {
	// close connection
	Close();
}

//close the socket
int CommSenderImp::Close() {
	if (socketFd != -1) {
		close(socketFd);
		socketFd = -1;

		return 0;
	}

	return -1;
}
