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

/*!	\param[in]
*/
encoder::encoder(const w_t *const image,
				 const sz_t width, const sz_t height, const sz_t lvls)
{
	
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
}



////////////////////////////////////////////////////////////////////////////////
// wtree class protected definitions



}	// end of namespace wic
