/*******************************************************************************
* file:         test_dbg_surface.cpp                                           *
* version:      0.1.0                                                          *
* author:       mice (mailto:my_mice@mail.ru, ICQ:332-292-380)                 *
* description:  tests for wicdbg::dbg_surface class                            *
* tests:        none                                                           *
*******************************************************************************/


////////////////////////////////////////////////////////////////////////////////
// headers

// standard C++ library headers
#include <iostream>

// external libraries headers
// none

// libwic headers
// none

// libwicdbg headers
#include <wic/libwicdbg/dbg_surface.h>

// qatest headers
#include "test_dbg_surface.h"


////////////////////////////////////////////////////////////////////////////////
// public function definitions

/*!
*/
bool test_dbg_surface()
{
	// Что тестируется
	std::cout << "wicdbg::dbg_surface class" << std::endl;

	wicdbg::dbg_surface surface(512, 512);

	for (wic::sz_t i = 0; surface.size() > i; ++i)
	{
		surface.get(i).m = rand() % 12;
	}

	surface.save<wicdbg::dbg_pixel::member_m>("test_dbg_surface.txt", false);
	surface.save<wicdbg::dbg_pixel::member_m>("test_dbg_surface.bmp", true);

	return false;
}
