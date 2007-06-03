/*******************************************************************************
* file:         test_prediction.cpp                                            *
* version:      0.1.0                                                          *
* author:       mice (mailto:my_mice@mail.ru, ICQ:332-292-380)                 *
* description:  tests of wic prediction routines                               *
* tests:        none                                                           *
*******************************************************************************/


////////////////////////////////////////////////////////////////////////////////
// headers

// standard C++ library headers
#include <iostream>

// external libraries headers
#include <imgs/img_rgb.h>
#include <imgs/bmp_file.h>
#include <imgs/bmp_dump.h>
#include <wavelets/cdf97/cdf97.h>

// libwic headers
#include <wic/libwic/wtree.h>

// qatest headers
#include "test_prediction.h"


////////////////////////////////////////////////////////////////////////////////
// public constants definitions

//!	\brief Точность требуемая для прохождения теста
static const double epsilon		= 0.000001;


////////////////////////////////////////////////////////////////////////////////
// protected function declarations

//!	\brief Сверяет два double значения
bool qa_dbl_compare(const double value_must_be, const double value_is,
					const char *const desc = 0);


////////////////////////////////////////////////////////////////////////////////
// protected function definitions

/*!	\param[in] value_must_be Эталонное значение
	\param[in] value_is Полученное значение
	\param[in] desc Строковое описание теста
	\return \c true если значения равны, иначе \c false
*/
bool qa_dbl_compare(const double value_must_be, const double value_is,
					const char *const desc)
{
	if (0 != desc) std::cout << desc << ": ";

	std::cout << "must[ " << value_must_be << " ],  ";
	std::cout << "is{ " << value_is << " }:  ";
	if (epsilon > fabs(value_must_be - value_is)) {
		std::cout << "ok." << std::endl;
		return true;
	}

    std::cout << "failed." << std::endl;
	return false;
}


////////////////////////////////////////////////////////////////////////////////
// public function definitions

/*!
*/
bool test_wtree_calc_pi()
{
	std::cout << "Pi prediction routine" << std::endl;

	// some constants to use
	static const int lvls		= 3;

	// first, load the image
	imgs::img_rgb rgb_image;
	if (0 != imgs::bmp_read(rgb_image, "../res/images/lena_eye_16x16.bmp"))
	{
		std::cout << "error: can't load image." << std::endl;

		return false;
	}

	// allocate some memory and copy converted to float image there
	const wic::sz_t image_pixels_count = rgb_image.w() * rgb_image.h();

	float *const image_wt = new float[image_pixels_count];
	if (0 == image_wt) {
		std::cout << "error: can't allocate memory." << std::endl;

		return false;
	}

	const imgs::img_rgb::rgb24_t *const rgb_image_bits =
		(imgs::img_rgb::rgb24_t *)(rgb_image.bits());

	for (wic::sz_t i = 0; image_pixels_count > i; ++i) {
		image_wt[i] = rgb_image_bits[i].r;
	}

	// do wavelet transform
	wt2d_cdf97_fwd(image_wt, rgb_image.h(), rgb_image.w(), lvls);

	imgs::bmp_dump<wic::w_t, wic::sz_t>::txt_dump(image_wt,
												  rgb_image.w(), rgb_image.h(),
												  "dumps/[enc]image_wt.in");

	wic::wtree wtree(rgb_image.w(), rgb_image.h(), lvls);
	wtree.load(image_wt);

	// do tests
	using namespace wic;
	const pi_t good_at_00_00	= 239.33333333;
	const pi_t good_at_15_00	= 3.11111111;
	const pi_t good_at_00_15	= 5.44444444;
	const pi_t good_at_15_15	= 2.55555555;
	const pi_t good_at_06_06	= 22.0625;

	const pi_t calc_at_00_00	= wtree.calc_pi<wic::wnode::member_w>(
										0 , 0,  wtree.sb().get_LL());
	const pi_t calc_at_15_00	= wtree.calc_pi<wic::wnode::member_w>(
										15, 0,  wtree.sb().get(3, 0));
	const pi_t calc_at_00_15	= wtree.calc_pi<wic::wnode::member_w>(
										0 , 15, wtree.sb().get(3, 1));
	const pi_t calc_at_15_15	= wtree.calc_pi<wic::wnode::member_w>(
										15, 15, wtree.sb().get(3, 2));
	const pi_t calc_at_06_06	= wtree.calc_pi<wic::wnode::member_w>(
										6, 6, wtree.sb().get(2, 2));

	// free memory
	delete[] image_wt;

	// output results
	bool ok = true;

	if (!qa_dbl_compare(good_at_00_00, calc_at_00_00, "(0, 0)")) ok = false;
	if (!qa_dbl_compare(good_at_00_15, calc_at_00_15, "(0, 15)")) ok = false;
	if (!qa_dbl_compare(good_at_15_00, calc_at_15_00, "(15, 0)")) ok = false;
	if (!qa_dbl_compare(good_at_15_15, calc_at_15_15, "(15, 15)")) ok = false;
	if (!qa_dbl_compare(good_at_06_06, calc_at_06_06, "(06, 06)")) ok = false;

	return ok;
}


