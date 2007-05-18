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

	//!	\name ������������ � �����������
	//@{

	//!	\brief ������������
	encoder(const w_t *const image,
			const sz_t width, const sz_t height, const sz_t lvls);

	//!	\brief ����������
	~encoder();

	//@}

	//!	\name ������� ����������
	//@{

	//!	\brief ������� �������������� ���������������� ����������� �����������
	void encode();

	//@}

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
		����������.

		� ������� ��������� ������� �������� �������� ���� ��������,
		������� ����� �������������� ��� ���������� ���������. ��� ���,
		�� ������ ������������� ��� �������� ������ ������������������
		������������, �������� ��������� ����� �������� wnode::member_wc.
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

	//!	\brief ������������ �������� <i>RD</i> ������� <i>��������</i>
	//! ��� �������� ������������ ��� ����������� ��� �������������� �������,
	//!	���� �� �� ��������� �� ����������� �����������.
	j_t _calc_rd_iteration(const p_t &p, const wk_t &k,
						   const lambda_t &lambda, const sz_t &model);

	//! \brief ���� ����������� ������������������ �������� ��� ������������
	wk_t _coef_fix(const p_t &p, const subbands::subband_t &sb,
				   const lambda_t &lambda);

	//! \brief ����������� <i>RD</i> ������� <i>��������</i> ��� ��������
	//!	���������� ������
	/*!	\todo �������� ���� ��� ���� �������
	*/
	j_t _calc_j0_value(const p_t &p)
	{
		j_t j0 = 0;

		for (wtree::coefs_iterator i = _wtree.iterator_over_children(p);
			 !i->end(); i->next())
		{
			const wnode &node = _wtree.at(i->get());
			j0 += (node.wc * node.wc + node.j0);
		}

		return j0;
	}

	//! \brief ����������� <i>RD</i> ������� <i>��������</i> ��� ��������
	//!	���������� � ���������� ������
	/*!	\todo �������� ���� ��� ���� �������
	*/
	j_t _calc_j1_value(const p_t &p, const subbands::subband_t &sb,
					   const lambda_t &lambda)
	{
		const sz_t model = _ind_spec<wnode::member_wc>(p, sb);

		j_t j1 = 0;

		for (wtree::coefs_iterator i = _wtree.iterator_over_children(p);
			 !i->end(); i->next())
		{
			const wnode &node = _wtree.at(i->get());
			j1 += _calc_rd_iteration(i->get(), node.wc, lambda, model);
		}

		return j1;
	}

	//@}

	//!	\name ���� �����������
	//@{

	//! \brief ��� 1. ����������������.
	void _encode_step_1(const lambda_t &lambda);

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
