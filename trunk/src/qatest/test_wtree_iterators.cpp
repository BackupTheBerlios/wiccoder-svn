/*******************************************************************************
* file:         test_wtree_iterators.cpp                                       *
* version:      0.1.0                                                          *
* author:       mice (mailto:my_mice@mail.ru, ICQ:332-292-380)                 *
* description:  tests of wtree (and others) iterators                          *
* tests:        none                                                           *
*******************************************************************************/


////////////////////////////////////////////////////////////////////////////////
// headers

// standard C++ library headers
#include <iostream>

// external libraries headers
// none

// libwic headers
#include <wic/libwic/wtree.h>
#include <wic/libwic/iterators.h>

// qatest headers
#include "test_wtree_iterators.h"


////////////////////////////////////////////////////////////////////////////////
// public constants definitions


////////////////////////////////////////////////////////////////////////////////
// protected function declarations


////////////////////////////////////////////////////////////////////////////////
// protected function definitions


////////////////////////////////////////////////////////////////////////////////
// public function definitions

/*!
*/
bool test_n_cutdown_iterator()
{
	// Что тестируется
	std::cout << "wic::n_cutdown_iterator() class" << std::endl;

	using namespace wic;

	// Тест для нулевого уровня: 3 дочерних элемента
	for (wic::n_cutdown_iterator i1(0); !i1.end(); i1.next()) {
	}

	// Тест для ненулевого уровня: 4 дочерних элемента
	for (wic::n_cutdown_iterator i2(1); !i2.end(); i2.next()) {
	}

	return true;
}
