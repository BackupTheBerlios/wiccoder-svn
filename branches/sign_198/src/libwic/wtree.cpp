/*!	\file     wtree.cpp
	\version  0.0.1
	\author   mice, ICQ: 332-292-380, mailto:wonder.mice@gmail.com
	\brief    ���������� ������ wic::wtree - ������� ������� ����������

	\todo     ����� �������� ������� ���� wtree.cpp
*/


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

/*!	����� ������ ���, ��� ������� ������.
*/
const q_t wtree::DEFAULT_Q		= q_t(1);


/*!	\param[in] width ������ ������� ������� (� ���������)
	\param[in] height ������ ����������� (� ���������)
	\param[in] lvls ���������� ������� ����������
*/
wtree::wtree(const sz_t width, const sz_t height, const sz_t lvls):
	_width(width), _height(height), _lvls(lvls),
	_nodes_count(width * height),
	_q(0),
	_nodes(0), _subbands(0)
{
	// �������� �����������
	assert(0 < _width);
	assert(0 < _height);
	assert(0 < _lvls);
	assert(_nodes_count == (_width * _height));
	#ifdef LIBWIC_DEBUG
	{
		const sz_t factor = (1 << _lvls);
		assert(0 == (_width  % factor));
		assert(0 == (_height % factor));
	}
	#endif

	// ��������� ������ � �������� ��������
	try
	{
		// �������� ���������� � ���������
		_subbands = new subbands(_width, _height, _lvls);

		if (0 == _subbands) throw std::bad_alloc();

		// ��������� ������ ��� ����� ������� ������������� (������)
		_nodes = new wnode[nodes_count()];

		if (0 == _nodes) throw std::bad_alloc();
	}
	catch (const std::exception &e)
	{
		// ������������ ���������� � ���������
		if (0 != _subbands) delete _subbands;

		// ������������ ������ ��� ����� ������� ������������� (������)
		if (0 != _nodes) delete[] _nodes;

		throw e;
	}

	#ifdef LIBWIC_DEBUG
	// ����� ���� ��������
	memset(_nodes, 0, nodes_size());
	#endif
}


/*!	����������� ���������� ������
*/
wtree::~wtree() {
	// ������������ ���������� � ���������
	if (0 != _subbands) delete _subbands;

	// ������������ ������ ��� ����� ������� ������������� (������)
	if (0 != _nodes) delete[] _nodes;
}


/*!	\param[in] q ������������

	������� ���������� ����������� �������������. �������� ������������
	������� �� ���� wnode::w, ��������� ����������� ���������� �
	���� wnode::wq � wnode::wc.

	\sa _filling_quantize
*/
void wtree::quantize(const q_t q)
{
	_filling_quantize(q);
}


/*!	��� �������� ����� ���� ������� ��� ���������� �������
	� �����������.

	\sa _filling_refresh
*/
void wtree::filling_refresh()
{
	_filling_refresh();
}


