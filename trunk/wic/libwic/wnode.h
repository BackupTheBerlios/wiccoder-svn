/*******************************************************************************
* file:         wnode.h                                                        *
* version:      0.1.0                                                          *
* author:       mice (mailto:my_mice@mail.ru, ICQ:332-292-380)                 *
* description:  not available                                                  *
* tests:        none                                                           *
*******************************************************************************/

#pragma once

#ifndef WIC_LIBWIC_WNODE
#define WIC_LIBWIC_WNODE

////////////////////////////////////////////////////////////////////////////////
// headers

// standard C++ library headers
#include <assert.h>

// libwic headers
#include <wic/libwic/types.h>


////////////////////////////////////////////////////////////////////////////////
// wic namespace
namespace wic {


////////////////////////////////////////////////////////////////////////////////
// types


////////////////////////////////////////////////////////////////////////////////
// wnode struct declaration

//! \brief Структура представляет собой узел дерева вейвлет коэффициентов
/*!
*/
struct wnode {
	w_t w;
	w_t wc;
	j_t j0;
	j_t j1;
	n_t n;
};



}	// end of namespace wic



#endif	// WIC_LIBWIC_WNODE
