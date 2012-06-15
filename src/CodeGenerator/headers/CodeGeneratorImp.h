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
// for Emacs -*- c++ -*-

#ifndef _CODEGENERATORIMP_H
#define _CODEGENERATORIMP_H

#include <iostream>
#include <fstream>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <dlfcn.h>

#include "WayPointCG.h"
#include "EventProcessor.h"
#include "EventProcessorImp.h"
#include "MessageMacros.h"
#include "Message.h"
#include "CGMessages.h"

using namespace std;

/** This is the code generator. It is a stateless class, as it does not
	* save any information about previously generated code -- that
	* information spreads through the system in the form of Function
	* objects. The functionality it provides is quite simple. All it does
	* is take a collection of symbolic configuration objects, generates
	* and compile the code and gives back the collection of functions and
	* modules in the form of waypoing configuration messages.
	*
	* The code generator is implemented as an event processor with only
	* one input message --a request for new code-- and one output
	* message with the code.
	*
	* This class is an implementation class. The interface class is
	* defined below.
**/
class CodeGeneratorImp: public EventProcessorImp {
private:
	const char* srcDir; // the source directory
	EventProcessor coordinator; // the coordinator (gets the message with the code)

	// Adds the preamble to the Main.m4 file
	void AddPreamble(ofstream &out);

	// Compiles the code, loads it and returns the module
	// objects is the list of .o files that need to be compiled
	void Build(string dirPath, void *&module, string objects);

	// process a new request for code!
	void Generate(string dirName, SymbolicWPConfigContainer &configMessages,
		WPConfDataContainer &generatedCode);

public:
	/** Need the path to the source tree so that we can compile code.
			Need to know where to send the generated message (coordinator)
	**/

	CodeGeneratorImp(const char* _srcDir, EventProcessor& _coordinator);
	virtual ~CodeGeneratorImp() {}

	// handler for the new message processing request
	MESSAGE_HANDLER_DECLARATION(GenerateNewCode);

	// handler for a DIE message
	MESSAGE_HANDLER_DECLARATION(Die);
};

#endif // _CODEGENERATORIMP_H
