/*!	\file     encoder.h
	\version  0.0.1
	\author   mice, ICQ: 332-292-380, mailto:wonder.mice@gmail.com
	\brief    �������� ������ wic::encoder

	\todo     ����� �������� ������� ���� encoder.h
*/

#pragma once

#ifndef WIC_LIBWIC_ENCODER
#define WIC_LIBWIC_ENCODER

////////////////////////////////////////////////////////////////////////////////
// include

// code configuration headers
#include <wic/libwic/defines.h>

// standard C++ library headers
#include <assert.h>
#include <algorithm>
#include <iostream>
#include <string>						// for debug output only
#include <fstream>						// for debug output only
#include <iomanip>						// for debug output only
#include <time.h>						// for debug output only

// external library header
// none

// libwicdbg headers
#ifdef LIBWIC_USE_DBG_SURFACE
#include <wic/libwicdbg/dbg_surface.h>
#endif

// libwic headers
#include <wic/libwic/types.h>
#include <wic/libwic/wtree.h>
#include <wic/libwic/acoder.h>
#include <wic/libwic/dpcm.h>
#include <wic/libwic/iterators.h>


////////////////////////////////////////////////////////////////////////////////
// defines

//!	\brief ���� ���� ������ ��������, � ������� encoder::_coef_fix ���
//!	������������� ����� �������������� 4 ��������: w, w + 1, w - 1 � 0. ����
//!	������ �� ��������, ����� ���������� ������ 3 ��������: w, w +/- 1, 0.
//!	���� �������� ���������� � ����������� �� �������� ������������ - ���� ���
//! ������������� � ����� ��� ������������� �������������
#define	COEF_FIX_USE_4_POINTS
#undef	COEF_FIX_USE_4_POINTS

//!	\brief ���� ���� ������ ��������, �� ������������� �������������
//!	������������� �� �����
/*!	��� ���������� ������������� ���������� ������ ����������, ��
	������������� ����� ������������� �� �����������.
	��������
*/
#define	COEF_FIX_DISABLED
#undef	COEF_FIX_DISABLED

//!	\brief ���� ���� ������ ��������, �� �� ����� ����������� ���������
//!	������ ����� �������������� ����������� ����������� ��������������
//!	�������
/*!	��������� ���� ����� ��� ������� ������������������, �� ������ �����
	���������� �������.

	\note ���� �� ������������
*/
#define	OPTIMIZATION_USE_VIRTUAL_ENCODING
#undef	OPTIMIZATION_USE_VIRTUAL_ENCODING

//!	\brief ����������� ��� ���������� ������ ������� ������ ���������
//!	<i>lambda_t</i>
/*!	������ ������� ������ ��������� <i>lambda_t</i> �����
	<i>(LAMBDA_SEARCH_K_LOW * q*q)</i>, ��� <i>q</i> ������� ������������.
*/
#define LAMBDA_SEARCH_K_LOW			(0.05f)

//!	\brief ����������� ��� ���������� ������� ������� ������ ���������
//!	<i>lambda_t</i>
/*!	������� ������� ������ ��������� <i>lambda_t</i> �����
	<i>(LAMBDA_SEARCH_K_HIGHT * q*q)</i>, ��� <i>q</i> ������� ������������.
*/
#define LAMBDA_SEARCH_K_HIGHT		(1.0f)	//(0.25f)

//!	\brief ���� ������ ������ ��������, ��� ����������� ������������� ��
//!	LL �������� ����� ����������� ����
/*!	\sa wic::dpcm
*/
#define USE_DPCM_FOR_LL_SUBBAND
#undef	USE_DPCM_FOR_LL_SUBBAND

