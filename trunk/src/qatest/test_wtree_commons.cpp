/*******************************************************************************
* file:         test_wtree_commons.cpp                                         *
* version:      0.1.0                                                          *
* author:       mice (mailto:my_mice@mail.ru, ICQ:332-292-380)                 *
* description:  tests of wtree common methods                                  *
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

// qatest headers
#include "test_wtree_commons.h"


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
bool test_wtree_leafs_top_left()
{
	// Что тестируется
	std::cout << "wtree::_leafs_top_left() method" << std::endl;

	using namespace wic;

	// Необходим для доступа к защищённым членам класса, которые
	// будут тестироваться
	class test: public wtree
	{
	public:
		test(): wtree(16, 16, 3) {}

		p_t get(const p_t &root, const sz_t &lvl, const sz_t &i)
		{
			return _leafs_top_left(root, lvl, i);
		}

		static bool ok(const p_t &good, const p_t &calc,
					   const char *const desc = 0)
		{
			if (0 != desc) std::cout << desc << ": ";

			std::cout << "must[ " << good.x << ", " << good.y << " ], ";
			std::cout << "is{ " << calc.x << ", " << calc.y << " }: ";

			if (good.x == calc.x && good.y == calc.y) {
				std::cout << "ok." << std::endl;
				return true;
			}
			
			std::cout << "failed." << std::endl;
			return false;
		}
	};

	// Эталонные значения
	const p_t good_0_0_lvl0_i0(0, 0);
	const p_t good_1_1_lvl1_i2(3, 3);
	const p_t good_1_0_lvl3_i1(4, 8);
	const p_t good_0_1_lvl2_i1(0, 6);

	// тестирование
	test t;
	const p_t calc_0_0_lvl0_i0 = t.get(p_t(0, 0), 0, 0);
	const p_t calc_1_1_lvl1_i2 = t.get(p_t(1, 1), 1, 2);
	const p_t calc_1_0_lvl3_i1 = t.get(p_t(1, 0), 3, 1);
	const p_t calc_0_1_lvl2_i1 = t.get(p_t(0, 1), 2, 1);

	// вывод результатов
	bool ok = true;

	if (!test::ok(good_0_0_lvl0_i0, calc_0_0_lvl0_i0, "(0,0,lvl0,i0)")) ok = false;
	if (!test::ok(good_1_1_lvl1_i2, calc_1_1_lvl1_i2, "(1,1,lvl1,i2)")) ok = false;
	if (!test::ok(good_1_0_lvl3_i1, calc_1_0_lvl3_i1, "(1,0,lvl3,i1)")) ok = false;
	if (!test::ok(good_0_1_lvl2_i1, calc_0_1_lvl2_i1, "(1,0,lvl3,i1)")) ok = false;

	return ok;
}
