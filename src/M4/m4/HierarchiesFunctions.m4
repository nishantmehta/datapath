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
include(Resources-T.m4)
m4_divert(-1)

# This program contains tools to automate the 
# process of writing interface classes that provide
# Swapping and Copying facility

# Code is automatically generated to remove boringness
# out of code writing

# The goal of these macros is to generate nice C++ code
# that looks hand-written

# The way these macros can be used is the following
# 
# M4_INTERFACE_CLASS(ClassName, BaseName, pointerVariable)
#   M4_INTERFACE_CONSTRUCTOR(listArguments)
#   M4_INTERFACE_DEFAULT_CONSTRUCTOR
#   M4_INTERFACE_FUNCTION(FctName, ReturnType, listArguments)
# M4_INTERFACE_CLASS_END
# 
# As in the rest of the M4 programs, litArguments is of the form
# </(parameterName, parameterType), ../>
# For example
#   M4_INTERFACE_CONSTRUCTOR(</(a,int), (b,A&))
# defines the constructor
#   ClassName(int a, A& b)
# 
# NOTE ON CONSTRUCTORS
#   M4_INTERFACE_DEFAULT_CONSTRUCTOR build a special type
#   of constructor in which the object is not created
#   and the pointer "pointerVariable" is set to NULL
#   The destructor of the base class always checks this.
#   
#   If a constructor with no arguments is needed, use
#   the macro M4_INTERFACE_CONSTRUCTOR()
#

dnl # HELPING MACROS
dnl # --------------

dnl # macro to extract the name of a column from a colulmn tuple
dnl # $1=column tuple of the form "(name, type)"
m4_define(</M4_COL_NAME/>,</m4_first($1)/>)

dnl # macro to extract the type of a column from a colulmn tuple
dnl # $1=column tuple of the form "(name, type)"
m4_define(</M4_COL_TYPE/>,</m4_second($1)/>)

dnl # macro to define the name of the class
dnl # $1=ClassName
dnl # $2=Base Class Name (of the Interface hierarchy)
dnl # $3=pointerVarable that holds the pointer to the implementation
m4_define(</M4_INTERFACE_CLASS/>,</dnl
m4_define(</M4_POINTER_VAR/>,$3)dnl
m4_define(</M4_CLASS_NAME/>,$1)dnl
// include the base class definition
</#/>include "$2.h"

// include the implementation definition
</#/>include "M4_CLASS_NAME<//>Imp.h"

/** Class to provide an interface to $1<//>Imp class.

    See $1<//>Imp.h for a description of the functions 
    and behavior of the class
*/
class $1 : public $2 {
public:
/>)

dnl # macro to end the class definition
dnl # no arguments
m4_define(</M4_INTERFACE_CLASS_END/>,</dnl

  // the virtual destructor
  virtual ~M4_CLASS_NAME<//>(){}
};
m4_undefine(</M4_POINTER_VAR/>)dnl
m4_undefine(</M4_CLASS_NAME/>)dnl
/>)

dnl # macro to define a default constructor
dnl # no argument
m4_define(</M4_INTERFACE_DEFAULT_CONSTRUCTOR/>,</dnl

  // default constructor
  M4_CLASS_NAME<//>(void){
dnl    delete M4_POINTER_VAR;
    M4_POINTER_VAR = NULL;
  }
/>)

dnl # macro to define a constructor
dnl # $1=listTypedArguments
m4_define(</M4_INTERFACE_CONSTRUCTOR/>,</dnl
m4_ifdef(</ARGS_WITH_TYPES_/>, </m4_undefine(</ARGS_WITH_TYPES_/>)/>)dnl
m4_ifdef(</ARGS_NO_TYPES_/>, </m4_undefine(</ARGS_NO_TYPES_/>)/>)dnl
m4_foreach(</_C_/>, </$1/>, </dnl
m4_append(</ARGS_WITH_TYPES_/>, M4_COL_TYPE(_C_) _<//>M4_COL_NAME(_C_),</</, />/>)dnl
m4_append(</ARGS_NO_TYPES_/>, _<//>M4_COL_NAME(_C_),</</, />/>)dnl
/>)dnl We have now the arguments, we write the function
m4_ifndef(</ARGS_WITH_TYPES_/>, </m4_define(</ARGS_WITH_TYPES_/>,void)/>)dnl
m4_ifndef(</ARGS_NO_TYPES_/>, </m4_define(</ARGS_NO_TYPES_/>,<//>)/>)dnl

  // constructor (creates the implementation object)
  M4_CLASS_NAME<//>(ARGS_WITH_TYPES_){
dnl    delete M4_POINTER_VAR;
    M4_POINTER_VAR = new M4_CLASS_NAME<//>Imp(ARGS_NO_TYPES_);
  }
/>)

dnl # macro to define a function
dnl # $1=FunctionName
dnl # $2=ReturnType (can be void)
dnl # $3=listTypedArguments
m4_define(</M4_INTERFACE_FUNCTION/>,</dnl
m4_ifdef(</ARGS_WITH_TYPES_/>, </m4_undefine(</ARGS_WITH_TYPES_/>)/>)dnl
m4_ifdef(</ARGS_NO_TYPES_/>, </m4_undefine(</ARGS_NO_TYPES_/>)/>)dnl
m4_foreach(</_C_/>, </$3/>, </dnl
m4_append(</ARGS_WITH_TYPES_/>, M4_COL_TYPE(_C_) _<//>M4_COL_NAME(_C_),</</, />/>)dnl
m4_append(</ARGS_NO_TYPES_/>, _<//>M4_COL_NAME(_C_),</</, />/>)dnl
/>)dnl We have now the arguments, we write the function
m4_ifndef(</ARGS_WITH_TYPES_/>, </m4_define(</ARGS_WITH_TYPES_/>,void)/>)dnl
m4_ifndef(</ARGS_NO_TYPES_/>, </m4_define(</ARGS_NO_TYPES_/>,<//>)/>)dnl

  $2 $1<//>(ARGS_WITH_TYPES_){
    M4_CLASS_NAME<//>Imp& obj = dynamic_cast<M4_CLASS_NAME<//>Imp&>(*M4_POINTER_VAR);
m4_case(</$2/>, void,</dnl
    obj.$1(ARGS_NO_TYPES_);
/>, </dnl
    return obj.$1(ARGS_NO_TYPES_);
/>)dnl
  }
/>)

m4_divert(0)dnl
