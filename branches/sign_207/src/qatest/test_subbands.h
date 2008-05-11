#pragma once

#ifndef QATEST_TEST_SUBBANDS_INCLUDED
#define QATEST_TEST_SUBBANDS_INCLUDED

////////////////////////////////////////////////////////////////////////////////
// headers

// standard C++ library headers
// none

// external libraries headers
// none

// libwic headers
// none


////////////////////////////////////////////////////////////////////////////////
// public function declaration

//! \brief Тестирует функцию lvl_from_point
bool test_lvl_from_point();

//! \brief Тестирует функцию from_point
//  \note Сравнивает адреса саббэндов
bool test_from_point();


#endif	// QATEST_TEST_SUBBANDS_INCLUDED