/*!
*/
bool test_wtree_calc_sj()
{
	std::cout << "Sj prediction routine" << std::endl;

	// some constants to use
	static const int lvls		= 3;

	// first, load the image
	imgs::img_rgb rgb_image;
	if (0 != imgs::bmp_read(rgb_image, "../res/images/lena_eye_16x16.bmp"))
	{
		std::cout << "error: can't load image." << std::endl;

		return false;
	}

	// allocate some memory and copy converted to float image there
	const wic::sz_t image_pixels_count = rgb_image.w() * rgb_image.h();

	float *const image_wt = new float[image_pixels_count];
	if (0 == image_wt) {
		std::cout << "error: can't allocate memory." << std::endl;

		return false;
	}

	const imgs::img_rgb::rgb24_t *const rgb_image_bits =
		(imgs::img_rgb::rgb24_t *)(rgb_image.bits());

	for (wic::sz_t i = 0; image_pixels_count > i; ++i) {
		image_wt[i] = rgb_image_bits[i].r;
	}

	// do wavelet transform
	wt2d_cdf97_fwd(image_wt, rgb_image.h(), rgb_image.w(), lvls);

	imgs::bmp_dump<wic::w_t, wic::sz_t>::txt_dump(image_wt,
												  rgb_image.w(), rgb_image.h(),
												  "dumps/[enc]image_wt.in");

	wic::wtree wtree(rgb_image.w(), rgb_image.h(), lvls);
	wtree.load(image_wt);

	// do tests
	using namespace wic;
	const pi_t good_at_09_01	= 21.14;
	const pi_t good_at_07_07	= 16.28;
	const pi_t good_at_06_06	= 71.612;
	const pi_t good_at_03_12	= 17.466;

	const pi_t calc_at_09_01	= wtree.calc_sj<wic::wnode::member_w>(
										9,  1,  wtree.sb().get(3, 0), true);
	const pi_t calc_at_07_07	= wtree.calc_sj<wic::wnode::member_w>(
										7,  7,  wtree.sb().get(2, 2), true);
	const pi_t calc_at_06_06	= wtree.calc_sj<wic::wnode::member_w>(
										6,  6,  wtree.sb().get(2, 2), false);
	const pi_t calc_at_03_12	= wtree.calc_sj<wic::wnode::member_w>(
										3,  12, wtree.sb().get(3, 1), false);

	const pi_t calc_at_09_01a	= wtree.calc_sj<wic::wnode::member_w>(
										9,  1,  wtree.sb().get(3, 0));
	const pi_t calc_at_07_07a	= wtree.calc_sj<wic::wnode::member_w>(
										7,  7,  wtree.sb().get(2, 2));
	const pi_t calc_at_06_06a	= wtree.calc_sj<wic::wnode::member_w>(
										6,  6,  wtree.sb().get(2, 2));
	const pi_t calc_at_03_12a	= wtree.calc_sj<wic::wnode::member_w>(
										3,  12, wtree.sb().get(3, 1));

	// free memory
	delete[] image_wt;

	// output results
	bool ok = true;

	if (!qa_dbl_compare(good_at_09_01, calc_at_09_01, "(9, 1)")) ok = false;
	if (!qa_dbl_compare(good_at_07_07, calc_at_07_07, "(7, 7)")) ok = false;
	if (!qa_dbl_compare(good_at_06_06, calc_at_06_06, "(6, 6)")) ok = false;
	if (!qa_dbl_compare(good_at_03_12, calc_at_03_12, "(3, 12)")) ok = false;

	if (!qa_dbl_compare(good_at_09_01, calc_at_09_01a, "(9, 1)a")) ok = false;
	if (!qa_dbl_compare(good_at_07_07, calc_at_07_07a, "(7, 7)a")) ok = false;
	if (!qa_dbl_compare(good_at_06_06, calc_at_06_06a, "(6, 6)a")) ok = false;
	if (!qa_dbl_compare(good_at_03_12, calc_at_03_12a, "(3, 12)a")) ok = false;

	return ok;
}


