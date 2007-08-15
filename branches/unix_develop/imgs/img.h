/*******************************************************************************
* file:         img.h                                                          *
* version:      0.0.1                                                          *
* author:       mice (mailto:my_mice@mail.ru, ICQ:332-292-380)                 *
* description:  not available                                                  *
* tests:        none                                                           *
*******************************************************************************/

#ifndef IMGS_IMG_INCLUDED
#define IMGS_IMG_INCLUDED

////////////////////////////////////////////////////////////////////////////////
// headers
#include <stdlib.h>
#include <malloc.h>


////////////////////////////////////////////////////////////////////////////////
// imgs namespace
namespace imgs {


////////////////////////////////////////////////////////////////////////////////
// img class declaration
class img {
public:
	// public types ------------------------------------------------------------
	// public constants --------------------------------------------------------
    // public functions --------------------------------------------------------
	img();
	~img();

	int w() const { return _w; }
	int h() const { return _h; }
	void *bits() const { return _get_bits(); }
	size_t bits_sz() const { return _bits_sz; }

protected:
	// protected constants -----------------------------------------------------
	static const int _BITS_IN_BYTE	= 8;

	// protected functions -----------------------------------------------------
	static size_t _calc_sz(const int w, const int h, const int bpp);
	void *_alloc_bits(const size_t bits_sz, void *const bits = NULL);
	void _free_bits(void *const bits);

	void _reset(const bool init = false);
	int _reset(const int w, const int h, const size_t bits_sz);

	void *_get_bits() const { return _bits; }
	void _set_bits(void *const bits, const size_t bits_sz);
	void _ch_bits(void *const bits, const size_t bits_sz);

private:
	int _w;
	int _h;

	void *_bits;
	size_t _bits_sz;
};



}	// namespace imgs



#endif	// IMGS_IMG_INCLUDED
