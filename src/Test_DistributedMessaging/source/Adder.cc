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
#include <iostream>
#include <sstream>

#include "Adder.h"
#include "RemoteMessage.h"
#include "MessageMacros.h"
#include "RemoteMessageMacros.h"
#include "Messages.h"
#include "CommunicationFramework.h"
#include "TestConf.h"

using namespace std;


AdderImp::AdderImp(void)
#ifdef DEBUG_EVPROC
: EventProcessorImp(true, "Adder") // comment to remove debug
#endif
{
  sum=0;

  // register the messages
  RegisterMessageProcessor(DieMessage::type, &Die, 0 /* highest priority */);
  RegisterMessageProcessor(AddMessage::type, &AddMe, 1 /*priority*/);
}

// AddMe handler definition
REMOTE_MESSAGE_HANDLER_DEFINITION_BEGIN(AdderImp, AddMe, AddMessage){
  // add the numer to the sum
  evProc.sum+=msg.number;

  cout << "Adder: got number " << msg.number << endl;

  if (msg.number == 0){
		ProxyEventProcessor proxySender;
		// rMsg is the message serialized
		// it knows where the message came from
		string remoteMachine = rMsg.GetMachineName();
		if (FindRemoteEventProcessor(remoteMachine, SERVICE_KILL, proxySender) == 0) {
			KillMessage_Factory(proxySender, evProc.sum);
		}
		else {
			cout << "ERROR cannot send to " << remoteMachine.c_str() << "." << endl;
		}
	}
}REMOTE_MESSAGE_HANDLER_DEFINITION_END

// Die handler definition
MESSAGE_HANDLER_DEFINITION_BEGIN(AdderImp, Die, DieMessage){
  cout<<"The total is: "<< evProc.sum << endl;

  // call the underlying die message to still die
  evProc.EventProcessorImp::processDieMessage(evProc,msg);
}MESSAGE_HANDLER_DEFINITION_END

