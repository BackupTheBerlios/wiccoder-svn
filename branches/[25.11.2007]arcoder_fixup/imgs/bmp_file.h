/*******************************************************************************
* file:         bmp_file.h                                                     *
* version:      0.0.1                                                          *
* author:       mice (mailto:my_mice@mail.ru, ICQ:332-292-380)                 *
* description:  not available                                                  *
* tests:        none                                                           *
*******************************************************************************/

#ifndef IMGS_BMP_FILE_INCLUDED
#define IMGS_BMP_FILE_INCLUDED

////////////////////////////////////////////////////////////////////////////////
// headers
#include <string>
#include <fstream>

#include "img_rgb.h"


////////////////////////////////////////////////////////////////////////////////
// imgs namespace
namespace imgs
{


////////////////////////////////////////////////////////////////////////////////
// bmp_file public declaration
int bmp_read(img_rgb &rgb, const std::string &filename);
int bmp_write(const img_rgb &rgb, const std::string &filename);

int bmp_read(img_rgb &rgb, std::istream &in);
int bmp_write(const img_rgb &rgb, std::ostream &out);



}	// namespace imgs



#endif	// IMGS_BMP_FILE_INCLUDED
