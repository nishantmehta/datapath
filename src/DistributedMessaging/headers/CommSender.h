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
#ifndef _COMM_SENDER_H_
#define _COMM_SENDER_H_

#include "MessageMacros.h"
#include "EventProcessor.h"
#include "EventProcessorImp.h"

#include <string>

using namespace std;


/**
	Communication sender opens a connection with a remote host and sends all the
	messages destined to that host from the local machine.
	The address of the remote host is passed at construction time.
	The remote host is always contacted on a pre-configured port.
*/
class CommSenderImp : public EventProcessorImp {
private:
	//communication socket
	int socketFd;

	//remote host name
	string remoteMachine;

	//boolean indicator for active sender
	bool isActive;

public:
	//constructor & destructor
	CommSenderImp(string _remoteMachine);
	virtual ~CommSenderImp();

	//close the socket
	int Close();

	//check to see if the sender is active (it has a connection to the remote host)
	bool IsActive() {return isActive;}

	// message handler for all remote messages
	MESSAGE_HANDLER_DECLARATION(ProcessRemoteMessage);
};

class CommSender : public EventProcessor {
public:
	//empty constructor needed for initialization
	CommSender() {}
	CommSender(string _remoteMachine) {
  	evProc = new CommSenderImp(_remoteMachine);
	}
	virtual ~CommSender() {}

	//return the internal EventProcessorImp needed for ProxyEventProcessor build
	EventProcessorImp* GetEvProc() { return dynamic_cast<EventProcessorImp*>(evProc); }

	//close the socket
	int Close() {
		CommSenderImp* sender = dynamic_cast<CommSenderImp*>(evProc);
		return sender->Close();
	}

	//check to see if the sender is active (it has a connection to the remote host)
	bool IsActive() {
		CommSenderImp* sender = dynamic_cast<CommSenderImp*>(evProc);
		return sender->IsActive();
	}
};

#endif // _COMM_SENDER_H_
