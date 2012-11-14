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

#ifndef _PATTERN_MATCHER_ONIG_H_
#define _PATTERN_MATCHER_ONIG_H_

#include <string>
#include <string.h>
#include "Errors.h"

#include <pthread.h>

#include "base/Types/STRING_LITERAL.h"

#include <oniguruma.h>

/** Meta-information
 *  GF_DESC
 *      NAME(</PatternMatcherOnig/>)
 *      INPUTS(</(target, STRING_LITERAL)/>)
 *      CONSTRUCTOR(</(regexp, STRING_LITERAL)/>)
 *
 *      LIBS(onig)
 *  END_DESC
 */

class PatternMatcherOnig {

    regex_t* reg;       // The compiled regular expression


    static pthread_mutex_t mutex __attribute__ ((weak));

public:

    // Constructor
    PatternMatcherOnig( STRING_LITERAL regexp ) {
      /* Onig NEEDS to run in single thread mode for the construction
	 of the regular expression. Failure to do so results in weird
	 bugs.

      */

        UChar * pattern = (UChar *) regexp;
        OnigErrorInfo einfo;

	pthread_mutex_lock(&mutex);
        int r = onig_new(&reg, pattern, pattern + strlen(regexp),
                ONIG_OPTION_DEFAULT, ONIG_ENCODING_UTF8, ONIG_SYNTAX_DEFAULT, &einfo);
	pthread_mutex_unlock(&mutex);

        if( r != ONIG_NORMAL ) {
            char s[ONIG_MAX_ERROR_MESSAGE_LEN];
            onig_error_code_to_str((UChar*) s, r, &einfo);
            FATAL( "Onig pattern matcher error: %s\n", s);
        }
    }

    // filtration function
    bool Filter( STRING_LITERAL target ) {
        int size = strlen(target);

        int r = onig_match( reg, (UChar *) target, (UChar *) target + size, (UChar *) target,

                NULL, ONIG_OPTION_NONE);

        return r >= 0;
    }

    // Destructor
    ~PatternMatcherOnig(void) {
        onig_free(reg);
    }
};

pthread_mutex_t PatternMatcherOnig::mutex = PTHREAD_MUTEX_INITIALIZER;

#endif // _PATTERN_MATCHER_ONIG_H_
