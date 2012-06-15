dnl #
dnl #  Copyright 2012 Alin Dobra and Christopher Jermaine
dnl #
dnl #  Licensed under the Apache License, Version 2.0 (the "License");
dnl #  you may not use this file except in compliance with the License.
dnl #  You may obtain a copy of the License at
dnl #
dnl #      http://www.apache.org/licenses/LICENSE-2.0
dnl #
dnl #  Unless required by applicable law or agreed to in writing, software
dnl #  distributed under the License is distributed on an "AS IS" BASIS,
dnl #  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
dnl #  See the License for the specific language governing permissions and
dnl #  limitations under the License.
dnl #
dnl This file describes the messages needed only in this test

///////////// TEST GENERATOR MESSAGE  ////////////

/** Message used in the EventProcessor unit test. 
    It is send by the Generator to the Adder
    
    Arguments:
	number: the number to be added
	sender: what adder sent the message	

    */

M4_CREATE_MESSAGE_TYPE(TestGeneratorMessage,
  </(number, long int)/>,
  </(sender, EventProcessor)/>)

///////////// TEST NEXT MESSAGE ///////////

/** Message send by the Adder to signal that a new number can be generated 

    Arguments: NONE
*/
M4_CREATE_MESSAGE_TYPE(TestNextMessage,
  <//>,<//>)
