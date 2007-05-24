/*******************************************************************************
* file:         encoder.cpp                                                    *
* version:      0.1.0                                                          *
* author:       mice (mailto:my_mice@mail.ru, ICQ:332-292-380)                 *
* description:  not available                                                  *
* tests:        none                                                           *
*******************************************************************************/


////////////////////////////////////////////////////////////////////////////////
// include

// standard C++ headers
// none

// libwic headers
#include <wic/libwic/encoder.h>


////////////////////////////////////////////////////////////////////////////////
// wic namespace
namespace wic {


////////////////////////////////////////////////////////////////////////////////
// encoder class public definitions

/*!	\param[in] image ��������� ������� �������������� ����������� ��� ������.
	\param[in] width ������ �����������.
	\param[in] height ������ �����������.
	\param[in] lvls ���������� ������� ������� ��������������.
*/
encoder::encoder(const w_t *const image,
				 const sz_t width, const sz_t height, const sz_t lvls):
	_wtree(width, height, lvls)
{
	// �������� ������������� ���������� � ������
	_wtree.load(image);

	// ��������� ������ ��� ��������������� ��������
	_aenc.mk_buf(width * height * sizeof(aencoder::tv_t));
}


/*!
*/
encoder::~encoder() {
}


/*!
*/
void encoder::encode() {
	_aenc.begin();

	const subbands &sb = _wtree.sb();

	for (wtree::coefs_iterator i = _wtree.iterator_over_subband(sb.get_LL());
		 !i->end(); i->next())
	{
		const p_t &root = i->get();
		_encode_step_1(root, 0);
		_encode_step_2(root, 0);
	}

	_aenc.end();
}



////////////////////////////////////////////////////////////////////////////////
// wtree class protected definitions

/*!	\param[in] s �������� ���������� �������� <i>S<sub>j</sub></i>
	\param[in] lvl ����� ������ ����������, �� �������� ��� ���� �����������
	\return ����� ���������� ������
*/
sz_t encoder::_ind_spec(const pi_t &s, const sz_t lvl) {
	if (0 == lvl) return 0;
	if (1 == lvl) return 1;

	if (26.0  <= s) return 1;
	if ( 9.8  <= s) return 2;
	if ( 4.1  <= s) return 3;
	if ( 1.72 <= s) return 4;

	return 5;
}


/*!	\param[in] p �������� ���������� �������� <i>P<sub>i</sub></i>
	\param[in] is_LL ������ �� ����������� �� �������� LL
	\return ����� ���������� ������
*/
sz_t encoder::_ind_map(const pi_t &p, const bool is_LL) {
	if (is_LL) return 0;

	if (4.0 <= p) return 4;
	if (1.1 <= p) return 3;
	if (0.3 <= p) return 2;

	return 1;
}


/*!	\param[in] m ����� ������ ��� �����������
	\param[in] wk �������� ������������ ��� �����������
	\return ������� �������, ����������� ��� ����������� ������������ �
	�������������� ���� ������

	\todo ���������� ��������� �������� ��� ������ ������ ������, �.�.
	������ ���������� � 0 ��� ��� �������������, ��� � ��� ���������.
*/
h_t encoder::_h_spec(const sz_t m, const wk_t &wk) {
	return _aenc.entropy(wk, m);
}


/*!	\param[in] m ����� ������ ��� �����������
	\param[in] n �������� ���������� �������� ���������� ������
	\return ������� �������, ����������� ��� ����������� ����������
	�������� ����������

	\todo ���������� ��������� �������� ��� ������ ������ ������, �.�.
	������ ���������� � 0 ��� ��� �������������, ��� � ��� ���������.
*/
h_t encoder::_h_map(const sz_t m, const n_t &n) {
	return _aenc.entropy(n, m);
}


/*!	\param[in] p �������������� ���������� �������� (������������)
	\param[in] k ������������������ (��� ������ ���������������) ��������
	������������
	\param[in] lambda �������� <i>lambda</i>, �������� �� <i>Rate/Distortion</i>
	������ ��� ���������� <i>RD</i> �������. ��� ��� �������� ������, ���
	������� ����� � �������� <i>RD</i> ������� ����� ������� ������� �������
	�� ����������� ������������ �������������� �������.
	\param[in] model ����� ������ ��������������� ������, ������� �����
	������������ ��� ����������� ������������
	\return �������� <i>RD</i> ������� <i>���������</i>

	\todo �������� ���� ��� ���� �������
	\todo �������� ����� ������� ��� ������ � �������� ������� ���������������
	������.
*/
j_t encoder::_calc_rd_iteration(const p_t &p, const wk_t &k,
								const lambda_t &lambda, const sz_t &model)
{
	const wnode &node = _wtree.at(p);

	const w_t dw = (wnode::dequantize(k, _wtree.q()) - node.w);

	return (dw*dw + lambda * _h_spec(model, k));
}


/*!	\param[in] p ���������� ������������ ��� �������������
	\param[in] sb �������, � ������� ��������� �����������
	\param[in] lambda �������� <i>lambda</i> ������������ ���
	���������� <i>RD</i> �������� (������� ��������). ������������
	����� ������ ����� ������� � �������� ���������.
	\return �������� ������������������� ������������

	�������� ������� ��������� �������� ���� �������� ��� �������������.

	\todo �������� ���� ��� ���� �������
*/
wk_t encoder::_coef_fix(const p_t &p, const subbands::subband_t &sb,
						const lambda_t &lambda)
{
	// ����� ������ � ������������� �������� ������������
	const sz_t	model	= _ind_spec<wnode::member_wc>(p, sb);
	const wk_t	&wq		= _wtree.at(p).wq;

	// ����������� ������ ����������� ��������
	#ifdef COEF_FIX_USE_4_POINTS
		static const sz_t vals_count	= 4;
		const wk_t w_vals[vals_count] = {0, wq, wq + 1, wq - 1};
	#else
		static const sz_t vals_count	= 3;
		const wk_t w_drift = (0 <= w)? -1; +1;
		const wk_t w_vals[vals_count] = {0, wq, wq + w_drift};
	#endif

	// ��������� �������� ��� ������ �������� RD �������
	wk_t k_optim = w_vals[0];
	j_t j_optim = _calc_rd_iteration(p, k_optim, lambda, model);

	// ����� ������������ �������� RD �������
	for (int i = 1; vals_count > i; ++i) {
		const wk_t &k = w_vals[i];
		const j_t j = _calc_rd_iteration(p, k, lambda, model);
		if (j < j_optim) {
			j_optim = j;
			k_optim = k;
		}
	}

	return k_optim;
}


/*!	\param[in] root ���������� ��������� �������� ���������������� ������
	\param[in] lambda �������� <i>lambda</i> ������������ ���
	���������� <i>RD</i> �������� (������� ��������). ������������
	����� ������ ����� ������� � �������� ���������.

	�� ������ ���� ����������� ����������� ������������� �������������
	�� ����� ��������� (� ���������� ��������) ������ ���������� �
	����������� ������ <i>RD-������� ��������</i> ��� ��������� ����������
	� ���������� ������� ������.
*/
void encoder::_encode_step_1(const p_t &root, const lambda_t &lambda)
{
	// ��������������� ��� ���� �������������� ������
	const sz_t lvl_j = _wtree.sb().lvls();
	const sz_t lvl_i = _wtree.sb().lvls() + subbands::LVL_PREV;

	// ���� �� ���������
	for (sz_t k = 0; subbands::SUBBANDS_ON_LEVEL > k; ++k)
	{
		// ������������� ��������������� �������������
		const subbands::subband_t &sb_j = _wtree.sb().get(lvl_j, k);

		for (wtree::coefs_iterator i = _wtree.iterator_over_leafs(root, sb_j);
			!i->end(); i->next())
		{
			const p_t &p = i->get();
			_wtree.at(p).wc = _coef_fix(p, sb_j, lambda);
		}

		// ������ RD-������� �������� ��� ��������� ���������� �
		// ���������� �������
		const subbands::subband_t &sb_i = _wtree.sb().get(lvl_i, k);

		for (wtree::coefs_iterator i = _wtree.iterator_over_leafs(root, sb_i);
			!i->end(); i->next())
		{
			const p_t &p = i->get();
			wnode &node = _wtree.at(p);
			node.j0 = _calc_j0_value<false>(p);
			node.j1 = _calc_j1_value(p, sb_j, lambda);
		}
	}
}


//! \brief ��� 2. �������� �������� ������ � �������� ���������� ������.
void encoder::_encode_step_2(const p_t &root, const lambda_t &lambda)
{
	// ��� 2.1. ����������� ����������� ��������� ������
	_optimize_branch_topology(root, 0, 0, 0);
}


}	// end of namespace wic
