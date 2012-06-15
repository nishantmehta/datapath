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
#ifndef _DATA_H_
#define _DATA_H_

#include <sys/types.h>
#include <stddef.h>

#include "Swap.h"
#include "ID.h"
#include "Errors.h"

/**
   This header file contains the definition of the Data hierachy
   
   The code contains a base class, a macro to create derived classes
   and the instantiations of the macro for all types of messages needed.
   
   To define a new type of data, the data creation macro has to be
   called in the following manner:
   
   M4_CREATE_DATA_TYPE(TypeName, BaseDataType, listCopyCostructed, listSwapped)
   
   where the each of the lists looks like:
   
   </</ (att_name, att_type), .../>/>
   
   The base data type has to be one of: Data, DataC, other bases declared with
   M4_CREATE_BASE_DATA_TYPE(NewBaseDataType, BaseDataType, listCopyCostructed, listSwapped)
   with BaseDataType any base type already defined.
 

   The attributes in the CopyConstructed list are copied in using assignment,
   the attributes in the swapped list are swaped in using the swap() method.
   Copying is done using = for CopyConstructed and copy() for swapped.
   
   Hopefully only basic types are copied in, the rest are swapped in.
   
   The macro will create a class, called TypeName that has members
   called as the attributes specified that are directly accessible.
   A constructor that insantiates all the attributes in the order
   specified is created.
   
   The method Type() is automatically created and will return the numeric type
   associated with the message. The static int TypeName::type is also defined
   as a public constant member that allows the numeric type to be retrieved.

   DataType::type is a static that can be used to check if an
   object has a particular type.
   
   The output should look like a hand generated C++ code

   NOTE: all the copy constructed members are passed by value and all
   the swapped members by reference (for efficiency).
   
*/

// Macro to stremline the testing of Data types
// Arguments:
//  obj is an object derived from Data 
//  TYPE is a type of a non-base derived class
#define CHECK_DATA_TYPE(obj, TYPE) ((obj).Type() == TYPE::type)

////////////////////////////////////////////////
// INCLUDES OF DATATYPES USED BY THE DATAS
/**
   The following lines should be uncommented if a debug class is used
   the name of the class shold replace MessageDebug
   define(DEBUG_CLASS, MessageDebug)
*/

//abstract message type definition
#define ABSTRACT_DATA_TYPE -1

/**
   Base message class for all data types. Data is used to transport
   information throught the system.

   Provides no functionality, just a basic type for messages. Allows only
   determining the type as a integer constant to allow conversion to the
   real type.
   
   When functionality is added to deal with another type of message,
   this class should be inherited from. The derived type can have as
   much intrnal structure and as many methods as desired. They will
   be accessible when the conversion to the true type is performed
   using dynamic_cast.
   
   For each derived class the static member type and the function Type()
   give the type as an off_t.
*/
class DataImp {
public:
  // constructor doing nothing
  DataImp() {}
  
  virtual ~DataImp() {}
  
  // Every new message type should redefine this
  virtual const off_t Type(void) const { return ABSTRACT_DATA_TYPE; }
  virtual const char* TypeName(void) const { return "Data"; }
};

/** internal Copyable Data */
class DataCImp : public DataImp{
public:
  
  // derived class must implement Clone to alow copy() in Data
  virtual DataImp* Clone(){ return NULL; }
  
  virtual ~DataCImp(){}
  virtual const off_t Type(void) const { return ABSTRACT_DATA_TYPE; }
  virtual const char* TypeName(void) const { return "DataC"; }
  
};

/** Front end class so usage is simple ans swapping paradigm works 
    
    For each derived concept, both classes have to be generated. Only
    the class derived from this is visible outside.
*/

class Data {
protected:
  DataImp* data; // pointer to data. Class behaves like a smart pointer

public:
	// the type
	static const off_t type = ABSTRACT_DATA_TYPE;

  // default constructor
  Data(){ data=NULL; }
  
  // destructor
  ~Data(){ if (data!=NULL) delete data; }

	// check if it is valid 
	bool IsValid(){ return data!=NULL; }

	// remove existing data
	void Kill(){ delete data; }

	// get the type
	const off_t Type() const { return (data==NULL)?ABSTRACT_DATA_TYPE:data->Type(); }
  
	const char *TypeName() const { return (data==NULL)?"Abstract Data Type":data->TypeName(); }

  // Swapping paradigm methods
  void swap(Data& who);
};

/** Front end class for all copyable data types 
 */
class DataC : public Data {
public:
  // default constructor
  DataC():Data(){}

  /** copy mehod. Needs clone in data */
  void copy(Data& who);
  
};

//////////// INLINE METHODS /////////////
inline void Data::swap(Data& who){
  SWAP(data, who.data);
}

inline void DataC::copy(Data& who){
  // get rid of our payload
	Kill();

  // now ask who to create a new data
  if ( who.IsValid()){
		DataC aux;
		who.swap(aux); // temporarily steal the content
		DataCImp* dataC = dynamic_cast<DataCImp*>(aux.data);
    data = dataC->Clone();
		who.swap(aux); // put content back
  }  
}


#endif //  _DATA_H_
