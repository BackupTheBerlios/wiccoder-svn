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
	_wtree(width, height, lvls),
	_acoder(width * height * sizeof(w_t) * 4)
{
	// �������� �����������
	assert(MINIMUM_LEVELS <= lvls);

	// �������� ������������� ���������� � ������
	_wtree.load(image);
}


/*!
*/
encoder::~encoder() {
}


/*!
*/
void encoder::encode(const lambda_t &lambda)
{
	_acoder.use(_mk_acoder_models<wnode::member_wq>());

	_acoder.encode_start();

	const subbands &sb = _wtree.sb();

	for (wtree::coefs_iterator i = _wtree.iterator_over_subband(sb.get_LL());
		 !i->end(); i->next())
	{
		const p_t &root = i->get();
		_optimize_tree(root, lambda);
		_encode_tree(root);
	}

	_acoder.encode_stop();

	_acoder.encode_start();

	_encode_wtree();

	_acoder.encode_stop();
}

void encoder::decode()
{
	_wtree.reset();

	_acoder.decode_start();

	_encode_wtree(true);

	_acoder.decode_stop();
}



////////////////////////////////////////////////////////////////////////////////
// wtree class protected definitions

/*!	\param[in] s �������� ���������� �������� <i>S<sub>j</sub></i>
	\param[in] lvl ����� ������ ����������, �� �������� ��� ���� �����������
	\return ����� ���������� ������

	\note ����� ��������, ��� ��� �������� � ������� ������� �������
	���������� ����������� ��������, ���������� �� ��������� <i>s</i>.
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

	\note ����� ��������, ��� ���� �������� <i>lvl</i> ����� <i>0</i>
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
*/
h_t encoder::_h_spec(const sz_t m, const wk_t &wk) {
	return _acoder.enc_entropy(wk, m);
}


/*!	\param[in] m ����� ������ ��� �����������
	\param[in] n �������� ���������� �������� ���������� ������
	\return ������� �������, ����������� ��� ����������� ����������
	�������� ����������
*/
h_t encoder::_h_map(const sz_t m, const n_t &n) {
	return _acoder.enc_entropy(n, m + ACODER_SPEC_MODELS_COUNT);
}


/*!	\param[in] m ����� ������ ��� �����������
	\param[in] wk �������� ������������ ��� �����������
*/
void encoder::_encode_spec(const sz_t m, const wk_t &wk) {
	_acoder.put(wk, m);
}


/*!	\param[in] m ����� ������ ��� �����������
	\param[in] n �������� ���������� �������� ���������� ������
*/
void encoder::_encode_map(const sz_t m, const n_t &n) {
	_acoder.put(n, m + ACODER_SPEC_MODELS_COUNT);
}


/*!	\param[in] m ����� ������ ��� �����������
	\return �������� ������������ ��� �����������
*/
wk_t encoder::_decode_spec(const sz_t m)
{
	return _acoder.get<wk_t>(m);
}


