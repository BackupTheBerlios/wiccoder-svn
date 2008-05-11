/*******************************************************************************
* file:         img_yuv.h                                                      *
* version:      0.0.1                                                          *
* author:       mice (mailto:my_mice@mail.ru, ICQ:332-292-380)                 *
* description:  not available                                                  *
* tests:        none                                                           *
*******************************************************************************/

#ifndef IMGS_IMG_YUV_INCLUDED
#define IMGS_IMG_YUV_INCLUDED

////////////////////////////////////////////////////////////////////////////////
// headers
#include "img.h"


////////////////////////////////////////////////////////////////////////////////
// imgs namespace
namespace imgs {


////////////////////////////////////////////////////////////////////////////////
// img_yuv class declaration
class img_yuv: public img {
public:
	// public types ------------------------------------------------------------
	typedef unsigned char sample_t;

	// public constants --------------------------------------------------------
	static const int YUV_FRMT_444	= 444;
	static const int YUV_FRMT_422	= 422;
	static const int YUV_FRMT_420	= 420;
	static const int YUV_FRMT_411	= 411;

    // public functions --------------------------------------------------------
	img_yuv();
	~img_yuv();

	sample_t *y() const { return _y; }
	sample_t *u() const { return _u; }
	sample_t *v() const { return _v; }

	size_t y_sz() const { return _y_sz; }
	size_t u_sz() const { return _u_sz; }
	size_t v_sz() const { return _v_sz; }

	int dx() const { return _dx; }
	int dy() const { return _dy; }
	int frmt() const { return _frmt; }

	int reset(const int w, const int h, const int frmt = YUV_FRMT_444);

protected:
	// protected constants -----------------------------------------------------
	// protected functions -----------------------------------------------------
	static size_t _calc_sz(const int w, const int h, const int dx, const int dy,
						   size_t &y_sz, size_t &u_sz, size_t &v_sz);
	static size_t _calc_sz(const int w, const int h, const int dx, const int dy);

	void _set_frmt(const int frmt);
	void _reset_yuv(const bool zero = false);
	int _reset(const int w, const int h, const int frmt);

private:
	sample_t *_y;
	sample_t *_u;
	sample_t *_v;

	size_t _y_sz;
	size_t _u_sz;
	size_t _v_sz;

	int _dx;
	int _dy;
	int _frmt;
};



}	// namespace imgs



#endif	// IMGS_IMG_YUV_INCLUDED
