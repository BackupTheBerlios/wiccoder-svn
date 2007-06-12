////////////////////////////////////////////////////////////////////////////////
// headers

// standard C++ library headers
#include <iostream>

// external libraries headers
// none

// libwic headers
#include <wic/libwic/wtree.h>
#include <wic/libwic/subbands.h>

// qatest headers
#include "test_subbands.h"


////////////////////////////////////////////////////////////////////////////////
// public function definitions

/*!
*/
bool test_lvl_from_point()
{
	std::cout << "subbands::lvl_from_point method" << std::endl;

	float* array = new float[256];
	memset(array, 0, sizeof(float) * 256);

	wic::wtree tree(16, 16, 3);
	tree.load(array);

	bool ok = true;
	if (tree.sb().lvl_from_point(wic::p_t(1,  1)) != wic::subbands::LVL_0) ok = false;
	if (tree.sb().lvl_from_point(wic::p_t(2,  0)) != (wic::sz_t) 1) ok = false;
	if (tree.sb().lvl_from_point(wic::p_t(6,  5)) != (wic::sz_t) 2) ok = false;
	if (tree.sb().lvl_from_point(wic::p_t(3, 15)) != (wic::sz_t) 3) ok = false;

	delete array;

	return ok;
}

/*!
*/
bool test_from_point()
{
	std::cout << "subbands::from_point method" << std::endl;

	float* array = new float[256];
	memset(array, 0, sizeof(float) * 256);

	wic::wtree tree(16, 16, 3);
	tree.load(array);

	bool ok = true;
	if (&(tree.sb().from_point(wic::p_t(1,  1))) != &(tree.sb().get_LL())) ok = false;
	if (&(tree.sb().from_point(wic::p_t(2,  0))) != &(tree.sb().get(1, wic::subbands::SUBBAND_HL))) ok = false;
	if (&(tree.sb().from_point(wic::p_t(6,  5))) != &(tree.sb().get(2, wic::subbands::SUBBAND_HH))) ok = false;
	if (&(tree.sb().from_point(wic::p_t(3, 15))) != &(tree.sb().get(3, wic::subbands::SUBBAND_LH))) ok = false;

	delete array;

	return ok;
}
