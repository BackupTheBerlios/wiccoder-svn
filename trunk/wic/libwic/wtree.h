/*******************************************************************************
* file:         wtree.h                                                        *
* version:      0.1.0                                                          *
* author:       mice (mailto:my_mice@mail.ru, ICQ:332-292-380)                 *
* description:  not available                                                  *
* tests:        none                                                           *
*******************************************************************************/

#pragma once

#ifndef WIC_LIBWIC_WTREE
#define WIC_LIBWIC_WTREE

////////////////////////////////////////////////////////////////////////////////
// include

// standard C++ library headers
#include <new>							// for std::bad_alloc exception class
#include <assert.h>

// libwic headers
#include <wic/libwic/types.h>
#include <wic/libwic/wnode.h>
#include <wic/libwic/subbands.h>
#include <wic/libwic/iterators.h>


////////////////////////////////////////////////////////////////////////////////
// wic namespace
namespace wic {


////////////////////////////////////////////////////////////////////////////////
// types


////////////////////////////////////////////////////////////////////////////////
// wtree class declaration

//! \brief ������ �������� ������� �������������
/*! ������� ����� ������ ������ ����������, ���������� �������� �������
	������������� � �������� � �� ���������. ��� ������� ������ �����
	������������� ��������� �����������:
	- �������� ������������ �������� ������� �������������
	- �������� ���������������� �������� ������� �������������
	- ��� �������� ������� �������� (��� ������� �������� ����� �
	  ��� ������� � ����������)
	- ��������� ������� ���������� ������
	- ������� ���������� ��� ���������� ������������

	������� ������ ���� ��������� ������ ���������� ����������. �����������
	����� �������������� ����������� ������ ������. �������� RD-�������
	�������� ����������� ��� ���������� ������������ ������.

	����� ����� ������ ���������� � ��������� ����������.

	\sa wnode, subbands
*/
class wtree {
public:
	// public types ------------------------------------------------------------

	//! \brief ��������� ��� ������ ��� ����������, ������������ ��� wic::p_t
	typedef some_iterator<p_t> coefs_iterator;

	// public constants --------------------------------------------------------

	// public methods ----------------------------------------------------------

	//!	\name ������������ � �����������
	//@{

	//!	\brief ������������
	wtree(const sz_t width, const sz_t height, const sz_t lvls);

	//!	\brief ����������
	~wtree();

	//@}

	//! \name ����� ���������� � ������
	//@{

	//! \brief ���������� ���������� ������������� �� ��� ������
	sz_t coefs() const { return (_width * _height); }

	//! \brief ���������� ���������� ���� ���������� ��������
	sz_t nodes_sz() const;

	//! \brief ���������� �������������� ������������
	/*!	\return ������������, ������� ��� ����������� ��� �����������
		����� ������ c ������� wtree::quantize
	*/
	const q_t &q() const { return _q; }

	//@}

	//!	\name ������ � ����� �������

	//@{

	//! \brief ��������� ������ �� ������
	void load(const w_t *const from);

	//! \brief ���������� �����������
	void quantize(const q_t q = 1);

	//! \brief ��������� ������, �������������� ����������� �����,
	//! ����������� �������� ����� wnode::wq � wnode::wk � wnode::w
	//! � ������� ��������� ����.
	void refresh();

	//@}

	//!	\name ������ � ���������� � ���������
	//{@

	//! \brief ���������� ���������� � ���������
	subbands &sb();

	//! \brief ���������� ���������� � ���������
	const subbands &sb() const;

	//@}

	//! \name ������ � ��������� ������
	//@{

	//! \brief ��������� �������� ������� �� �����������
	const wnode &at(const sz_t x, const sz_t y) const;
	
	//! \brief ��������� �������� ������� �� �����������
	wnode &at(const sz_t x, const sz_t y);

	//! \brief ��������� �������� ������� �� �����������
	const wnode &at(const p_t &p) const { return at(p.x, p.y); }
	
	//! \brief ��������� �������� ������� �� �����������
	wnode &at(const p_t &p) { return at(p.x, p.y); }

	//! \brief ��������� ��������� ��������
	p_t get_pos(const wnode &node) const;

	//!	\brief ���������� ���������� ������������� ��������
	p_t prnt(const p_t &c);

	//! \brief ��������� ������ �������������
	void cut(const p_t &p);

	//! \brief ��������� �������� �������� ��������, �������� ���
	//!	�������������. ���� �������� � ������ ������������ �� ����������,
	//! ����� ���������� �������� �� ���������.
	/*!	\param[in] x ���������� \c x ��������
		\param[in] y ���������� \c y ��������
		\param[in] def �������� �� ���������, ������� �������, ����
		�������� � ������ ������������ �� ����������
		\return �������� �������� ��� �������� �� ���������, ����
		������� �� ����������.
	*/
	template <const wnode::wnode_members member>
	typename wnode::type_selector<member>::result get_safe(
			const sz_t x, const sz_t y,
			const typename wnode::type_selector<member>::result &def = 0)
	{
		if (0 > x || x >= _width) return def;
		if (0 > y || y >= _height) return def;

		return  _nodes[x + _width * y].get<member>();
	}

