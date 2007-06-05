/*!	\file     encoder.cpp
	\version  0.0.1
	\author   mice, ICQ: 332-292-380, mailto:wonder.mice@gmail.com
	\brief    ���������� ������ wic::encoder

	\todo     ����� �������� ������� ���� encoder.h
*/


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
		_optimize_tree(root, 0);
		_encode_tree(root);
	}

	_aenc.end();
}



////////////////////////////////////////////////////////////////////////////////
// wtree class protected definitions

/*!	\param[in] s �������� ���������� �������� <i>S<sub>j</sub></i>
	\param[in] lvl ����� ������ ����������, �� �������� ��� ���� �����������
	\return ����� ���������� ������

	\note ����� ��������, ��� ��� ������� � ������� ������� �������
	���������� ����������� ��������, ���������� �� ��������� <i>s</i>.
	<i>pi</i>.
*/
sz_t encoder::_ind_spec(const pi_t &s, const sz_t lvl) {
	if (subbands::LVL_0 == lvl) return 0;
	if (subbands::LVL_1 == lvl) return 1;

	if (26.0  <= s) return 1;
	if ( 9.8  <= s) return 2;
	if ( 4.1  <= s) return 3;
	if ( 1.72 <= s) return 4;

	return 5;
}


/*!	\param[in] pi �������� ���������� �������� <i>P<sub>i</sub></i>
	\param[in] lvl ����� ������ ����������, �� �������� ��� ���� ���������
	������� ���������� ������
	\return ����� ���������� ������

	\note ����� ��������, ��� ���� �������� <i>is_LL</i> ����� <i>true</i>
	������� ������ ���������� ������� ������, ���������� �� ���������
	<i>pi</i>.
*/
sz_t encoder::_ind_map(const pi_t &pi, const sz_t lvl) {
	if (subbands::LVL_0 == lvl) return 0;

	if (4.0 <= pi) return 4;
	if (1.1 <= pi) return 3;
	if (0.3 <= pi) return 2;

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
	\return �������� <i>RD-������� ���������</i>

	\note ������� ��������� ��� ��������� �� ����� ���������.
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

	\note ������� ��������� ��� ������������� �� ����� ���������

	\sa COEF_FIX_USE_4_POINTS

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

	// ������� ������������������� �������� ������������
	return k_optim;
}


/*!	\param[in] p ���������� ������������� ��������
	\param[in] sb_j �������, � ������� ��������� �������� ��������
	\param[in] lambda �������� <i>lambda</i> ������������ ���
	���������� <i>RD</i> �������� (������� ��������). ������������
	����� ������ ����� ������� � �������� ���������.

	\note ������� ��������� ������ ��� ������������ ��������� �� ��
	<i>LL</i> ��������.
*/
void encoder::_coefs_fix(const p_t &p, const subbands::subband_t &sb_j,
						 const lambda_t &lambda)
{
	// ���� �� �������� ���������
	for (wtree::coefs_iterator i = _wtree.iterator_over_children(p);
		 !i->end(); i->next())
	{
		const p_t &p = i->get();
		wnode &node = _wtree.at(p);
		node.wc = _coef_fix(p, sb_j, lambda);
	}
}


/*!	\param[in] p ���������� ������������� ��������
	\param[in] sb_j �������, � ������� ��������� �������� ��������
	\param[in] lambda �������� <i>lambda</i> ������������ ���
	���������� <i>RD</i> �������� (������� ��������). ������������
	����� ������ ����� ������� � �������� ���������.

	\note ������� ��������� ������ ��� ������������ ��������� ��
	����� ���������.
*/
void encoder::_coefs_fix_uni(const p_t &p, const subbands::subband_t &sb_j,
							 const lambda_t &lambda)
{
	// ���� �� �������� ���������
	for (wtree::coefs_iterator i = _wtree.iterator_over_children_uni(p);
		 !i->end(); i->next())
	{
		const p_t &p = i->get();
		wnode &node = _wtree.at(p);
		node.wc = _coef_fix(p, sb_j, lambda);
	}
}