//!	\brief �������� ����������� ������ ������������� � ��������� ������
/*!	
*/
#define ENCODE_SIGN_IN_SEPARATE_MODELS
#undef	ENCODE_SIGN_IN_SEPARATE_MODELS



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
	// public constants --------------------------------------------------------

	//!	\brief ����������� ���������� ���������� ������� ����������
	static const sz_t MINIMUM_LEVELS			= 3;

	//!	\brief ����� ������, ������������ ��� ����������� ������������� ��
	//!	LL ��������
	static const sz_t ACODER_SPEC_LL_MODELS		= 0;

	//!	\brief ���������� �������, ������������ �������������� ������� ���
	//!	����������� �������������
	static const sz_t ACODER_SPEC_MODELS_COUNT	= 6;

	//!	\brief ���������� �������, ������������ �������������� ������� ���
	//!	����������� ��������� ��������� ����������
	static const sz_t ACODER_MAP_MODELS_COUNT	= 5;

	//!	\brief ���������� �������, ������������ �������������� ������� ���
	//!	����������� ������ �������������
	static const sz_t ACODER_SIGN_MODELS_COUNT	= 27*3;

	static const sz_t ACODER_SIGN_MODELS_OFFSET	= ACODER_SPEC_MODELS_COUNT +
												  ACODER_MAP_MODELS_COUNT;

	//!	\brief ����� ���������� �������, ������������ �������������� �������
	static const sz_t ACODER_TOTAL_MODELS_COUNT	= ACODER_SPEC_MODELS_COUNT +
												  ACODER_MAP_MODELS_COUNT +
												  ACODER_SIGN_MODELS_COUNT;

	//!	\brief ��������� ���������������� ������������ ��������� ��� ��������
	//!	�������� ������� ��������������� ������ (������������� ����������
	//!	��������� � �������)
	static const short MODELS_DESC_NONE			= 0;

	//!	\brief ��������� ���������������� ������������ ��������� ��� ��������
	//!	�������� ������� ��������������� ������ (������������� ���������
	//!	models_desc1_t)
	static const short MODELS_DESC_V1			= 1;

	//!	\brief ��������� ���������������� ������������ ��������� ��� ��������
	//!	�������� ������� ��������������� ������ (������������� ���������
	//!	models_desc2_t)
	static const short MODELS_DESC_V2			= 2;

	// public types ------------------------------------------------------------

	//!	\brief �������� ������� ��������������� ������
	/*!	��������� �������� ����������� �������� ������� ��������������� ������.
		��������� ��� ����������, ������� ��������� ������, ������� �������
		����������� ��� �����������.
	*/
	#pragma pack(push, 1)
	struct models_desc1_t
	{
		//!	\brief ����������� ����������� ��� ������ #0
		short mdl_0_min;
		//!	\brief ������������ ����������� ��� ������ #0
		short mdl_0_max;
		//!	\brief ����������� ����������� ��� ������ #1
		short mdl_1_min;
		//!	\brief ������������ ����������� ��� ������ #1
		short mdl_1_max;
		//!	\brief ����������� ����������� ��� ������� #2..#5
		short mdl_x_min;
		//!	\brief ������������ ����������� ��� ������� #2..#5
		short mdl_x_max;
	};
	#pragma pack(pop)

	//!	\brief �������� ������� ��������������� ������ (����� ������)
	/*!	�������� �������� ������� ��������������� ������. ������������ �����
		����� ��������� ������ ��������� models_desc_t. 

		\sa models_desc_t
	*/
	#pragma pack(push, 1)
	struct models_desc2_t
	{
		static const sz_t desced = ACODER_SPEC_MODELS_COUNT +
								   ACODER_MAP_MODELS_COUNT;

		short mins[desced];
		short maxs[desced];
		unsigned short abs_avgs[desced];
	};
	#pragma pack(pop)

	//!	\brief ��������������� �������� ������� ��������������� ������
	/*!	��������� ���������� � ���� ��������� ������� �������������
		�������� ������� ��������������� ������. ����������� �������,
		���������� � ���� ���������� ������������� ����������
		�������������� ������ �������������, ���� ������ ������� ���
		���� (� �������� ������ �� ����������).
	*/
	#pragma pack(push, 1)
	struct models_desc_t
	{
		//!	\brief ����������� ��������� �������� ������������� ��������
		//!	������� ��������������� ������
		union versions_u
		{
			//!	\brief ������ 1
			models_desc1_t v1;
			//!	\brief ������ 2
			models_desc2_t v2;
		};

		//!	\brief ������ ���������, ������������ ��� �������� ��������
		//!	������� ��������������� ������
		/*!	��������� ��������:
			- MODELS_DESC_V1 ���� ������������ ��������� models_desc1_t
			- MODELS_DESC_V2 ���� ������������ ��������� models_desc2_t
		*/
		short version;

		//!	\brief �������� ������� ��������������� ������
		versions_u md;
	};
	#pragma pack(pop)

	//!	\brief ��������� ��� �������� ���������� ����������� ��� ������������
	//!	������������� �������������� ������
	/*!	��������� �������� ��� ����������, ����������� ��� ���������
		��������. �� ������ ����� ��������� ��������:
		- �������������� ������������
		- �������� ������� ��������������� ������
	*/
	#pragma pack(push, 1)
	struct tunes_t
	{
		//!	\brief ������������
		q_t q;
		//!	\brief ������ ��������������� ������
		models_desc_t models;
	};
	#pragma pack(pop)

	//!	\brief ��������� ��������� ��������� ���������� �����������
	//!	��������� ������ ����� �������
	/*!	��������� ���� � ���� ���������� � ����������� ������ �����
		�� ����� ������ ������ ��������� - ����������� ���������
		�������� � ������� �������.
	*/
	struct optimize_result_t
	{
		//!	\brief �������� ������������, ��� ������� ���� �����������
		//!	�����������
		q_t q;
		//!	\brief �������� ��������� <i>lambda</i> ������������� ���
		//!	���������� <i>RD</i> �������� (<i>������� ��������</i>)
		lambda_t lambda;
		//!	\brief �������� <i>RD ������� ��������</i> ��� ����� �������
		j_t j;
		//!	\brief ��������� ������� ���������� ���, ������������� �� �����������
		//!	������ ������� (�������� �����������)
		/*!	������ ���� �������� (����������� ���������� ���������) ������
			���� ��� ����������� ��� ������ ����� ��� ������������ �����������
			(��. #OPTIMIZATION_USE_VIRTUAL_ENCODING). �������� ���������
			��������������� ������� ������� ������, ����������� ��� ������
			����������� ��� ������������� ����������� �����������. ��������
			������� ������� ������ ��������� ������.
		*/
		h_t bpp;

		//!	\brief �������� ������� ��������������� ������
		/*!	� ��� ���� ����� ���� �������� �������� ������� ���������������
			������, ���� � �������� ����������� ��������� ������� ���������
			����� ������� ������ ��� ����������� �������� �������. �����
			�������� ������� �������� ������ �����, ����� ����
			models_desc_t::version �� ����� MODELS_DESC_NONE.
		*/
		models_desc_t models;

		//!	\brief ������� ����������� ��������� �����������
		/*!	���� <i>true</i>, �� � �������� ����������� ���� ���������
			�������� ����������� �������� ������� � ��� ����� �� �����������
			��������.
		*/
		bool real_encoded;
	};

	//!	\brief ��������� �������������� ��������� ����������� �����������
	/*! ������ ��������� ����� ����� ������������� �������� � �� ��������
		����������� ��� ������������ �������������� �����������. �� ������
		���� ��������� ����� ������� ����� � ������� � �������� ������
		�����������, � ����� � ����������� ���������� ��������� ������
		����������� (��������, ����������� ��� ������� ����������).
	*/
	struct enc_result_t
	{
		//!	\brief ��������� ���������� �����������
		optimize_result_t optimization;
		//!	\brief ������� ���������� ���, ������������� �� �����������
		//!	������ ������� (�������� �����������)
		/*!	������ ���� �������� (����������� ���������� ���������) ������ �
			�������� �������� ���������������, � �� ����������� ���������������
			������� ������� ������.
		*/
		h_t bpp;
	};

	//!	\brief ��� ��� ������� ��������� ������, ���������� �� ����������
	//!	����� ����������� ��������� ���������� ������
	typedef void (* optimize_tree_callback_f)(const p_t &root,
											  void *const param);

	//!	\brief ��� ��� ������� ��������� ������, ���������� �� ����������
	//!	����� ����������� ��������� ������ ������ ������� �������������
	/*!	\param[in] result ��������� ������������ �����������
		\param[in] param ���������������� ��������
	*/
	typedef void (* optimize_callback_f)(const optimize_result_t &result,
										 void *const param);

	// public methods ----------------------------------------------------------

	//!	\name ������������ � �����������
	//@{

	//!	\brief ������������
	encoder(const sz_t width, const sz_t height, const sz_t lvls);

	//!	\brief ����������
	~encoder();

	//@}

	//!	\name ��������� ��������
	//@{

	//!	\brief ������������� ������� ��������� ������, ���������� ���
	//!	���������� ����������� ��������� ����� ������ ������� �������������
	void optimize_tree_callback(const optimize_tree_callback_f &callback,
								void *const param = 0);

	//!	\brief ������������� ������� ��������� ������, ���������� ���
	//!	���������� ����������� ��������� ������ ������ ������� �������������
	void optimize_callback(const optimize_callback_f &callback,
						   void *const param = 0);

	//@}

	//!	\name ������� ����������
	//@{

	//!	\brief ���������� ����������� ����������� � ������� ���������
	//!	����������� <i>q</i> � <i>lambda</i>
	enc_result_t encode(const w_t *const w, const q_t q, const lambda_t &lambda,
						tunes_t &tunes);

	//!	\brief ���������� ����������� ����������� ��� ������������� ���������
	//!	<i>lambda</i>, �������� �������� <i>q</i> ����������� �������� �������
	//!	<i>RD �������� ��������</i>
	enc_result_t encode_fixed_lambda(
						const w_t *const w, const lambda_t &lambda,
						tunes_t &tunes,
						const q_t &q_min, const q_t &q_max, const q_t &q_eps,
						const j_t &j_eps = 0, const sz_t &max_iterations = 0,
						const bool precise_bpp = false);

	//!	\brief ���������� ����������� ����������� ��� ������������� ���������
	//!	<i>lambda</i>, �������� �������� <i>q</i> ����������� �������� �������
	//!	<i>RD �������� ��������</i> (���������� ������)
	enc_result_t encode_fixed_lambda(const w_t *const w, const lambda_t &lambda,
									 tunes_t &tunes);

	//!	\brief ���������� ����������� ����������� ��� ������������� ���������
	//!	<i>q</i>, �������� �������� <i>lambda</i> ������� ������� ��������
	//!	������� ������
	enc_result_t encode_fixed_q(
						const w_t *const w, const q_t &q,
						const h_t &bpp, const h_t &bpp_eps,
						tunes_t &tunes,
						const lambda_t &lambda_min,
						const lambda_t &lambda_max,
						const lambda_t &lambda_eps = 0,
						const sz_t &max_iterations = 0,
						const bool precise_bpp = false);

	//!	\brief ���������� ����������� ����������� ��� ������������� ���������
	//!	<i>q</i>, �������� �������� <i>lambda</i> ������� ������� ��������
	//!	������� ������ (���������� ������)
	enc_result_t encode_fixed_q(const w_t *const w, const q_t &q,
								const h_t &bpp, tunes_t &tunes);

	//!	\brief ���������� ����������� �����������, �������� ���������
	//!	<i>q</i> � <i>lambda</i> ����� ������ �������� ������� ������ �
	//!	����������� ������� �����������
	enc_result_t encode_fixed_bpp(
						const w_t *const w,
						const h_t &bpp, const h_t &bpp_eps,
						const q_t &q_min, const q_t &q_max, const q_t &q_eps,
						const lambda_t &lambda_eps,
						tunes_t &tunes, const bool precise_bpp = false);

	//!	\brief ���������� ����������� �����������, �������� ���������
	//!	<i>q</i> � <i>lambda</i> ����� ������ �������� ������� ������ �
	//!	����������� ������� ����������� (���������� ������)
	enc_result_t encode_fixed_bpp(const w_t *const w, const h_t &bpp,
								  tunes_t &tunes);

	//@}

	//!	\name ������� �������������
	//@{

	//!	\brief ������� �������������� ���������������� �������������
	//!	������� �����������
	void decode(const byte_t *const data, const sz_t data_sz,
				const tunes_t &tunes);

	//@}

	//!	\name ������ � ���������� ���������
	//@{

	//!	\brief ������ ������� �������������
	/*!	\return ����������� ������ �� ���������� ������ ������ wic::wtree
	*/
	const wtree &spectrum() const { return _wtree; }

	//!	\brief ������ ������� �������������
	/*!	\return ������ �� ���������� ������ ������ wic::wtree
	*/
	wtree &spectrum() { return _wtree; }

	//!	\brief ������������ �������������� �����
	/*!	\return ����������� ������ �� ���������� ������ ������ wic::acoder
	*/
	const acoder &coder() const { return _acoder; }

	//!	\brief ������������ �������������� �����
	/*!	\return ������ �� ���������� ������ ������ wic::acoder
	*/
	acoder &coder() { return _acoder; }

	//@}

	//!	\brief ��������������� �������
	//@{

	//!	\brief ����������� ���������� �������� � �������� �����������
	//!	<i>lambda</i>
	static lambda_t quality_to_lambda(const double &quality);

	//@}

