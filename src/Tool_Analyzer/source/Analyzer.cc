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
#include <lemon/arg_parser.h>
#include "SqliteSelector.h"

using namespace lemon;

int main(int argc, char **argv)
{
  // Initialize the argument parser
  lemon::ArgParser ap(argc, argv);

  std::string sender = "", receiver = "", msgType = "";
  double tsf = -1.0, tst=-1.0, dless = -1.0, dmore = -1.0;
  bool screen, all;
  int maxduration = 0, minduration =0;

  // Add the sender option
  ap.refOption("all", "Analyzing every message", all);

  // Add the sender option
  ap.refOption("s", "Module sending the message", sender);

  // Add the destination option
  ap.refOption("r", "Module receving the message", receiver);

  // Add a Message Type option
  ap.refOption("msgt", "Message Type of the message", msgType);

  // Add a Timestamp From option
  ap.refOption("tsf", "Messages passed from this time duration", tsf);

  // Add a Timestamp to option
  ap.refOption("tst", "Messages passed till this time duration", tst);

  // Add a less than duration option
  ap.refOption("dless", "Execution time of enclosed block less than given duration", dless);

  // Add a more than duration option
  ap.refOption("dmore", "Execution time of enclosed block more than given duration", dmore);

  ap.refOption("maxduration", "Maximum execution time", maxduration);

  ap.refOption("minduration", "Minimum execution time", minduration);

  // Add a display on screen option
  ap.refOption("screen", "Display data on screen", screen, false);



  // Bundle -gr* options into a group
  ap.optionGroup("main", "all")
	.optionGroup("main", "s")
    .optionGroup("main", "r")
    .optionGroup("main", "msgt")
    .optionGroup("main", "tsf")
    .optionGroup("main", "tst")
    .optionGroup("main", "dless")
    .optionGroup("main", "dmore")
    .optionGroup("main", "maxduration")
    .optionGroup("main", "minduration");

  // Set the group mandatory
  ap.mandatoryGroup("main");

  // Throw an exception when problems occurs. The default behavior is to
  // exit(1) on these cases, but this makes Valgrind falsely warn
  // about memory leaks.
  ap.throwOnProblems();

  // Perform the parsing process
  // (in case of any error it terminates the program)
  // The try {} construct is necessary only if the ap.trowOnProblems()
  // setting is in use.
  try {
    ap.parse();
  } catch (lemon::ArgParserException &) { return 1; }

  //Now call SqliteSelector
  SqliteSelector sqlSel;
  sqlSel.DataSelector(sender, receiver, msgType, tsf, tst, dless, dmore, maxduration, minduration, screen, all);

  return 0;
}
