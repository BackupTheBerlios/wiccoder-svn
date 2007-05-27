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
#include <new>							// for std::bad_alloc exception class
#include <assert.h>
#include <vector>

// libwic headers
#include <wic/libwic/types.h>


////////////////////////////////////////////////////////////////////////////////
// wic namespace
namespace wic {


////////////////////////////////////////////////////////////////////////////////
// basic_iterator class declaration
//! \brief ������� ����� ��� ����������
/*!	������������� ����� ��������� ��� ���������.

	� �������� �������� ����� ���������� ������ ���� <i>ival_t</i>
*/
template <class ival_t>
class basic_iterator {
	public:
	// public types ------------------------------------------------------------

	//! \brief ��������� ��� ��������� �������, ������� �������� ���
	//!	������������� ������
	typedef ival_t ival_type;

	// public methods ----------------------------------------------------------

	//!	\brief ����������. �����������, ��� ����������.
	virtual ~basic_iterator() {}

	//! \brief ���������� ����������� ������ �� ������� �������� ���������
	/*!	\return ������� �������� ���������
	*/
	virtual const ival_t &get() const = 0;

	//! \brief ��������� � ���������� ��������
	/*!	\return ����� �������� ��������� �������� ���������

		�������� ��������� ����� ���� �������� (��� �� ������������), ����
		��������� ����� ������������������ �������� �
		basic_iterator::end() ���������� <i>true</i>. ����� �������, �����
		������� ������ basic_iterator::next() ���������� ���������
		������� ���������� ������������������ ��������.
	*/
	virtual const ival_t &next() = 0;

	//! \brief ���������, ����������� �� ��������
	/*!	\return \c true ���� �������� ����� ������������������
	*/
	virtual const bool end() const = 0;

};



////////////////////////////////////////////////////////////////////////////////
// base_square_iterator class declaration
//! \brief �������� �� ��������� ������������� �������
/*! ������������� ���������� ���������� ��������� �� ������������� ���������
	�������.

	�����-�������� ������� \c point_t ���������� ���, ������� �����
	�������������� ��� �������� ��������� ���������. ��������� ���� ������
	������������:
	- ��� \c size_type, ������������ ���, ������������ ��� ���������
	- ����� size_type getx() ������������ ���������� x
	- ����� size_type gety() ������������ ���������� y

	\todo ������������� basic_square_iterator � basic_2d_iterator
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
		_top_left(top_left), _bottom_right(bottom_right)
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
		_top_left(src._top_left), _bottom_right(src._bottom_right)
	{
	}

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
	bool _move_left(const typename point_t::size_type &x = 1) {
		if (_top_left.getx() + x > _point.getx()) return false;

		_point.getx() -= x;

		return true;
	}

	//!	\brief �������� ������� ���������� ������
	/*!	\param[in] x �������� ������ �� ��� x
		\return \c true ���� ����� ����� �������� � ��� ����������, �����
		\c false
	*/
	bool _move_right(const typename point_t::size_type &x = 1) {
		if (_point.getx() + x > _bottom_right.getx()) return false;

		_point.getx() += x;

		return true;
	}

	//!	\brief �������� ������� ���������� �����
	/*!	\param[in] y �������� ������ �� ��� y
		\return \c true ���� ����� ����� �������� � ��� ����������, �����
		\c false
	*/
	bool _move_up(const typename point_t::size_type &y = 1) {
		if (_top_left.gety() + y > _point.gety()) return false;

		_point.gety() -= y;

		return true;
	}