/*!	��� �������� ����� ���� ������� ��� ���������� �������
	� �������������.
*/
void wtree::wipeout()
{
	for (sz_t i = 0; nodes_count() > i; ++i)
	{
		wnode &node		= _nodes[i];
		node.w			= 0;
		node.wq			= node.wc = 0;
		node.n			= 0;
		node.invalid	= true;
	}
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


/*!	\param[in] node ������ �� �������
	\return ���������� �������� � �������

	\note ������� ������ ���� ����������� �� ����� �������
*/
p_t wtree::get_pos(const wnode &node) const
{
	assert(_nodes <= &node);

	const sz_t offset = sz_t(&node - _nodes);

	assert(offset < nodes_count());

	return p_t(offset % _width, offset / _height);
}


/*!	\param[in] c ���������� ��������
	\return ���������� ������������� ��������

	\note ������ ������� �� ��������� ��� ���������, �������� �������
	������������� � <i>LL</i> ��������
*/
p_t wtree::prnt(const p_t &c)
{
	assert(!sb().test_LL(c));

	return p_t(c.x / 2, c.y / 2);
}


/*!	\param[in] c ���������� ��������
	\return ���������� ������������� ��������

	\note ������� �������� ���������, �.�. ������ ����� ��������
*/
p_t wtree::prnt_uni(const p_t &c)
{
	assert(c.x > sb().get_LL().x_max ||
		   c.y > sb().get_LL().y_max);
	assert(c.x < _width && c.y < _height);

	const subbands::subband_t &sb_HH =
					sb().get(subbands::LVL_1, subbands::SUBBAND_HH);

	// ��������, ����� �� ������������ ������� � LL ��������
	if (c.x <= sb_HH.x_max && c.y <= sb_HH.y_max)
	{
		const subbands::subband_t &sb_LL = sb().get_LL();

		if (c.y <= sb_LL.y_max) return p_t(c.x - sb_LL.width, c.y);
		if (c.x <= sb_LL.x_max) return p_t(c.x, c.y - sb_LL.height);

		return p_t(c.x - sb_LL.width, c.y - sb_LL.height);
	}

	return p_t(c.x / 2, c.y / 2);
}


/*!	\param[in] lvl ������� ������� ����������, �� ������� ���������
	������� � ��������������� ��������� ��������� ����������
	\return �������� �� ������������ ��������� ��� ������� ������ ����������
	�������� ���������� ������
*/
wtree::n_iterator wtree::iterator_through_n(const sz_t lvl) const {
	return new n_cutdown_iterator(lvl);
}


/*!	\param[in] p ���������� ��������, ������� ����� �������� �������������
	� ��������� �������� ���������� ������.
	\param[in] branch ���������� ������������� ��������, ������� ������ �����.
	\return ��������� ������� ���������� ������, ����������������� ���, ���
	������ ���� ����� (������� ����������� ������� � ������������ <i>p</i>)
	�������� �������������.
*/
n_t wtree::child_n_mask(const p_t &p, const p_t &branch) const
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
*/
n_t wtree::child_n_mask_LL(const p_t &p) const 
{
	assert(subbands::LVL_1 == sb().from_point(p).lvl);

	const subbands::subband_t &sb_LL = sb().get_LL();

	if (p.y <= sb_LL.y_max) return (1 << 0);
	if (p.x <= sb_LL.x_max) return (1 << 1);

	return (1 << 2);
}


/*!	\param[in] p ���������� ��������, ������� ����� �������� �������������
	� ��������� �������� ���������� ������.
	\param[in] branch ���������� ������������� ��������, ������� ������ �����.
	\return ��������� ������� ���������� ������, ����������������� ���, ���
	������ ���� ����� (������� ����������� ������� � ������������ <i>p</i>)
	�������� �������������.
*/
n_t wtree::child_n_mask_uni(const p_t &p, const p_t &branch) const {
	if (sb().test_LL(branch)) return child_n_mask_LL(p);

	return child_n_mask(p, branch);
}


/*!	\param[in] branch ���������� ��������, ����������� �����
	\param[in] n ��������� ������� ���������� ������

	�� ����, ������ ������� ��� ������������ ���������� ������ ����������
	������ (������ ����������� ������ �� �����������).

	������� �� ����������� ���������� ����, ��� wtree::cut_leafs().
*/
void wtree::uncut_leafs(const p_t &branch, const n_t n)
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

		if (test_n_mask(n, mask)) _uncut_branch(p);
		// _uncut_branch(p, !test_n_mask(n, mask));
	}

	at(branch).n = n;
}


/*!	\param[in] sb ������� �� �������� ����� ����������� ����� � �������
	������������� ���������
	\return �������� �� ��������
*/
wtree::coefs_iterator wtree::iterator_over_subband(
		const subbands::subband_t &sb) const
{
	return new snake_2d_iterator(p_t(sb.x_min, sb.y_min),
								 p_t(sb.x_max, sb.y_max));
}


/*!	\param[in] root ���������� ��������� ��������
	\param[in] sb �������, ������ �� �������� ����� ��������������� �� �����
	�������� ��������.
	\return �������� �� �������

	\todo �������� ������� ��������� �������� � ������� ��������� �����������

	\sa _leafs_top_left()
*/
wtree::coefs_iterator wtree::iterator_over_leafs(
		const p_t &root, const subbands::subband_t &sb) const
{
	const p_t b(_leafs_top_left(root, sb.lvl, sb.i));
	const p_t e(b.x + sb.tree_w - 1, b.y + sb.tree_h - 1);

	return new snake_2d_iterator(b, e);
}


/*!	\param[in] root ���������� ��������� ��������
	\param[in] lvl �������, ������ � �������� ����� ��������������� �� �����
	�������� ��������
	\param[in] i ������ �������� ������ ������, ������ �� �������� ����
	���������������
	\return �������� �� �������

	\sa _leafs_top_left()
*/
wtree::coefs_iterator wtree::iterator_over_leafs(
		const p_t &root, const sz_t lvl, const sz_t i) const
{
	return iterator_over_leafs(root, sb().get(lvl, i));
}

/*!	\return �������� �� ����� �������
*/
wtree::coefs_iterator wtree::iterator_over_wtree() const
{
	assert(0 < _width && 0 < _height);

	return new snake_2d_iterator(p_t(0, 0), p_t(_width - 1, _height - 1));
}


////////////////////////////////////////////////////////////////////////////////
// wtree class protected definitions

/*!	\param[in] q ������������ ������������

	������� �������� ����������� ����� wnode::w � �������� ��������� � ����
	wnode::wq
*/
void wtree::_quantize(const q_t q)
{
	assert(1 <= q);

	_q = q;

	for (sz_t i = 0; nodes_count() > i; ++i)
	{
		wnode &node = _nodes[i];
		node.wq = wnode::quantize(node.w, q);
	}
}


