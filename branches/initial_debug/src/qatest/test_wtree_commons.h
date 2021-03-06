/*******************************************************************************
* file:         test_wtree_commons.h                                           *
* version:      0.1.0                                                          *
* author:       mice (mailto:my_mice@mail.ru, ICQ:332-292-380)                 *
* description:  tests of wtree common methods                                  *
* tests:        none                                                           *
*******************************************************************************/

#pragma once

#ifndef QATEST_TEST_WTREE_COMMONS_INCLUDED
#define QATEST_TEST_WTREE_COMMONS_INCLUDED

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

//! \brief ��������� ������� wtree::_leafs_top_left()
bool test_wtree_leafs_top_left();

//! \brief ��������� ������� wtree::prnt() � wtree::prnt_uni()
bool test_wtree_prnt();

//! \brief T�������� ������� wtree::child_n_mask()
bool test_child_n_mask();

//! \brief ��������� ������� wtree::child_n_mask_LL()
bool test_child_n_mask_LL();

//! \brief ��������� ������� wtree::iterator_over_leafs()
bool test_iterator_over_leafs();



#endif	// QATEST_TEST_WTREE_COMMONS_INCLUDED
