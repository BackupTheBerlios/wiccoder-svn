/*******************************************************************************
* file:         img_rgb.cpp                                                    *
* version:      0.0.1                                                          *
* author:       mice (mailto:my_mice@mail.ru, ICQ:332-292-380)                 *
* description:  not available                                                  *
* tests:        none                                                           *
*******************************************************************************/


////////////////////////////////////////////////////////////////////////////////
// headers
#include "img_rgb.h"


////////////////////////////////////////////////////////////////////////////////
// imgs namespace
namespace imgs {


////////////////////////////////////////////////////////////////////////////////
// img_rgb class public definition
img_rgb::img_rgb() {
	_bpp = 0;
}


img_rgb::~img_rgb() {
}


int img_rgb::reset(const int w, const int h, const int bpp) {
	_bpp = bpp;

	const size_t nsz = _calc_sz(w, h, bpp);

	return _reset(w, h, nsz);
}


////////////////////////////////////////////////////////////////////////////////
// img_rgb class protected definition



}	// namespace imgs