/*!	\param[in] q ������������ ������������

	������� �������� ����������� ����� wnode::w � �������� ��������� � ����
	wnode::wq. ����� ������������ ��������� ��������:
	- ���������� �������� �� ���� wnode::wq ���������� � ���� wnode::wc.
	- ���� wnode::j0 � wnode::j1 ������������ � 0.
	- ��� ����� ���������� ��� ����������� (wnode::n ������������ � 0)
	- ��� �������� ���������� ��� ������������ (wnode::invalid
	  ��������������� � <i>false</i>)
*/
void wtree::_filling_quantize(const q_t q)
{
	assert(1 <= q);

	_q = q;

	for (sz_t i = 0; nodes_count() > i; ++i)
	{
		wnode &node		= _nodes[i];
		node.wc			= node.wq = wnode::quantize(node.w, q);
		node.j0			= node.j1 = 0;
		node.n			= 0;
		node.invalid	= false;
	}
}


/*!	��������� ��������� ��������:
	- �������� �� ���� wnode::wq ���������� � ���� wnode::wc.
	- ���� wnode::j0 � wnode::j1 ������������ � 0.
	- ��� ����� ���������� ��� ����������� (wnode::n ������������ � 0)
	- ��� �������� ���������� ��� ������������ (wnode::invalid
	  ��������������� � <i>false</i>)
*/
void wtree::_filling_refresh()
{
	for (sz_t i = 0; nodes_count() > i; ++i)
	{
		wnode &node		= _nodes[i];
		node.wc			= node.wq;
		node.j0			= node.j1 = 0;
		node.n			= 0;
		node.invalid	= false;
	}
}


/*!	\param[in] prnt ���������� ������������� ��������
	\return ���������� �������� ������ ��������� �������� �� ������
	�������� ���������

	\warning ������� �� �������� ��� ������������� � ������������
	������������ ��������� �� <i>LL</i> ��������, ��� ��� � ���� ������
	�������� �������� ����������� � ������ ���������.
*/
p_t wtree::_children_top_left(const p_t &prnt) const {
	return p_t(2 * prnt.x, 2 * prnt.y);
}


/*!	\param[in] root ���������� ��������� �������� ������
	\param[in] lvl ������� �� ������� ��������� ��������� ���� �������
	\param[in] i ������ �������� �� ������ (���������� subbands::_get())
	\return ���������� �������� ������ �������� ����� ������� �������
	������.

	\note ��� ������� ��������� �������� ��� ������ �������� ���������
	<i>lvl</i> ��� ��� ����� �������� � ������� ���������� �����������
	����.

	\todo �������� ����� ��������� ��������
*/
p_t wtree::_leafs_top_left(const p_t &root, const sz_t lvl, const sz_t i) const
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


/*!	\param[in] branch ���������� ��������, ������������ �����

	������� ������ ������������� ���� wnode::invalid ��� ��������
	�� <i>branch</i> ��������� � �������� <i>false</i>.

	\note ������� <i>branch</i> �� ����� ���� �� <i>LL</i> ��������
	��� � �������� ������������ �� ��������� ������ ����������.
*/
void wtree::_uncut_branch(const p_t &branch)
{
	// �������� �����������
	assert(sb().from_point(branch).lvl > subbands::LVL_0);
	assert(sb().from_point(branch).lvl < lvls());

	// ������� �������� ���������
	for (coefs_iterator i = iterator_over_children(branch);
		 !i->end(); i->next())
	{
		at(i->get()).invalid = false;
	}
}


/*!	\param[in] prnt ���������� ������������� ��������
	\return ��������� �� �������� �� �������� ���������

	\note ������ ������� �� ��������� ��� ������������ ��������� ��
	<i>LL</i> ��������.

	\attention ���������� ������������ �������� <i>delete</i> ���
	������������ ������, ���������� ������������ ����������.
*/
basic_iterator<p_t> *wtree::_iterator_over_children(const p_t &prnt) const
{
	// ���������� �������� ������ ��������� ��������
	const p_t c = _children_top_left(prnt);

	// �������� ���������
	return new snake_2d_iterator(p_t(c.x    , c.y    ),
								 p_t(c.x + 1, c.y + 1));
}


/*!	\param[in] prnt ���������� ������������� ��������
	\return ��������� �� �������� �� �������� ���������

	\note ������ ������� �� ��������� ��� ������������ ��������� �� ��
	<i>LL</i> ��������.

	\attention ���������� ������������ �������� <i>delete</i> ���
	������������ ������, ���������� ������������ ����������.
*/
basic_iterator<p_t> *wtree::_iterator_over_LL_children(const p_t &prnt) const
{
	const subbands::subband_t &sb_LL = sb().get_LL();

	assert(sb().test(prnt, sb_LL));

	return new LL_children_iterator(sb_LL.width, sb_LL.height, prnt);
}



}	// end of namespace wic
