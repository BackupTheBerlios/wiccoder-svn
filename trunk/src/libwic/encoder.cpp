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

	_encode_step_1();

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


/*!	\param[in] p ���������� ������������ ��� �������������
	\param[in] lambda �������� <i>lambda</i> ������������ ���
	���������� <i>RD</i> �������� (������� ��������). ������������
	����� ������ ����� ������� � �������� ���������.
	\param[in] sb �������, � ������� ��������� �����������
	\return ������������������ �������� ������������

	\todo ����������� ��� �������
*/
wk_t encoder::_coef_fix(const p_t &p, const lambda_t &lambda,
						const subbands::subband_t &sb)
{
	const wnode &node = _wtree.at(p);

	const wk_t &wq = node.wq;

	const wk_t wk_vals[4] = {wq, wq+1, wq-1, 0};
	wk_t wc = 0;

	for (int i = 0; 4 > i; ++i) {
		const wk_t &k = wk_vals[i];
		const w_t wr = wnode::dequantize(k, _wtree.q());
		const w_t dw = (wr - node.w);
		(dw * dw) + lambda * _h_spec(_ind_spec(_wtree.calc_sj(p.x, p.y, true, sb) , sb.lvl),  k);
	}

	return wc;
}


/*!
	\todo ������� some_iterator �������� ����������� ��������
	(snake_square_iterator::going_left())
	\todo ����������� ��� �������
*/
void encoder::_encode_step_1() {
	// ��������������� ��� ���� �������������� ������
	const sz_t lvl = _wtree.sb().lvls() + subbands::LVL_PREV;

	// ������������� ��������������� �������������
	for (sz_t k = 0; subbands::SUBBANDS_ON_LEVEL > k; ++k) {
		const subbands::subband_t &sb = _wtree.sb().get(lvl, k);

		// ��� ���� ������������� � ��������
		for (wtree::coefs_iterator i = _wtree.iterator_over_subband(sb);
			!i->end(); i->next())
		{
			_coef_fix(i->get(), 0, sb);
		}
	}	
}



}	// end of namespace wic
