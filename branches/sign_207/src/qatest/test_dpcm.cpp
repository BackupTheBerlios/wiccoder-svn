/*!	\file     test_dpcm.cpp
	\version  0.0.1
	\author   mice, ICQ: 332-292-380, mailto:wonder.mice@gmail.com
	\brief    ����� ������ wic::dpcm

	\todo     ����� �������� ������� ���� test_dpcm.cpp
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
	// ��� �����������
	std::cout << "wic::dpcm() class" << std::endl;

	static const char *const data	= "Lazy dog jump over freaky fox...";

	bool ok = true;

	typedef wic::dpcm dpcm;

	for (int i = 0; int(strlen(data)) > i; ++i)
	{
		/*
		if (data[i] != dpcm::decode(dpcm::encode(data[i], 0), 0))
		{
			if (ok)
			{
				std::cout << "Errors in:" << std::endl;
				ok = false;
			}

			std::cout << "\tdata[" << std::setw(4) << i << "] = '";
			std::cout << data[i] << "'" << std::endl;
		}
		*/
	}

	return ok;
}
