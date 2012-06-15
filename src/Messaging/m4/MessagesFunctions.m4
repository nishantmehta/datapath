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
dnl # This file contains functions used to generate the Message classes

include(Resources-T.m4)
m4_divert(-1)

m4_define(</HashName/>,</dnl
<//>m4_esyscmd(</aux=`echo -n $1 | md5sum | cut -c 1-16`; echo 0x${aux}ULL />) dnl
/>)dnl

dnl # HELPING MACROS
dnl # --------------

dnl # macro to extract the name of a column from a colulmn tuple
dnl # $1=column tuple of the form "(name, type)"
m4_define(</M4_COL_NAME/>,</m4_first($1)/>)

dnl # macro to extract the type of a column from a colulmn tuple
dnl # $1=column tuple of the form "(name, type)"
m4_define(</M4_COL_TYPE/>,</m4_second($1)/>)


dnl # In the following macro we use ALIST to form lists of arguments to functions
dnl # ALIST has to be undefined before the for loop forming it and undefined after use
dnl # The Autoconf people must have defined m4_append exactly for the purpose of forming these lists

dnl # this macro creates a new message type
dnl # $1=TypeName
dnl # $2=listCopyCostructed
dnl # $3=listSwapped
dnl # see the text in Message.h.m4 for the full usage explanation
dnl # the code looks weird to ensure nice formating of the C++ output
m4_define(</M4_CREATE_MESSAGE_TYPE/>,</dnl
dnl # we define some macros so we do not write redundant code
dnl # first we undefine these macros so we avoid interference
dnl
m4_ifdef(</ARGS_WITH_TYPES_/>, </m4_undefine(</ARGS_WITH_TYPES_/>)/>)dnl
m4_ifdef(</ARGS_NO_TYPES_/>, </m4_undefine(</ARGS_NO_TYPES_/>)/>)dnl
m4_ifdef(</COPY_ARGS_DEF/>, </m4_undefine(</COPY_ARGS_DEF/>)/>)dnl
dnl # now we define them
dnl # copy parameters first ($2)
m4_foreach(</_C_/>, </$2/>, </dnl
m4_append(</ARGS_WITH_TYPES_/>, M4_COL_TYPE(_C_) _<//>M4_COL_NAME(_C_),</</, />/>)dnl
m4_append(</ARGS_NO_TYPES_/>, _<//>M4_COL_NAME(_C_),</</, />/>)dnl
m4_append(</COPY_ARGS_DEF/>, M4_COL_NAME(_C_)(_<//>M4_COL_NAME(_C_)),</</, />/>)dnl
/>)dnl
m4_foreach(</_C_/>, </$3/>, </dnl
m4_append(</ARGS_WITH_TYPES_/>, M4_COL_TYPE(_C_)& _<//>M4_COL_NAME(_C_),</</, />/>)dnl
m4_append(</ARGS_NO_TYPES_/>, _<//>M4_COL_NAME(_C_),</</, />/>)dnl
/>)dnl

class $1 : public Message {
public:
  //members

dnl # go through the attributes and define them
m4_foreach(</_C_/>, </$2/>, </dnl
  M4_COL_TYPE(_C_) M4_COL_NAME(_C_);
/>)dnl
m4_foreach(</_C_/>, </$3/>, </dnl
  M4_COL_TYPE(_C_) M4_COL_NAME(_C_);
/>)dnl

private:
  //constructor
  $1(dnl
m4_ifdef(</ARGS_WITH_TYPES_/>,</ARGS_WITH_TYPES_/>, void)<//>dnl
 )dnl
m4_ifdef(</COPY_ARGS_DEF/>,:
    // copy constructed members
    </COPY_ARGS_DEF/>)
  {
    // swap members
m4_foreach(</_C_/>, </$3/>, </dnl
    M4_COL_NAME(_C_).swap(_<//>M4_COL_NAME(_C_));
/>)dnl
  }

m4_ifdef(</ARGS_WITH_TYPES_/>,</dnl
  // private default constructor so nobody can build this stuff
  $1(void);
/>, )
public:
	//destructor
	virtual ~$1() {}

  // type
  static const off_t type=HashName($1);
  virtual off_t Type(void){ return HashName($1); }
	virtual const char* TypeName(void){ return "$1"; }

  // friend declarations
m4_ifdef(</DEBUG_CLASS/>,</dnl
  friend class DEBUG_CLASS;
/>)dnl
  friend void $1_Factory(EventProcessor& dest dnl
m4_ifdef(</ARGS_WITH_TYPES_/>,</</,/>/></ARGS_WITH_TYPES_/>, ));

};

// Factory function to build $1
inline void $1_Factory(EventProcessor& dest dnl
m4_ifdef(</ARGS_WITH_TYPES_/>,</</,/>/></ARGS_WITH_TYPES_/>, )){
dnl
  Message* msg = new $1(dnl
m4_ifdef(</ARGS_NO_TYPES_/>,</ARGS_NO_TYPES_/>, ));
  dest.ProcessMessage(*msg);
}
/>)dnl

m4_divert(0)
dnl # if DEBUG_CLASS is defined, we do a forward definition
m4_ifdef(</DEBUG_CLASS/>,</
class DEBUG_CLASS;
/>)
