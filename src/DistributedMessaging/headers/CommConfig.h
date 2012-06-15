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
#ifndef _COMM_CONFIG_H_
#define _COMM_CONFIG_H_

//port number for communication listener
#define LISTEN_PORT 11111

//the maximum number of outstanding connections in the listener
#define LISTENER_MAX_OUT_CONNECTIONS 10

//message queue id for killer generator
#define KILL_MSG_QUEUE_ID 1111

//kill message type for killer generator message queue
#define KILL_MSG_TYPE 999

#endif // _COMM_CONFIG_H_
