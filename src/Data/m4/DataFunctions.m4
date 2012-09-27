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
dnl
include(Resources-T.m4)
dnl before we deactivate the output, put a message that this file is generated
m4_divert(0)dnl
/* This file is generated from the corresponding .h.m4 file (look in m4/ directory)

   ANY MODIFICATIONS ARE WIPED OUT NEXT TIME maker RUNS OR CHANGES ARE MADE TO .h.m4 file. 
	 BETTER NOT TO TOUCH -- looking is encouraged, though
*/
m4_divert(-1)
m4_define(</HashName/>, m4_defn(</M4_HASH_NAME/>))
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

dnl # this macro creates a new base data type	      
dnl # a base data type can be used as the second argument for the next macro
dnl # $1=NewBaseType
dnl # $2=SubHierarchy (Data, DataC, other)
dnl # $3=listCopyCostructed
dnl # $4=listSwapped
dnl # the macro just defines the macros M4_nameBaseType_ListCopyConstructed and
dnl # M4_nameBaseType_ListSwapped and calls the class instantiation macro to do
dnl # the rest
m4_define(</M4_CREATE_BASE_DATA_TYPE/>,</dnl
dnl # we expect the macros with SubHierarchy to be already defined
dnl # put lists of base class in first
<//>m4_ifdef(</M4_$2_ListCopyConstructed/>, </dnl
<//><//>m4_define(</M4_$1_ListCopyConstructed/>, m4_quote(M4_$2_ListCopyConstructed))/>)dnl
<//>m4_ifdef(</M4_$2_ListSwapped/>, </dnl
<//><//>m4_define(</M4_$1_ListSwapped/>, m4_quote(M4_$2_ListSwapped))/>)dnl
<//>m4_ifdef(</M4_$2_ListArgsNoTypes/>, </dnl
<//><//>m4_define(</M4_$1_ListArgsNoTypes/>, m4_quote(M4_$2_ListArgsNoTypes))/>)dnl
<//>m4_ifdef(</M4_$2_ListArgsWithTypes/>, </dnl
<//><//>m4_define(</M4_$1_ListArgsWithTypes/>, m4_quote(M4_$2_ListArgsWithTypes))/>)dnl
dnl # add the current arguments
<//>m4_foreach(</_C_/>, </$3/>,</dnl
<//><//>m4_append(</M4_$1_ListCopyConstructed/>, _C_, </</, />/>)dnl
<//><//>m4_append(</M4_$1_ListArgsWithTypes/>, M4_COL_TYPE(_C_) const & _<//>M4_COL_NAME(_C_), </</, />/>)dnl
<//><//>m4_append(</M4_$1_ListArgsNoTypes/>, _<//>M4_COL_NAME(_C_), </</, />/>)dnl
<//>/>)dnl
<//>m4_foreach(</_C_/>, </$4/>,</dnl
<//><//>m4_append(</M4_$1_ListSwapped/>, _C_, </</, />/>)dnl
<//><//>m4_append(</M4_$1_ListArgsWithTypes/>, M4_COL_TYPE(_C_)& _<//>M4_COL_NAME(_C_), </</, />/>)dnl
<//><//>m4_append(</M4_$1_ListArgsNoTypes/>, _<//>M4_COL_NAME(_C_), </</, />/>)dnl
<//>/>)dnl
dnl # determine if the base is copyable. Devine M4_BaseName_Copyable if it is
<//>m4_ifdef(</M4_$2_Copyable/>, </m4_define(</M4_$1_Copyable/>)/>)dnl
dnl # call the instantiation macro
<//>M4_CREATE_DATA_TYPE($1, $2, </$3/>, </$4/>)dnl
/>)

dnl # declare DataC as being copyable so descendents are copyable
m4_define(</M4_DataC_Copyable/>)

