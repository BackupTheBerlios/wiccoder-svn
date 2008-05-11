/*******************************************************************************
* file:         img.cpp                                                        *
* version:      0.0.1                                                          *
* author:       mice (mailto:my_mice@mail.ru, ICQ:332-292-380)                 *
* description:  not available                                                  *
* tests:        none                                                           *
*******************************************************************************/


////////////////////////////////////////////////////////////////////////////////
// headers
#include "img.h"


////////////////////////////////////////////////////////////////////////////////
// imgs namespace
namespace imgs {


////////////////////////////////////////////////////////////////////////////////
// img class public definition
img::img() {
	_reset(true);
}


img::~img() {
	_reset();
}


////////////////////////////////////////////////////////////////////////////////
// img class protected definition
size_t img::_calc_sz(const int w, const int h, const int bpp) {
	if (0 >= w || 0 >= h || 0 >= bpp) return 0;

	return (w * h * bpp + bpp % _BITS_IN_BYTE) / _BITS_IN_BYTE;
}


void *img::_alloc_bits(const size_t bits_sz, void *const bits) {
	return realloc(bits, bits_sz);
}


void img::_free_bits(void *const bits) {
	free(bits);
}


void img::_reset(const bool init) {
	_w = 0;
	_h = 0;

	if (!init) _free_bits(_bits);
	_bits = NULL;
	_bits_sz = 0;
}


int img::_reset(const int w, const int h, const size_t bits_sz) {
	_w = w;
	_h = h;

	if (0 >= bits_sz) {
		_free_bits(_bits);
		_bits = NULL;
		_bits_sz = 0;
	} else if (_bits_sz != bits_sz) {
		void *const nbits = _alloc_bits(bits_sz, _bits);
		if (NULL == nbits) return -1;

		_bits = nbits;
		_bits_sz = bits_sz;
	}

	return 0;
}


void img::_set_bits(void *const bits, const size_t bits_sz) {
	_bits = bits;
	_bits_sz = bits_sz;
}


void img::_ch_bits(void *const bits, const size_t bits_sz) {
	_free_bits(_bits);
	_bits = NULL;
	_bits_sz = 0;

	_set_bits(bits, bits_sz);
}



}	// namespace imgs
