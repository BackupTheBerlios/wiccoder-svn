/*******************************************************************************
* file:         img_rgb.h                                                      *
* version:      0.0.1                                                          *
* author:       mice (mailto:my_mice@mail.ru, ICQ:332-292-380)                 *
* description:  not available                                                  *
* tests:        none                                                           *
*******************************************************************************/

#ifndef IMGS_IMG_RGB_INCLUDED
#define IMGS_IMG_RGB_INCLUDED

////////////////////////////////////////////////////////////////////////////////
// headers
#include <string>
#include <fstream>

#include "img.h"


////////////////////////////////////////////////////////////////////////////////
// imgs namespace
namespace imgs {


////////////////////////////////////////////////////////////////////////////////
// img_rgb class declaration
class img_rgb: public img {
public:
	// public types ------------------------------------------------------------
	#pragma pack(1)
	struct rgb24_t {
		unsigned char b;
		unsigned char g;
		unsigned char r;
	};
	#pragma pack()

	// public constants --------------------------------------------------------
    // public functions --------------------------------------------------------
	img_rgb();
	~img_rgb();

	int bpp() const { return _bpp; }
	int reset(const int w, const int h, const int bpp);

protected:
	// protected constants -----------------------------------------------------
	// protected functions -----------------------------------------------------

private:
	int _bpp;
};



}	// namespace imgs



#endif	// IMGS_IMG_RGB_INCLUDED