protected:
	// protected types ---------------------------------------------------------

	//! \brief ��������� ��������� ��������� ����������� �����
	struct _branch_topology_t
	{
		//!	\brief ��������� ������� ���������� ������
		n_t n;
		//!	\brief �������� ������� <i>��������</i>, ��� ����������
		//!	���������� ������, ������������� ���� <i>n</i>
		j_t j;
	};

	// protected methods -------------------------------------------------------

	//!	\name ������ � �������� ��������������� ������
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
		�� ����� ������������� ��� �������� ������ ������������������
		������������, �������� ��������� ����� �������� wnode::member_wc.
	*/
	template <const wnode::wnode_members member>
	sz_t _ind_spec(const p_t &p, const subbands::subband_t &sb)
	{
		return _ind_spec(_wtree.calc_sj<member>(p.x, p.y, sb),
						 sb.lvl);
	}

	//! \brief ��������� ������� IndMap(<i>P<sub>i</sub></i>) �� 35.pdf
	sz_t _ind_map(const pi_t &pi, const sz_t lvl);

	//!	\brief ��������� ������� IndMap(<i>P<sub>i</sub></i>) �� 35.pdf
	/*!	\param[in] p ���������� �������� � ���������� ��������� ����������
		\param[in] children_sb �������, � ������� ��������� ��������,
		�������� �� ������� � ������������ <i>p</i>
		\return ����� ���������� ������

		������� ���������� ���������� �������� � �������� �������
		����������.

		� ������� ��������� ������� �������� �������� ���� ��������,
		������� ����� �������������� ��� ���������� ���������. ��� ���,
		�� ����� ������������� ��� �������� ������ ������������������
		������������, �������� ��������� ����� �������� wnode::member_wc.
	*/
	template <const wnode::wnode_members member>
	sz_t _ind_map(const p_t &p, const subbands::subband_t &children_sb)
	{
		// ������� ���������� �������� Pi
		const pi_t pi_avg = _wtree.calc_pi_avg<member>(p, children_sb);

		// ����� ������ ��� ����������� ��������� ��������� ����������
		return _ind_map(pi_avg, children_sb.lvl + subbands::LVL_PREV);
	}

	//!	\brief ���������� ������ ������ ��� ����������� ����� ������������
	/*!	\param[in] p ���������� ������������
		\param[in] sb �������, � ������� ����� �������
		\return ������ ������ ��� ����������� ����� ������������
	*/
	template <const wnode::wnode_members member>
	inline sz_t _ind_sign(const p_t &p, const subbands::subband_t &sb)
	{
		// ��� LL �������� ��������� ����������� ����� �� �����������,
		// ������� � ����� ������ ������������ �������� ����� ������
		// ��������������� ������ ��� ����������� �����
		if (subbands::LVL_0 == sb.lvl) return 0;

		return _wtree.sign_num<member>(p, sb, ACODER_SIGN_MODELS_OFFSET);
	}

	//!	\brief ���������� ������ ������ ��� ����������� ����� ������������
	/*!	\param[in] p ���������� ������������
		\return ������ ������ ��� ����������� ����� ������������

		����� ��������� ������ �������, ������� �� ������� ������ ��������
		�������� � ������� ����� �����������.
	*/
	template <const wnode::wnode_members member>
	inline sz_t _ind_sign(const p_t &p)
	{
		return _ind_sign<member>(p, _wtree.sb().from_point(p));
	}

	//!	\brief ��������� ������ ��� ����������� ������ �������������
	/*!	\param[in,out] models ������ �������, � ������� ����� ���������
		������ ��� ����������� ������ �������������.
	*/
	void _ins_acoder_sign_models(acoder::models_t &models) const
	{
		// �������� �������� ������ ��� ����������� ������ �������������
		acoder::model_t model;
		model.abs_avg	= 0;
		model.min		= wnode::signp_min();
		model.max		= wnode::signp_max();

		// ���������� ������� � ������
		models.insert(models.end(), ACODER_SIGN_MODELS_COUNT, model);
	}

	//!	\brief ������ ������, ������������ �������������� ������� ��
	//!	������ ������������� � ������������ ��������� � �������
	/*!	\return ������ ��� ��������������� ������

		���������� ����� ��������� �������, ������ ����� ���� �������,
		��������, ��� �������.
		\sa acoder::use()
	*/
	template <const wnode::wnode_members member>
	acoder::models_t _mk_acoder_models()
	{
		// �������� ������� ��� �����������
		acoder::models_t models;
		acoder::model_t model;

		// ������ ������ �������� ������� ��������������� ������ ��
		// ����� �������������� �������� ������ ���������� ��������
		model.abs_avg = 0;

		// ������ ��� �������������
		// ����� ������������ � ������������� �������� �������������
		wnode::type_selector<member>::result w_min = 0;
		wnode::type_selector<member>::result w_max = 0;

		_wtree.minmax<member>(_wtree.iterator_over_wtree(), w_min, w_max);

		// +/-1 ����� ������ �������������
		model.min = w_min - 1;
		model.max = w_max + 1;

		models.insert(models.end(), ACODER_SPEC_MODELS_COUNT, model);

		// �������� ������� ��� ����������� ��������� ��������� ����������
		model.min = 0;
		model.max = 0x7;
		models.push_back(model);

		model.max = 0xF;
		models.insert(models.end(), ACODER_MAP_MODELS_COUNT - 1, model);

		// �������� ������� ��� ����������� ������ �������������
		_ins_acoder_sign_models(models);

		// �������� �����������
		assert(ACODER_TOTAL_MODELS_COUNT == models.size());

		return models;
	}

	//!	\brief ������ ������ ��� ��������������� ������ �� �� ��������
	acoder::models_t _mk_acoder_models(const models_desc1_t &desc) const;

	//!	\brief ������ ������ ��� ��������������� ������ �� �� ��������
	acoder::models_t _mk_acoder_models(const models_desc2_t &desc) const;

	//!	\brief ������ ������ ��� ��������������� ������ �� �� ��������
	acoder::models_t _mk_acoder_models(const models_desc_t &desc) const;

	//!	\brief ������ �������� �������, ������������ �������������� �������,
	//!	����������� �� ����������� ��������� (�������� ����������� �
	//!	������������ ��������� ��������� ���������)
	models_desc1_t _mk_acoder_smart_models() const;

	//!	\brief ������ �������� �������, ������������ �������������� �������,
	//! ������ �� ����������� ���������� ����������� ��� �������������
	//!	��������
	models_desc2_t _mk_acoder_post_models(const acoder &ac) const;

	//!	\brief ���������� ��������� ������� ��������������� ������
	models_desc_t _setup_acoder_models(const bool use_models = true);

	//!	\brief ���������� ��������� ������� ��������������� ������
	models_desc_t _setup_acoder_post_models(const bool use_models = true);

	//!	\brief ��������������� ������ ��������������� ������ ����� ����������
	//!	����������� (��������������� �������)
	bool _restore_spoiled_models(const optimize_result_t &result,
								 const acoder::models_t &models);

	//@}

	//!	\name ��������� ��������������� �����������
	//@{

	//! \brief ������������ ������� ������� ��� ����������� ������������.
	//! ��������� ������� <i>H<sub>spec</sub></i>.
	h_t _h_spec(const sz_t m, const wk_t &wk);

	//! \brief ������������ ������� ������� ��� ����������� ����������
	//! �������� ���������� ������. ��������� ������� <i>H<sub>map</sub></i>.
	h_t _h_map(const sz_t m, const n_t &n);

	//!	\brief �������� ����������� �������������� ���������
	void _encode_spec(const sz_t m, const wk_t &wk,
					  const bool virtual_encode = false);

	//!	\brief �������� ����������� �������������� ���������, ������ ����
	//!	���������� � ����������� �������
	void _encode_spec_se(const wk_t &wk, const sz_t spec_m,
						 const sz_t sign_m, const bool virtual_encode = false);

	//!	\brief �������� ��������� ������� ���������� ������ ��������������
	//!	���������
	void _encode_map(const sz_t m, const n_t &n,
					 const bool virtual_encode = false);

	//!	\brief ���������� ����������� �������������� ���������
	wk_t _decode_spec(const sz_t m);

	//!	\brief ���������� ����������� �������������� ���������, ������ ����
	//!	������������ �� ����������� �������
	wk_t _decode_spec_se(const sz_t spec_m, const sz_t sign_m);

	//!	\brief ���������� ��������� ������� ���������� ������ ��������������
	//!	���������
	n_t _decode_map(const sz_t m);

	//@}

	//! \name ������������� �������������
	//@{

	//!	\brief ������������ �������� <i>RD</i> ������� <i>��������</i>
	//! ��� �������� ������������ ��� ����������� ��� �������������� �������,
	//!	���� �� �� ��������� �� ����������� �����������.
	j_t _calc_rd_iteration(const p_t &p, const wk_t &k,
						   const lambda_t &lambda, const sz_t &model);

	//! \brief ���� ����������� ������������������ �������� ��� ������������
	wk_t _coef_fix(const p_t &p, const subbands::subband_t &sb,
				   const lambda_t &lambda);

	//! \brief ��������� ������������� ������ �������� ���������
	//!	(���� 1.1 � 2.3 � ���������, ��� ��������� <i>LL</i> ��������)
	void _coefs_fix(const p_t &p, const subbands::subband_t &sb_j,
					const lambda_t &lambda);

	//! \brief ��������� ������������� ������ �������� ���������
	//!	(���� 1.1 � 2.3 � ���������, � ���������� <i>LL</i> ��������)
	void _coefs_fix_uni(const p_t &p, const subbands::subband_t &sb_j,
						const lambda_t &lambda);

	//@}

	//!	\name ���������� � ��������� ���������� ������
	//@{

	//! \brief ����������� �������� <i>RD-������� ��������</i> ��� ��������
	//!	���������� ������
	/*!	\param[in] p ���������� �������� ��� �������� ����� �������������
		�������� <i>RD-������� ��������</i>.
		\return �������� <i>RD-������� ��������</i>.

		����� ��������, ��� ������� ���������� ������ <i>RD-�������
		��������</i>, ������������ ���������� �����, ������� � ��������
		���������. ������� �������, ��� ������� � ������������ <i>p</i>
		����� �� ����������. ���� ���������� �������� �������
		<i>use_node_j0</i> � <i>true</i>, �� ����� ����� ������ ��������
		�������� ��������� (� ��� �����, ���������� �� ����� ������), ��
		��� ����� ����������, ����� ���� wnode::j0 ����� ������ � ����������
		��������.

		��� ���������� ����� �� ����������� �� ������������� � ����������,
		������� ������� ������� �� ����������� ����������� ����� ������ �����
		0. �������������, ��������� <i>RD-������� ��������</i> ��������
		������� ������, ������� �� ������� ��� ���������� �����. ��������
		������� <i>use_node_j0</i> ��������� �������, ������� �� ��� ��������
		������ ��������� ������ ���������� ��� �������� ��������� � �����
		������ �������. ��� �������� ���� ����������� �������� ���� wnode::j0
		� ������ ��� ���������� �������������.

		\note ������� �� ��������� ��� ��������� �� <i>LL</i> ��������.

		\todo <b>���������� �������� ���� ��� ���� �������</b>
		\todo �������� ��-�� ����� ���-�� ������� � ���� ��������� ����������,
		������� �� ��� �� � ���������.
	*/
	template <const bool use_node_j0>
	j_t _calc_j0_value(const p_t &p)
	{
		j_t j0 = 0;

		for (wtree::coefs_iterator i = _wtree.iterator_over_children(p);
			 !i->end(); i->next())
		{
			const wnode &node = _wtree.at(i->get());

			if (use_node_j0) {
				j0 += (node.w * node.w + node.j0);
			} else {
				j0 += (node.w * node.w);
			}
		}

		return j0;
	}

	//! \brief ����������� <i>RD-������� ��������</i> ��� ��������
	//!	���������� ������ (��� ����� ������� ���������� � ����������
	//!	������)
	j_t _calc_j1_value(const p_t &p, const subbands::subband_t &sb,
					   const lambda_t &lambda);

	//!	\brief ����������� <i>RD-������� ��������</i> ��� ����� ������
	//!	(��� 3 ���������)
	j_t _calc_jx_value(const p_t &root, const j_t &j_map,
					   const lambda_t &lambda);

	//!	\brief ����������� �������� <i>RD-������� ��������</i> ��� ���������
	//!	���������� � ���������� ������. ������������ ���������� ��� ���������
	//!	���������� (����������, �����������) ������ ������. ������ ��� ������
	//!	���������� ������ ���������� (� ���������� ��������)
	void _prepare_j(const p_t &p, const subbands::subband_t &sb,
					const lambda_t &lambda);

	//!	\brief ����������� �������� <i>RD-������� ��������</i> ��� ���������
	//!	���������� � ���������� ������. ������������ ���������� ��� ���������
	//!	���������� (����������, �����������) ������ ������. ������ ��� ����
	//!	������� ����������, ����� ������ ���������� (� ���������� ��������)
	void _prepare_j(const p_t &p, const subbands::subband_t &sb,
					const j_t &j, const lambda_t &lambda);

	//@}

	//!	\name ����������� ��������� ������
	//@{

	//!	\brief ���������� ������� ������� �������� ��� ����� ���
	//!	����������� � ��������� (����� �� �� <i>LL</i> ��������)
	j_t _topology_calc_j(const p_t &branch, const n_t n);

	//!	\brief ���������� ������� ������� �������� ��� ����� ���
	//!	����������� � ��������� (����� �� <i>LL</i> ��������)
	j_t _topology_calc_j_LL(const p_t &branch, const n_t n);

	//!	\brief ���������� ������� ������� �������� ��� ����� ���
	//!	����������� � ��������� (����� �� ������ ��������)
	/*!	\param[in] branch ���������� ��������, ������������ � �������
		�����
		\param[in] n ��������� ������� ����������, ���������������
		��������� �����
		\return �������� ������� �������� ��� ��������� ��������� �
		<i>n</i>
	*/
	inline j_t _topology_calc_j_uni(const p_t &branch, const n_t n)
	{
		return (_wtree.sb().test_LL(branch))
				? _topology_calc_j_LL(branch, n)
				: _topology_calc_j(branch, n);
	}

	//!	\brief ���������� ����������� ��������� �����, ���� ����������
	//!	�������� ������.
	/*!	\param[in] branch ���������� ������������� ��������, �������
		������ �����.
		\param[in] sb �������, ���������� ������� <i>branch</i>
		\param[in] lambda �������� <i>lambda</i> ������� ��������� �
		���������� <i>RD</i> ������� � ������������ ����� ������ �����
		<i>R (rate)</i> � <i>D (distortion)</i> ������� �������
		<i>��������</i>.
		\return ��������� ������� ���������� ������

		�������� ����������� ��������� �������� ������ � <i>35.pdf</i>

		\note ������� ��������� ��� ���� ������ (��� ������� ������ �
		<i>LL</i> ��������, ��� � ��� ���� ���������).
	*/
	_branch_topology_t
	_optimize_branch_topology(const p_t &branch,
							  const subbands::subband_t &sb,
							  const lambda_t &lambda);

	//@}

	//!	\name ����������� ��������� ��������
	//@{

	//!	\brief ��������� ����������� ��������� �������� � ���������
	//!	� ������� ������ ����������.
	void _encode_tree_root(const p_t &root,
						   const bool virtual_encode = false);

	//!	\brief ��������� ����������� ������ ������
	void _encode_tree_leafs(const p_t &root, const sz_t lvl,
							const bool virtual_encode = false);

	//!	\brief ��������� ����������� ���������� ������
	void _encode_tree(const p_t &root,
					  const bool virtual_encode = false);

	//@}

	//!	\name ����������� ����� �������
	//@{

	//!	\brief ��������� �����������/������������� �������� ���������
	//!	�������
	void _encode_wtree_root(const bool decode_mode = false);

	//!	\brief ��������� �����������/������������� �� �������� ���������
	//!	������� ����� ������
	void _encode_wtree_level(const sz_t lvl,
							 const bool decode_mode = false);

	//!	\brief ��������� �����������/������������� ����� �������
	void _encode_wtree(const bool decode_mode = false);

	//@}

	//!	\name ����������� ��������� ���������� ������
	//@{

	//! \brief ��� 1. ����������������.
	void _optimize_tree_step_1(const p_t &root, const lambda_t &lambda);

	//! \brief ��� 2. �������� �������� ������ � �������� ���������� ������.
	void _optimize_tree_step_2(const p_t &root, const lambda_t &lambda);

	//! \brief ��� 3. ���������� <i>RD-������� ��������</i> ��� ����� ������
	void _optimize_tree_step_3(const p_t &root, const lambda_t &lambda);

	//!	\brief ��������������� ��������� ���� ����������� �������� ������
	j_t _optimize_tree(const p_t &root, const lambda_t &lambda);

	//@}

	//!	\name ����������� ���������� �����������
	//@{

	//!	\brief ���������� ����������� ��������� ���� ������ � �������
	optimize_result_t _optimize_wtree(const lambda_t &lambda,
									  const bool refresh_wtree = false,
									  const bool virtual_encode = false,
									  const bool precise_bpp = false);

	//!	\brief ���������� ����������� ��������� ���� ������ � �������
	//! � ��������������� ���������� ������� ��������������� ������
	optimize_result_t _optimize_wtree_m(const lambda_t &lambda,
										const models_desc_t &models,
										const bool refresh_wtree = false,
										const bool virtual_encode = false,
										const bool precise_bpp = false);

	//!	\brief ���������� ����������� ��������� ���� ������ � �������
	//! � ��������������� ������������ � ���������� ���������������
	//!	������.
	optimize_result_t _optimize_wtree_q(const lambda_t &lambda, const q_t &q,
										const bool virtual_encode = false,
										const bool precise_bpp = false);

	//!	\brief ���������� ����� ������������ <i>q</i> ��� �������� ���������
	//!	<i>lambda</i>, ����������� �������� <i>RD ������� �������� J</i>
	optimize_result_t _search_q_min_j(
							const lambda_t &lambda,
							const q_t &q_min, const q_t &q_max,
							const q_t &q_eps, const j_t &j_eps = 0,
							const bool virtual_encode = false,
							const sz_t &max_iterations = 0,
							const bool precise_bpp = false);

	//!	\brief ���������� ����� ��������� <i>lambda</i>, �������� ���
	//! ��� ������� <i>bpp</i>
	optimize_result_t _search_lambda_at_bpp(
							const h_t &bpp, const h_t &bpp_eps,
							const lambda_t &lambda_min,
							const lambda_t &lambda_max,
							const lambda_t &lambda_eps,
							const bool virtual_encode = false,
							const sz_t &max_iterations = 0,
							const bool precise_bpp = false);

	//!	\brief ���������� �������� ������ ��������� <i>lambda</i> ���
	//! �������� ��������� <i>q</i> ��� ������� _search_q_lambda_for_bpp
	optimize_result_t _search_q_lambda_for_bpp_iter(
							const q_t &q, const h_t &bpp,
							const h_t &bpp_eps, const lambda_t &lambda_eps,
							models_desc_t &models, w_t &d, h_t &deviation,
							const bool virtual_encode = false,
							const sz_t &max_iterations = 0,
							const bool precise_bpp = false);

	//!	\brief ���������� ����� ���������� <i>q</i> � <i>lambda</i>, �������
	//!	������� ������ ������� ������ �� ����������� �����������
	optimize_result_t _search_q_lambda_for_bpp(
							const h_t &bpp, const h_t &bpp_eps,
							const q_t &q_min, const q_t &q_max,
							const q_t &q_eps, const lambda_t &lambda_eps,
							models_desc_t &models,
							const bool virtual_encode = false,
							const bool precise_bpp = false);

	//@}

	//!	\name ���������� ��������� �����������
	//@{

	//!	\brief ��������� �������� ����������� ������� �������������
	//!	������� ��������������, ��������� ������� ��������� ���������������
	//!	������
	h_t _real_encode();

	//!	\brief ��������� �������� ����������� ������� �������������
	//!	������� ��������������, �������������� ��������� ������� ���������
	//!	��������������� ������
	h_t _real_encode_tight(models_desc_t &desc, const bool double_path = true);

	//@}

	//!	\name ���������� �������������
	//@{

	//!	\brief ���������� �������� ������� ������� ������� �� �����������
	//!	������ �������� ����������� (�������)
	h_t _calc_encoded_bpp(const bool including_tunes = true);

	//@}

	//!	\name ����� ��������� ���������� �������
	//@{

	//!	\brief ��������� �� ����������� ���� �������� [136, 0]
	void _test_wc_136_0();

	//@}

