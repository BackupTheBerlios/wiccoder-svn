/*******************************************************************************
* file:         iterators.cpp                                                  *
* version:      0.1.0                                                          *
* author:       mice (mailto:my_mice@mail.ru, ICQ:332-292-380)                 *
* description:  not available                                                  *
* tests:        none                                                           *
*******************************************************************************/


////////////////////////////////////////////////////////////////////////////////
// include

// standard C++ headers
// none

// libwic headers
#include <wic/libwic/iterators.h>


////////////////////////////////////////////////////////////////////////////////
// wic namespace
namespace wic {


////////////////////////////////////////////////////////////////////////////////
// basic_2d_iterator class public definitions

/*!
*/
basic_2d_iterator::basic_2d_iterator(const sz_t x, const sz_t y):
	_p(x, y)
{
}


/*!
*/
basic_2d_iterator::basic_2d_iterator(const p_t &p):
	_p(p)
{
}


/*!
*/
basic_2d_iterator::basic_2d_iterator(const basic_2d_iterator &i):
	_p(i._p)
{
}



}	// end of namespace wic
