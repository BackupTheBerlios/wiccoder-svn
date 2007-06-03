/*!	\file     wtree.h
	\version  0.0.1
	\author   mice, ICQ: 332-292-380, mailto:wonder.mice@gmail.com
	\brief    �������� ������ wic::wtree - ������� ������� ����������

	\todo     ����� �������� ������� ���� wtree.h
*/


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

	\note �������� �� ��������, ����� ������������ ����� ��� �������
	������� �������, � �� ���� ���������� ������ (������)

	\sa wnode, subbands
*/
class wtree {
public:
	// public types ------------------------------------------------------------

	//! \brief ��������� ��� ������ ��� ����������, ������������ ��� wic::p_t
	typedef some_iterator<p_t> coefs_iterator;

	//!	\brief ��������� ��� ������ ��� ����������, ������������ ��� wic::n_t
	typedef some_iterator<n_t> n_iterator;

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

	//!	\brief ���������� ������ ������� (� ���������)
	sz_t width() const { return _width; }

	//!	\brief ���������� ������ ������� (� ���������)
	sz_t height() const { return _height; }

	//!	\brief ���������� ���������� ������� ����������
	sz_t lvls() const { return _lvls; }

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

	//!	\name ������ � ����� ��������

	//@{

	//! \brief ��������� ������ �� ������
	void load(const w_t *const from);

	//! \brief ���������� ����������� ����� �������
	void quantize(const q_t q = 1);

	//! \brief ��������� ������, �������������� ����������� �����,
	//! ����������� �������� ����� wnode::wq � wnode::wk � wnode::w,
	//!	������������ wnode::n � ������� ��������� ����.
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

	//! \brief ��������� ��������� �������� � �������
	p_t get_pos(const wnode &node) const;

	//!	\brief ���������� ���������� ������������� ��������
	//!	(������ ��� �� <i>LL</i> ��������)
	p_t prnt(const p_t &c);

	//!	\brief ���������� ���������� ������������� ��������
	//! (������������� ������ ��� ���� ���������)
	p_t prnt_uni(const p_t &c);

	//! \brief ��������� �������� �������� ��������, �������� ���
	//!	�������������. ���� �������� � ������ ������������ �� ����������,
	//! ����� ���������� �������� �� ���������.
	/*!	\param[in] x ���������� <i>x</i> ��������
		\param[in] y ���������� <i>y</i> ��������
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
		������� ��������� �� ��������� ���������� ��������
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

		������� ���������� ������� <i>(x, y)</i> � ��� ������� ��� �������
		���������� �������� <i>P<sub>i</sub></i>.

		������, ������� ��������� ��� ������ ��������, �� �� ���������,
		��� ������� �� ������������ ��� �������� �������� � <i>LL</i>
		��������.

		��. ������� (4) �� 35.pdf

		\todo ������� ������ �������� ����� ������, �������� ����������
		��������� ��������� �� ��������, ��� �������� � ���������
		������������� ���������� �����.
	*/
	template <const wnode::wnode_members member>
	pi_t calc_pi(const sz_t x, const sz_t y,
				 const subbands::subband_t &sb)
	{
		const pi_t i1	= abs(get_safe<member>(x - 1, y,     sb)) + 
						  abs(get_safe<member>(x + 1, y,     sb)) +
						  abs(get_safe<member>(x    , y - 1, sb)) + 
						  abs(get_safe<member>(x    , y + 1, sb));

		const pi_t i2	= abs(get_safe<member>(x + 1, y + 1, sb)) + 
						  abs(get_safe<member>(x + 1, y - 1, sb)) +
						  abs(get_safe<member>(x - 1, y + 1, sb)) + 
						  abs(get_safe<member>(x - 1, y - 1, sb));

		return pi_t(4 * abs(get_safe<member>(x, y, sb)) + 2 * i1 + i2) / 16;
	}

