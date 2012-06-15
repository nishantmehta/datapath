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
/** Unit test for the RemoteMessage.

    Behaves as both client and server. If no command line argument,
    run as server, otherwise as client (the argument is the machine name).

    Client Task: read from the keyboard an integer and send it through the
    network to an adder on the other side. When we send 0, we
    stop. This test mimics the EventGenerator test but goes through
    the network.

		Command line arguments:
		#1 name of the machine to connect to
*/

#include "CommConfig.h"
#include "KeyboardReader.h"
#include "CommSender.h"
#include "TestConf.h"
#include "Adder.h"
#include "Killer.h"
#include "CommunicationFramework.h"

#include <iostream>
#include "stdlib.h"


int main(int argc, char* argv[]){

	//start the communication framework
	StartCommunicationFramework();

	if (argc!=2){ // Server
		cout << "Running as server. If you want to run as client " << endl;
		cout << "Usage: client machine_name" << endl;

		// start the adder
		Adder adder;
		RegisterAsRemoteEventProcessor(adder, SERVICE_ADD);
		adder.ForkAndSpin(); // starts one independent thread for the adder

		// block until the server is killed
		WaitCommunicationFrameworkDeath();

		UnregisterRemoteEventProcessor(SERVICE_ADD);
		KillEvProc(adder);

		//sleep(1);
	} else { // Client
		// start the keyboard generator
		KeyboardReader reader(argv[1]);
		reader.Run();

		Killer killer;
		RegisterAsRemoteEventProcessor(killer, SERVICE_KILL);
		killer.ForkAndSpin(); // starts one independent thread for the killer

		// block until the client is killed
		WaitCommunicationFrameworkDeath();

		UnregisterRemoteEventProcessor(SERVICE_KILL);
		KillEvProc(killer);

		reader.Kill();

		//sleep(1);
	}

	//stop the communication framework
	StopCommunicationFramework();

	return 0;
}
