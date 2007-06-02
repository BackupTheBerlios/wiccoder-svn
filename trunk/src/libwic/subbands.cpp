/*******************************************************************************
* file:         subbands.cpp                                                   *
* version:      0.1.0                                                          *
* author:       mice (mailto:my_mice@mail.ru, ICQ:332-292-380)                 *
* description:  not available                                                  *
* tests:        none                                                           *
*******************************************************************************/


////////////////////////////////////////////////////////////////////////////////
// headers

// standard C++ library headers
// none

// libwic headers
#include <wic/libwic/types.h>
#include <wic/libwic/subbands.h>


////////////////////////////////////////////////////////////////////////////////
// wic namespace
namespace wic {


////////////////////////////////////////////////////////////////////////////////
// subbands class public definitions

/*!	\param[in] width ������ �����������
	\param[in] height ������ �����������
	\param[in] lvls ���������� ������� ������� ��������������
*/
subbands::subbands(const sz_t width, const sz_t height, const sz_t lvls) {
	// ���������� ���������� �����������
	_width	= width;
	_height	= height;
	_lvls	= lvls;

	// ���������� ���������� ���������
	_mcount	= SUBBANDS_PER_LEVEL * _lvls;
	_count	= _mcount + SUBBANDS_ON_0_LEVEL;

	// ��������� ������
	_sb = new subband_t[_count];
	if (0 == _sb) throw std::bad_alloc();

	// ���������� HH ��������
	sz_t x_min = _width / 2;
	sz_t y_min = _height / 2;
	sz_t x_max = _width - 1;
	sz_t y_max = _height - 1;

	// ���������� ���������� � ��������� (����� LL)
	for (sz_t lvl = _lvls; 0 < lvl; --lvl) {

		_mk_lvl(lvl, x_min, y_min, x_max, y_max);

		x_max = x_min - 1;
		y_max = y_min - 1;
		x_min /= 2;
		y_min /= 2;
	}

	// ���������� �������� � ����������
	_LL_width	= (x_max + 1);
	_LL_height	= (y_max + 1);
	const sz_t trees_count = _LL_width * _LL_height;

	// ���������� ���������� � LL ��������
	subband_t &sb_ll = subbands::get_LL();
	sb_ll.x_min = 0;
	sb_ll.y_min = 0;
	sb_ll.x_max = x_max;
	sb_ll.y_max = y_max;
	sb_ll.count = trees_count;
	sb_ll.prnt	= 0;
	sb_ll.lvl	= 0;
	sb_ll.i		= 0;

	// ������� ���������� ����� ������������� �� ���� ������ � ��������
	for (sz_t i = 0; _count > i; ++i) {
		subband_t &sb	= subbands::get(i);
		sb.width		= (sb.x_max - sb.x_min + 1);
		sb.height		= (sb.y_max - sb.y_min + 1);
		sb.tree_w		= sb.width / _LL_width;
		sb.tree_h		= sb.height / _LL_height;
		sb.npt			= sb.tree_w * sb.tree_h;
	}
}


/*!
*/
subbands::~subbands() {
	if (0 != _sb) delete[] _sb;
}


/*!	\param[in] lvl ����� ������, ���������� ��������� �� ������� ����������
	������.
*/
sz_t subbands::subbands_on_lvl(const sz_t lvl) const
{
	assert(0 <= lvl && lvl <= _lvls);

	if (LVL_0 == lvl) return SUBBANDS_ON_0_LEVEL;

	return SUBBANDS_PER_LEVEL;
}


////////////////////////////////////////////////////////////////////////////////
// subbands class protected definitions

/*!	\param[in] i ����� (������) ��������
	\return ��������� �� ������� � ��������� ��������

	�������� ������������� �� ������������ �������. LL ������� �����
	������ 0, ����� ������� HH ������� ����� ���������� ������.
*/
subbands::subband_t *subbands::_get(const sz_t i) const {
	assert(0 <= i && i < _count);

	if (0 == i) return (_sb + _mcount);

	return (_sb + (i - SUBBANDS_ON_0_LEVEL));
}


/*!	\param[in] lvl ������� ����������
	\param[in] i ����� ������� � ������ \c lvl

	������� ���������� - ����� �� 0 �� ���������� ������� ����������
	(������������). �� ������ 0 ��������� ������ ���� LL ������� �
	�������� \c i �� �����������. ��� ������ ������ ������ (������� ���
	���������), ��� ������ ��� �����.

	�������� ������ ������ ���������� �� �������:
	- HL ������� ����� ������ 0 (��������� subbands::SUBBAND_HL)
	- LH ������� ����� ������ 1 (��������� subbands::SUBBAND_LH)
	- HH ������� ����� ������ 2 (��������� subbands::SUBBAND_HH)

	��� ������ �� �������� �������� ������� ��� ������� � ���������,
	����� ������� �������� ��� ���������.
*/
subbands::subband_t *subbands::_get(const sz_t lvl, const sz_t i) const
{
	assert(0 <= i && i < SUBBANDS_PER_LEVEL);
	assert(0 <= lvl && lvl <= _lvls);

	if (0 == lvl) return (_sb + _mcount);

	const sz_t k = (lvl - 1) * SUBBANDS_PER_LEVEL + i;

	assert(0 <= k && k <= _count);

	return (_sb + k);
}


/*!	\param[in] lvl ������� ������� ��������������
	\param[in] x_min ���������� HH ��������
	\param[in] y_min ���������� HH ��������
	\param[in] x_max ���������� HH ��������
	\param[in] y_max ���������� HH ��������
*/
void subbands::_mk_lvl(const sz_t lvl,
					   const sz_t x_min, const sz_t y_min,
					   const sz_t x_max, const sz_t y_max)
{
	// ���������� ������������� � ��������� �� ���� ������
	const sz_t count = x_min * y_min;

	// ������ �� HL, LH � HH �������� ����� ������ ��������������
	subband_t &sb_hl = get(lvl, SUBBAND_HL);
	subband_t &sb_lh = get(lvl, SUBBAND_LH);
	subband_t &sb_hh = get(lvl, SUBBAND_HH);

	// ������� HL
	sb_hl.x_min = x_min;
	sb_hl.y_min = 0;
	sb_hl.x_max = x_max;
	sb_hl.y_max = y_min - 1;
	sb_hl.count = count;
	sb_hl.prnt	= _get(lvl + LVL_PREV, SUBBAND_HL);
	sb_hl.lvl	= lvl;
	sb_hl.i		= SUBBAND_HL;

	// ������� LH
	sb_lh.x_min = 0;
	sb_lh.y_min = y_min;
	sb_lh.x_max = x_min - 1;
	sb_lh.y_max = y_max;
	sb_lh.count = count;
	sb_lh.prnt	= _get(lvl + LVL_PREV, SUBBAND_LH);
	sb_lh.lvl	= lvl;
	sb_lh.i		= SUBBAND_LH;

	// ������� HH
	sb_hh.x_min = x_min;
	sb_hh.y_min = y_min;
	sb_hh.x_max = x_max;
	sb_hh.y_max = y_max;
	sb_hh.count = count;
	sb_hh.prnt	= _get(lvl + LVL_PREV, SUBBAND_HH);
	sb_hh.lvl	= lvl;
	sb_hh.i		= SUBBAND_HH;
}



}	// namespace wic
