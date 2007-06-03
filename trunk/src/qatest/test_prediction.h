/*******************************************************************************
* file:         test_prediction.h                                              *
* version:      0.1.0                                                          *
* author:       mice (mailto:my_mice@mail.ru, ICQ:332-292-380)                 *
* description:  tests of wic prediction routines                               *
* tests:        none                                                           *
*******************************************************************************/

#pragma once

#ifndef QATEST_TEST_PREDICTION_INCLUDED
#define QATEST_TEST_PREDICTION_INCLUDED

////////////////////////////////////////////////////////////////////////////////
// headers

// standard C++ library headers
// none

// external libraries headers
// none

// libwic headers
// none


////////////////////////////////////////////////////////////////////////////////
// public constants declarations

//!	\brief �������� ��������� ��� ����������� �����
extern const double epsilon;


////////////////////////////////////////////////////////////////////////////////
// public function declaration

//! \brief ��������� ������� ������������ Pi
bool test_wtree_calc_pi();

//! \brief ��������� ������� ������������ Sj
bool test_wtree_calc_sj();

//! \brief ��������� ������� �������� ������������ Pi �� ���� �������� ���������
bool test_wtree_calc_pi_avg();



#endif	// QATEST_TEST_PREDICTION_INCLUDED
