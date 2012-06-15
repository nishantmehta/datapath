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

include(MessagesFunctions.m4)
m4_divert_push(-1)

dnl # this macro creates a new remote message type
dnl # $1=TypeName
dnl # $2=listCopyCostructed
dnl # $3=listSwapped
dnl # see the text in Message.h.m4 for the full usage explanation
dnl # the code looks weird to ensure nice formating of the C++ output
m4_define(</M4_CREATE_REMOTE_MESSAGE_TYPE/>,</dnl
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

class $1 : public RemoteMessage {
public:
  ARCHIVER_ACCESS_DECLARATION;

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

  //constructor with the extra parameter typeIndicator
  $1(dnl
m4_ifdef(</ARGS_WITH_TYPES_/>,</ARGS_WITH_TYPES_/></</, />/>)<//>dnl
bool _typeIndicator )dnl
 : m4_ifdef(</COPY_ARGS_DEF/>,
    // copy constructed members
    </COPY_ARGS_DEF/></</, />/>) RemoteMessage(_typeIndicator)
  {
    // swap members
m4_foreach(</_C_/>, </$3/>, </dnl
    M4_COL_NAME(_C_).swap(_<//>M4_COL_NAME(_C_));
/>)dnl
  }

public:

dnl # if we have no arguments, we do not need this constructor
m4_ifdef(</ARGS_WITH_TYPES_/>, dnl
  //empty constructor required for deserialization
  $1()dnl
    : RemoteMessage()dnl
  { }
,)<//>dnl

	virtual ~$1() {}

  // type
  static const off_t type=HashName($1);
  virtual off_t Type(void){ if (typeIndicator == true) return HashName($1); else return RemoteMessage::Type();}
  virtual const char* TypeName(void){ return "$1"; }

  // archiver
  ARCHIVER(ar){
    ar & ARCHIVER_BASE_CLASS(RemoteMessage) dnl
<//>m4_foreach(</_C_/>, </$2/>, </ & M4_COL_NAME(_C_) />)dnl
<//>m4_foreach(</_C_/>, </$3/>, </ & M4_COL_NAME(_C_) />)dnl
;
  }

  // friend declarations
m4_ifdef(</DEBUG_CLASS/>,</dnl
  friend class DEBUG_CLASS;
/>)dnl
  friend void $1_Factory(EventProcessor& dest dnl
m4_ifdef(</ARGS_WITH_TYPES_/>,</</,/>/></ARGS_WITH_TYPES_/>, ));

};

//export class for serializer
ARCHIVER_CLASS_EXPORT($1)

// Factory function to build $1
inline void $1_Factory(EventProcessor& dest dnl
m4_ifdef(</ARGS_WITH_TYPES_/>,</</,/>/></ARGS_WITH_TYPES_/>, )){
dnl
  Message* msg = new $1(dnl
m4_ifdef(</ARGS_NO_TYPES_/>,</ARGS_NO_TYPES_/></</, />/>) false );
  dest.ProcessMessage(*msg);
}
/>)dnl

m4_divert(0)
