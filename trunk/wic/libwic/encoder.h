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

// external library header
#include <arcoder/aencoder.h>
#include <arcoder/adecoder.h>

// libwic headers
#include <wic/libwic/types.h>
#include <wic/libwic/wtree.h>
#include <wic/libwic/iterators.h>


////////////////////////////////////////////////////////////////////////////////
// defines

//!	\brief ���� ���� ������ ��������, � ������� encoder::_coef_fix ���
//!	������������� ����� �������������� 4 ��������: w, w + 1, w - 1 � 0. ����
//!	������ �� ��������, ����� ���������� ������ 3 ��������: w, w +/- 1, 0.
//!	���� �������� ���������� � ����������� �� �������� ������������ - ���� ���
//! ������������� � ����� ��� ������������� �������������
#define COEF_FIX_USE_4_POINTS


////////////////////////////////////////////////////////////////////////////////
// wic namespace
namespace wic {


////////////////////////////////////////////////////////////////////////////////
// encoder class declaration

//! \brief ������ ��� ��������. ������������ ������ �����������.
/*! ����� ������������� ������ ��� ������ �����������. ��������� �������
	���� � �� �� ����������� ��������� ��� ��� ������� ���������� �����������.
*/
class encoder {
public:
	// public types ------------------------------------------------------------

	//!	\brief ��������� ��� ��������������� ��������
	typedef wtc::aencoder<wk_t> aencoder;

	//!	\brief ��������� ��� ��������������� ��������
	typedef wtc::adecoder<wk_t> adecoder;

	// public constants --------------------------------------------------------
	// public methods ----------------------------------------------------------

	//!	\brief ������������
	encoder(const w_t *const image,
			const sz_t width, const sz_t height, const sz_t lvls);

	//!	\brief ����������
	~encoder();

	//!	\brief ������� �������������� ���������������� ����������� �����������
	void encode();

protected:
	// protected methods -------------------------------------------------------

	//!	\name ����� ������ ��������������� ������ �� ��������
	//@{

	//! \brief ��������� ������� IndSpec(<i>S<sub>j</sub></i>) �� 35.pdf
	sz_t _ind_spec(const pi_t &s, const sz_t lvl);

	//! \brief ��������� ������� IndSpec(<i>S<sub>j</sub></i>) �� 35.pdf.
	/*!	\param[in] p ���������� ��������
		\param[in] sb �������, � ������� ��������� �������
		\return ����� ���������� ������

		������� ���������� ���������� �������� � �������� �������
		����������. � ������� ��������� ������� �������� ��������
		���� ��������, ������� ����� �������������� � �����������.
	*/
	template <const wnode::wnode_members member>
	sz_t _ind_spec(const p_t &p, const subbands::subband_t &sb) {
		return _ind_spec(_wtree.calc_sj<member>(p.x, p.y, sb), sb.lvl);
	}

	//! \brief ��������� ������� IndMap(<i>P<sub>i</sub></i>) �� 35.pdf
	sz_t _ind_map(const pi_t &p, const bool is_LL = false);

	//@}

	//!	\name ��������� ��������������� �����������
	//@{

	//! \brief ������������ ������� ������� ��� ����������� ������������.
	//! ��������� ������� <i>H<sub>spec</sub></i>.
	h_t _h_spec(const sz_t m, const wk_t &wk);

	//! \brief ������������ ������� ������� ��� ����������� ����������
	//! �������� ���������� ������. ��������� ������� <i>H<sub>map</sub></i>.
	h_t _h_map(const sz_t m, const n_t &n);

	//@}

	//!	\name �������� ����������� ��� �����������
	//@{

	//! \brief ���������� ������������� ������������
	/*!	\param[in] p ���������� ������������ ��� �������������
		\param[in] sb �������, � ������� ��������� �����������
		\param[in] lambda �������� <i>lambda</i> ������������ ���
		���������� <i>RD</i> �������� (������� ��������). ������������
		����� ������ ����� ������� � �������� ���������.
		\return �������� ������������������� ������������

		�������� ������� ��������� �������� ���� �������� ��� �������������.

		\todo ����������� ��� �������
		\todo ������, ����� ��, ����� ��� ���� ���������
		\todo �������� ��������������� ������ ��� �������� RD ��������
	*/
	template <const wnode::wnode_members member>
	typename wnode::type_selector<member>::result _coef_fix(
			const p_t &p, const subbands::subband_t &sb,
			const lambda_t &lambda)
	{
		// types
		typedef wnode::type_selector<member>::result member_t;

		// get reference to node
		const wnode &node = _wtree.at(p);

		// ����������� ��� �������������
		const member_t &w = node.get<member>();

		// ������������
		const q_t q = _wtree.q();

		// ����������� ��������
		#ifdef COEF_FIX_USE_4_POINTS
			static const sz_t vals_count	= 4;
			const member_t w_vals[vals_count] = {w, w + 1, w - 1, 0};
		#else
			static const sz_t vals_count	= 3;
			const dsz_t w_drift = (0 <= w)? -1; +1;
			const member_t w_vals[vals_count] = {w, w + w_drift, 0};
		#endif

		member_t k_optim = w_vals[0];
		j_t j_optim = _calc_j(p, sb, k, q, lambda);

		for (int i = 1; vals_count > i; ++i) {
			const member_t &k = w_vals[i];
			const w_t wr = wnode::dequantize(k, _wtree.q());
			const w_t dw = (wr - node.w);
			(dw * dw) + lambda * _h_spec(_ind_spec<member>(p, sb),  k);
		}

		return k_optim;
	}

	//@}

	//!	\name ���� �����������
	//@{

	//! \brief ��� 1. ����������������.
	void _encode_step_1();

	//@}

private:
	// private data ------------------------------------------------------------

	//! \brief ������� ������ �������������
	wtree _wtree;

	//! \brief �������������� �������
	aencoder _aenc;
};



}	// end of namespace wic



#endif	// WIC_LIBWIC_ENCODER
