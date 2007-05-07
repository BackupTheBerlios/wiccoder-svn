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
	�������������� ��� �������� ��������� ���������. ��������� ���� ������
	������������:
	- ��� \c size_type, ������������ ���, ������������ ��� ���������
	- ����� size_type x() ������������ ���������� x
	- ����� size_type y() ������������ ���������� y
*/
template <class point_t>
class basic_square_iterator: public basic_iterator<point_t> {
public:
	// public types ------------------------------------------------------------

	//! \brief ��� ������������ ��� ������������� ��������� ���������
	typedef point_t point_type;

	// public methods ----------------------------------------------------------

	//! \brief �����������
	/*!	\param[in] start ��������� �����, � ������� ������� ��������
		\param[in] top_left ���������� ��������-������ ���� ���������������
		�������������� 
		\param[in] bottom_right ���������� �������-������� ����
		��������������� ��������������

		���������� ����������� ������������, �.�. ��� ���������� ���������
		�� �������� ����� ��������� �������.
	*/
	basic_square_iterator(const point_t &start,
						  const point_t &top_left,
						  const point_t &bottom_right):
		_point(start),
		_top_left(top_lef), _bottom_right(bottom_right)
	{
		assert(_top_left.getx()	<= _point.getx());
		assert(_point.getx()	<= _bottom_right.getx());
		assert(_top_left.gety()	<= _point.gety());
		assert(_point.gety()	<= _bottom_right.gety());
	}

	//! \brief ���������� �����������
	/*!	\param[in] src �������� ������
	*/
	basic_square_iterator(const basic_square_iterator &src):
		_point(src._point),
		_top_left(src._top_lef), _bottom_right(src._bottom_right)
	{
	}

	//!	\brief ����������
	~basic_square_iterator();

	//! \brief ����������� ������������ basic_iterator::get()
	virtual const point_t &get() const {
		return _point;
	}

protected:
	// protected methods--------------------------------------------------------

	//!	\brief �������� ������� ���������� �����
	/*!	\param[in] x �������� ������ �� ��� x
		\return \c true ���� ����� ����� �������� � ��� ����������, �����
		\c false
	*/
	bool _move_left(const typename point_t::size_type &x) {
		if (_top_left.getx() <=_point.getx() - x) return false;

		_point.getx() -= x;

		return true;
	}

	//!	\brief �������� ������� ���������� ������
	/*!	\param[in] x �������� ������ �� ��� x
		\return \c true ���� ����� ����� �������� � ��� ����������, �����
		\c false
	*/
	bool _move_right(const typename point_t::size_type &x) {
		if (_point.getx() + x <= _bottom_right.getx()) return false;

		_point.getx() += x;

		return true;
	}

	//!	\brief �������� ������� ���������� �����
	/*!	\param[in] y �������� ������ �� ��� y
		\return \c true ���� ����� ����� �������� � ��� ����������, �����
		\c false
	*/
	bool _move_up(const typename point_t::size_type &y) {
		if (_top_left.gety() <=_point.gety() - y) return false;

		_point.gety() -= y;

		return true;
	}

	//!	\brief �������� ������� ���������� ����
	/*!	\param[in] y �������� ������ �� ��� y
		\return \c true ���� ����� ����� �������� � ��� ����������, �����
		\c false
	*/
	bool _move_down(const typename point_t::size_type &y) {
		if (_point.gety() + y <= _bottom_right.gety()) return false;

		_point.gety() += y;

		return true;
	}

private:
	// private data ------------------------------------------------------------

	//! \brief ������� �������� ���������
	point_t _point;

	//! \brief ���������� ������-�������� ���� ��������������� ��������������
	point_t _top_left;

	//! \brief ���������� �������-������� ���� ��������������� ��������������
	point_t _bottom_right;

};


class snake_square_iterator: public basic_square_iterator<p_t> {
};



}	// wtc namespace



#endif	// WIC_LIBWIC_ITERATORS_INCLUDED
