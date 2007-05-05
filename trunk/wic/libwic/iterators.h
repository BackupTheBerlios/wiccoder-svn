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


////////////////////////////////////////////////////////////////////////////////
// wic namespace
namespace wic {


////////////////////////////////////////////////////////////////////////////////
// basic_iterator class declaration
//! \brief ������� ����� ��� ����������
/*!	������������� ����� ��������� ��� ���������.

	� �������� �������� ����� ���������� ������ ���� \� ival_t
*/
template <class ival_t>
class basic_iterator {
	public:
	// public types ------------------------------------------------------------

	//! \brief ��������� ��� ��������� �������, ������� �������� ���
	//!	������������� ������
	typedef ival_t ival_type;

	// public methods ----------------------------------------------------------

	//! \brief ���������� ����������� ������ �� ������� �������� ���������
	/*!	\return ������� �������� ���������
	*/
	virtual const ival_t &get() const = 0;

	//! \brief ��������� � ���������� ��������
	/*!	\return ����� �������� ��������� �������� ���������
	*/
	virtual const ival_t &next() = 0;

	//! \brief ���������, ����������� �� ��������
	/*!	\return \c true ���� �������� ����� ������������������
	*/
	virtual const bool end() const = 0;

};



////////////////////////////////////////////////////////////////////////////////
// base_square_iterator class declaration
//! \brief �������� �� ��������� ���������� �������
/*! ������������� ���������� ���������� ��������� �� ���������� ���������
	�������.

	�����-�������� ������� \c point_t ���������� ���, ������� �����
	�������������� ��� �������� ��������� ���������. ��� ������ ������������:
	- ��� \c size_type, ������������ ���, ������������ ��� ���������
	- ���� x
*/
template <class point_t>
class basic_square_iterator: public basic_iterator<point_t> {
public:
	// public types ------------------------------------------------------------

	//! \brief ��� ������������ ��� ������������� ��������� ���������
	typedef point_t point_type;

	// public methods ----------------------------------------------------------

	//! \brief �����������
	/*!	\param[in] top_left
		\param[in] bottom_right
	*/
	basic_square_iterator(const point_t &top_left,
						  const point_t &bottom_right):
		top_left.get_x()
	{

	}

	//! \brief ���������� �����������
	basic_square_iterator(const basic_square_iterator &src):
		_point(src._point)
	{}

	//!	\brief ����������
	~basic_square_iterator();

	//! \brief ����������� ������������ basic_iterator::get()
	virtual const ival_t &get() const = 0;

	//! \brief ����������� ������������ basic_iterator::next()
	virtual const ival_t &next() = 0;

	//! \brief ����������� ������������ basic_iterator::end()
	virtual const bool end() const = 0;

protected:
private:
	// private data ------------------------------------------------------------

	//! \brief ������� �������� ���������
	point_t _point;

};



}	// wtc namespace



#endif	// WIC_LIBWIC_ITERATORS_INCLUDED
