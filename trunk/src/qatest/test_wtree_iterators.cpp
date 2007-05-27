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
#include <vector>

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

namespace std {

//!	\brief �������� == ��� ���� <i>p_t</i> ��������� ���������
bool operator == (const wic::p_t &p1, const wic::p_t &p2);


}


////////////////////////////////////////////////////////////////////////////////
// protected function definitions

namespace std {

/*!	\param[in] p1 ��������� ���������� ������ �����
	\param[in] p2 ��������� ���������� ������ �����
	\return <i>true</i> ���� ���������� ���������, ����� <i>false</i>

	\note �������� ������� � ������������ ��� <i>std</i> ����� � ����
	����� ������ ������ ����������� ���������� <i>C++</i>
*/
bool operator == (const wic::p_t &p1, const wic::p_t &p2) {
	return ((p1.x == p2.x) && (p1.y == p2.y));
}


}


////////////////////////////////////////////////////////////////////////////////
// public function definitions

/*!
*/
bool test_n_cutdown_iterator()
{
	// ��� �����������
	std::cout << "wic::n_cutdown_iterator() class" << std::endl;

	using namespace wic;

	bool ok = true;

	// ���� ��� �������� ������: 3 �������� ��������
	std::vector<n_t> i1_good, i1_calc;
	i1_good.push_back(7);
	i1_good.push_back(6);
	i1_good.push_back(5);
	i1_good.push_back(4);
	i1_good.push_back(3);
	i1_good.push_back(2);
	i1_good.push_back(1);
	i1_good.push_back(0);
	for (wic::n_cutdown_iterator i1(0); !i1.end(); i1.next()) {
		i1_calc.push_back(i1.get());
	}

	std::cout << "n_cutdown_iterator for level 0 ";
	if (i1_good == i1_calc) {
		std::cout << "ok.";
	} else {
		std::cout << "failed.";
		ok = false;
	}
	std::cout << std::endl;

	// ���� ��� ���������� ������: 4 �������� ��������
	std::vector<n_t> i2_good, i2_calc;
	i2_good.push_back(15);
	i2_good.push_back(14);
	i2_good.push_back(13);
	i2_good.push_back(12);
	i2_good.push_back(11);
	i2_good.push_back(10);
	i2_good.push_back(9);
	i2_good.push_back(8);
	i2_good.push_back(7);
	i2_good.push_back(6);
	i2_good.push_back(5);
	i2_good.push_back(4);
	i2_good.push_back(3);
	i2_good.push_back(2);
	i2_good.push_back(1);
	i2_good.push_back(0);
	for (wic::n_cutdown_iterator i2(1); !i2.end(); i2.next()) {
		i2_calc.push_back(i2.get());
	}

	std::cout << "n_cutdown_iterator for level !0 ";
	if (i2_good == i2_calc) {
		std::cout << "ok.";
	} else {
		std::cout << "failed.";
		ok = false;
	}
	std::cout << std::endl;

	return ok;
}

/*!
*/
bool test_snake_square_iterator()
{
	// ��� �����������
	std::cout << "wic::snake_square_iterator class" << std::endl;

	using namespace wic;

	bool ok = true;

	// ��������� ����������� �� �������� 3�3.
	// ����� ������� ���� (1, 1), ������ ������ - (3, 3)
	std::vector<p_t> i1_good, i1_calc;
	i1_good.push_back(p_t(1, 1));
	i1_good.push_back(p_t(2, 1));
	i1_good.push_back(p_t(3, 1));
	i1_good.push_back(p_t(3, 2));
	i1_good.push_back(p_t(2, 2));
	i1_good.push_back(p_t(1, 2));
	i1_good.push_back(p_t(1, 3));
	i1_good.push_back(p_t(2, 3));
	i1_good.push_back(p_t(3, 3));
	for (wic::snake_square_iterator i1(p_t(1, 1), p_t(3,3)); !i1.end(); i1.next()) {
		i1_calc.push_back(i1.get());
	}

	if (i1_good == i1_calc) {
		std::cout << "ok.";
	} else {
		std::cout << "failed.";
		ok = false;
	}
	std::cout << std::endl;

	return ok;
}

/*!
*/
bool test_LL_children_iterator()
{
	// ��� �����������
	std::cout << "wic::LL_children_iterator class" << std::endl;

	using namespace wic;

	bool ok = true;

	// LL ������� ����� ������ 5�5. ��������� ����� ��
	// �������� ��������� ����� �� ������ ���������� � ����. �������:
	// ������ �������, ����� ������, ������ ������
	std::vector<p_t> i1_good, i1_calc;
	i1_good.push_back(p_t(7, 4));
	i1_good.push_back(p_t(2, 9));
	i1_good.push_back(p_t(7, 9));
	for (wic::LL_children_iterator i1(5, 5, p_t(2, 4)); !i1.end(); i1.next()) {
		i1_calc.push_back(i1.get());
	}

	if (i1_good == i1_calc) {
		std::cout << "ok.";
	} else {
		std::cout << "failed.";
		ok = false;
	}
	std::cout << std::endl;

	return ok;
}