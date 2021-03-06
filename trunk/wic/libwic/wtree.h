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
#include <wic/libwic/dpcm.h>


////////////////////////////////////////////////////////////////////////////////
// defines

//!	\brief ���� ������ ��������, �� ���� ��������� ���������  ��� ��������
//!	�������� <i>P<sub>i</sub></i> ����� ������������ � ������������ �
//!	����������� ��������� �������� � �������
/*!	\note ���� ��������, ��� ��� ���������� ����� ������� ����������
	����������.
*/
#define RECALC_PI_FACTOR_ON_EDGES
//#undef RECALC_PI_FACTOR_ON_EDGES

//!	\brief ���� ������ ��������, �� ���� ��������� ���������  ��� ��������
//!	�������� <i>S<sub>j</sub></i> ����� ������������ � ������������ �
//!	����������� ��������� �������� � �������
/*!	\note ���� ��������, ��� ��� ���������� ����� ������� ����������
	����������.
*/
#define RECALC_SJ_FACTOR_ON_EDGES
//#undef RECALC_SJ_FACTOR_ON_EDGES

//!	\brief
#define ADVANCED_SIGN_NUM
//#undef ADVANCED_SIGN_NUM

#define ADVANCED_CALC_SJ
//#undef ADVANCED_CALC_SJ


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

	//! \brief ��������� ��� ������������� ���������, ������������� ���
	//!	wic::p_t
	typedef cumulative_iterator<p_t> coefs_cumulative_iterator;

	//!	\brief ��������� ��� ������ ��� ����������, ������������ ��� wic::n_t
	typedef some_iterator<n_t> n_iterator;

	// public constants --------------------------------------------------------

	//!	\brief �������� ������������ �� ���������
	static const q_t DEFAULT_Q;

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
	/*!	\return ���������� ������������� �� ��� ������
	*/
	const sz_t &nodes_count() const { return _nodes_count; }

	//! \brief ���������� ���������� ���� ���������� ��������
	/*!	\return ���������� ����, ������� ���� ��������� ��� �������� ����
		���������� � �������� (�� ������ ���������� � ���������)
	*/
	sz_t nodes_size() const { return (_nodes_count * sizeof(wnode)); }

	//! \brief ���������� �������������� ������������
	/*!	\return ������������, ������� ��� ����������� ��� �����������
		������ c ������� wtree::quantize ��� ������������� � �������
		wtree::dequantize
	*/
	const q_t &q() const { return _q; }

	//@}

	//!	\name ������ � ����� ��������
	//@{

	//!	\brief ������������� ��������� ���� ���� ��������� ������� � ��������
	//!	��������
	/*!	\param[in] value ��������, � ������� ����� ���������� ���� member ����
		��������� �������.

		������� ������ �������� ��� �������, �� ������� �������, � �������������
		��� ���������������� ��������� ������ ����� ������� �� ����������.
	*/
	template <const wnode::wnode_members member>
	void set_field(const typename wnode::type_selector<member>::result &value)
	{
		for (sz_t i = 0; nodes_count() > i; ++i)
		{
			_nodes[i].get<member>() = value;
		}
	}

	//! \brief ��������� ���� ��������� �������
	/*!	\param[in] from ���� ������ �� �������� ����� ��������� ����
		���������

		��� ������� ������� ������������ � �������������, ��� ��� ���
		����� �������� ����������� �������.

		�������� ������� <i>member</i> �������� ������������ ���� ���������.
		�������� ������� <i>value_t</i> ��� ����������� ��������� ����
		�� �������� �������� �������� ����� �� ���� ����. �������� �����
		������������� ������������� � ������� ����.
	*/
	template <const wnode::wnode_members member, class value_t>
	void load_field(const value_t *const from)
	{
		assert(0 != from);

		typedef typename wnode::type_selector<member>::result wnm_t;

		for (sz_t i = 0; nodes_count() > i; ++i)
		{
			_nodes[i].get<member>() = static_cast<wnm_t>(from[i]);
		}
	}

	//! \brief ��������� ������ �� ������ � ������������
	/*!	\param[in] from ������� ������, �������� ������������� ��������,
		����� �����������
		\param[in] q ������������

		������� ����� ������������� ��������� ����������� � <i>q = 1</i>
		(�������� wtree::quantize()).
	*/
	template <class value_t>
	void load(const value_t *const from, const q_t q = DEFAULT_Q)
	{
		load_field<wnode::member_w>(from);

		quantize(q);
	}

	//!	\brief ������� �������� ������� � ������������
	/*!	\param[in] from ������� ������, �������� ������������� ��������,
		����� �����������
		\param[in] q ������������

		������� ���������� ������� �������� ������� ��� �������������,
		��� ��� ������ ������� wtree::load(). �������� ���� ����� �����
		wnode::w (���������� ����������� ������������) � wnode::wq
		(���������� ��������������� ������������) �� ����������. �������
		�������� ��� ����� ������� ����������� ������ ������� ����������
		����������� (����� � �������� filling_refresh()).

		������������ ������ ������� ����������� � ���, ��� ��� ����������
		�������� ������������� � ����������� � ���� ������, � �� � ���,
		��� ��� ������ ������� load().
	*/
	template <class value_t>
	void cheap_load(const value_t *const from, const q_t q = DEFAULT_Q)
	{
		assert(0 != from);
		assert(1 <= q);

		_q = q;

		for (sz_t i = 0; nodes_count() > i; ++i) {
			wnode &node = _nodes[i];
			node.w	= static_cast<w_t>(from[i]);
			node.wq	= wnode::quantize(node.w, q);
		}
	}

	//!	\brief ��������� �������� ����� � ������
	/*!	\param[out] to ���� ������, � ������� ����� ��������� �������� ����
		���������
	*/
	template <const wnode::wnode_members member, class value_t>
	void save(value_t *const to) const
	{
		for (sz_t i = nodes_count() - 1; 0 <= i; --i)
		{
			to[i] = static_cast<value_t>(_nodes[i].get<member>());
		}
	}

	//! \brief ���������� ����������� ����� �������
	void quantize(const q_t q = DEFAULT_Q);

	//! \brief ���������� ������������� ����� �������
	/*!	\param[in] q ������������
	*/
	template <const wnode::wnode_members member>
	void dequantize(const q_t q = DEFAULT_Q)
	{
		_deqantize<member>(q);
	}

	//! \brief ��������� ������, ����������� �������� ���� wnode::wc
	//!	� wnode::wq, �������� �������� ������� <i>��������</i>, ��������
	//!	��� ����� ��� �����������, � �������� ��� ����������
	void filling_refresh();

	//! \brief ��������� ������, ������� �������� ����� wnode::w, wnode::wq
	//!	� wnode::wc, �������� ��� ����� ��� �����������, � �������� ���
	//!	�� ����������.
	void wipeout();

	//@}

	//!	\name ������ � ���������� � ���������
	//{@

	//! \brief ���������� ���������� � ���������
	/*!	\return ������ �� ������ wi�::subbands
		\sa subbands
	*/
	subbands &sb()
	{
		assert(0 != _subbands);

		return (*_subbands);
	}

	//! \brief ���������� ���������� � ���������
	/*!	\return ����������� ������ �� ������ wi�::subbands
		\sa subbands
	*/
	const subbands &sb() const
	{
		assert(0 != _subbands);

		return (*_subbands);
	}

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
		\param[in] def �������� �� ���������, ������� ��������, ����
		�������� � ������ ������������ �� ����������
		\return �������� �������� ��� �������� �� ���������, ����
		������� �� ����������.
	*/
	template <const wnode::wnode_members member>
	typename wnode::type_selector<member>::result get_safe(
			const sz_t x, const sz_t y,
			const typename wnode::type_selector<member>::result &def = 0) const
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
		\param[in] def �������� �� ���������, ������� ��������, ����
		�������� � ������ ������������ �� ����������
		\return �������� �������� ��� �������� �� ���������, ����
		������� ��������� �� ��������� ���������� ��������
	*/
	template <const wnode::wnode_members member>
	typename wnode::type_selector<member>::result get_safe(
			const sz_t x, const sz_t y,
			const subbands::subband_t &sb,
			const typename wnode::type_selector<member>::result &def = 0) const
	{
		if (sb.x_min > x || x > sb.x_max) return def;
		if (sb.y_min > y || y > sb.y_max) return def;

		return  _nodes[x + _width * y].get<member>();
	}

	//@}

	//!	\name �������������� ����������
	//@{

	//!	\brief ������� ����������� � ������������ �������� � �������
	/*!	\param[in,out] set ��������, �������� ��������� ��� ������
		������������� � ������������ ���������
		\param[out] min ���������� ������� ������ ��������� �����������
		��������
		\param[out] max ���������� ������� ������ ��������� ������������
		��������
		\return <i>true</i> ���� ��������� �� ������ � ���������� <i>min</i>
		� <i>max</i> �������� ��������� ��������, ����� <i>false</i>
	*/
	template <const wnode::wnode_members member>
	bool minmax(coefs_iterator &set,
				typename wnode::type_selector<member>::result &min,
				typename wnode::type_selector<member>::result &max) const
	{
		// ������������ ���
		typedef wnode::type_selector<member>::result value_t;

		// �������� �� ������ ���������
		if (set->end()) return false;

		// ������������� ����������
		min = max = at(set->get()).get<member>();

		// ����� ������������ � ������������� ��������
		for (set->next(); !set->end(); set->next())
		{
			const wnode &node = at(set->get());
			const value_t &value = node.get<member>();

			if (value < min) min = value;
			else if (value > max) max = value;
		}

		return true;
	}

	//!	\brief ������� ����������� � ������������ �������������� �������� �
	//!	�������� ��� ���������� � �������������� ����
	/*!	\param[in] sb ������� � ������� ����� ������������ �����
		\param[out] min ���������� ������� ������ ��������� �����������
		��������
		\param[out] max ���������� ������� ������ ��������� ������������
		��������
		\return <i>true</i> ���� ���������� ��������� ���������� �������� ��
		������ � ���������� <i>min</i> � <i>max</i> �������� ���������
		��������, ����� <i>false</i>
	*/
	template <const wnode::wnode_members member>
	bool dpcm_minmax(const subbands::subband_t &sb,
					 typename wnode::type_selector<member>::result &min,
					 typename wnode::type_selector<member>::result &max) const
	{
		// ������������ ���
		typedef wnode::type_selector<member>::result value_t;

		// �������� �� ��������
		coefs_iterator set = iterator_over_subband(sb);

		// �������� �� ������ ���������
		if (set->end()) return false;

		// ������������� ����������
		const p_t &p = set->get();
		min = max = dpcm::encode(at(p).get<member>(),
								 dpcm_predict<member>(p, sb));

		// ����� ������������ � ������������� ��������
		for (set->next(); !set->end(); set->next())
		{
			const p_t &p = set->get();
			const value_t dv = dpcm::encode(at(p).get<member>(),
											dpcm_predict<member>(p, sb));

			if (dv < min) min = dv;
			else if (dv > max) max = dv;
		}

		return true;
	}

	//!	\brief ������� ������������ ���������� ����� ����� ������ ���������
	//!	�� ���������� ���������
	/*!	\param[in,out] set ��������, �������� ��������� ���������
		\return ������������ ����������

		��������� ������� <i>member1</i> � <i>member2</i> ���������� ����
		���������, ����� �������� ����� �������� ������������ ����������.
		�������� ������� <i>result_t</i> ����� �����, ��� ��� �� ������ ��
		�������� ������������ ���������� (������ ������� ���������
		���������� ������ ����� ����), ������� ����� ����� ���� ���������
		���� ��������.

		\note ����� ������� ����� ��� ������� ��� ���������� ����������
		������������ (������������ �������� wtree::q()), ��� �� ����������
		������������� ���������� ���� ��� �������� ������������� ����������.
	*/
	template <const wnode::wnode_members member, class result_t>
	result_t distortion(coefs_iterator &set) const
	{
		// �������� �����������
		assert(member != wnode::member_w);
		assert(member == wnode::member_wq || member == wnode::member_wc);

		// ������������� ����������
		result_t d = 0;

		// ������� ������������� ����������
		while (!set->end())
		{
			const wnode &node = at(set->get());

			const result_t dw = (wnode::dequantize(node.get<member>(), q())
								 - node.w);

			d += (dw * dw);

			set->next();
		}

		return d;
	}

	//!	\brief ������� ������������ ���������� ����� ������ wnode::w �
	//!	wnode::wq
	/*!	\return ������������ ����������
	*/
	template <class result_t>
	result_t distortion_wq() const
	{
		return distortion<wnode::member_wq, result_t>(iterator_over_wtree());
	}

	//!	\brief ������� ������������ ���������� ����� ������ wnode::w �
	//!	wnode::wc
	/*!	\return ������������ ����������
	*/
	template <class result_t>
	result_t distortion_wc() const
	{
		return distortion<wnode::member_wc, result_t>(iterator_over_wtree());
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
		#ifdef RECALC_PI_FACTOR_ON_EDGES
		int sums	= 4;
		#endif

		const pi_t i1	= abs(get_safe<member>(x - 1, y,     sb)) + 
						  abs(get_safe<member>(x + 1, y,     sb)) +
						  abs(get_safe<member>(x    , y - 1, sb)) + 
						  abs(get_safe<member>(x    , y + 1, sb));

		#ifdef RECALC_PI_FACTOR_ON_EDGES
		if (_subbands->test(p_t(x - 1, y    ), sb)) sums += 2;
		if (_subbands->test(p_t(x + 1, y    ), sb)) sums += 2;
		if (_subbands->test(p_t(x    , y - 1), sb)) sums += 2;
		if (_subbands->test(p_t(x    , y + 1), sb)) sums += 2;
		#endif

		const pi_t i2	= abs(get_safe<member>(x + 1, y + 1, sb)) + 
						  abs(get_safe<member>(x + 1, y - 1, sb)) +
						  abs(get_safe<member>(x - 1, y + 1, sb)) + 
						  abs(get_safe<member>(x - 1, y - 1, sb));

		#ifdef RECALC_PI_FACTOR_ON_EDGES
		if (_subbands->test(p_t(x + 1, y + 1), sb)) sums += 1;
		if (_subbands->test(p_t(x + 1, y - 1), sb)) sums += 1;
		if (_subbands->test(p_t(x - 1, y + 1), sb)) sums += 1;
		if (_subbands->test(p_t(x - 1, y - 1), sb)) sums += 1;
		#endif

		#ifdef RECALC_PI_FACTOR_ON_EDGES
		return pi_t(4 * abs(get_safe<member>(x, y, sb)) + 2 * i1 + i2) /
					pi_t(sums);
		#else
		return pi_t(4 * abs(get_safe<member>(x, y, sb)) + 2 * i1 + i2) / 16;
		#endif
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

		#ifdef ADVANCED_CALC_SJ
		// ���� ������������ ��������� ����������:
		// HL - �������, ��������������� ��� ��������� HH
		// LH - �������, ��������������� ����� �� �������� HH
		// HH - ������������ �������

		// ��������� ��������� �������� ������������, ������������� ���
		// ���������� �������� (dx, dy), � ����� ��� ����� ������������
		// ��� ���������� ���������� ����� (k). ��������� �����
		// ��������������� ����� ��� ���������� �������� ��������
		// ���������� �����, ���������� � ����������� ��������
		// ������������� (cv()), � ����� �����, ����������� ����� ��
		// ��������������� �������� ����������� � ��������� �������� (test()).
		struct k_t
		{
			// �������� �� �����������. �������� ����� �������������
			// ������������ �������� ����������� ������ (�������� going_left
			// � ���������� side). ��� ������������� �������� ����� dx
			// (��������, dx = 1) ����� ���� �������� ������� ������� ���
			// ��� ���������� ����� (������, ��� ����������� ������ �����).
			// ��� ������������� �������� ��������� dx (��������, dx = -1)
			// ����� ���� �������� �������, ������� ����� �� ��� ����������
			// (�����, ��� ����������� ������ �����).
			sz_t dx;
			// �������� �� ���������. ������������� �������� �������������
			// ��������� ������.
			sz_t dy;
			// ��� ������������ � ��������� �����
			pi_t k;

			// ��������� ������� ���������� ����� ��������� � �����������
			// �������� ���������
			pi_t cv(const sz_t x, const sz_t y, const subbands::subband_t &sb,
					const sz_t aside, const wtree *const t) const
			{
				return k * abs(t->get_safe<member>(x + aside*dx, y - dy, sb));
			}

			// ���������, ����� �� ��������������� �������� ������� �
			// ��������� ��������, ��� ��������� ����������� ������
			bool test(const sz_t x, const sz_t y, const subbands::subband_t &sb,
					  const sz_t aside, const wtree *const t) const
			{
				return t->_subbands->test(p_t(x + aside*dx, y - dy), sb);
			}
		};

		// ��� �������� HL (�������)
		static const k_t HL_k0 = {0, 2, 0.4};	//           0.4
		static const k_t HL_k1 = {0, 1, 1.3};	//           1.0
		static const k_t HL_k2 = {1, 0, 1.0};	//      1.0  X

		// ��� �������� LH (�����)
		static const k_t LH_k0 = {2, 0, 0.4};	//
		static const k_t LH_k1 = {1, 0, 1.0};	//           1.0
		static const k_t LH_k2 = {0, 1, 1.3};	// 0.4  1.0  X

		// ��� �������� HH (������������)
		static const k_t HH_k0 = {1, 1, 1.0};	//
		static const k_t HH_k1 = {0, 1, 1.0};	//      0.4  1.0
		static const k_t HH_k2 = {1, 0, 1.0};	//      1.0  X

		// ����������� ���������� ����
		static const pi_t HL_sum = HL_k0.k + HL_k1.k + HL_k2.k;
		static const pi_t LH_sum = LH_k0.k + LH_k1.k + LH_k2.k;
		static const pi_t HH_sum = HH_k0.k + HH_k1.k + HH_k2.k;

		#endif

		#ifdef RECALC_SJ_FACTOR_ON_EDGES
		pi_t sums = 0;
		#endif

		// ������� ���������� �����
		#ifdef ADVANCED_CALC_SJ
			pi_t sum = 0;
			pi_t v_max = 0;
			switch (sb.i)
			{
			case subbands::SUBBAND_HL:
				sum = HL_k0.cv(x, y, sb, side, this) +
					  HL_k1.cv(x, y, sb, side, this) +
					  HL_k2.cv(x, y, sb, side, this);
				if (HL_k0.test(x, y, sb, side, this)) sums += HL_k0.k;
				if (HL_k1.test(x, y, sb, side, this)) sums += HL_k1.k;
				if (HL_k2.test(x, y, sb, side, this)) sums += HL_k2.k;
				break;

			case subbands::SUBBAND_LH:
				sum = LH_k0.cv(x, y, sb, side, this) +
					  LH_k1.cv(x, y, sb, side, this) +
					  LH_k2.cv(x, y, sb, side, this);
				if (LH_k0.test(x, y, sb, side, this)) sums += LH_k0.k;
				if (LH_k1.test(x, y, sb, side, this)) sums += LH_k1.k;
				if (LH_k2.test(x, y, sb, side, this)) sums += LH_k2.k;
				break;

			case subbands::SUBBAND_HH:
				sum = HH_k0.cv(x, y, sb, side, this) +
					  HH_k1.cv(x, y, sb, side, this) +
					  HH_k2.cv(x, y, sb, side, this);
				if (HH_k0.test(x, y, sb, side, this)) sums += HH_k0.k;
				if (HH_k1.test(x, y, sb, side, this)) sums += HH_k1.k;
				if (HH_k2.test(x, y, sb, side, this)) sums += HH_k2.k;
				break;

			default:
				assert("invalid subband index");
			}
		#else
			pi_t sum = 0.4 * abs(pi_t(get_safe<member>(x + side, y + top, sb))) +
							 abs(pi_t(get_safe<member>(x + side, y      , sb))) +
							 abs(pi_t(get_safe<member>(x       , y + top, sb)));
		#endif

		#ifdef RECALC_SJ_FACTOR_ON_EDGES
			#ifdef ADVANCED_CALC_SJ
				// sums ��� ����� ������ ��������
			#else
				if (_subbands->test(p_t(x + side, y + top), sb)) sums += 0.4;
				if (_subbands->test(p_t(x + side, y      ), sb)) sums += 1;
				if (_subbands->test(p_t(x       , y + top), sb)) sums += 1;
			#endif

			if (0 != sums) sum = (2.4 * sum / sums);
		#endif

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

	//!	\brief ������������ ���������� �������� ��� �������������
	//!	�������, ������������ ��� ����������� � �������������� ����
	/*!	\param[in] p ���������� ��������������� ��������
		\param[in] sb ������� � ������� ������������� ��������������
		�������
		\return ���������� �������� ��������

		������� ������������, ��� ����� ������������ �������, ������� �
		������ �������� �������.
	*/
	template <const wnode::wnode_members member>
	typename wnode::type_selector<member>::result dpcm_predict(
						const p_t &p, const subbands::subband_t &sb) const
	{
		// ���, ������������ ��� �����������
		typedef wnode::type_selector<member>::result member_t;

		// ��� ������� �������� �� �������� ������ ���������� 0
		if (p.x == sb.x_min && p.y == sb.y_min) return 0;

		// ����������� ����������� ������� �� ������ ������
		const sz_t dx = (0 == (p.y % 2))? +1: -1;

		// ��� ���� ������� ������ ��������� �������� �������� �����������
		// �������� � ��� �� ������
		if (sb.y_min == p.y) return at(p.x - dx, sb.y_min).get<member>();

		// ��� ����� ������� ����� � ������ ��������� ��������� ��������
		// �������� ����������� �������� � ��� �� ������, � ���� �������
		// ����� ��� ��������, ����� ��������� �������� �������� ��������
		// ��������
		if (sb.x_min == p.x || sb.x_max == p.x)
		{
			return get_safe<member>(p.x - dx, p.y, sb,
									at(p.x, p.y - 1).get<member>());
		}

		// ��� ���� ��������� ��������� �������� (x) ����������� �� �������:
		// (b + d - (a + c) / 2), ��� �������� a, b, c, d ����� ���������
		// ������������ ������������ �������� x ��� 0 < dx:
		// a  b  c
		// d  x
		// � ��� 0 > dx:
		// c  b  a
		//    x  d
		const member_t a = at(p.x - dx, p.y - 1).get<member>();
		const member_t b = at(p.x,      p.y - 1).get<member>();
		const member_t c = at(p.x + dx, p.y - 1).get<member>();
		const member_t d = at(p.x - dx, p.y    ).get<member>();

		return (b + d - (a + c) / 2);
	}

	//!	\brief ��������� ������� "������" ����� ������������ ������ �� ������
	//!	�������� ���������
	/*!	\param[in] p ���������� �������� ��� �������� ����� ������� �������
		\param[in] sb �������, � ������� ����� �������
		\param[in] offset �������� ������� ����� ���������� � ����������
		\return ������� "������" ����� ������������
	*/
	template <const wnode::wnode_members member>
	typename sz_t sign_num(const p_t &p, const subbands::subband_t &sb,
						   const sz_t &offset = 0) const
	{
		// �������� ������ ��������
		assert(subbands::SUBBAND_HL == sb.i ||
			   subbands::SUBBAND_LH == sb.i ||
			   subbands::SUBBAND_HH == sb.i);

		// ������������ ����������
		static const sz_t ck0 = 1;
		static const sz_t ck1 = wnode::signp_max() + 1;
		static const sz_t ck2 = ck1 * ck1;
		static const sz_t ck3 = ck1 * ck1 * ck1;

		// ����������� ����������� ������� �� ������ ������.
		// +1 ��� ������ ����� (������ ����� �������)
		// -1 ��� �������� ����� (������ ������ ������)
		const sz_t dx = (0 == (p.y % 2))? +1: -1;

		// �������� �������� �������������
		sz_t shh;
		sz_t shl;
		sz_t slh;

		// ����������� ������ �������� �������������
		#ifdef ADVANCED_SIGN_NUM
			switch (sb.i)
			{
			case subbands::SUBBAND_HL:
				shh = wnode::signp(get_safe<member>(p.x,        p.y - 2, sb));
				shl = wnode::signp(get_safe<member>(p.x,        p.y - 1, sb));
				slh = wnode::signp(get_safe<member>(p.x - dx,   p.y,     sb));
				break;

			case subbands::SUBBAND_LH:
				shh = wnode::signp(get_safe<member>(p.x,        p.y - 1, sb));
				shl = wnode::signp(get_safe<member>(p.x - dx,   p.y,     sb));
				slh = wnode::signp(get_safe<member>(p.x - 2*dx, p.y,     sb));
				break;

			case subbands::SUBBAND_HH:
				shh = wnode::signp(get_safe<member>(p.x - dx,  p.y - 1, sb));
				shl = wnode::signp(get_safe<member>(p.x - dx,  p.y,     sb));
				slh = wnode::signp(get_safe<member>(p.x,       p.y - 1, sb));
				break;

			default:
				assert("invalid subband index");
			}
		#else
			shh = wnode::signp(get_safe<member>(p.x - dx, p.y - 1, sb));
			shl = wnode::signp(get_safe<member>(p.x - dx, p.y,     sb));
			slh = wnode::signp(get_safe<member>(p.x,      p.y - 1, sb));
		#endif

		// ���������� �������
		return (ck0*shh + ck1*shl + ck2*slh + ck3*sb.i + offset);
	}

	//@}

	//!	\name ������� ��� ������ � ���������� ���������� ����������
	//@{

	//!	\brief ���������� �������� �� ��������� ���������� ��������
	//!	����������
	n_iterator iterator_through_n(const sz_t lvl) const;

	//!	\brief ������������ ��������� ������� ���������� ������, � �������
	//!	��� �������� ����� ������ ���������. ������ ��� �� <i>LL</i>
	//!	���������.
	n_t child_n_mask(const p_t &p, const p_t &branch) const;

	//!	\brief ������������ ��������� ������� ���������� ������, � �������
	//!	��� �������� ����� ������ ���������. ������ ��� <i>LL</i> ��������.
	n_t child_n_mask_LL(const p_t &p) const;

	//!	\brief ������������ ��������� ������� ���������� ������, � �������
	//!	��� �������� ����� ������ ���������. ������������� ������ ��� ����
	//!	���������.
	n_t child_n_mask_uni(const p_t &p, const p_t &branch) const;

	//!	\brief ���������, ������������� ������ � ��������� ��������
	//!	���������� �� ����� ������.
	/*!	\param[in] n ��������� ������� ���������� ������
		\param[in] mask ����� �������� ������, ������� ����������
		��������� �� ������������.
		\return <i>true</i> ���� ��� ������������� � <i>mask</i> �����
		�� ���������. <i>false</i> ���� ���� �� ���� ����� ���������.
	*/
	inline bool test_n_mask(const n_t &n, const n_t &mask) const {
		return (mask == (n & mask));
	}

	//!	\brief ���������� ��������� ������� ���������� ������, � �������
	//! ��� ����� ���������
	/*!	\return ��������� ������� ���������� ������
	*/
	inline n_t get_clear_n() const { return 0; }

	//@}

	//!	\name ����������� ��������� ������
	//@{

	//!	\brief ��������� ����� � ������������ � ��������� ��������� ����������
	//!	������
	/*!	\param[in] branch ���������� ��������, ����������� �����
		\param[in] n ��������� ������� ���������� ������

		������� ��������� ���������� ������, ����������� ������� ��������
		�������� �������� �� <i>branch</i>, � ������������ � ��������� ���������
		���������� ������.

		��������� ��� ������ �� ����� ���������, ����� ��� ��� ����� �� �������
		������� <i>lvls() - 2</i>

		�������� ������� ��������� ������� ���� �������� wnode, ������� �����
		��������� ��������� <i>0</i>. �������� ��������� �������� �����
		��������� - wnode::member_wc. ��� ����������� ����� ����� ������������
		��� �������.
	*/
	template <const wnode::wnode_members member>
	void cut_leafs(const p_t &branch, const n_t n)
	{
		// �������� �����������
		assert(lvls() + subbands::LVL_PREV > sb().from_point(branch).lvl);

		const bool is_LL = sb().test_LL(branch);

		for (coefs_iterator i = iterator_over_children_uni(branch);
			 !i->end(); i->next())
		{
			const p_t &p = i->get();

			const n_t mask = (is_LL)? child_n_mask_LL(p)
									: child_n_mask(p, branch);

			if (!test_n_mask(n, mask)) _cut_branch<member>(p);
		}

		at(branch).n = n;
	}

	//!	\brief ��������� �������������� �������
	void uncut_leafs(const p_t &branch, const n_t n);

	//@}

	//!	\name ���������� ���������� �� �������������
	//@{

	//! \brief ���������� �������� �� ��������
	coefs_iterator iterator_over_subband(const subbands::subband_t &sb) const;

	//! \brief ���������� �������� �� �������� ��������� (������������
	//!	������� �� �� <i>LL</i> ��������)
	/*!	\param[in] prnt ���������� ������������� ��������
		\return �������� �� �������� ���������

		\note ������ ������� �� ��������� ��� ������������ ������������� ��
		<i>LL</i> ��������.

		\sa _iterator_over_children()
	*/
	wtree::coefs_iterator iterator_over_children(const p_t &prnt) const
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
	wtree::coefs_iterator iterator_over_LL_children(const p_t &prnt) const
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
	wtree::coefs_iterator iterator_over_children_uni(const p_t &prnt) const
	{
		return (sb().test_LL(prnt))
				? _iterator_over_LL_children(prnt)
				: _iterator_over_children(prnt);
	}

	//! \brief ���������� �������� �� �������� ������������� (�������)
	coefs_iterator iterator_over_leafs(const p_t &root,
									   const subbands::subband_t &sb) const;

	//! \brief ���������� �������� �� �������� ������������� (�������)
	coefs_iterator iterator_over_leafs(const p_t &root,
									   const sz_t lvl, const sz_t i) const;

	//!	\brief ���������� �������� �� ����� �������
	coefs_iterator iterator_over_wtree() const;

	//@}

protected:
	// protected methods -------------------------------------------------------

	//!	\name ������� ���������� � ����� �������
	//@{

	//!	\brief ���������� ����������� ������������� �������
	void _quantize(const q_t q);

	//!	\brief ���������� ����������� ������������� ������� � ���������� ������
	//!	����� ��������� (��� ��� ���������� ������� ����� ������� �����������)
	void _filling_quantize(const q_t q);

	//!	\brief ��������� ������������� ������������� �������
	/*!	\param[in] q ������������ ������������

		������� �������� ������������� ���� �� ������ (�������� �������
		<i>member</i>) � �������� ��������� � ���� wnode::w
	*/
	template <const wnode::wnode_members member>
	void _deqantize(const q_t q)
	{
		assert(1 <= q);

		_q = q;

		for (sz_t i = 0; nodes_count() > i; ++i)
		{
			wnode &node = _nodes[i];
			node.w = wnode::dequantize(node.get<member>(), q);
		}
	}

	//! \brief ��������� ������
	void _filling_refresh();

	//@}

	//!	\name ������� ��� ������ � ���������� ��������������
	//@{

	//!	\brief ���������� ���������� �������� ������ �������� �� ������
	//!	�������� ���������
	p_t _children_top_left(const p_t &prnt) const;

	//! \brief ���������� ���������� �������� ������ �������� ����� �������
	//!	������
	p_t _leafs_top_left(const p_t &root, const sz_t lvl, const sz_t i) const;

	//!	\brief �� ��������� �������� �������� �������������� �����������
	//!	������
	bool _going_left(const sz_t x, const sz_t y);

	//@}

	//!	\name ����������� ��������� ������
	//@{

	//!	\brief ��������� ����� ������
	/*!	\param[in] branch ���������� �������� ������������ ����������� �����.

		������� ������������ ��������� �������� �� ����� ���������� ���������
		� ����������� �����:
		- �������� ������� ��� <i>����������������</i>, ��������� ����
		  wnode::invalid � <i>true</i>.
		- �������� ��������� ������� ���������� ������, ��������� � ���������,
		  ������� ��� �������� ����� ��� �����������.

		������� ��������� <i>�����</i> ����������, ����������� ������ ��� ��
		����������� �����. ��� ��������� �������� ����������, ������� �����
		���������� ��� ����, ��� ��������� �������� ���������� ������ � ����
		��������� ��������� ���� ��������� ����� ���� (��� ����� ���������).
		������� ������� ��������� ��������� ���������� ������ �� ����
		��������� ��������� �������� ����������� �������� ���������� ������
		�������.

		�������� ������� ��������� ������� ���� �������� wnode, ������� �����
		��������� ��������� <i>0</i>. �������� ��������� �������� �����
		��������� - wnode::member_wc. ��� ����������� ����� ����� ������������
		��� �������.

		\note ����������, ����� <i>branch</i> ��� �� �� <i>LL</i> �������� �
		�� �� ��������� � ���������� ������.

		\attention ������� �������� ����������� ������, ��� ��� ��
		������������ ��������� ������� ���������� � ������������� ��������
		�����
	*/
	template <const wnode::wnode_members member>
	void _cut_branch(const p_t &branch)
	{
		// �������� �����������
		assert(sb().from_point(branch).lvl > subbands::LVL_0);
		assert(sb().from_point(branch).lvl < lvls());

		n_t &branch_n = at(branch).n;

		// ������� �������� ���������
		for (coefs_iterator i = iterator_over_children(branch);
			 !i->end(); i->next())
		{
			const p_t &p = i->get();
			wnode &node = at(p);

			// ���������� ������� ���������� ��������� �������� �����
			// ��� ��� ��������� ������� ������� ��������� ������� ����������
			// ���� ����� �������� p
			if (0 != branch_n) _cut_branch<member>(p);

			node.invalid = true;
			node.n = 0;
			node.get<member>() = 0;
		}

		branch_n = 0;
	}

	//!	\brief ��������� �������������� (����������) �����
	void _uncut_branch(const p_t &branch);

	//@}

	//!	\name ���������� ����������
	//@{

	//! \brief ���������� �������� �� �������� ��������� (������������ �������
	//!	�� �� <i>LL</i> ��������)
	basic_iterator<p_t> *_iterator_over_children(const p_t &prnt) const;

	//! \brief ���������� �������� �� �������� ��������� (������������ �������
	//!	�� <i>LL</i> ��������)
	basic_iterator<p_t> *_iterator_over_LL_children(const p_t &prnt) const;

	//@}

private:
	// private data ------------------------------------------------------------

	//! \brief ������ ����� �������
	sz_t _width;

	//! \brief ������ ����� �������
	sz_t _height;

	//! \brief ���������� ������� ���������� �������
	sz_t _lvls;

	//!	\brief ����� ���������� ��������� � �������
	sz_t _nodes_count;

	//!	\brief ������������, ������� ��� ����������� � ��������� ���
	q_t	_q;

	//! \brief ����� �������
	wnode *_nodes;

	//! \brief �������� ����������
	subbands *_subbands;
};



}	// end of namespace wic



#endif	// WIC_LIBWIC_WTREE
