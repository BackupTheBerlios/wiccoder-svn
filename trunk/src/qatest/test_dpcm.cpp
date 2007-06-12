/*!	\file     test_dpcm.cpp
	\version  0.0.1
	\author   mice, ICQ: 332-292-380, mailto:wonder.mice@gmail.com
	\brief    Тесты класса wic::dpcm

	\todo     Более подробно описать файл test_dpcm.cpp
*/


////////////////////////////////////////////////////////////////////////////////
// headers

// standard C++ library headers
#include <iostream>
#include <vector>

// external libraries headers
// none

// libwic headers
#include <wic/libwic/dpcm.h>

// qatest headers
#include "test_dpcm.h"


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
bool test_dpcm()
{
	// Что тестируется
	std::cout << "wic::dpcm() class" << std::endl;

	static const char *const data	= "Lazy dog jump over freaky fox...";

	bool ok = true;

	wic::dpcm<int> dpcm;

	dpcm.encode_start(3);
	dpcm.decode_start(3);
	for (int i = 0; strlen(data) > i; ++i)
	{
		if (data[i] != dpcm.decode(dpcm.encode(data[i])))
		{
			if (ok)
			{
				std::cout << "Errors in:" << std::endl;
				ok = false;
			}

			std::cout << "\tdata[" << std::setw(4) << i << "] = '";
			std::cout << data[i] << "'" << std::endl;
		}
	}
	dpcm.encode_stop();
	dpcm.decode_stop();

	return ok;
}
