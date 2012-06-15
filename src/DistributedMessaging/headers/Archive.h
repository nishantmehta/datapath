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
/**
	This header file contains macros to automate the generation of
	serialization/deserialization. All code should use macros defined
	here so that we can switch easily to a different archiver.
*/

#ifndef _ARCHIVE_H_
#define _ARCHIVE_H_

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/export.hpp>


/**
	Macro to define an archiver

	The macro is used as follows:

	ARCHIVER(ar){
	ar & ...
	}
*/

#define ARCHIVER(ar) \
template<class Archive>\
void serialize(Archive& ar, const unsigned int version)

#define ARCHIVER_REGISTER_TYPE(ar, className) \
	ar.register_type(static_cast<className*>(NULL));

#define ARCHIVER_CLASS_EXPORT(className) \
	BOOST_CLASS_EXPORT(className);


/** macro that injects code in the class to help archiver get access */
#define ARCHIVER_ACCESS_DECLARATION \
	friend class boost::serialization::access;

#define ARCHIVER_ABSTRACT_CLASS(className) \
	BOOST_SERIALIZATION_ASSUME_ABSTRACT(className);

#define ARCHIVER_BASE_CLASS(className) \
	boost::serialization::base_object<className>(*this)

//boost binary archives macros
#define BINARY_ARCHIVE_OUTPUT boost::archive::binary_oarchive
#define BINARY_ARCHIVE_INPUT boost::archive::binary_iarchive


#endif // _ARCHIVE_H_
