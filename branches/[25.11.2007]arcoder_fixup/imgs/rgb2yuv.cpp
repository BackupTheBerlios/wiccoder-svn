/*******************************************************************************
* file:         rgb2yuv.cpp                                                   *
* version:      0.0.1                                                          *
* author:       mice (mailto:my_mice@mail.ru, ICQ:332-292-380)                 *
* description:  not available                                                  *
* tests:        none                                                           *
*******************************************************************************/


////////////////////////////////////////////////////////////////////////////////
// headers
#include "rgb2yuv.h"


////////////////////////////////////////////////////////////////////////////////
// imgs namespace
namespace imgs {


////////////////////////////////////////////////////////////////////////////////
// rgb2yuv protected declaration
int _byte_clip(const int val);


////////////////////////////////////////////////////////////////////////////////
// rgb2yuv public definition
int rgb2yuv(const img_rgb &rgb, img_yuv &yuv, const int frmt) {
	if (24 != rgb.bpp()) return -1;
	if (img_yuv::YUV_FRMT_444 != frmt) return -1;

	if (0 != yuv.reset(rgb.w(), rgb.h(), frmt)) return -1;

	img_rgb::rgb24_t *const rgb_bits = (img_rgb::rgb24_t *)rgb.bits();
	img_yuv::sample_t *const &y_bits = yuv.y();
	img_yuv::sample_t *const &u_bits = yuv.u();
	img_yuv::sample_t *const &v_bits = yuv.v();

	// unsigned int *const line_buf = (unsigned int *)malloc(rgb.w() * sizeof(unsigned int));
	// if (NULL == line_buf) return -1;

	const int dline = rgb.w();
	const int dline_uv = rgb.w() / yuv.dy();

	img_rgb::rgb24_t *rgbl_bits = rgb_bits;
	img_yuv::sample_t *yl_bits = yuv.y();
	img_yuv::sample_t *ul_bits = yuv.u();
	img_yuv::sample_t *vl_bits = yuv.v();

	for (int cy = rgb.h() - 1; 0 <= cy; --cy) {

		for (int cx = rgb.w() - 1; 0 <= cx; --cx) {
			const img_rgb::rgb24_t &rgb_pxl = rgbl_bits[cx];
			yl_bits[cx] = (( 66*rgb_pxl.r + 129*rgb_pxl.g +  25*rgb_pxl.b + 128) >> 8) +  16;
			ul_bits[cx] = ((-38*rgb_pxl.r -  74*rgb_pxl.g + 112*rgb_pxl.b + 128) >> 8) + 128;
			vl_bits[cx] = ((112*rgb_pxl.r -  94*rgb_pxl.g -  18*rgb_pxl.b + 128) >> 8) + 128;
		}

		rgbl_bits += dline;
		yl_bits += dline;
		ul_bits += dline_uv;
		vl_bits += dline_uv;
	}

	return 0;
}


int yuv2rgb(const img_yuv &yuv, img_rgb &rgb, const int bpp) {
	if (24 != bpp) return -1;
	if (img_yuv::YUV_FRMT_444 != yuv.frmt()) return -1;

	if (0 != rgb.reset(yuv.w(), yuv.h(), bpp)) return -1;

	img_yuv::sample_t *const &y_bits = yuv.y();
	img_yuv::sample_t *const &u_bits = yuv.u();
	img_yuv::sample_t *const &v_bits = yuv.v();
	img_rgb::rgb24_t *const rgb_bits = (img_rgb::rgb24_t *)rgb.bits();

	// unsigned int *const line_buf = (unsigned int *)malloc(rgb.w() * sizeof(unsigned int));
	// if (NULL == line_buf) return -1;

	const int dline = yuv.w();
	const int dline_uv = yuv.w() / yuv.dy();

	img_yuv::sample_t *yl_bits = yuv.y();
	img_yuv::sample_t *ul_bits = yuv.u();
	img_yuv::sample_t *vl_bits = yuv.v();
	img_rgb::rgb24_t *rgbl_bits = rgb_bits;

	for (int cy = rgb.h() - 1; 0 <= cy; --cy) {

		for (int cx = rgb.w() - 1; 0 <= cx; --cx) {
			img_rgb::rgb24_t &rgb_pxl = rgbl_bits[cx];
			const int kc = int(yl_bits[cx]) - 16;
			const int kd = int(ul_bits[cx]) - 128;
			const int ke = int(vl_bits[cx]) - 128;

			rgb_pxl.r = _byte_clip((298*kc +          409*ke + 128) >> 8);
			rgb_pxl.g = _byte_clip((298*kc - 100*kd - 208*ke + 128) >> 8);
			rgb_pxl.b = _byte_clip((298*kc + 516*kd          + 128) >> 8);
		}

		yl_bits += dline;
		ul_bits += dline_uv;
		vl_bits += dline_uv;
		rgbl_bits += dline;
	}

	return 0;
}


////////////////////////////////////////////////////////////////////////////////
// rgb2yuv protected definitions
int _byte_clip(const int val) {
	if (0 > val) return 0;
	if (0xFF < val) return 0xFF;
	
	return val;
}



}	// namespace imgs