/*!	\param[in] m ����� ������ ��� �����������
	\return �������� ���������� �������� ���������� ������
*/
n_t encoder::_decode_map(const sz_t m)
{
	return _acoder.get<n_t>(m + ACODER_SPEC_MODELS_COUNT);
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

	const double h = _h_spec(model, k);

	return (dw*dw + lambda * h);
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
	return _wtree.at(p).wq;

	// ����� ������ � ������������� �������� ������������
	const sz_t	model	= _ind_spec<wnode::member_wc>(p, sb);
	const wk_t	&wq		= _wtree.at(p).wq;

	// ����������� ������ ����������� ��������
	#ifdef COEF_FIX_USE_4_POINTS
		static const sz_t vals_count	= 4;
		const wk_t w_vals[vals_count] = {0, wq, wq + 1, wq - 1};
	#else
		static const sz_t vals_count	= 3;
		const wk_t w_drift = (0 <= wq)? -1: +1;
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
	// ��������� ��������� ��������
	const sz_t lvl_j = sb.lvl + subbands::LVL_NEXT;
	const subbands::subband_t &sb_j = _wtree.sb().get(lvl_j, sb.i);

	// ����� ������ ��� ����������� ��������� ��������� ����������
	const sz_t model = _ind_map<wnode::member_wc>(branch, sb_j);

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


/*!	\param[in] root ���������� ��������� ��������

	������� ��������� �����������:
	- ������������ ��� �������� ��������
	- ���������� �������� ���������� ��� �������� ��������
	- ������������� ������������� ������ � ������� ������ ����������
	- ������������� ������������� ������ �� ������� ������ ����������
	  ���� �� �� ������ � ����������� �����
*/
void encoder::_encode_tree_root(const p_t &root)
{
	// ��������� ��������� ��������
	const wnode &root_node = _wtree.at(root);

	// ������������ ����������� � �������� ������
	_encode_spec(_ind_spec(0, subbands::LVL_0), root_node.wc);

	// ������������ ��������� ������� ���������� � �������� ������
	_encode_map(_ind_map(0, subbands::LVL_0), root_node.n);

	// ����������� �������� ������������� � ������� ������
	for (wtree::coefs_iterator i = _wtree.iterator_over_LL_children(root);
		 !i->end(); i->next())
	{
		// ������������ ������������ � ������� ������
		_encode_spec(_ind_spec(0, subbands::LVL_1), _wtree.at(i->get()).wc);
	}

	/*
	// ����������� ������������� �� ������� ������
	static const sz_t LVL_2 = subbands::LVL_1 + subbands::LVL_NEXT;

	for (wtree::coefs_iterator i = _wtree.iterator_over_LL_children(root);
		 !i->end(); i->next())
	{
		// ���������� �������� ������������� �������� � ������� ������
		const p_t &p = i->get();

		// ����� ����������, ��� ������� ������� �� ��������
		const n_t mask = _wtree.child_n_mask_LL(p);

		// ��������� � ���������� �������, ���� ����� ���������
		if (!_wtree.test_n_mask(root_node.n, mask)) continue;

		// ������� � ������� ����� ������� ������������ �������
		const subbands::subband_t &sb_i =
						_wtree.sb().from_point(p, subbands::LVL_1);

		// ������� � ������� ����� �������� �������� �� ������� ������
		// (�������� �� sb_i)
		const subbands::subband_t &sb_j =
						_wtree.sb().get(LVL_2, sb_i.i);

		// ����� �� ���������, �������� �������� ������������
		for (wtree::coefs_iterator i = _wtree.iterator_over_children(p);
			 !i->end(); i->next())
		{
			// ���������� �������� ��������� �������� �� ������� ������
			const p_t &c = i->get();

			// ����������� ������������
			_encode_spec(_ind_spec<wnode::member_wc>(c, sb_j),
						 _wtree.at(c).wc);
		}
	}
	*/
}


/*!	\param[in] root ���������� �������� ��������
	\param[in] lvl ����� ������ ����������
*/
void encoder::_encode_tree_leafs(const p_t &root, const sz_t lvl)
{
	// ���������� ��� ������� �������
	const sz_t lvl_g = lvl;
	const sz_t lvl_j = lvl_g + subbands::LVL_PREV;
	const sz_t lvl_i = lvl_j + subbands::LVL_PREV;

	// ���� �� ��������� � ������
	for (sz_t k = 0; _wtree.sb().subbands_on_lvl(lvl) > k; ++k)
	{
		const subbands::subband_t &sb_g = _wtree.sb().get(lvl_g, k);
		const subbands::subband_t &sb_j = _wtree.sb().get(lvl_j, k);

		// ����������� �������������
		for (wtree::coefs_iterator g = _wtree.iterator_over_leafs(root, sb_g);
			 !g->end(); g->next())
		{
			const p_t &p_g = g->get();

			wnode &node_g = _wtree.at(p_g);

			if (node_g.invalid) continue;

			const sz_t model = _ind_spec<wnode::member_wc>(p_g, sb_g);

			_encode_spec(model, node_g.wc);
		}

		// �� ������������� ������ ��� ��������� ��������� ����������
		if (_wtree.lvls() == lvl) continue;

		// ����������� ��������� ��������� ����������
		for (wtree::coefs_iterator j = _wtree.iterator_over_leafs(root, sb_j);
			 !j->end(); j->next())
		{
			const p_t &p_j = j->get();

			const p_t &p_i = _wtree.prnt_uni(p_j);
			const wnode &node_i = _wtree.at(p_i);

			// ����� ����������, ��� ������� ������� �� ��������
			const n_t mask = _wtree.child_n_mask_uni(p_j, p_i);

			// ��������� � ���������� �������, ���� ����� ���������
			if (!_wtree.test_n_mask(node_i.n, mask)) continue;

			wnode &node_j = _wtree.at(p_j);

			const sz_t model = _ind_map<wnode::member_wc>(p_j, sb_g);

			_encode_map(model, node_j.n);
		}
	}
}


/*!	\param[in] root ���������� ��������� �������� ������
*/
void encoder::_encode_tree(const p_t &root)
{
	// ����������� ��������� � �������� ��������� ������
	_encode_tree_root(root);

	// ����������� ��������� ������ �� ��������� �������
	const sz_t first_lvl = subbands::LVL_1 + subbands::LVL_NEXT;
	const sz_t final_lvl = _wtree.lvls();

	for (sz_t lvl = first_lvl; final_lvl >= lvl; ++lvl)
	{
		_encode_tree_leafs(root, lvl);
	}
}


/*!	\param[in] decode_mode ���� <i>false</i> ������� ����� ���������
	����������� �������, ����� (���� <i>true</i>) ����� ���������
	������������� �������.
*/
void encoder::_encode_wtree_root(const bool decode_mode)
{
	// LL c������
	const subbands::subband_t &sb_LL = _wtree.sb().get_LL();

	// ������ ��� ����������� � LL ��������
	const sz_t spec_LL_model = _ind_spec(0, sb_LL.lvl);
	const sz_t map_LL_model = _ind_map(0, sb_LL.lvl);

	// (��)����������� ������������� � ��������� ��������� ����������
	// �� LL ��������
	for (wtree::coefs_iterator i = _wtree.iterator_over_subband(sb_LL);
		 !i->end(); i->next())
	{
		// ���������� ��������
		const p_t &p_i = i->get();

		// ��� ������� �� LL ��������
		wnode &node = _wtree.at(p_i);

		if (decode_mode)
		{
			// ������������� ������������ � �������� ����������
			node.wc = _decode_spec(spec_LL_model);
			node.n = _decode_map(map_LL_model);

			// ���������� ������ � ������������ � ���������� ���������
			// ����������
			_wtree.uncut_leafs(p_i, node.n);
		}
		else
		{
			// ����������� ������������ � �������� ����������
			_encode_spec(spec_LL_model, node.wc);
			_encode_map(map_LL_model, node.n);
		}
	}

	// ������ ��� ����������� ������������� � ������� ������
	const sz_t spec_1_model = _ind_spec(0, subbands::LVL_1);

	// (��)����������� ������������� �� ��������� ������� ������
	for (sz_t k = 0; subbands::SUBBANDS_ON_LEVEL > k; ++k)
	{
		// ��������� ������� � ������� ������
		const subbands::subband_t &sb = _wtree.sb().get(subbands::LVL_1, k);

		// ���� �� ���� ��������� �� �������� � ������� ������
		for (wtree::coefs_iterator i = _wtree.iterator_over_subband(sb);
			 !i->end(); i->next())
		{
			// (��)����������� ����������� ������������
			if (decode_mode)
				_wtree.at(i->get()).wc = _decode_spec(spec_1_model);
			else
				_encode_spec(spec_1_model, _wtree.at(i->get()).wc);
		}
	}
}


/*!	\param[in] lvl ����� ������, ������������ �� ������� �����
	������������
	\param[in] decode_mode ���� <i>false</i> ������� ����� ���������
	����������� �������, ����� (���� <i>true</i>) ����� ���������
	������������� �������.

	\todo �������� �� ����� ������ wtree::uncut_leafs() ��� ��������� �
	����������� ����� ��� ����������� ���������� ��������� ����������.
	������ ����� ����� ����� �������������� ������������������� ��� ����
	wnode::invalid ��������� <i>true</i>
*/
void encoder::_encode_wtree_level(const sz_t lvl,
								  const bool decode_mode)
{
	// ����������� ����������� ��� ������� �������
	const sz_t lvl_g = lvl;
	const sz_t lvl_j = lvl_g + subbands::LVL_PREV;
	const sz_t lvl_i = lvl_j + subbands::LVL_PREV;

	// ���� �� ��������� �� ������
	for (sz_t k = 0; _wtree.sb().subbands_on_lvl(lvl) > k; ++k)
	{
		// ������� �� ������ ������������ �� �������� �����
		// ������������ � ������������ ��� ��� ����
		const subbands::subband_t &sb_g = _wtree.sb().get(lvl_g, k);
		const subbands::subband_t &sb_j = _wtree.sb().get(lvl_j, k);

		// ����������� �������������
		for (wtree::coefs_iterator g = _wtree.iterator_over_subband(sb_g);
			 !g->end(); g->next())
		{
			// ���������� ��������
			const p_t &p_g = g->get();

			// ��� �������
			wnode &node_g = _wtree.at(p_g);

			// ��������� � ����������, ���� ������������ ����� �
			// ����������� �����
			if (node_g.invalid) continue;

			// �������� ������ ��� (��)����������� ������������
			const sz_t model = _ind_spec<wnode::member_wc>(p_g, sb_g);

			// (��)����������� ������������
			if (decode_mode)
				node_g.wc = _decode_spec(model);
			else
				_encode_spec(model, node_g.wc);
		}

		// �� ������������� ������ ��� ��������� ��������� ����������
		if (_wtree.lvls() == lvl) continue;

		// ����������� ��������� ��������� ����������
		for (wtree::coefs_iterator j = _wtree.iterator_over_subband(sb_j);
			 !j->end(); j->next())
		{
			// ���������� ��������
			const p_t &p_j = j->get();

			// ���������� ������������� ��������
			const p_t &p_i = _wtree.prnt_uni(p_j);

			// ������ �� ������������ �������
			const wnode &node_i = _wtree.at(p_i);

			// ����� ����������, ��� ������� ������� �� ��������
			const n_t mask = _wtree.child_n_mask_uni(p_j, p_i);

			// ��������� � ���������� �������, ���� ����� ���������
			if (!_wtree.test_n_mask(node_i.n, mask))
			{
				// ����������� �������� ���� wnode::invalid � <i>true</i>
				// ��� ��� ����� ���������
				if (decode_mode)
					_wtree.uncut_leafs(p_j, _wtree.get_clear_n());

				continue;
			}

			// �������� ��������
			wnode &node_j = _wtree.at(p_j);

			// ����� ������ ��� ����������� ���������� �������� ����������
			const sz_t model = _ind_map<wnode::member_wc>(p_j, sb_g);

			if (decode_mode)
			{
				// ������������� �������� ����������
				node_j.n = _decode_map(model);

				// ���������� ������ � ������������ � ���������� ���������
				// ����������
				_wtree.uncut_leafs(p_j, node_j.n);
			}
			else
			{
				// ����������� �������� ����������
				_encode_map(model, node_j.n);
			}
		}
	}
}


/*!	\param[in] decode_mode ���� <i>false</i> ������� ����� ���������
	����������� �������, ����� (���� <i>true</i>) ����� ���������
	������������� �������.
*/
void encoder::_encode_wtree(const bool decode_mode)
{
	// (��)����������� �������� ���������
	_encode_wtree_root(decode_mode);

	// (��)����������� ��������� ���������
	const sz_t first_lvl = subbands::LVL_1 + subbands::LVL_NEXT;
	const sz_t final_lvl = _wtree.lvls();

	// ���� �� �������
	for (sz_t lvl = first_lvl; final_lvl >= lvl; ++lvl)
	{
		_encode_wtree_level(lvl, decode_mode);
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
				_wtree.cut_leafs<wnode::member_wc>(p, optim_topology.n);

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
	_wtree.cut_leafs<wnode::member_wc>(root, optim_topology.n);

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