dnl # this macro creates a new data type
dnl # $1=TypeName
dnl # $2=SubHierarchy (Data, DataC, other)
dnl # $3=listCopyCostructed
dnl # $4=listSwapped
dnl # see the text in Data.h for the full usage explanation
dnl # the code looks weird to ensure nice formating of the C++ output
m4_define(</M4_CREATE_DATA_TYPE/>,</dnl
dnl # we define some macros so we do not write redundant code
dnl # first we undefine these macros so we avoid interference
dnl
<//>m4_ifdef(</ARGS_WITH_TYPES_/>, </m4_undefine(</ARGS_WITH_TYPES_/>)/>)dnl
<//>m4_ifdef(</M4_$2_ListArgsWithTypes/>, </m4_define(</ARGS_WITH_TYPES_/>, m4_quote(M4_$2_ListArgsWithTypes))/>)
<//>m4_ifdef(</ARGS_NO_TYPES_/>, </m4_undefine(</ARGS_NO_TYPES_/>)/>)dnl
<//>m4_ifdef(</M4_$2_ListArgsNoTypes/>, </m4_define(</ARGS_NO_TYPES_/>, m4_quote(M4_$2_ListArgsNoTypes))/>)
<//>m4_ifdef(</COPY_ARGS_DEF/>, </m4_undefine(</COPY_ARGS_DEF/>)/>)dnl
dnl # now we define them
dnl # we first get data from 
dnl # copy parameters first ($3)
<//>m4_foreach(</_C_/>, </$3/>, </dnl
<//><//>m4_append(</ARGS_WITH_TYPES_/>, M4_COL_TYPE(_C_) const & _<//>M4_COL_NAME(_C_),</</, />/>)dnl
<//><//>m4_append(</ARGS_NO_TYPES_/>, _<//>M4_COL_NAME(_C_),</</, />/>)dnl
<//><//>m4_append(</COPY_ARGS_DEF/>, M4_COL_NAME(_C_)(_<//>M4_COL_NAME(_C_)),</</, />/>)dnl
<//>/>)dnl
<//>m4_foreach(</_C_/>, </$4/>, </dnl
<//><//>m4_append(</ARGS_WITH_TYPES_/>, M4_COL_TYPE(_C_)& _<//>M4_COL_NAME(_C_),</</, />/>)dnl
<//><//>m4_append(</ARGS_NO_TYPES_/>, _<//>M4_COL_NAME(_C_),</</, />/>)dnl
<//>/>)dnl

// forward definition
class $1;

/** Defining the implementation class first */
class $1<//>Imp : public $2<//>Imp {
protected:
  //members

dnl # go through the attributes and define them
dnl # only the new attributes
<//>m4_foreach(</_C_/>, </$3/>, </dnl
  M4_COL_TYPE(_C_) M4_COL_NAME(_C_);
<//>/>)dnl
<//>m4_foreach(</_C_/>, </$4/>, </dnl
  M4_COL_TYPE(_C_) M4_COL_NAME(_C_);
<//>/>)dnl

public:
  //constructor
  $1Imp(dnl
<//>m4_ifdef(</ARGS_WITH_TYPES_/>,</ARGS_WITH_TYPES_/>, void)<//>dnl
 ):$2Imp(dnl
<//>m4_ifdef(</M4_$2_ListArgsNoTypes/>, </M4_$2_ListArgsNoTypes/>, ) )dnl
m4_ifdef(</COPY_ARGS_DEF/>,</</,/>/>
    // copy constructed members
    </COPY_ARGS_DEF/>)
  {
    // swap members
m4_foreach(</_C_/>, </$4/>, </dnl
    M4_COL_NAME(_C_).swap(_<//>M4_COL_NAME(_C_));
/>)dnl
  }

dnl # do we need a Clone() method? 
<//>m4_ifdef(</M4_$2_Copyable/>,</dnl
  // Clone method. Can create a copy of the object
  DataImp* Clone(){
    // create copyes of all data

dnl # XXX M4_$1_ListCopyConstructed XXX
dnl # XXX M4_$2_ListCopyConstructed XXX

dnl # from base class first
<//>m4_ifdef(</M4_$2_ListCopyConstructed/>,</dnl
<//><//>m4_foreach(</_C_/>, M4_$2_ListCopyConstructed, </dnl
    M4_COL_TYPE(_C_) _<//>M4_COL_NAME(_C_)=M4_COL_NAME(_C_);
<//><//>/>)dnl
<//>/>)dnl
<//>m4_ifdef(</M4_$2_ListSwapped/>,</dnl
<//><//>m4_foreach(</_C_/>, M4_$2_ListSwapped, </dnl
    M4_COL_TYPE(_C_) _<//>M4_COL_NAME(_C_);
    _<//>M4_COL_NAME(_C_).copy(M4_COL_NAME(_C_));
<//><//>/>)dnl
<//>/>)dnl

dnl # now current class
<//>m4_foreach(</_C_/>, </$3/>, </dnl
    M4_COL_TYPE(_C_) _<//>M4_COL_NAME(_C_)=M4_COL_NAME(_C_);
<//>/>)dnl
<//>m4_foreach(</_C_/>, </$4/>, </dnl
    M4_COL_TYPE(_C_) _<//>M4_COL_NAME(_C_);
    _<//>M4_COL_NAME(_C_).copy(M4_COL_NAME(_C_));
<//>/>)dnl
		
    return new $1Imp(m4_ifdef(</ARGS_NO_TYPES_/>,</ARGS_NO_TYPES_/>, ));						
  }
/>)

  //destructor
  virtual ~$1Imp() {}

  // type
  virtual const off_t Type(void) const { return HashName($1);}
	virtual const char* TypeName(void) const { return "$1"; }

	friend class $1;

};

/* The front end class $1 now */

class $1 : public $2 {
public:
  // the type
  static const off_t type=HashName($1);

  //constructor
  $1(m4_ifdef(</ARGS_WITH_TYPES_/>,</ARGS_WITH_TYPES_/>, void)<//>){
	  data = new $1<//>Imp(m4_ifdef(</ARGS_NO_TYPES_/>,</ARGS_NO_TYPES_/>, ));
  }

dnl # we have to make sure we do not define default constructor if the above 
dnl # constructor has no arguments
m4_ifdef(</ARGS_WITH_TYPES_/>, </dnl
  // default constructor
	$1():$2(){}
/>)

  // access methods for all new data
<//>m4_foreach(</_C_/>, </$3/>, </dnl
  M4_COL_TYPE(_C_)& get_<//>M4_COL_NAME(_C_)(){ 
    $1Imp* myData = dynamic_cast<$1Imp*>(data);
    FATALIF(myData == NULL, "Trying to get member M4_COL_NAME(_C_) of an invalid or wrong type object");
    return myData->M4_COL_NAME(_C_); 
  }
<//>/>)dnl
<//>m4_foreach(</_C_/>, </$4/>, </dnl
  M4_COL_TYPE(_C_)& get_<//>M4_COL_NAME(_C_)(){ 
    $1Imp* myData = dynamic_cast<$1Imp*>(data);
    FATALIF(myData == NULL, "Trying to get member M4_COL_NAME(_C_) of an invalid or wrong type object");
    return myData->M4_COL_NAME(_C_); 
  }
<//>/>)dnl

	// Test if the object is what it claims to be
	bool IsValid(){
		return (dynamic_cast<$1<//>Imp*>(data) != NULL);
  }	

};

/>)dnl

dnl # macro to include in a file definitions of base data types (without class redefinition)
dnl # Arguments:
dnl # $1=filename
m4_define(</M4_INCLUDE_DATA_DEFS/>, </dnl
<//>dnl # deactivate temporarily M4_CREATE_DATA_TYPE macro so it does nothing
<//>m4_pushdef(</M4_CREATE_DATA_TYPE/>, )dnl
<//>m4_divert(-1) dnl to ensure we get no output
<//>m4_include($1)dnl
<//>m4_divert(0)
<//>m4_popdef(</M4_CREATE_DATA_TYPE/>)
/>)

m4_divert(0)dnl