	//!	\brief ��������� ������� �� �������� ��������� �������� ����������
	//!	�������� <i>P<sub>i</sub></i>
	/*!	\param[in] prnt ���������� ������������� ��������, �������� ��
		�������� ����� �������������� ��� ���������� ���������� ��������.
		\param[in] children_sb ������� � ������� ������������� ��������
		��������.
		\return ������� �� �������� ��������� �������� ���������� ��������
		<i>P<sub>i</sub></i>

		������� ��������� ������ ��� ������������ ��������� �� �� <i>LL</i>
		��������. ��� �� �����, ������� ���������� 0 ��� ��������� ��
		<i>LL</i> ��������.

		��������� �������� <i>member</i> ��������� �������� ���� ��������
		��� ������� ��������. ������ ��� ����� wnode::member_wc.

		��� ����� ��������� ���������� ������ <i>��� 2.1</i> � �������
		<i>(5)</i> � <i>35.pdf</i>.

		\attention ������� �������� ������ �������� �� ��, ��� ��������
		<i>children_sb</i> ������������ ����� ������ �������, � �������
		������������� �������� ��������, � �� �������, � ������� ���������
		��� ������������ ������� � ������������ <i>prnt</i>
	*/
	template <const wnode::wnode_members member>
	pi_t calc_pi_avg(const p_t &prnt, const subbands::subband_t &children_sb)
	{
		// �������� �� LL �������
		if (sb().test_LL(prnt)) return 0;

		pi_t pi = 0;

		for (coefs_iterator i = iterator_over_children(prnt);
			!i->end(); i->next())
		{
			const p_t &c = i->get();
			pi += calc_pi<member>(c.x, c.y, children_sb);
		}

		return (pi / 4);
	}

