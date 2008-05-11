#pragma once

#ifndef QATEST_TEST_ACODER_INCLUDED
#define QATEST_TEST_ACODER_INCLUDED

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


////////////////////////////////////////////////////////////////////////////////
// public function declaration

//! \brief Интеграционный тест аркодера
bool test_acoder_integration();

//!	\brief Проверяет работоспособность новых функций загрузки и сохранения
//!	моделей арифметического кодера, которые основаны на использовании функции
//!	update_model()
bool test_acoder_dummy_saves();



#endif	// QATEST_TEST_ACODER_INCLUDED