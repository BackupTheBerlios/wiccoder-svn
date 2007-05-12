/*******************************************************************************
* file:         main.cpp                                                       *
* version:      0.1.0                                                          *
* author:       mice (mailto:my_mice@mail.ru, ICQ:332-292-380)                 *
* description:  tests of wic modules                                           *
* tests:        none                                                           *
*******************************************************************************/


////////////////////////////////////////////////////////////////////////////////
// headers

// standard C++ library headers
#include <iostream>
#include <list>

// libwic headers
// none

// qatest headers
#include "test_prediction.h"


////////////////////////////////////////////////////////////////////////////////
// public function declaration


////////////////////////////////////////////////////////////////////////////////
// public function definitions


////////////////////////////////////////////////////////////////////////////////
// main function definition
int main(int argc, char **args) {
	// types
	typedef bool (* test_func_t)();
	typedef std::list<test_func_t> tests_list_t;

	// list for tests
	tests_list_t tests_list;
	int tests_ok	= 0;
	int tests_bad	= 0;

	// populate list
	tests_list.push_back(test_wtree_calc_pi);
	tests_list.push_back(test_wtree_calc_sj);

	// for each item in list
	for (tests_list_t::const_iterator i = tests_list.begin();
		 tests_list.end() != i; ++i)
	{
		std::cout << std::endl << "> Test: ";
		const bool ok = (*i)();

		if (ok) {
			std::cout << "Test ok."<< std::endl << std::endl;
			++tests_ok;
		} else {
			std::cout << "Test failed."<< std::endl << std::endl;
			++tests_bad;
		}
	}

	// print over all results
	std::cout << "Tests results:" << std::endl;
	std::cout << "Tests ok: \t" << tests_ok << "/"
			  << (unsigned int)(tests_list.size()) << std::endl;
	std::cout << "Tests failed: \t" << tests_bad << "/"
			  << (unsigned int)(tests_list.size()) << std::endl;

	// exit
	return ((tests_ok == tests_list.size())? 0: -1);
}
