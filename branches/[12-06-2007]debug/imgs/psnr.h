/*******************************************************************************
* file:         psnr.h                                                         *
* version:      0.0.1                                                          *
* author:       mice (mailto:my_mice@mail.ru, ICQ:332-292-380)                 *
* description:  not available                                                  *
* tests:        none                                                           *
*******************************************************************************/

#ifndef IMGS_PSNR_INCLUDED
#define IMGS_PSNR_INCLUDED

////////////////////////////////////////////////////////////////////////////////
// headers
#include <math.h>

#include "img.h"


////////////////////////////////////////////////////////////////////////////////
// imgs namespace
namespace imgs {


////////////////////////////////////////////////////////////////////////////////
// psnr class declaration
template <
	class p_t = unsigned char,
	class d_t = int,
	d_t mx_p = 255
>
class psnr {
public:
	// public types ------------------------------------------------------------
	// public constants --------------------------------------------------------
	static const double PSNR_ERROR;
	static const double PSNR_SCALE;

    // public functions --------------------------------------------------------
	psnr();
	~psnr();

	inline static double ptr(const p_t *const ptr1, const p_t *const ptr2,
							 const size_t sz);

protected:
	inline static double _psnr_ptr(const p_t *const ptr1, const p_t *const ptr2,
								   const size_t sz);

private:
};



////////////////////////////////////////////////////////////////////////////////
// psnr class public definitions
template <class p_t, class d_t, d_t mx_p>
const double psnr<p_t, d_t, mx_p>::PSNR_ERROR		= 0;
template <class p_t, class d_t, d_t mx_p>
const double psnr<p_t, d_t, mx_p>::PSNR_SCALE		= 20;


template <class p_t, class d_t, d_t mx_p>
psnr<p_t, d_t, mx_p>::psnr() {
}


template <class p_t, class d_t, d_t mx_p>
psnr<p_t, d_t, mx_p>::~psnr() {
}


template <class p_t, class d_t, d_t mx_p>
inline double psnr<p_t, d_t, mx_p>::ptr(
	const p_t *const ptr1, const p_t *const ptr2,
	const size_t sz)
{
	return _psnr_ptr(ptr1, ptr2, sz);
}



////////////////////////////////////////////////////////////////////////////////
// psnr class protected definitions
template <class p_t, class d_t, d_t mx_p>
inline double psnr<p_t, d_t, mx_p>::_psnr_ptr(
	const p_t *const ptr1, const p_t *const ptr2,
	const size_t sz)
{
	if (NULL == ptr1 || NULL == ptr2) return PSNR_ERROR;

	double s = 0;

	for (size_t i = 0; sz > i; ++i) {
		const d_t d = d_t(ptr1[i]) - d_t(ptr2[i]);
		s += (d * d);
	}

	s /= sz;

	s = PSNR_SCALE * log10(double(mx_p) / sqrt(s));

	return s;
}



}	// namespace imgs



#endif	// IMGS_PSNR_INCLUDED
