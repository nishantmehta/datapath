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
#include "TS_Average.h"

/*
  This file is a test file to use TS_Average.h 

  This file compiles (and executes)

  Problems: - I am not sure if AddState is called in the right way
            - I don't understand how to use getNextResult (added some comments below)

  General question: it looks like this GLA has some code included to compute the average. 
  How would we use GLAs if we have multiple things to compute, like:
  - compute some difference versus earlier GLA (previous second)
  - do a regression 
  


 */

//compile as: g++ TimeSeriesTest.cpp -o TimeSeriesTest -I../DataTypes/headers -I../Global/headers



int main() 
{ 



  DATETIME   dateStart=0 ; // I did not find a way to set this to some date 
  DATETIME  dateEnd=0 ;

  using namespace std; // make definitions visible
 

  // create TS object

  TS_Average TS(1, dateStart, dateEnd); // this is the overall GLA
  TS_Average TS1(1, dateStart, dateEnd); // GLA on Data1
  TS_Average TS2(1, dateStart, dateEnd); // GLA on Data2

  // fill it with some numbers. Data1
  for (int n=1; n<=10; n++) {
    double dRand = rand();
    TS.AddItem(dateStart, dRand);
		TS1.AddItem(dateStart, dRand);
    cout << n << ": "<< dRand << endl;
  }

  // fill it with some numbers. Data1
  for (int n=1; n<=10; n++) {
    double dRand = rand();
    TS.AddItem(dateStart, dRand);
		TS2.AddItem(dateStart, dRand);
    cout << n << ": "<< dRand << endl;
  }

  // addState (I suppose I call it once) (this looks kind of weird; i 
  // tried passing TS as '&TS' but that gave an error)

	//Add TS2 to TS1 to join them
	TS1.AddState(TS2);

	// Compare TS and TS1. They should be identical

   // get values
   /*
     I am not sure how to call GetNextResult - it looks like I define variables count, sum, avg
     that get passed by reference so these will be updated

     There is a 'someGLA' definition in TS_Average.h I don't understand where that is coming from.
    */

   INT granularity = 1;
   BIGINT count = 0;
   DOUBLE sum = 0;
   DOUBLE avg = 0;

#define PRINT_TS(x) \
   while (x.GetNextResult(dateStart, granularity, count, sum, avg)){	\
		 cout << "count: " << count << ", avg: " << avg << endl;					\
	 }

	 PRINT_TS(TS);
	 PRINT_TS(TS1);

#undefine PRINT_TS

  return 0; 
};
