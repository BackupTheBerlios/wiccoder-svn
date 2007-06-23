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

// standard C++ library headers
#include <assert.h>
#include <algorithm>
#include <string>						// for debug output only
#include <fstream>						// for debug output only
#include <iomanip>						// for debug output only
#include <time.h>						// for debug output only

// external library header
// none

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

#define LIBWIC_DEBUG


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

	//!	\brief �������� ������� ��������������� ������
	/*!	��������� �������� ����������� �������� ������� ��������������� ������.
		��������� ��� ����������, ������� ��������� ������, ������� �������
		����������� ��� �����������.
	*/
	struct models_desc_t
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

	//!	\brief ��������� ��� �������� ���������� ����������� ��� ������������
	//!	������������� �������������� ������
	/*!	\todo ����� �������� ������� ��� ��������� � � �������������
	*/
	struct header_t
	{
		//!	\brief ������������
		q_t q;
		//!	\brief ������ ��������������� ������
		models_desc_t models;
	};

	// public constants --------------------------------------------------------

	//!	\brief ����������� ���������� ���������� ������� ����������
	static const sz_t MINIMUM_LEVELS			= 3;

	//!	\brief ���������� �������, ������������ �������������� ������� ���
	//!	����������� �������������
	static const sz_t ACODER_SPEC_MODELS_COUNT	= 6;

	//!	\brief ���������� �������, ������������ �������������� ������� ���
	//!	����������� ��������� ��������� ����������
	static const sz_t ACODER_MAP_MODELS_COUNT	= 5;

	//!	\brief ����� ���������� �������, ������������ �������������� �������
	static const sz_t ACODER_TOTAL_MODELS_COUNT	= ACODER_SPEC_MODELS_COUNT +
												  ACODER_MAP_MODELS_COUNT;

	// public methods ----------------------------------------------------------

	//!	\name ������������ � �����������
	//@{

	//!	\brief ������������
	encoder(const sz_t width, const sz_t height, const sz_t lvls);

	//!	\brief ����������
	~encoder();

	//@}

	//!	\name ������� ����������
	//@{

	//!	\brief ������� �������������� ���������������� ����������� �����������
	void encode(const w_t *const w, const q_t q, const lambda_t &lambda,
				header_t &header);

	//@}

	//!	\name ������� �������������
	//@{

	//!	\brief ������� �������������� ���������������� ������������� �����������
	void decode(const byte_t *const data, const sz_t data_sz,
				const header_t &header);

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

protected:
	// protected types ---------------------------------------------------------

	//! \brief ��������� ��������� ��������� ����������� �����
	struct _branch_topology_t {
		//!	\brief ��������� ������� ���������� ������
		n_t n;
		//!	\brief �������� ������� <i>��������</i>, ��� ����������
		//!	���������� ������, ������������� ���� <i>n</i>
		j_t j;
	};

	//!	\brief ��������� ��������� ��������� ���������� �����������
	//!	��������� ������ ����� �������
	struct _optimize_result_t {
		//!	\brief �������� ������������, ��� ������� ����
		//!	����������� �����������
		q_t q;
		//!	\brief �������� ��������� <i>lambda</i> ������������� ���
		//!	���������� <i>RD</i> �������� (������� ��������)
		lambda_t lambda;
		//!	\brief �������� ������� <i>��������</i> ��� ����� �������
		j_t j;
		//!	\brief ������� ���������� ���, ������������� �� �����������
		//!	������ ������� (�������� �����������)
		h_t bpp;
	};

	//!	\brief ��������� ��������� ��������� ������ (�������) ���������
	//!	�����������
	struct _search_result_t {
		//!	\brief ��������� ���������� ����������� ��������� ������
		_optimize_result_t optimized;
	};

	//!	\brief ��������� ��������� ��������� �����������
	struct _encode_result_t {
		//!	\brief �������� ������� <i>��������</i> ��� ����� �������
		j_t j;
		//!	\brief ������� ���������� ���, ������������� �� �����������
		//!	������ ������� (�������� �����������)
		h_t bpp;
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

		// �������� �����������
		assert(ACODER_TOTAL_MODELS_COUNT == models.size());

		return models;
	}

	//!	\brief ������ ������ ��� ��������������� ������ �� �� ��������
	acoder::models_t _mk_acoder_models(const models_desc_t &desc);

	//!	\brief ������ ������, ������������ �������������� �������,
	//!	����������� �� ����������� ���������
	encoder::models_desc_t _mk_acoder_smart_models();

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

	//!	\brief �������� ��������� ������� ���������� ������ ��������������
	//!	���������
	void _encode_map(const sz_t m, const n_t &n,
					 const bool virtual_encode = false);

	//!	\brief ���������� ����������� �������������� ���������
	wk_t _decode_spec(const sz_t m);

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
	_optimize_result_t _optimize_wtree(const lambda_t &lambda,
									   const bool virtual_encode = false);

	//!	\brief ���������� ����������� ��������� ���� ������ � �������
	//! � ��������������� ������������ � ���������� ���������������
	//!	������.
	_optimize_result_t _optimize_wtree(const lambda_t &lambda,
									   const q_t &q, models_desc_t &models,
									   const bool virtual_encode = false);

	//!	\brief ���������� ����� ��������� <i>lambda</i>, �������� ���
	//! ��� ������� <i>bpp</i>
	_search_result_t _search_lambda(const h_t &bpp,
									const lambda_t &lambda_min,
									const lambda_t &lambda_max,
									const h_t &bpp_eps,
									const lambda_t &lambda_eps);

	_search_result_t _search_q_min_j(const lambda_t &lambda,
									 models_desc_t &models,
									 const q_t &q_min, const q_t &q_max,
									 const q_t &j_eps, const q_t &q_eps);

	//@}

private:
	// private data ------------------------------------------------------------

	//! \brief ������� ������ �������������
	wtree _wtree;

	//! \brief �������������� �����
	acoder _acoder;

	#ifdef LIBWIC_DEBUG
	//!	\brief ����������� �������� ����� ��� ������ ���������� �
	//!	���������� ������
	std::ofstream _dbg_out_stream;
	#endif
};



}	// end of namespace wic



#endif	// WIC_LIBWIC_ENCODER
