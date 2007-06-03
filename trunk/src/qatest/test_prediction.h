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

//!	\brief Точность требуемая для прохождения теста
extern const double epsilon;


////////////////////////////////////////////////////////////////////////////////
// public function declaration

//! \brief Тестирует подсчёт коэффициента Pi
bool test_wtree_calc_pi();

//! \brief Тестирует подсчёт коэффициента Sj
bool test_wtree_calc_sj();

//! \brief Тестирует подсчёт среднего коэффициента Pi по всем дочерним элементам
bool test_wtree_calc_pi_avg();



#endif	// QATEST_TEST_PREDICTION_INCLUDED