/*!
*/
bool test_wtree_calc_pi_avg()
{
	std::cout << "PiAvg prediction routine" << std::endl;

	// some constants to use
	static const int lvls		= 3;

	// first, load the image
	imgs::img_rgb rgb_image;
	if (0 != imgs::bmp_read(rgb_image, "../res/images/lena_eye_16x16.bmp"))
	{
		std::cout << "error: can't load image." << std::endl;

		return false;
	}

	// allocate some memory and copy converted to float image there
	const wic::sz_t image_pixels_count = rgb_image.w() * rgb_image.h();

	float *const image_wt = new float[image_pixels_count];
	if (0 == image_wt) {
		std::cout << "error: can't allocate memory." << std::endl;

		return false;
	}

	const imgs::img_rgb::rgb24_t *const rgb_image_bits =
		(imgs::img_rgb::rgb24_t *)(rgb_image.bits());

	for (wic::sz_t i = 0; image_pixels_count > i; ++i) {
		image_wt[i] = rgb_image_bits[i].r;
	}

	// do wavelet transform
	wt2d_cdf97_fwd(image_wt, rgb_image.h(), rgb_image.w(), lvls);

	imgs::bmp_dump<wic::w_t, wic::sz_t>::txt_dump(image_wt,
												  rgb_image.w(), rgb_image.h(),
												  "dumps/[enc]image_wt.in");

	wic::wtree wtree(rgb_image.w(), rgb_image.h(), lvls);
	wtree.load(image_wt);

	// do tests
	using namespace wic;
	const pi_t good_at_2_0_2_0	= 29.25520833;
	const pi_t good_at_6_6_3_2	= 7.0625;
    
	const pi_t calc_at_2_0_2_0 = wtree.calc_pi_avg<wic::wnode::member_wc>(
										p_t(2, 0),  wtree.sb().get(2, 0));
	const pi_t calc_at_6_6_3_2 = wtree.calc_pi_avg<wic::wnode::member_wc>(
										p_t(6, 6),  wtree.sb().get(3, 2));

	// free memory
	delete[] image_wt;

	// output results
	bool ok = true;
	if (!qa_dbl_compare(good_at_2_0_2_0, calc_at_2_0_2_0, "((2, 0), (2, 0))")) ok = false;
	if (!qa_dbl_compare(good_at_6_6_3_2, calc_at_6_6_3_2, "((6, 6), (3, 2))")) ok = false;

	return ok;
}