	//! \brief ����������� �������� ���������� �������� <i>S<sub>j</sub></i>
	/*!	\param[in] x ���������� x "������" ����� 2x2
		\param[in] y ���������� y "������" ����� 2x2
		\param[in] sb �������������� �������
		\param[in] going_left <i>true</i> ���� ����������� ������ �����,
		����� <i>false</i>. � ����������� �� ����� ��������� ��������������
		����� ����� �����.
		\return �������� ���������� �������� <i>S<sub>j</sub></i>

		������� ��� ���������� �������� ���������� �� ��� �������� ���������
		<i>(x, y)</i> � �� ������ ��������� �������� ��� �������� ��������
		<i>(x, y)</i>. �� ���� ������� ������� <i>(x, y)</i> ������ ������
		�� ������� ������� �� �������. ��� �� ����� ������� ���������
		������������ ���������� ��������� � ������ ���� �������, ���������
		��� ���� ������� �������� ��������.

		��. ������� (6) �� <i>35.pdf</i>

		\note ���������� �����, �������������� � ������� ������������
		����� ������������ (����) �� <i>35.pdf</i>

		\todo ������� ������ �������� ����� ������, �������� ����������
		��������� ��������� �� ��������, ��� �������� � ���������
		������������� ���������� �����.
	*/
	template <const wnode::wnode_members member>
	pi_t calc_sj(const sz_t x, const sz_t y,
				 const subbands::subband_t &sb,
				 const bool going_left)
	{
		// �������� �� ������� �� ������ ���� �������
		if (subbands::LVL_1 >= sb.lvl) return 0;

		// �������� ��� ������� �������������
		static const dsz_t	top		= (-1);
		// �������� ��� ������� �������������
		       const dsz_t	side	= (going_left)? (+1): (-1);

		// ������� ���������� �����
		const pi_t sum = 0.4 *
						 abs(pi_t(get_safe<member>(x + side, y + top, sb))) +
						 abs(pi_t(get_safe<member>(x + side, y      , sb))) +
						 abs(pi_t(get_safe<member>(x       , y + top, sb)));

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

		\return �������� ���������� �������� <i>S<sub>j</sub></i>
	*/
	template <const wnode::wnode_members member>
	pi_t calc_sj(const sz_t x, const sz_t y,
				 const subbands::subband_t &sb)
	{
		return calc_sj<member>(x, y, sb, _going_left(x, y));
	}

	//@}

	//!	\name ������� ��� ������ � ���������� ���������� ����������
	//@{

	//!	\brief ���������� �������� �� ��������� ���������� ��������
	//!	����������
	n_iterator iterator_through_n(const sz_t lvl);

	//!	\brief ������������ ��������� ������� ���������� ������, � �������
	//!	��� �������� ����� ������ ���������.
	n_t child_n_mask(const p_t &p, const p_t &branch);

	//!	\brief ������������ ��������� ������� ���������� ������, � �������
	//!	��� �������� ����� ������ ���������. ������ ��� <i>LL</i> ��������.
	n_t child_n_mask_LL(const p_t &p);

	//!	\brief ���������, ������������� ������ � ��������� ��������
	//!	���������� �� ����� ������.
	/*!	\param[in] n ��������� ������� ���������� ������
		\param[in] mask ����� �������� ������, ������� ����������
		��������� �� ������������.
		\return <i>true</i> ���� ��� ������������� � <i>mask</i> �����
		�� ���������. <i>false</i> ���� ���� �� ���� ����� ���������.
	*/
	inline bool test_n_mask(const n_t &n, const n_t &mask) {
		return (mask == (n & mask));
	}

	//@}

	//!	\name ����������� ��������� ������
	//@{

	//!	\brief ��������� ����� � ������������ � ��������� ��������� ����������
	//!	������
	void cut_leafs(const p_t &branch, const n_t n);

	//@}

	//!	\name ���������� ���������� �� �������������
	//@{

	//! \brief ���������� �������� �� ��������
	coefs_iterator iterator_over_subband(const subbands::subband_t &sb);

	//! \brief ���������� �������� �� �������� ��������� (������������
	//!	������� �� �� <i>LL</i> ��������)
	/*!	\param[in] prnt ���������� ������������� ��������
		\return �������� �� �������� ���������

		\note ������ ������� �� ��������� ��� ������������ ������������� ��
		<i>LL</i> ��������.

		\sa _iterator_over_children()
	*/
	wtree::coefs_iterator iterator_over_children(const p_t &prnt)
	{
		return _iterator_over_children(prnt);
	}

	//! \brief ���������� �������� �� �������� ��������� (������������
	//!	������� �� <i>LL</i> ��������)
	/*!	\param[in] prnt ���������� ������������� ��������
		\return �������� �� �������� ���������

		\note ������ ������� �� ��������� ��� ������������ ������������� �� ��
		<i>LL</i> ��������.

		\sa _iterator_over_LL_children()
	*/
	wtree::coefs_iterator iterator_over_LL_children(const p_t &prnt)
	{
		return _iterator_over_LL_children(prnt);
	}

	//! \brief ���������� �������� �� �������� ��������� (������������
	//!	������� �� ������ ��������)
	/*!	\param[in] prnt ���������� ������������� ��������
		\return �������� �� �������� ���������

		\note ������� �������, ��� ���� ������������ ������� �� <i>LL</i>
		��������, �������� �������� ����� � ������ ���������.

		\sa _iterator_over_children(), _iterator_over_LL_children()
	*/
	wtree::coefs_iterator iterator_over_children_uni(const p_t &prnt)
	{
		return (sb().test_LL(prnt))
				? _iterator_over_LL_children(prnt)
				: _iterator_over_children(prnt);
	}

	//! \brief ���������� �������� �� �������� ������������� (�������)
	coefs_iterator iterator_over_leafs(const p_t &root,
									   const subbands::subband_t &sb);

	//! \brief ���������� �������� �� �������� ������������� (�������)
	coefs_iterator iterator_over_leafs(const p_t &root,
									   const sz_t lvl, const sz_t i);

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

	//! \brief ���������� ���������� �������� ������ �������� ����� �������
	//!	������
	p_t _leafs_top_left(const p_t &root, const sz_t lvl, const sz_t i);

	//!	\brief �� ��������� �������� �������� �������������� �����������
	//!	������
	bool _going_left(const sz_t x, const sz_t y);

	//@}

	//!	\name ����������� ��������� ������
	//@{

	//!	\brief ��������� ����� ������
	void _cut_branch(const p_t &branch);

	//@}

	//!	\name ���������� ����������
	//@{

	//! \brief ���������� �������� �� �������� ��������� (������������ �������
	//!	�� �� <i>LL</i> ��������)
	basic_iterator<p_t> *_iterator_over_children(const p_t &prnt);

	//! \brief ���������� �������� �� �������� ��������� (������������ �������
	//!	�� <i>LL</i> ��������)
	basic_iterator<p_t> *_iterator_over_LL_children(const p_t &prnt);

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
