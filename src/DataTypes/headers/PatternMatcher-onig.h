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
#ifndef P_MATCH_OING
#define P_MATCH_OING

#include "HString.h"
#include <string>
#include <oniguruma.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>

using namespace std;

/** Onig pattern matcher. It is really fast but it needs mutex to protect creation and destruction */

class PatternMatcher {

private:
  regex_t* reg; // teh compiler regex
  static pthread_mutex_t mutex; // the mutex that protects the creation/destruction

public:

  // Dynamic xpressive
  PatternMatcher(string regexp);

  // match function
  bool IsMatch (const HString& str);

  // destructor
  ~PatternMatcher(void);
};

// the weak attribute shoudl ensure that only one of the copies linked
pthread_mutex_t  PatternMatcher::mutex __attribute__((weak)) =  PTHREAD_MUTEX_INITIALIZER;

inline
PatternMatcher :: PatternMatcher (string regexp) {


  UChar* pattern = (UChar*) regexp.c_str();
  OnigErrorInfo einfo;

  pthread_mutex_lock(&mutex);
  int r = onig_new(&reg, pattern, pattern + strlen((char* )pattern),
          ONIG_OPTION_DEFAULT, ONIG_ENCODING_ASCII, ONIG_SYNTAX_DEFAULT, &einfo);

  if (r != ONIG_NORMAL) {
    char s[ONIG_MAX_ERROR_MESSAGE_LEN];
    onig_error_code_to_str((UChar*)s, r, &einfo);
    FATAL( "OING PATTERN MATCHER ERROR: %s\n", s);
  }
  pthread_mutex_unlock(&mutex);
}

inline
bool PatternMatcher :: IsMatch (const HString& str) {
  const char* target = (const char*) str;
  // we can now safely get the size
  int size = strlen(target); // str.GetStrSize();

  return (onig_match(reg, (UChar*)target, (UChar*)target+size, (UChar*)target, NULL, ONIG_OPTION_NONE) >=0);

}

inline PatternMatcher::~PatternMatcher(void){
  pthread_mutex_lock(&mutex);
  onig_free(reg);
  onig_end();
  pthread_mutex_unlock(&mutex);
}

#endif // P_MATCH_OING
