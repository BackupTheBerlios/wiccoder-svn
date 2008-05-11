/*******************************************************************************
* file:         bmp_dump.h                                                     *
* version:      0.0.1                                                          *
* author:       mice (mailto:my_mice@mail.ru, ICQ:332-292-380)                 *
* description:  not available                                                  *
* tests:        none                                                           *
*******************************************************************************/

#ifndef IMGS_BMP_DUMP_INCLUDED
#define IMGS_BMP_DUMP_INCLUDED

////////////////////////////////////////////////////////////////////////////////
// headers
#include <math.h>
#include <string>
#include <ostream>
#include <fstream>
#include <iomanip>

#include "img.h"
#include "img_rgb.h"
#include "bmp_file.h"


////////////////////////////////////////////////////////////////////////////////
// imgs namespace
namespace imgs {


////////////////////////////////////////////////////////////////////////////////
// bmp_dump class declaration
template <
	class v_t,
	class dim_t = unsigned int
>
class bmp_dump {
public:
	// public types ------------------------------------------------------------
	// public constants --------------------------------------------------------
	static const int DEF_FW			= 5;

    // public functions --------------------------------------------------------
	bmp_dump();
	~bmp_dump();

	inline static bool dump(const v_t *const vs,
							const dim_t w, const dim_t h,
							std::ostream &out);
	inline static bool dump(const v_t *const vs,
							const dim_t w, const dim_t h,
							const std::string &file);

	inline static bool txt_dump(const v_t *const vs,
								const dim_t w, const dim_t h,
								std::ostream &out,
								const dim_t fw = DEF_FW);
	inline static bool txt_dump(const v_t *const vs,
								const dim_t w, const dim_t h,
								const std::string &file,
								const dim_t fw = DEF_FW);

protected:
	inline static bool _dump(const v_t *const vs, const dim_t w, const dim_t h,
							 img_rgb &rgb);

	inline static bool _txt_dump(const v_t *const vs,
								 const dim_t w, const dim_t h,
								 const dim_t fw,
								 std::ostream &out);

private:
};


////////////////////////////////////////////////////////////////////////////////
// public functions declarations
template <class v_t, class dim_t>
inline static void txt_dump_v(const v_t v, const dim_t fw,
							  std::ostream &out);
template <class dim_t>
inline static void txt_dump_v(const char v, const dim_t fw,
							  std::ostream &out);
template <class dim_t>
inline static void txt_dump_v(const unsigned char v, const dim_t fw,
							  std::ostream &out);


////////////////////////////////////////////////////////////////////////////////
// bmp_dump class public definitions
template <class v_t, class dim_t>
bmp_dump<v_t, dim_t>::bmp_dump() {
}


template <class v_t, class dim_t>
bmp_dump<v_t, dim_t>::~bmp_dump() {
}


template <class v_t, class dim_t>
inline bool bmp_dump<v_t, dim_t>::dump(const v_t *const vs,
									   const dim_t w, const dim_t h,
									   std::ostream &out)
{
	img_rgb rgb;

	if (!_dump(vs, w, h, rgb)) return false;

	if (0 != bmp_write(rgb, out)) return false;

	return true;
}


template <class v_t, class dim_t>
inline bool bmp_dump<v_t, dim_t>::dump(const v_t *const vs,
									   const dim_t w, const dim_t h,
									   const std::string &file)
{
	img_rgb rgb;

	if (!_dump(vs, w, h, rgb)) return false;

	if (0 != bmp_write(rgb, file)) return false;

	return true;
}


template <class v_t, class dim_t>
inline bool bmp_dump<v_t, dim_t>::txt_dump(const v_t *const vs,
										   const dim_t w, const dim_t h,
										   std::ostream &out,
										   const dim_t fw = DEF_FW)
{
	return _txt_dump(vs, w, h, fw, out);
}


template <class v_t, class dim_t>
inline bool bmp_dump<v_t, dim_t>::txt_dump(const v_t *const vs,
										   const dim_t w, const dim_t h,
										   const std::string &file,
										   const dim_t fw = DEF_FW)
{
	std::ofstream of(file.c_str());

	if (!of.good()) return false;

	return _txt_dump(vs, w, h, fw, of);
}


////////////////////////////////////////////////////////////////////////////////
// bmp_dump class protected definitions
template <class v_t, class dim_t>
inline bool bmp_dump<v_t, dim_t>::_dump(const v_t *const vs,
										const dim_t w, const dim_t h,
										img_rgb &rgb)
{
	if (NULL == vs) return false;
	if (0 >= w || 0 >= h) return false;

	const dim_t sz = w * h;

	// find min / max
	v_t min = vs[0];
	v_t max = vs[0];

	for (dim_t i = 0; sz > i; ++i) {
		const v_t &v = vs[i];
		if (v > max) max = v;
		if (v < min) min = v;
	}

	rgb.reset(w, h, 24);

	const v_t d = max - min;
	
	for (dim_t y = 0; h > y; ++y) {
		for (dim_t x = 0; w > x; ++x) {
			const dim_t i = x + w*y;

			const v_t v = (0 != d)? v_t((vs[i] - min) * 255 / d): 0;

			const unsigned char c = (unsigned char)v;

			((img_rgb::rgb24_t *)rgb.bits())[i].r = c;
			((img_rgb::rgb24_t *)rgb.bits())[i].g = c;
			((img_rgb::rgb24_t *)rgb.bits())[i].b = c;
		}
	}

	return true;
}


template <class v_t, class dim_t>
inline bool bmp_dump<v_t, dim_t>::_txt_dump(const v_t *const vs,
											const dim_t w, const dim_t h,
											const dim_t fw,
											std::ostream &out)
{
	if (NULL == vs) return false;
	if (0 >= w || 0 >= h) return false;

	for (dim_t y = 0; h > y; ++y) {
		for (dim_t x = 0; w > x; ++x) {
			// out << std::setw(fw) << vs[x + w*y];
			txt_dump_v(vs[x + w*y], fw, out);
		}

		out << std::endl;
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////////
// public functions definitions
template <class v_t, class dim_t>
inline void txt_dump_v<v_t, dim_t>(const v_t v, const dim_t fw,
								   std::ostream &out)
{
	out << std::setw(fw) << v;
}


template <class dim_t>
inline void txt_dump_v<char, dim_t>(const char v, const dim_t fw,
									std::ostream &out)
{
	out << std::setw(fw) << int(v);
}


template <class dim_t>
inline void txt_dump_v<unsigned char, dim_t>(const unsigned char v, const dim_t fw,
											 std::ostream &out)
{
	out << std::setw(fw) << int(v);
}



}	// namespace imgs



#endif	// IMGS_BMP_DUMP_INCLUDED
