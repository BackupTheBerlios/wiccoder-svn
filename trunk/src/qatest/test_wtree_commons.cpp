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

//!	\brief Оператор != для типа <i>p_t</i> двумерных координат
bool operator != (const wic::p_t &p1, const wic::p_t &p2);

//!	\brief Сравнивает значение координат и выводит информацию о сравнении
bool _compare_points(const wic::p_t &good, const wic::p_t &calc,
					 const char *const about = 0);


////////////////////////////////////////////////////////////////////////////////
// protected function definitions

/*!	\param[in] p1 Двумерные координаты первой точки
	\param[in] p2 Двумерные координаты второй точки
	\return <i>true</i> если координаты не равны, иначе <i>false</i>
*/
bool operator != (const wic::p_t &p1, const wic::p_t &p2) {
	return ((p1.x != p2.x) || (p1.y != p2.y));
}


/*!	\param[in] good Правильные координаты
	\param[in] calc Координаты полученный при расчётах
	\return <i>true</i> если координаты не равны, иначе <i>false</i>
*/
bool _compare_points(const wic::p_t &good, const wic::p_t &calc,
					 const char *const about)
{
	if (0 != about) std::cout << about << ": ";

	std::cout << "must[ " << good.x << ", " << good.y << " ], ";
	std::cout << "is[ " << calc.x << ", " << calc.y << " ]: ";

	const bool ok = (good.x == calc.x && good.y == calc.y);

	if (ok) std::cout << "ok.";
	else std::cout << "failed.";

	std::cout << std::endl;

	return ok;
}


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

/*!
*/
bool test_child_n_mask()
{
	// Что тестируется
	std::cout << "wtree::child_n_mask() method" << std::endl;

	using namespace wic;

	// exected results
	n_t n_good_4_0 = 1;
	n_t n_good_5_0 = 2;
	n_t n_good_4_1 = 4;
	n_t n_good_5_1 = 8;

	// calculated result
	wtree tree(16, 16, 3);
	n_t n_calc_4_0 = tree.child_n_mask(p_t(4, 0), p_t(2, 0));
	n_t n_calc_5_0 = tree.child_n_mask(p_t(5, 0), p_t(2, 0));
	n_t n_calc_4_1 = tree.child_n_mask(p_t(4, 1), p_t(2, 0));
	n_t n_calc_5_1 = tree.child_n_mask(p_t(5, 1), p_t(2, 0));
	
	// comparison
	bool ok = true;
	if (n_calc_4_0 != n_good_4_0) ok = false;
	if (n_calc_5_0 != n_good_5_0) ok = false;
	if (n_calc_4_1 != n_good_4_1) ok = false;
	if (n_calc_5_1 != n_good_5_1) ok = false;

	return ok;
}

/*!
*/
bool test_wtree_prnt()
{
	// Что тестируется
	std::cout << "wtree::prnt() and wtree::prnt_uni() methods" << std::endl;

	wic::wtree wtree(16, 16, 3);

	// Эталонные значения
	const wic::p_t good_11_03(5, 1);
	const wic::p_t good_07_15(3, 7);
	const wic::p_t good_03_02(1, 0);
	const wic::p_t good_02_01(0, 1);
	const wic::p_t good_00_02(0, 0);

	// тестирование wic::wtree::prnt()
	std::cout << "wic::wtree::prnt()" << std::endl;

	bool prnt_ok = true;
	const wic::p_t calc_11_03a = wtree.prnt(wic::p_t(11,  3));
	const wic::p_t calc_07_15a = wtree.prnt(wic::p_t( 7, 15));

	if (!_compare_points(good_11_03, calc_11_03a, "11_03")) prnt_ok = false;
	if (!_compare_points(good_07_15, calc_07_15a, "07_15")) prnt_ok = false;

	// тестирование wic::wtree::prnt_uni()
	std::cout << "wic::wtree::prnt_uni()" << std::endl;

	bool prnt_uni_ok = true;
	const wic::p_t calc_11_03b = wtree.prnt_uni(wic::p_t(11,  3));
	const wic::p_t calc_07_15b = wtree.prnt_uni(wic::p_t( 7, 15));
	const wic::p_t calc_03_02b = wtree.prnt_uni(wic::p_t( 3,  2));
	const wic::p_t calc_02_01b = wtree.prnt_uni(wic::p_t( 2,  1));
	const wic::p_t calc_00_02b = wtree.prnt_uni(wic::p_t( 0,  2));

	if (!_compare_points(good_11_03, calc_11_03b, "11_03")) prnt_uni_ok = false;
	if (!_compare_points(good_07_15, calc_07_15b, "07_15")) prnt_uni_ok = false;
	if (!_compare_points(good_03_02, calc_03_02b, "03_02")) prnt_uni_ok = false;
	if (!_compare_points(good_02_01, calc_02_01b, "02_01")) prnt_uni_ok = false;
	if (!_compare_points(good_00_02, calc_00_02b, "00_02")) prnt_uni_ok = false;

	return (prnt_ok && prnt_uni_ok);
}
