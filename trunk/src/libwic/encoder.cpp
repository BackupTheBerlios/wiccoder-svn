/*******************************************************************************
* file:         encoder.cpp                                                    *
* version:      0.1.0                                                          *
* author:       mice (mailto:my_mice@mail.ru, ICQ:332-292-380)                 *
* description:  not available                                                  *
* tests:        none                                                           *
*******************************************************************************/


////////////////////////////////////////////////////////////////////////////////
// include

// standard C++ headers
// none

// libwic headers
#include <wic/libwic/encoder.h>


////////////////////////////////////////////////////////////////////////////////
// wic namespace
namespace wic {


////////////////////////////////////////////////////////////////////////////////
// encoder class public definitions

/*!
*/
encoder::encoder() {
}


/*!
*/
encoder::~encoder() {
}


/*!
*/
void encoder::encode() {
	wtree tree(0, 0, 0);
	const sz_t lvl = tree.sb().lvls() + subbands::LVL_PREV;
	wtree_iterator 
}



////////////////////////////////////////////////////////////////////////////////
// wtree class protected definitions



}	// end of namespace wic