	//! \brief ��������� �������� �������� ��������, �������� ���
	//!	���������� � ��������� ��������. ���� ������� � ������
	//!	������������ ��������� �� ��������� ��������, ����� ����������
	//!	�������� �� ���������.
	/*!	\param[in] x ���������� \c x ��������
		\param[in] y ���������� \c y ��������
		\param[in] sb ������� � ������� ������ ��������� �������.
		\param[in] def �������� �� ���������, ������� �������, ����
		�������� � ������ ������������ �� ����������
		\return �������� �������� ��� �������� �� ���������, ����
		������� �� ����������.
	*/
	template <const wnode::wnode_members member>
	typename wnode::type_selector<member>::result get_safe(
			const sz_t x, const sz_t y,
			const subbands::subband_t &sb,
			const typename wnode::type_selector<member>::result &def = 0)
	{
		if (sb.x_min > x || x > sb.x_max) return def;
		if (sb.y_min > y || y > sb.y_max) return def;

		return  _nodes[x + _width * y].get<member>();
	}

	//@}

	//! \name ������� ���������� �������
	//@{

	//! \brief ����������� �������� ���������� �������� <i>P<sub>i</sub></i>
	/*!	\param[in] x ���������� x ������ ����� 3x3
		\param[in] y ���������� y ������ ����� 3x3
		\param[in] sb �������������� �������

		��. ������� (4) �� 35.pdf
	*/
	template <const wnode::wnode_members member>
	pi_t calc_pi(const sz_t x, const sz_t y,
				 const subbands::subband_t &sb)
	{
		const pi_t i1	=	get_safe<member>(x - 1, y,     sb) + 
							get_safe<member>(x + 1, y,     sb) +
						 	get_safe<member>(x    , y - 1, sb) + 
							get_safe<member>(x    , y + 1, sb);

		const pi_t i2	=	get_safe<member>(x + 1, y + 1, sb) + 
							get_safe<member>(x + 1, y - 1, sb) +
						 	get_safe<member>(x - 1, y + 1, sb) + 
							get_safe<member>(x - 1, y - 1, sb);

		return pi_t(4 * get_safe<member>(x, y, sb) + 2 * i1 + i2) / 16;
	}

	//! \brief ����������� �������� ���������� �������� <i>S<sub>j</sub></i>
	/*!	\param[in] x ���������� x "������" ����� 2x2
		\param[in] y ���������� y "������" ����� 2x2
		\param[in] sb �������������� �������
		\param[in] going_left \c true ���� ����������� ������ �����,
		����� \c false. � ����������� �� ����� ��������� �������������� �����
		����� �����.

		��. ������� (6) �� 35.pdf
	*/
	template <const wnode::wnode_members member>
	pi_t calc_sj(const sz_t x, const sz_t y,
				 const subbands::subband_t &sb,
				 const bool going_left)
	{
		// �������� ��� ������� �������������
		static const dsz_t	top		= (-1);
		// �������� ��� ������� �������������
		       const dsz_t	side	= (going_left)? (+1): (-1);

		// ������� ���������� �����
		const pi_t sum = 0.4 * pi_t(get_safe<member>(x + side, y + top, sb)) +
							   pi_t(get_safe<member>(x + side, y      , sb)) +
							   pi_t(get_safe<member>(x       , y + top, sb));

		// ������������ �����������
		const p_t p = prnt(p_t(x, y));
		const subbands::subband_t &prnt_sb = *(sb.prnt);

		return (0.36 * pi_t(calc_pi<member>(p.x, p.y, prnt_sb)) +
				1.06 * sum);
	}

	//! \brief ����������� �������� ���������� �������� <i>S<sub>j</sub></i>
	/*!	\param[in] x ���������� x "������" ����� 2x2
		\param[in] y ���������� y "������" ����� 2x2
		\param[in] sb �������������� �������

		������������� ���������� ����������� ������, ��������� ��� �����
		������� _going_left().

		��. ������� (6) �� 35.pdf
	*/
	template <const wnode::wnode_members member>
	pi_t calc_sj(const sz_t x, const sz_t y,
				 const subbands::subband_t &sb)
	{
		return calc_sj<member>(x, y, sb, _going_left(x, y));
	}

	//@}

	//!	\name ���������� ����������
	//@{

	//! \brief ���������� �������� �� ��������
	coefs_iterator iterator_over_subband(const subbands::subband_t &sb);

	//! \brief ���������� �������� �� �������� �������������
	coefs_iterator iterator_over_children(const p_t &prnt);

	//@}

protected:
	// protected methods -------------------------------------------------------

	//!	\name ������� ���������� � ����� �������
	//@{

	//! \brief ��������� ��� ���������� � �������� � 0
	void _reset_trees_content();

	//@}

	//!	\name ������� ��� ������ � ���������� ��������������
	//@{

	//!	\brief ���������� ���������� �������� ������ �������� �� ������
	//!	�������� ���������
	p_t _children_top_left(const p_t &prnt);

	//!	\brief �� ��������� �������� �������� �������������� �����������
	//!	������
	bool _going_left(const sz_t x, const sz_t y);

	//@}

private:
	// private data ------------------------------------------------------------

	//! \brief ������ ����� �������
	sz_t _width;

	//! \brief ������ ����� �������
	sz_t _height;

	//! \brief ���������� ������� ���������� �������
	sz_t _lvls;

	//!	\brief ������������, ������� ��� ����������� � ��������� ���
	q_t	_q;

	//! \brief ����� �������
	wnode *_nodes;

	//! \brief �������� ����������
	subbands *_subbands;
};



}	// end of namespace wic



#endif	// WIC_LIBWIC_WTREE
