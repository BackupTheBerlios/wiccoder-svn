/*******************************************************************************
* file:         bmp_file.cpp                                                   *
* version:      0.0.1                                                          *
* author:       mice (mailto:my_mice@mail.ru, ICQ:332-292-380)                 *
* description:  not available                                                  *
* tests:        none                                                           *
*******************************************************************************/


////////////////////////////////////////////////////////////////////////////////
// headers
#include "bmp_file.h"


////////////////////////////////////////////////////////////////////////////////
// imgs namespace
namespace imgs {


////////////////////////////////////////////////////////////////////////////////
// bmp_file private definition
static const WORD _type_BM		= 0x4d42;


////////////////////////////////////////////////////////////////////////////////
// bmp_file public definition
int bmp_read(img_rgb &rgb, const std::string &filename) {
	std::ifstream fi(filename.c_str(), std::ios_base::binary);
	if (!fi.good()) return -1;

	return bmp_read(rgb, fi);
}


int bmp_write(const img_rgb &rgb, const std::string &filename) {
	std::ofstream fo(filename.c_str(), std::ios_base::binary);
	if (!fo.good()) return -1;

	return bmp_write(rgb, fo);
}


int bmp_read(img_rgb &rgb, std::istream &in) {
	BITMAPFILEHEADER bmp_fh;
	in.read((char *)&bmp_fh, sizeof(bmp_fh));
	if (!in.good()) return -1;

	if (_type_BM != bmp_fh.bfType) return -1;

	BITMAPINFOHEADER bmp_ih;
	in.read((char *)&bmp_ih, sizeof(bmp_ih));
	if (!in.good()) return -1;

	in.seekg(bmp_fh.bfOffBits);
	if (!in.good()) return -1;

	if (0 != rgb.reset(bmp_ih.biWidth, bmp_ih.biHeight, bmp_ih.biBitCount)) return -1;

	in.read((char *)rgb.bits(), (std::streamsize)rgb.bits_sz());
	if (!in.good()) return -1;

	return 0;
}


int bmp_write(const img_rgb &rgb, std::ostream &out) {
	BITMAPINFOHEADER bmp_ih;
	bmp_ih.biSize = sizeof(bmp_ih);
	bmp_ih.biWidth = rgb.w();
	bmp_ih.biHeight = rgb.h();
	bmp_ih.biPlanes = 1;
	bmp_ih.biBitCount = rgb.bpp();
	bmp_ih.biCompression = BI_RGB;
	bmp_ih.biSizeImage = 0;
	bmp_ih.biXPelsPerMeter = 0;
	bmp_ih.biYPelsPerMeter = 0;
	bmp_ih.biClrUsed = 0;
	bmp_ih.biClrImportant = 0;

	BITMAPFILEHEADER bmp_fh;
	bmp_fh.bfSize = sizeof(bmp_fh);
	bmp_fh.bfType = _type_BM;
	bmp_fh.bfReserved1 = bmp_fh.bfReserved2 = 0;
	bmp_fh.bfOffBits = bmp_fh.bfSize + bmp_ih.biSize;

	out.write((char *)&bmp_fh, bmp_fh.bfSize);
	if (!out.good()) return -1;

	out.write((char *)&bmp_ih, bmp_ih.biSize);
	if (!out.good()) return -1;

	out.write((char *)rgb.bits(), (std::streamsize)rgb.bits_sz());
	if (!out.good()) return -1;

	return 0;
}



}	// namespace imgs
