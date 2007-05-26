/*******************************************************************************
* file:         codec.h                                                        *
* version:      0.1.0                                                          *
* author:       mice (mailto:my_mice@mail.ru, ICQ:332-292-380)                 *
* description:  not available                                                  *
* tests:        none                                                           *
*******************************************************************************/

#pragma once

#ifndef WIC_LIBWIC_TYPES
#define WIC_LIBWIC_TYPES

////////////////////////////////////////////////////////////////////////////////
// include


////////////////////////////////////////////////////////////////////////////////
// wic namespace
namespace wic {


////////////////////////////////////////////////////////////////////////////////
// types

//! \brief ���� (8 ���), ��� �����
typedef unsigned char byte_t;

//! \brief ��� �������������� ������� �������������
typedef float w_t;

//! \brief ��� ������������� ������� ������������� (���������������)
typedef int wk_t;

//! \brief ��� ��� ��������� ���������� ������
typedef unsigned char n_t;

//! \brief ��� ��� �������� RD-������� ��������
typedef double j_t;

//! \brief ��� �������� (������, ������, ������)
/*!	����� �������� ��������, ��� ������ ��� �������� ������������� ��
	<b>������</b>. ��������� ����� ��������� ���������� ��� ��������,
	������� �� ������� �������� ���� ��� �� \c unsigned.
*/
typedef int sz_t;

//!	\brief ��� ��� ��������� ��������
typedef int dsz_t;

//!	\\brief ��� ��� ������������
typedef float q_t;

//! \brief ��� ��� ���������� ������� <i>P<sub>i</sub></i> �
//!	<i>S<sub>j</sub></i> �� 35.pdf
typedef double pi_t;

//! \brief ��� ��� �������� ������� ������ � ��������
//!	<i>H<sub>spec</sub></i> � <i>H<sub>map</sub></i>  �� 35.pdf
typedef double h_t;

//!	\brief ��� ��� ��������� ����������� <i>lambda</i>. ������������
//! ��� ���������� ������� ��������
typedef double lambda_t;

//!	\brief ������������� ��������� ����������
/*!	���� ��� ����� ���� ������� ������� ���������� � ����� ������.
	��� �� ���� �������, ����� ���������� ������������� ����
	��������� �����������, � ������� ����� ����� ����������� ��������
	������ � ���������� ������������. ��������, ����� ��������� ��
	��������� ����������� ��������� ������ ������� ������ ��������
	�� ������ � �������������� ������������.
*/
struct p_t {
	//! \brief ���, ������� ����� ����������� ��� ���������
	typedef sz_t size_type;

	//! \brief ��������������� �����������
	/*!	\param[in] x x ����������
		\param[in] y y ����������
	*/
	p_t(const size_type &x, const size_type &y): x(x), y(y) {}

	bool operator == (const p_t& other) const { return ((x == other.x) && (y == other.y)); }

	//! \brief ���������� �������� ���������� x
	const size_type &getx() const { return x; }

	//! \brief ���������� �������� ���������� y
	const size_type &gety() const { return y; }

	//! \brief ���������� �������� ���������� x
	size_type &getx() { return x; }

	//! \brief ���������� �������� ���������� y
	size_type &gety() { return y; }

	//! \brief x ����������
	size_type x;

	//! \brief y ����������
	size_type y;
};



}	// end of namespace wic



#endif	// WIC_LIBWIC_TYPES
