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

// external library header
// none

// libwic headers
#include <wic/libwic/types.h>
#include <wic/libwic/wtree.h>
#include <wic/libwic/acoder.h>
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
	encoder(const w_t *const image,
			const sz_t width, const sz_t height, const sz_t lvls);

	//!	\brief ����������
	~encoder();

	//@}

	//!	\name ������� ����������
	//@{

	//!	\brief ������� �������������� ���������������� ����������� �����������
	void encode(const lambda_t &lambda);

	//@}

	//!	\name ������� �������������
	//@{

	void decode();

	//@}

	//!	\name ������ � ���������� ���������
	//@{

	//!	\brief ������ ������� �������������
	/*!	\return ������ �� ���������� ������ ������ wic::wtree
	*/
	const wtree &spectrum() const { return _wtree; }

	//@}

protected:
	// protected types ---------------------------------------------------------

	//! \brief ��������� ��������� ��������� ����������� �����
	struct _branch_topology_t {
		//!	\brief ��������� ������� ���������� ������
		n_t n;
		//!	\brief �������� ������� ��������, ��� ���������� ����������
		//!	������, ������������� ���� <i>n</i>
		j_t j;
	};

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

	//@}

	//!	\name ��������� ��������������� �����������
	//@{

	//!	\brief ������ ������, ������������ �������������� ������� ��
	//!	������ ���������� �� �������
	/*!	\return ������ ��� ��������������� ������

		\sa acoder::use()
	*/
	template <const wnode::wnode_members member>
	acoder::models_t _mk_acoder_models()
	{
		// ����� ������������ � ������������� �������� �������������
		wnode::type_selector<member>::result w_min = 0;
		wnode::type_selector<member>::result w_max = 0;

		_wtree.minmax<member>(_wtree.iterator_over_wtree(), w_min, w_max);

		// �������� ������� ��� ����������� �������������
		acoder::models_t models;

		acoder::model_t model;
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

	//! \brief ������������ ������� ������� ��� ����������� ������������.
	//! ��������� ������� <i>H<sub>spec</sub></i>.
	h_t _h_spec(const sz_t m, const wk_t &wk);

	//! \brief ������������ ������� ������� ��� ����������� ����������
	//! �������� ���������� ������. ��������� ������� <i>H<sub>map</sub></i>.
	h_t _h_map(const sz_t m, const n_t &n);

	//!	\brief �������� ����������� �������������� ���������
	void _encode_spec(const sz_t m, const wk_t &wk);

	//!	\brief �������� ��������� ������� ���������� ������ ��������������
	//!	���������
	void _encode_map(const sz_t m, const n_t &n);

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
				j0 += (node.wc * node.wc + node.j0);
			} else {
				j0 += (node.wc * node.wc);
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
				? _topology_calc_j(branch, n)
				: _topology_calc_j_LL(branch, n);
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
	void _encode_tree_root(const p_t &root);

	//!	\brief ��������� ����������� ������ ������
	void _encode_tree_leafs(const p_t &root, const sz_t lvl);

	//!	\brief ��������� ����������� ���������� ������
	void _encode_tree(const p_t &root);

	//@}

	//!	\name ����������� ����� �������
	//@{

	//!	\brief ��������� �����������/������������� �������� ���������
	//!	�������
	void _encode_wtree_root(const bool decode_mode = false);

	//!	\brief ��������� �����������/������������� �� �������� ���������
	//!	�������
	void _encode_wtree_subband(const subbands::subband_t &sb,
							   const bool decode_mode = false);

	//!	\brief ��������� �����������/������������� ����� �������
	void _encode_wtree(const bool decode_mode = false);

	//@}

	//!	\name ����������� ��������� ������
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

private:
	// private data ------------------------------------------------------------

	//! \brief ������� ������ �������������
	wtree _wtree;

	//! \brief �������������� �����
	acoder _acoder;
};



}	// end of namespace wic



#endif	// WIC_LIBWIC_ENCODER
