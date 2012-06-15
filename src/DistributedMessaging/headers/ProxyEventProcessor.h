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
#ifndef _PROXY_EVENT_PROCESSOR_
#define _PROXY_EVENT_PROCESSOR_

#include "EventProcessor.h"
#include "EventProcessorImp.h"
#include "Numa.h"
#include "Message.h"


/**
	This is a special handle for sending messages remotely.
	It only communicates with the special event processor in the
	communication framework.
*/
class ProxyEventProcessor : public EventProcessor {
protected:
	// service is the equivalent of the port in TCP
	// it is used to route the messages at destination
	off_t service;

public:
	// constructor & destructor
	ProxyEventProcessor(void):EventProcessor(){ service = -1; }
	ProxyEventProcessor(EventProcessorImp* _commSender, off_t _service);
	virtual ~ProxyEventProcessor(){};

	virtual void ProcessMessage(Message& msg);

	// methods used to pass around proxy event processors
	void swap(ProxyEventProcessor&);
	void copy(ProxyEventProcessor&);
};


#endif // _PROXY_EVENT_PROCESSOR_
