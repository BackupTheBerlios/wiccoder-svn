/*******************************************************************************
* file:         codec.cpp                                                      *
* version:      0.1.0                                                          *
* author:       mice (mailto:my_mice@mail.ru, ICQ:332-292-380)                 *
* description:  not available                                                  *
* tests:        none                                                           *
*******************************************************************************/


////////////////////////////////////////////////////////////////////////////////
// include

// libwic headers
#include <wic/libwic/codec.h>
#include <wic/libwic/encoder.h>


////////////////////////////////////////////////////////////////////////////////
// wic namespace
namespace wic {


////////////////////////////////////////////////////////////////////////////////
// encode and decode functions definition

/*!	\param[in] in Data to encode and encode options
	\param[in] out Resulting data and information
*/
bool encode(const encode_in_t &in, encode_out_t &out) {
	//wic::encoder encoder(in.image, in.width, in.height, in.lvls);

	//encoder.encode(0);

	return false;
}


/*!	\param[in] in Data to decode and decode options
	\param[in] out Resulting data and information
*/
bool decode(const decode_in_t &in, decode_out_t &out) {
	return false;
}



}	// end of namespace wic
