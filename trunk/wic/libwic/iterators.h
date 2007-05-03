/*******************************************************************************
* file:         iterators.h                                                    *
* version:      0.1.0                                                          *
* author:       mice (mailto:my_mice@mail.ru, ICQ:332-292-380)                 *
* description:  not available                                                  *
* tests:        none                                                           *
*******************************************************************************/

#pragma once

#ifndef WIC_LIBWIC_ITERATORS_INCLUDED
#define WIC_LIBWIC_ITERATORS_INCLUDED

////////////////////////////////////////////////////////////////////////////////
// headers

// standard C++ library headers
#include <assert.h>

// libwic headers
#include <wic/libwic/types.h>
#include <wic/libwic/subbands.h>


////////////////////////////////////////////////////////////////////////////////
// wic namespace
namespace wic {


////////////////////////////////////////////////////////////////////////////////
// basic_2d_iterator class declaration
//! \brief ������� ����� ��� ��������� ����������
/*!	
*/
class basic_2d_iterator {
	public:
	// public methods ----------------------------------------------------------

	//! \brief �����������
	basic_2d_iterator(const sz_t x, const sz_t y);

	//! \brief �����������
	basic_2d_iterator(const p_t &p);

	//!	\brief ���������� �����������
	basic_2d_iterator(const basic_2d_iterator &i);

	//!	\brief ����������
	~basic_2d_iterator();

	//! \brief ���������� ����������� ������ �� ������� �������� ���������
	const sz_t &p() const { return _p; }

	//! \brief ���������� ������ �� ������� �������� ���������
	sz_t &p() { return _p; }

protected:
private:
	// private data ------------------------------------------------------------

	//! \brief ������� ���������� ���������
	p_t _p;
};


////////////////////////////////////////////////////////////////////////////////
// subband_iterator class declaration

//! \brief
class subband_iterator: public basic_2d_iterator {
public:
	// public methods ----------------------------------------------------------

	//! \brief �����������
	subband_iterator(const subbands::subband_t &subband);

	//!	\brief ����������
	~subband_iterator();

protected:
private:
	// private data ------------------------------------------------------------

};



}	// wtc namespace



#endif	// WIC_LIBWIC_ITERATORS_INCLUDED