private:
	// private data ------------------------------------------------------------

	//! \brief ������� ������ �������������
	wtree _wtree;

	//! \brief �������������� �����
	acoder _acoder;

	//!	\brief ������� ���������� ����� ���������� �������� �����������
	//!	����� ����� ������ ������� �������������
	optimize_tree_callback_f _optimize_tree_callback;

	//!	\brief ��������, ������������ � ������� ��������� ������
	//!	_optimize_tree_callback
	void *_optimize_tree_callback_param;

	//!	\brief ������� ���������� ����� ���������� �������� �����������
	//!	������ ������ ������� �������������
	optimize_callback_f _optimize_callback;

	//!	\brief ��������, ������������ � ������� ��������� ������
	//!	_optimize_callback
	void *_optimize_callback_param;

	#ifdef LIBWIC_DEBUG
	//!	\brief ����������� �������� ����� ��� ������ ���������� �
	//!	���������� ������
	std::ofstream _dbg_out_stream;
	#endif

	#ifdef LIBWIC_USE_DBG_SURFACE
	//!	\brief ���������� ����������� ��� ����� ����������� ������ �����,
	//!	������������ �� ����� ����������� ��������� ������
	wicdbg::dbg_surface _dbg_opt_surface;

	//!	\brief ���������� ����������� ��� ����� ����������� ������ �����,
	//!	������������ �� ����� ��������� �����������
	wicdbg::dbg_surface _dbg_enc_surface;

	//!	\brief ���������� ����������� ��� ����� ����������� ������ �����,
	//!	������������ �� ����� �������������
	wicdbg::dbg_surface _dbg_dec_surface;
	#endif
};



}	// end of namespace wic



#endif	// WIC_LIBWIC_ENCODER
