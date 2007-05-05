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
typedef double w_t;

//! \brief ��� ������������� ������� �������������
typedef int wk_t;

//! \brief ��� ��� ��������� ���������� ������
typedef unsigned char n_t;

//! \brief ��� ��� �������� RD-������� ��������
typedef double j_t;

//! \brief ��� �������� (������, ������, ������)
typedef unsigned int sz_t;

//!	\\brief ��� ��� ������������
typedef double q_t;

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

	//! \brief ���������� �������� ���������� x
	const size_type &get_x() const { return x; }

	//! \brief ���������� �������� ���������� y
	const size_type &get_y() const { return y; }

	//! \brief ���������� �������� ���������� x
	const size_type &get_x() { return x; }

	//! \brief ���������� �������� ���������� y
	const size_type &get_y() { return y; }

	//! \brief x ����������
	size_type x;

	//! \brief y ����������
	size_type y;
};



}	// end of namespace wic



#endif	// WIC_LIBWIC_TYPES
