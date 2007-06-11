/*******************************************************************************
* file:         img_yuv.cpp                                                    *
* version:      0.0.1                                                          *
* author:       mice (mailto:my_mice@mail.ru, ICQ:332-292-380)                 *
* description:  not available                                                  *
* tests:        none                                                           *
*******************************************************************************/


////////////////////////////////////////////////////////////////////////////////
// headers
#include "img_yuv.h"


////////////////////////////////////////////////////////////////////////////////
// imgs namespace
namespace imgs {


////////////////////////////////////////////////////////////////////////////////
// img_yuv class public definition
img_yuv::img_yuv() {
	_set_frmt(YUV_FRMT_444);
	_reset_yuv(true);
}


img_yuv::~img_yuv() {
}


int img_yuv::reset(const int w, const int h, const int frmt) {
	return _reset(w, h, frmt);
}


////////////////////////////////////////////////////////////////////////////////
// img_yuv class protected definition
size_t img_yuv::_calc_sz(const int w, const int h, const int dx, const int dy,
						 size_t &y_sz, size_t &u_sz, size_t &v_sz)
{
	y_sz = w * h * sizeof(sample_t);
	const int dxy = dx * dy;
	u_sz = v_sz = (y_sz + y_sz % dxy) / dxy;

	return (y_sz + u_sz + v_sz);
}


size_t img_yuv::_calc_sz(const int w, const int h, const int dx, const int dy) {
	size_t y_sz, u_sz, v_sz;
	return _calc_sz(w, h, dx, dy, y_sz, u_sz, v_sz);
}


void img_yuv::_set_frmt(const int frmt) {
	switch (_frmt = frmt) {
		case YUV_FRMT_444: _dx = 1; _dy = 1; break;
		case YUV_FRMT_422: _dx = 2; _dy = 1; break;
		case YUV_FRMT_420: _dx = 2; _dy = 2; break;
		case YUV_FRMT_411: _dx = 4; _dy = 1; break;

		default: _dx = 1; _dy = 1;
	}
}


void img_yuv::_reset_yuv(const bool zero) {
	if (zero) {
		_y = _u = _v = NULL;
		_y_sz = _u_sz = _v_sz = 0;
	} else {
		_y = (sample_t *)_get_bits();
		_u = _y + _y_sz;
		_v = _u + _u_sz;
	}
}


int img_yuv::_reset(const int w, const int h, const int frmt) {
	_set_frmt(frmt);

	const size_t nsz = _calc_sz(w, h, _dx, _dy, _y_sz, _u_sz, _v_sz);

	if (0 != img::_reset(w, h, nsz)) {
		_reset_yuv(true);

		return -1;
	}

	_reset_yuv(false);

	return 0;
}



}	// namespace imgs
