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

// libwic headers
#include <wic/libwic/subbands.h>


////////////////////////////////////////////////////////////////////////////////
// public function declaration

//! \brief Тестирует класс subbands
bool test_subbands();


////////////////////////////////////////////////////////////////////////////////
// public function definitions
bool test_subbands() {

	try {
		wic::subbands subbands();
	}
	catch (const std::exception &e) {
		std::cout << "Exception: " << e.what() << std::endl;

		return false;
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////////
// main function definition
int main(int argc, char **args) {
	return -1;
}
