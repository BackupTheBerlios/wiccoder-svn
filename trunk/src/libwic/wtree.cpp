/*******************************************************************************
* file:         wtree.cpp                                                      *
* version:      0.1.0                                                          *
* author:       mice (mailto:my_mice@mail.ru, ICQ:332-292-380)                 *
* description:  not available                                                  *
* tests:        none                                                           *
*******************************************************************************/


////////////////////////////////////////////////////////////////////////////////
// include

// standard C++ headers
#include <memory.h>

// libwic headers
#include <wic/libwic/wtree.h>


////////////////////////////////////////////////////////////////////////////////
// wic namespace
namespace wic {


////////////////////////////////////////////////////////////////////////////////
// wtree class public definitions

/*!	\param[in] width ������ �����������
	\param[in] height ������ �����������
	\param[in] lvls ���������� ������� ����������
*/
wtree::wtree(const sz_t width, const sz_t height, const sz_t lvls):
	_nodes(0), _subbands(0), _q(0)
{
	// ���������� ���������� �������
	_width	= width;
	_height	= height;
	_lvls	= lvls;

	// ��������� ������ ��� ����� ������� ������������� (������)
	_nodes = new wnode[coefs()];
	if (0 == _nodes) throw std::bad_alloc();

	// ����� ���� �������� � ����
	_reset_trees_content();

	// �������� ���������� � ���������
	_subbands = new subbands(_width, _height, _lvls);
	if (0 == _nodes) throw std::bad_alloc();
}


/*!	����������� ���������� ������
*/
wtree::~wtree() {
	// ������������ ���������� � ���������
	if (0 != _subbands) delete _subbands;

	// ������������ ������ ��� ����� ������� ������������� (������)
	if (0 != _nodes) delete[] _nodes;
}


/*!	\return ���������� ����, ������� ���� ��������� ��� �������� ����
	���������� � �������� (�� ������ ���������� � ���������)
*/
sz_t wtree::nodes_sz() const {
	return (coefs() * sizeof(wnode));
}


/*!	\param[in] from ������� ������, �������� ������������� ��������,
	����� �����������

	������� ����� ������������� ��������� ����������� � <i>q = 1</i>
	(�������� wtree::quantize()).
*/
void wtree::load(const w_t *const from) {
	assert(0 != from);

	// _reset_trees_content();

	for (sz_t i = 0; coefs() > i; ++i) {
		_nodes[i].w = from[i];
	}

	quantize();
}


/*!	\param[in] q ������������

	������� ���������� ����������� �������������. �������� ������������
	������� �� wnode::w, ��������� ����������� ���������� � wnode::wq.

	����� ����������� ������� �������� wtree::refresh() ��� ������
	��������� ����� ��������� ������.
*/
void wtree::quantize(const q_t q) {
	for (sz_t i = 0; coefs() > i; ++i) {
		wnode &node = _nodes[i];
		node.wq = wnode::quantize(node.w, q);
	}

	_q = q;

	refresh();
}


/*! ������� ���������� ��������� ��������:
	- ��������� ������������ wnode::wc = wnode::wq � ��� ����� ���
	  ��������� �������� ��� ������������������� ������������
	- �������� �������� ������� �������� (wnode::j0 � wnode::j1)
	- �������� ��������� ������� ���������� wnode::n, ����� ��� ����� ������
	  �� ������������
	- �������� ������� ���������� ������������ (wnode::invalid), �����
	  ��� �������� ���������
*/
void wtree::refresh()
{
	// ��� ������� �������� �� ������
	for (sz_t i = 0; coefs() > i; ++i) {
		// ������ �� ��������� �������
		wnode &node = _nodes[i];

		// ��������������� �������� ������������������� ������������
		node.wc			= node.wq;
		// �������� ������� �������� �� ��������
		node.j0			= 0;
		node.j1			= 0;
		// ������ �� ���������
		node.n			= 0;
		// ���� �������
		node.invalid	= false;
	}
}


/*!	\return ����������� ������ �� ������ wiv::subbands
	\sa subbands
*/
subbands &wtree::sb() {
	assert(0 != _subbands);

	return (*_subbands);
}


/*!	\return ����������� ������ �� ������ wiv::subbands
	\sa subbands
*/
const subbands &wtree::sb() const {
	assert(0 != _subbands);

	return (*_subbands);
}


/*!	\param[in] x X ����������
	\param[in] y Y ����������
	\return ����������� ������ �� �������� �������� �������
*/
const wnode &wtree::at(const sz_t x, const sz_t y) const {
	assert(0 <= x && x < _width && 0 <= y && y < _height);
	return _nodes[x + y*_width];
}


/*!	\param[in] x X ����������
	\param[in] y Y ����������
	\return ������ �� �������� �������� �������
*/
wnode &wtree::at(const sz_t x, const sz_t y) {
	assert(0 <= x && x < _width && 0 <= y && y < _height);
	return _nodes[x + y*_width];
}


/*!	\param[in] node ������ �� ����
*/
p_t wtree::get_pos(const wnode &node) const {
	assert(_nodes <= &node);

	const sz_t offset = sz_t(&node - _nodes);

	assert(offset < coefs());

	return p_t(offset % _width, offset / _height);
}


/*!	\param[in] c ���������� ��������
*/
p_t wtree::prnt(const p_t &c) {
	assert(0 <= c.x && c.x < _width);
	assert(0 <= c.y && c.y < _height);

	return p_t(c.x / 2, c.y / 2);
}


/*!	\param[in] lvl ������� ������� ����������, �� ������� ���������
	������� � ��������������� ��������� ��������� ����������
	\return �������� �� ������������ ��������� ��� ������� ������ ����������
	�������� ���������� ������
*/
wtree::n_iterator wtree::iterator_through_n(const sz_t lvl) {
	return new n_cutdown_iterator(lvl);
}


/*!	\param[in] p ���������� ��������, ������� ����� �������� �������������
	� ��������� �������� ���������� ������.
	\param[in] branch ���������� ������������� ��������, ������� ������ �����.
	\return ��������� ������� ���������� ������, ����������������� ���, ���
	������ ���� ����� (������� ����������� ������� � ������������ <i>p</i>)
	�������� �������������.

	\todo ���������� �������������� ��� �������
*/
n_t wtree::child_n_mask(const p_t &p, const p_t &branch)
{
	const p_t ctl = _children_top_left(branch);

	const sz_t shift = (p.x - ctl.x) + 2*(p.y - ctl.y);

	return n_t(1 << shift);
}


/*!	\param[in] p ���������� ��������, ������� ����� �������� �������������
	� ��������� �������� ���������� ������.
	\return ��������� ������� ���������� ������, ����������������� ���, ���
	������ ���� ����� (������� ����������� ������� � ������������ <i>p</i>)
	�������� �������������.

	\note ������ ������� �������� ������ � ����������, ����������� �������
	��������� ��������� �� <i>LL</i> �������� (������� ����� � <i>1��</i>
	������ ����������).

	\todo ���������� �������������� ��� �������
*/
n_t wtree::child_n_mask_LL(const p_t &p) {
	const subbands::subband_t &sb_LL = sb().get_LL();

	if (p.y <= sb_LL.y_max) return (1 << 0);
	if (p.x <= sb_LL.x_max) return (1 << 1);

	return (1 << 2);
}


/*!	\param[in] branch ���������� ��������, ����������� �����
	\param[in] n ��������� ������� ���������� ������

	������� ��������� ���������� ������, ����������� ������� ��������
	�������� �������� �� <i>branch</i>, � ������������ � ��������� ���������
	���������� ������.
*/
void wtree::cut_leafs(const p_t &branch, const n_t n)
{
	const bool is_LL = sb().test_LL(branch);

	for (coefs_iterator i = iterator_over_children_uni(branch);
		 !i->end(); i->next())
	{
		const p_t &p = i->get();

		const n_t mask = (is_LL)? child_n_mask_LL(p)
								: child_n_mask(p, branch);

		if (!test_n_mask(n, mask)) _cut_branch(p);
	}

	at(branch).n = n;
}


/*!	\param[in] sb ������� �� �������� ����� ����������� ����� � �������
	������������� ���������
	\return �������� �� ��������
*/
wtree::coefs_iterator wtree::iterator_over_subband(
	const subbands::subband_t &sb)
{
	return new snake_square_iterator(p_t(sb.x_min, sb.y_min),
									 p_t(sb.x_max, sb.y_max));
}


/*!	\param[in] root ���������� ��������� ��������
	\param[in] sb �������, ������ �� �������� ����� ��������������� �� �����
	�������� ��������.
	\return �������� �� �������

	\todo �������� ������� ��������� �������� � ������� ��������� �����������
	\todo ��� ������� ���������� ��������������
*/
wtree::coefs_iterator wtree::iterator_over_leafs(const p_t &root,
												 const subbands::subband_t &sb)
{
	const p_t c = _leafs_top_left(root, sb.lvl, sb.i);

	return new snake_square_iterator(p_t(c.x, c.y),
									 p_t(c.x + sb.tree_w, c.y + sb.tree_h));
}


/*!	\param[in] root ���������� ��������� ��������
	\param[in] lvl �������, ������ � �������� ����� ��������������� �� �����
	�������� ��������
	\param[in] i ������ �������� ������ ������, ������ �� �������� ����
	���������������
	\return �������� �� �������
*/
wtree::coefs_iterator wtree::iterator_over_leafs(const p_t &root,
												 const sz_t lvl, const sz_t i)
{
	return iterator_over_leafs(root, sb().get(lvl, i));
}


////////////////////////////////////////////////////////////////////////////////
// wtree class protected definitions

/*!	������� ������, ���������� ������� �������������. ��� ��������
	������������ � ��������������� � 0.
*/
void wtree::_reset_trees_content() {
	memset(_nodes, 0, nodes_sz());
}


/*!	\param[in] prnt ���������� ������������� ��������
	\return ���������� �������� ������ ��������� �������� �� ������
	�������� ���������

	\warning ������� �� �������� ��� ������������� � ������������
	������������ ��������� �� <i>LL</i> ��������, ��� ��� � ���� ������
	�������� �������� ����������� � ������ ���������.
*/
p_t wtree::_children_top_left(const p_t &prnt) {
	return p_t(2 * prnt.x, 2 * prnt.y);
}


/*!	\param[in] root ���������� ��������� �������� ������
	\param[in] lvl ������� �� ������� ��������� ��������� ���� �������
	\param[in] i ������ �������� �� ������ (���������� subbands::_get())
	\return ���������� �������� ������ �������� ����� ������� �������
	������.

	\todo �������� ����� ��������� ��������
*/
p_t wtree::_leafs_top_left(const p_t &root, const sz_t lvl, const sz_t i)
{
	// ����� ����� ���������� � LL ��������
	const subbands::subband_t &sb_LL = sb().get_LL();

	// �������������, ��� �������� ������� �� LL ��������
	assert(sb_LL.x_min <= root.x && root.x <= sb_LL.x_max);
	assert(sb_LL.y_min <= root.y && root.y <= sb_LL.y_max);

	// ���� ������� �������, �� ���������� �� ������� �������� ����������
	// �������� �������
	if (0 == lvl) return root;

	// ������� ���������� �������� �� �������� ������� ������ � ������
	// ��������
	const subbands::subband_t &sb_1 =  sb().get(subbands::LVL_1, i);

	const p_t leaf_1(sb_1.x_min + root.x, sb_1.y_min + root.y);

	// ���������� ���������, �� ������� ���������� ��������� ����������
	// ��� �������� �� ��������� ������� (��� ����� 2^(lvl - 1))
	const sz_t factor = 1 << (lvl - 1);

	return p_t(factor * leaf_1.x, factor * leaf_1.y);
}


/*!	\param[in] x x ���������� �������� (�� ������������ � ������� ����������)
	\param[in] y y ���������� ��������
	\return \c true, ���� �������������� ����������� ������ - �����,
	����� - \c false.

	������� ������������, ��� ������������ ����� "������", ��� �������
	��������� ��������� �������:
	- ������ ������ �������������� �� ������ �������
	- ������ ����� �������������� �� �������� �������

	������������ �
	calc_sj(const sz_t x, const sz_t y, const subbands::subband_t &sb)
	��� ��������������� ������������� ����������� ������.
*/
bool wtree::_going_left(const sz_t x, const sz_t y) {
	// this is to prevent gcc warning, that variable x not used
	(void)x;

	return (0 != y % 2);
}


/*!	\param[in] prnt ���������� ������������� ��������
	\return ��������� �� �������� �� �������� ���������

	\note ������ ������� �� ��������� ��� ������������ ��������� ��
	<i>LL</i> ��������.

	\attention ���������� ������������ �������� <i>delete</i> ���
	������������ ������, ���������� ������������ ����������.
*/
basic_iterator<p_t> *wtree::_iterator_over_children(const p_t &prnt)
{
	// ���������� �������� ������ ��������� ��������
	const p_t c = _children_top_left(prnt);

	// �������� ���������
	return new snake_square_iterator(p_t(c.x    , c.y    ),
									 p_t(c.x + 1, c.y + 1));
}


/*!	\param[in] prnt ���������� ������������� ��������
	\return ��������� �� �������� �� �������� ���������

	\note ������ ������� �� ��������� ��� ������������ ��������� �� ��
	<i>LL</i> ��������.

	\attention ���������� ������������ �������� <i>delete</i> ���
	������������ ������, ���������� ������������ ����������.
*/
basic_iterator<p_t> *wtree::_iterator_over_LL_children(const p_t &prnt)
{
	const subbands::subband_t &sb_LL = sb().get_LL();

	assert(sb().test(prnt, sb_LL));

	return new LL_children_iterator(sb_LL.width, sb_LL.height, prnt);
}


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
	������� ������� ��������� ��������� ���������� ������ �� 2��
	��������� ��������� �������� ����������� �������� ���������� ������
	�������.

	\note ������� ��������� ������������ �������� �� ����� ���������,
	������� �������� �� <i>LL</i> ��������.

	\attention ������� �������� ����������� ������, ��� ��� ��
	������������ ��������� ������� ���������� � ������������� ��������
	�����
*/
void wtree::_cut_branch(const p_t &branch) {
	assert(0 <= branch.x && branch.x < _width);
	assert(0 <= branch.y && branch.y < _height);

	const n_t &branch_n = at(branch).n;

	for (coefs_iterator i = iterator_over_children_uni(branch);
		 !i->end(); i->next())
	{
		const p_t &p = i->get();
		wnode &node = at(p);

		// ���������� ������� ���������� ��������� �������� �����
		// ��� ��� ��������� ������� ������� ��������� ������� ����������
		// ���� ����� �������� p
		if (0 != branch_n) _cut_branch(p);

		node.invalid = true;
		node.n = 0;
	}
}



}	// end of namespace wic