/*!	\param[in] p ���������� �������� ��� �������� ����� �������������
	\param[in] sb �������, � ������� ��������� ������������ �� �����������
	�����. ������� �������, ���� ������� �������� ��� ����, � �������
	��������� ������� � ������������ <i>p</i>.
	\param[in] lambda �������� <i>lambda</i> ������� ��������� � ����������
	<i>RD</i> ������� � ������������ ����� ������ ����� <i>R (rate)</i> �
	<i>D (distortion)</i> ������� <i>������� ��������</i>.
	\return �������� <i>RD ������� ��������</i>.

	\note ������� �� ��������� ��� ��������� �� <i>LL</i> ��������.

	\sa _calc_j0_value()

	\todo ���������� �������� ���� ��� ���� �������.
*/
j_t encoder::_calc_j1_value(const p_t &p, const subbands::subband_t &sb,
							const lambda_t &lambda)
{
	// ��������� ������ ������ ��� ����������� �������������
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


/*!	\param[in] root ���������� ��������� ��������
	\param[in] j_map �������� <i>RD-������� ��������</i> ����������
	� ���� 2.6.
	\param[in] lambda �������� <i>lambda</i> ������� ��������� � ����������
	<i>RD</i> ������� � ������������ ����� ������ ����� <i>R (rate)</i> �
	<i>D (distortion)</i> ������� <i>������� ��������</i>.
	\return �������� <i>RD-������� ��������</i>.

	������� ����� ��������� ���������� �������� <i>RD-������� ��������</i> �
	����� wnode::j0 � wnode::j1 ��������� ��������.
*/
j_t encoder::_calc_jx_value(const p_t &root, const j_t &j_map,
							const lambda_t &lambda)
{
	assert(_wtree.sb().test_LL(root));

	// �������� ������ �� LL �������
	const subbands::subband_t &sb_LL = _wtree.sb().get_LL();

	j_t j = j_map;

	// ���� �� �������� ���������
	for (wtree::coefs_iterator i = _wtree.iterator_over_LL_children(root);
		 !i->end(); i->next())
	{
		const p_t &p = i->get();

		const wnode &node = _wtree.at(p);

		j += _calc_rd_iteration(p, node.wc, lambda,
								_ind_spec<wnode::member_wc>(p, sb_LL));
	}

	wnode &node = _wtree.at(root);

	return (node.j0 = node.j1 = j);
}


/*!	\param[in] p ���������� ��������, ��� �������� ����������� ����������
	�������� <i>J</i> (<i>RD ������� ��������</i>) 
	\param[in] sb_j ������� � ������� ��������� �������� ��������
	\param[in] lambda �������� <i>lambda</i> ������������ ���
	���������� <i>RD</i> �������� (������� ��������). ������������
	����� ������ ����� ������� � �������� ���������.

	������� ������ ����������� ��� �������� � ������ <i>lvl</i>, ��
	������� <i>lvl + subbands::LVL_PREV</i>.

	\note ������� �� ��������� ��� ��������� �� <i>LL</i> ��������.
*/
void encoder::_prepare_j(const p_t &p, const subbands::subband_t &sb_j,
						 const lambda_t &lambda)
{
	wnode &node = _wtree.at(p);

	node.j0 = _calc_j0_value<false>(p);
	node.j1 = _calc_j1_value(p, sb_j, lambda);
}


/*!	\param[in] p ���������� ��������, ��� �������� ����������� ����������
	�������� <i>J</i> (<i>RD ������� ��������</i>) 
	\param[in] sb_j ������� � ������� ��������� �������� ��������
	\param[in] j �������� ������� ��������, ���������� ��� �������
	����������� ��������� ������
	\param[in] lambda �������� <i>lambda</i> ������������ ���
	���������� <i>RD</i> �������� (������� ��������). ������������
	����� ������ ����� ������� � �������� ���������.

	������� ������ ����������� ��� �������� � ������ <i>lvl</i>, ��
	������� <i>lvl + subbands::LVL_PREV</i>.

	\note ������� �� ��������� ��� ��������� �� <i>LL</i> ��������.
*/
void encoder::_prepare_j(const p_t &p, const subbands::subband_t &sb_j,
						 const j_t &j, const lambda_t &lambda)
{
	wnode &node = _wtree.at(p);

	node.j0 = _calc_j0_value<true>(p);
	node.j1 = j + _calc_j1_value(p, sb_j, lambda);
}


/*!	\param[in] branch ���������� ��������, ������������ � �������
	�����
	\param[in] n ��������� ������� ����������, ���������������
	��������� �����
	\return �������� ������� �������� ��� ��������� ��������� �
	<i>n</i>
*/
j_t encoder::_topology_calc_j(const p_t &branch, const n_t n)
{
	j_t j = 0;

	for (wtree::coefs_iterator i =
				_wtree.iterator_over_children(branch);
		 !i->end(); i->next())
	{
		const p_t &p = i->get();
		const wnode &node = _wtree.at(p);
		const n_t mask = _wtree.child_n_mask(p, branch);
		j += (_wtree.test_n_mask(n, mask))? node.j1: node.j0;
	}

	return j;
}


//!	����������� � ��������� (����� �� <i>LL</i> ��������)
/*!	\param[in] branch ���������� ��������, ������������ � �������
	�����
	\param[in] n ��������� ������� ����������, ���������������
	��������� �����
	\return �������� ������� �������� ��� ��������� ��������� �
	<i>n</i>
*/
j_t encoder::_topology_calc_j_LL(const p_t &branch, const n_t n)
{
	// ��������� �������� ��� ������� ��������
	j_t j = 0;

	for (wtree::coefs_iterator i =
				_wtree.iterator_over_LL_children(branch);
		 !i->end(); i->next())
	{
		const p_t &p = i->get();
		const wnode &node = _wtree.at(p);
		const n_t mask = _wtree.child_n_mask_LL(p);
		j += (_wtree.test_n_mask(n, mask))? node.j1: node.j0;
	}

	return j;
}


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
encoder::_branch_topology_t
encoder::_optimize_branch_topology(const p_t &branch,
								   const subbands::subband_t &sb,
								   const lambda_t &lambda)
{
	assert(subbands::LVL_0 != sb.lvl);

	// ��������� ��������� ��������
	const sz_t lvl_j = sb.lvl + subbands::LVL_NEXT;
	const subbands::subband_t &sb_j = _wtree.sb().get(lvl_j, sb.i);

	// ������� ���������� �������� Pi
	const pi_t pi_avg = _wtree.calc_pi_avg<wnode::member_wc>(branch, sb_j);

	// ����� ������ ��� ����������� ��������� ��������� ����������
	const sz_t model = _ind_map(pi_avg, sb.lvl);

	// ����� �������� ����������� ���������
	wtree::n_iterator i = _wtree.iterator_through_n(sb.lvl);

	// ������ �������� ����� ������
	_branch_topology_t optim_topology;
	optim_topology.n	= i->get();
	optim_topology.j	= _topology_calc_j_uni(branch,
											   optim_topology.n);

	// ����������� ��������
	for (i->next(); !i->end(); i->next())
	{
		const n_t &n = i->get();

		const j_t j_sum = _topology_calc_j_uni(branch, n);

		const j_t j = (j_sum + lambda * _h_map(model, n));

		if (j < optim_topology.j) {
			optim_topology.j = j;
			optim_topology.n = n;
		}
	}

	return optim_topology;
}


/*!	\param[in] branch 
*/
void encoder::_encode_leafs(const p_t &branch)
{
	(void)branch;
	/*
	for (wtree::coefs_iterator i = _wtree.iterator_over_leafs(branch);
		 !i->end(); i->next())
	{
	}
	*/
}


/*!	\param[in] root ���������� ��������� �������� ������
*/
void encoder::_encode_tree(const p_t &root)
{
	// ������������ ����������� � �������� ������

	// ������������ ��������� ������� ���������� � �������� ������

	// ����������� �������� �������������
	for (wtree::coefs_iterator i = _wtree.iterator_over_LL_children(root);
		 !i->end(); i->next())
	{
		// ������������ ������������ � ������� ������
	}

	// ����������� ��������� ��������� ���������� � ������� ������
	for (wtree::coefs_iterator i = _wtree.iterator_over_LL_children(root);
		 !i->end(); i->next())
	{
		// ������������ ��������� �������� ���������� � ������� ������
		// ���� ����� �� ���������
	}

	for (sz_t lvl = subbands::LVL_1; _wtree.lvls() > lvl; ++lvl)
	{
		for (sz_t k = 0; _wtree.sb().subbands_on_lvl(lvl) > k; ++k)
		{
			for (wtree::coefs_iterator i =
						_wtree.iterator_over_leafs(root, lvl, k);
				 !i->end(); i->next())
			{
				// �������� ��������� ������� ���������� ��� ������������
			}
		}
	}
}


/*!	\param[in] root ���������� ��������� �������� ���������������� ������
	\param[in] lambda �������� <i>lambda</i> ������������ ���
	���������� <i>RD</i> �������� (������� ��������). ������������
	����� ������ ����� ������� � �������� ���������.

	�� ������ ���� ����������� ����������� ������������� �������������
	�� ����� ��������� (� ���������� ��������) ������ ���������� �
	����������� ������ <i>RD-������� ��������</i> ��� ��������� ����������
	� ���������� ��������� ������� ������.
*/
void encoder::_optimize_tree_step_1(const p_t &root, const lambda_t &lambda)
{
	// ��������������� ��� ���� �������������� ������
	const sz_t lvl_i = _wtree.sb().lvls() + subbands::LVL_PREV;
	const sz_t lvl_j = _wtree.sb().lvls();

	// ���� �� ���������
	for (sz_t k = 0; subbands::SUBBANDS_ON_LEVEL > k; ++k)
	{
		const subbands::subband_t &sb_i = _wtree.sb().get(lvl_i, k);
		const subbands::subband_t &sb_j = _wtree.sb().get(lvl_j, k);

		// ���� �� ��������� �� �������������� ������ ������
		for (wtree::coefs_iterator i = _wtree.iterator_over_leafs(root, sb_i);
			!i->end(); i->next())
		{
			// ������������ ������� �� �������������� ������
			const p_t &p = i->get();

			// ��� 1.1. ������������� ��������������� �������������
			_coefs_fix(p, sb_j, lambda);

			// ��� 1.2. ������ RD-������� �������� ��� ��������� ���������� �
			// ���������� �������
			_prepare_j(p, sb_j, lambda);
		}
	}
}


/*!	\param[in] root ���������� ��������� �������� ������
	\param[in] lambda �������� <i>lambda</i> ������������ ���
	���������� <i>RD</i> �������� (������� ��������). ������������
	����� ������ ����� ������� � �������� ���������.
*/
void encoder::_optimize_tree_step_2(const p_t &root, const lambda_t &lambda)
{
	// ���� 2.1 - 2.5 ----------------------------------------------------------

	// ���� �� �������
	for (sz_t lvl_i = _wtree.sb().lvls() + 2*subbands::LVL_PREV;
		 0 < lvl_i; --lvl_i)
	{
		const sz_t lvl_j = lvl_i + subbands::LVL_NEXT;

		// ���� �� ���������
		for (sz_t k = 0; subbands::SUBBANDS_ON_LEVEL > k; ++k) {

			// ��������� ������ �� ��������
			const subbands::subband_t &sb_i = _wtree.sb().get(lvl_i, k);
			const subbands::subband_t &sb_j = _wtree.sb().get(lvl_j, k);

			// ���� �� ������������ ���������
			for (wtree::coefs_iterator i =
						_wtree.iterator_over_leafs(root, sb_i);
				 !i->end(); i->next())
			{
				const p_t &p = i->get();

				// ��� 2.1. ����������� ����������� ��������� ������
				const _branch_topology_t optim_topology =
						_optimize_branch_topology(p, sb_i, lambda);

				// ��� 2.2. ��������� ��������� ������
				_wtree.cut_leafs(p, optim_topology.n);

				// ��� 2.3. ������������� ��������������� �������������
				_coefs_fix(p, sb_j, lambda);

				// ��� 2.4. ���������� ��� ��������� ���������� ������
				_prepare_j(p, sb_j, optim_topology.j, lambda);
			}
		}
		// �� ������ ������, ���� �����-���� ���� ������� sz_t �����������
		// ����� :^)
		// if (0 == lvl) break;
	}
}


/*!	\param[in] root ���������� ��������� �������� ������
	\param[in] lambda �������� <i>lambda</i> ������������ ���
	���������� <i>RD</i> �������� (������� ��������). ������������
	����� ������ ����� ������� � �������� ���������.
*/
void encoder::_optimize_tree_step_3(const p_t &root, const lambda_t &lambda)
{
	const subbands::subband_t &sb_LL = _wtree.sb().get_LL();

	// ��� 2.6. ����������� ����������� ��������� ������
	const _branch_topology_t optim_topology =
			_optimize_branch_topology(root, sb_LL, lambda);

	// ��� 2.7. ��������� ��������� ������
	_wtree.cut_leafs(root, optim_topology.n);

	// ��� 3. ���������� RD-������� �������� ��� ����� ������
	_calc_jx_value(root, optim_topology.j, lambda);
}


/*!	\param[in] root ���������� ��������� �������� ������
	\param[in] lambda �������� <i>lambda</i> ������������ ���
	���������� <i>RD</i> �������� (������� ��������). ������������
	����� ������ ����� ������� � �������� ���������.
	\return �������� <i>RD-������� ��������</i> ��� ������
*/
j_t encoder::_optimize_tree(const p_t &root, const lambda_t &lambda)
{
	_optimize_tree_step_1(root, lambda);
	_optimize_tree_step_2(root, lambda);
	_optimize_tree_step_3(root, lambda);

	return (_wtree.at(root).j1);
}



}	// end of namespace wic
