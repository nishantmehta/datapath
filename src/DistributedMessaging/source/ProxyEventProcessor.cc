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
#include "ProxyEventProcessor.h"
#include "EventProcessorImp.h"
#include "Message.h"
#include "RemoteMessage.h"
#include "EventProcessor.h"
#include "Swap.h"


ProxyEventProcessor::ProxyEventProcessor(EventProcessorImp* _commSender, off_t _service):
	EventProcessor(_commSender), service(_service) {}

void ProxyEventProcessor::ProcessMessage(Message& msg) {
	// any message sent to the proxy must be a remote message
	// the program will die otherwise
	RemoteMessage& rMsg = dynamic_cast<RemoteMessage&>(msg);
	rMsg.SetService(service);
	//force the message to report the type as a remote message
	rMsg.ResetRemote();
	evProc->ProcessMessage(rMsg);
}

void ProxyEventProcessor::swap(ProxyEventProcessor& withMe) {
	EventProcessor::swap(withMe);
	SWAP(service, withMe.service);
}

void ProxyEventProcessor::copy(ProxyEventProcessor& fromMe) {
	EventProcessor::copy(fromMe);
	service = fromMe.service;
}