	//!	\brief �������� ������� ���������� ����
	/*!	\param[in] y �������� ������ �� ��� y
		\return \c true ���� ����� ����� �������� � ��� ����������, �����
		\c false
	*/
	bool _move_down(const typename point_t::size_type &y = 1) {
		if (_point.gety() + y > _bottom_right.gety()) return false;

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



////////////////////////////////////////////////////////////////////////////////
// snake_square_iterator class declaration
//!	\brief ��������, ���������� ������������� ������� �������
/*! ������� ������������ �������:
	\code
	- - - - - - - - >
	                |
	                V
	< - - - - - - - -
	|
	V
	- - - - - - - - >
	                |
			        V
	< - - - - - - - -
	\endcode
*/
class snake_square_iterator: public basic_square_iterator<p_t> {
public:
	// public methods ----------------------------------------------------------

	//! \brief �����������
	/*!	\param[in] top_left ���������� ��������-������ ���� ���������������
		�������������� 
		\param[in] bottom_right ���������� �������-������� ����
		��������������� ��������������

		���������� ����������� ������������, �.�. ��� ���������� ���������
		�� �������� ����� ��������� �������.

		� �������� ��������� ����� ����� ���� �������-����� ����.

		\sa basic_square_iterator
	*/
	snake_square_iterator(const point_type &top_left,
						  const point_type &bottom_right):
		basic_square_iterator<point_type>(top_left, top_left, bottom_right)
	{
		_points_left = (bottom_right.getx() -  top_left.getx() + 1) *
					   (bottom_right.gety() -  top_left.gety() + 1);

		_going_left = false;
	}

	//! \brief ���������� �����������
	/*!	\param[in] src �������� ������
	*/
	snake_square_iterator(const snake_square_iterator &src):
		basic_square_iterator<point_type>(src), _points_left(src._points_left),
		_going_left(src._going_left)
	{
	}

	//! \brief ����������� ������������ basic_iterator::next()
	virtual const point_type &next() {
		if (_going_left) {
			if (_move_left()) --_points_left;
			else if (_move_down()) {
				--_points_left;
				_going_left = false;
			} else _points_left = 0;
		} else {
			if (_move_right()) --_points_left;
			else if (_move_down()) {
				--_points_left;
				_going_left = true;
			} else _points_left = 0;
		}

		return get();
	}

	//! \brief ����������� ������������ basic_iterator::end()
	virtual const bool end() const {
		return (0 >= _points_left);
	}

	//! \brief ���������� ���������� �����
	const sz_t &points_left() const { return _points_left; }

	//!	\brief ������� ����������� �������
	bool going_left() const { return _going_left; }

protected:
private:
	// private data ------------------------------------------------------------

	//!	\brief ���������� ��������, ���������� �� �����
	sz_t _points_left;

	//!	\brief ������� ����������� �����������.
	bool _going_left;

};



////////////////////////////////////////////////////////////////////////////////
// snake_square_iterator class declaration
//!	\brief �������� �� �������� ��������� �������� �� <i>LL</i> ��������.
/*! �������� �� 3�� �������� ��������� �������� �� <i>LL</i> ��������.

	\note �� ������ ������ ���������� ����� ������ ������. ��� ����������
	����������� ����������� std::vector � � �������������� ��������� ������
	�������. �� ���-��� ���������� ������������� � <i>LL</i> �������� ������
	�� ��� ������, ������� ���� �� ��� ����.

	\todo ��������� ����� ��������� ���������� ����� ������
	\todo ���� ����� ���������� ��������������
*/
class LL_children_iterator: public basic_iterator<p_t> {
public:
	// public constants --------------------------------------------------------

	//! \brief ���������� �������� ���������
	static const sz_t LL_CHILDREN_COUNT		= 3;

	// public methods ----------------------------------------------------------

	//! \brief �����������
	/*! \param[in] LL_w ������ <i>LL</i> �������� (� ���������)
		\param[in] LL_h ������ <i>LL</i> �������� (� ���������)
		\param[in] p ���������� ������������� ��������
	*/
	LL_children_iterator(const sz_t &LL_w, const sz_t &LL_h,
						 const p_t &p):
		_i(LL_CHILDREN_COUNT - 1)
	{
		_children.reserve(LL_CHILDREN_COUNT);
		_children.push_back(p_t(p.x + LL_w, p.y + LL_h));
		_children.push_back(p_t(p.x       , p.y + LL_h));
		_children.push_back(p_t(p.x + LL_w, p.y));
	}

	//! \brief ����������� ������������ basic_iterator::get()
	virtual const p_t &get() const { return _children[_i]; }

	//! \brief ����������� ������������ basic_iterator::next()
	virtual const p_t &next() {
		if (0 < _i) --_i;

		return get();
	}

	//! \brief ����������� ������������ basic_iterator::end()
	virtual const bool end() const {
		return (0 == _i);
	}

protected:
	// protected types ---------------------------------------------------------

	//!	\brief ��� ��� ������� ��������� �������� ���������
	typedef std::vector<p_t> _children_t;

private:
	// private data ------------------------------------------------------------

	//!	\brief ������ �������� ��������� �������� � ������� ��������� ��������
	//!	���������.
	_children_t::size_type _i;

	//!	\brief ������� ��������� �������� ���������.
	_children_t _children;

};



////////////////////////////////////////////////////////////////////////////////
// n_cutdown_iterator class declaration
//!	\brief �������� �� ������������ ��������� �������� ���������� ������
/*!	���������� ����������� ���������� ���������� ����������� ������. � �����
	������ ��� ����� ���������. �������� ���������� ���������� ��������:
	�������������� ���������� ���������� �������� ����������.
*/
class n_cutdown_iterator: public basic_iterator<n_t>
{
public:
	// public constants --------------------------------------------------------

	//!	\brief ����� ��������� ���������� � ���������� ������ ��� ����������
	//!	������������� �������� �� ������� �������� (2^3)
	static const n_t MAX_VARIATIONS_LVL_0	= 8;

	//!	\brief ����� ��������� ���������� � ���������� ������ ��� ����������
	//!	������������� �������� �� ��������� �������� (2^4)
	static const n_t MAX_VARIATIONS_LVL_X	= 16;

	// public methods ----------------------------------------------------------

	//!	\brief �����������
	/*!	\param[in] lvl �������, �� ������� ��������� �������� � ���������������
		��������� ��������� ����������. �������� � �������� ������ ����� 3
		�������, ������������� �������� 8 ��������� �������� �� ����������. �
		���� ��������� ��������� 4 ������� � 16 ��������� ������� ����������.
	*/
	n_cutdown_iterator(const sz_t lvl): _end(false) {
		assert(0 <= lvl);

		_n = (0 == lvl)? (MAX_VARIATIONS_LVL_0 - 1): (MAX_VARIATIONS_LVL_X - 1);
	}

	//! \brief ����������� ������������ basic_iterator::get()
	virtual const n_t &get() const { return _n; }

	//! \brief ����������� ������������ basic_iterator::next()
	virtual const n_t &next() {
		(0 != _n)? --_n: _end = true;

		return _n;
	}

	//! \brief ����������� ������������ basic_iterator::end()
	virtual const bool end() const { return _end; }

protected:
private:
	// private data ------------------------------------------------------------

	//!	\brief ������� �������� ���������� �������� ���������� ������
	n_t _n;

	//!	\brief ������� ���������� ������������������ ��������
	bool _end;
};



////////////////////////////////////////////////////////////////////////////////
// some_iterator class declaration
//!	\brief ������������� ������ ��� ����������
/*!	���� ����� ������������ ��� ���������� ����. ������� ������ ����������
	������� ����� ������ ������ ����� ����������. ���� ��������� ����� �������,
	������� �� ����� ��� ���������� ����� ��������� ������� - ����������
	���������� ������� ������������ ���������.

	\attention �� ������������� ��������� ��������� � ���� ��� ���
	������������. ������ ����� ����� ��������� �������, ������������ ���������.

	\attention ����� ����� ���������� �������� �������� ���������� ������ ��
	�������������� ������. ������� ����� �������� ������������ ����������
	�������������.

	\sa basic_iterator
*/
template <class ival_t>
class some_iterator {
public:
	// public types ------------------------------------------------------------

	//! \brief ��� ��������������� ���������
	typedef basic_iterator<ival_t> iterator_type;

	//! \brief ��� ����� ��������� (��� ��������)
	typedef some_iterator<ival_t> self_type;

	// public constants --------------------------------------------------------

	//! \brief ��������� �������� ��� �������� ������
	static const int INIT_REF_COUNT			= 1;

	// public methods ----------------------------------------------------------

	//! \brief �����������
	/*!	\param[in] iterator ������ ��������� ��� ������� ������ �������

		������� ������ ������� ������ �� �������� � �������������� ��� � 1
	*/
	some_iterator(iterator_type *const iterator):
		_iterator(iterator),
		_ref_count_ptr(new int(INIT_REF_COUNT))
	{
		assert(0 != _iterator);

		if (0 == _ref_count_ptr) throw std::bad_alloc();
	}

	//! \brief ���������� �����������
	/*! \param[in] src �������� ��������

		����������� �������� �������� ������ �� �������
	*/
	some_iterator(const self_type &src):
		_iterator(src._iterator),
		_ref_count_ptr(src._ref_count_ptr)
	{
		assert(0 != _iterator);
		assert(0 != _ref_count_ptr);

		++(*_ref_count_ptr);
	}


	//! \brief ����������
	/*!	��������� �������� �������� ������ �� ������� � ���� ���
		���������� ������ ���� - ����������� �������������� ��������
		� ������� ������.
	*/
	~some_iterator() {
		assert(0 != _ref_count_ptr);

		if (0 < --(*_ref_count_ptr)) return;

		if (0 != _iterator) delete _iterator;
		delete _ref_count_ptr;
	}

	//! \brief ���������� ��������� �� �������������� ��������
	iterator_type *operator->() const { return _iterator; }

	//! \brief ���������� ��������� �� �������������� ��������
	iterator_type *get() const { return _iterator; }

	//! \brief ���������� ������ �� �������������� ��������
	iterator_type &operator()() const { return (*_iterator); }

private:
	// private data ------------------------------------------------------------

	//! \brief ��������� �� �������������� ��������
	iterator_type *const _iterator;

	//! \brief ��������� �� �������� ���������� ������� ������ ��
	//!	�������������� ��������
	int *const _ref_count_ptr;

};



}	// wic namespace



#endif	// WIC_LIBWIC_ITERATORS_INCLUDED
