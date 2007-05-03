/*******************************************************************************
* file:         encoder.h                                                      *
* version:      0.1.0                                                          *
* author:       mice (mailto:my_mice@mail.ru, ICQ:332-292-380)                 *
* description:  not available                                                  *
* tests:        none                                                           *
*******************************************************************************/

#pragma once

#ifndef WIC_LIBWIC_ENCODER
#define WIC_LIBWIC_ENCODER

////////////////////////////////////////////////////////////////////////////////
// include

// standard C++ library headers
#include <assert.h>

// libwic headers
#include <wic/libwic/types.h>
#include <wic/libwic/wtree.h>


////////////////////////////////////////////////////////////////////////////////
// wic namespace
namespace wic {


////////////////////////////////////////////////////////////////////////////////
// types


////////////////////////////////////////////////////////////////////////////////
// encoder class declaration

//! \brief 
/*! 
*/
class encoder {
public:
	// public constants --------------------------------------------------------
	// public types ------------------------------------------------------------
	// public methods ----------------------------------------------------------

	//!	\brief Конструкртор
	encoder();

	//!	\brief Деструктор
	~encoder();

	void encode();

protected:
	// protected methods -------------------------------------------------------

private:
	// private data ------------------------------------------------------------

};



}	// end of namespace wic



#endif	// WIC_LIBWIC_ENCODER
