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


/*!	\param[in] prnt ���������� ������������� ������������
	\return �������� �� �������� �������������
*/
wtree::coefs_iterator wtree::iterator_over_children(const p_t &prnt)
{
	// ���������� �������� ������ ��������� ��������
	const p_t c = _children_top_left(prnt);

	// �������� ���������
	return new snake_square_iterator(p_t(c.x    , c.y    ),
									 p_t(c.x + 1, c.y + 1));
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
	������������ ��������� �� �������� LL, ��� ��� � ���� ������ ��������
	�������� ����������� � ������ ���������.
*/
p_t wtree::_children_top_left(const p_t &prnt) {
	return p_t(2 * prnt.x, 2 * prnt.y);
}



}	// end of namespace wic
