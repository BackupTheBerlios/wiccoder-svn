/*******************************************************************************
* file:         rgb2yuv.h                                                      *
* version:      0.0.1                                                          *
* author:       mice (mailto:my_mice@mail.ru, ICQ:332-292-380)                 *
* description:  not available                                                  *
* tests:        none                                                           *
*******************************************************************************/

#ifndef IMGS_RGB2YUV_INCLUDED
#define IMGS_RGB2YUV_INCLUDED

////////////////////////////////////////////////////////////////////////////////
// headers
#include "img_rgb.h"
#include "img_yuv.h"


////////////////////////////////////////////////////////////////////////////////
// imgs namespace
namespace imgs {


////////////////////////////////////////////////////////////////////////////////
// rgb2yuv public declaration
int rgb2yuv(const img_rgb &rgb, img_yuv &yuv, const int frmt = img_yuv::YUV_FRMT_444);
int yuv2rgb(const img_yuv &yuv, img_rgb &rgb, const int bpp);



}	// namespace imgs



#endif	// IMGS_RGB2YUV_INCLUDED
