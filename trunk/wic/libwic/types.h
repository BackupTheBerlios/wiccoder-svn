/*!	\file     types.h
	\version  0.0.1
	\author   mice, ICQ: 332-292-380, mailto:wonder.mice@gmail.com
	\brief    �������� ������� ������������ �����

	\todo     ����� �������� ������� ���� types.h
*/

#pragma once

#ifndef WIC_LIBWIC_TYPES
#define WIC_LIBWIC_TYPES

////////////////////////////////////////////////////////////////////////////////
// include


////////////////////////////////////////////////////////////////////////////////
// wic namespace
namespace wic {


////////////////////////////////////////////////////////////////////////////////
// constants

//!	\brief ���������� ����� � �����
static const int BITS_PER_BYTE	= 8;


////////////////////////////////////////////////////////////////////////////////
// types

//! \brief ���� (8 ���), ��� �����
typedef unsigned char byte_t;

//! \brief ��� �������������� ������� �������������
typedef float w_t;

//! \brief ��� ������������� ������� ������������� (���������������)
typedef int wk_t;

//! \brief ��� ��� ��������� ��������� ���������� ������
/*!	��������� ������� ���������� ������ ������������ ����� ������� �����.
	��� ������������� � 0 ��������� �� ����������� �����, ��� �������������
	� 1 ��������� �� ����������� �����. ��� ��������� ����������
	������������ ������� �������.

	����� ����� ��������� ������������ ����� ������� ���� � ����������
	������������� ��������, ����������� (�����������) �����. ���� ����������
	������ (������� ���� ����� ������� ������). ��� LL ��������:
	\verbatim
	      bit_0
	bit_1 bit_2
	\endverbatim

	��� ��������� ���������:
	\verbatim
	bit_0 bit_1
	bit_2 bit_3
	\endverbatim
*/
typedef unsigned char n_t;

//! \brief ��� ��� �������� <i>RD-������� ��������</i>
typedef double j_t;

//! \brief ��� �������� (������, ������, ������)
/*!	����� �������� ��������, ��� ������ ��� �������� ������������� ��
	<b>������</b>. ��������� ����� ��������� ���������� ��� ��������,
	������� <b>�� �������</b> �������� ���� ��� �� <i>unsigned</i>.
*/
typedef int sz_t;

//!	\brief ��� ��� ������� ��������� � ���������� ��������
typedef int dsz_t;

//!	\brief ��� ��� ������������
typedef float q_t;

//! \brief ��� ��� ���������� ������� <i>P<sub>i</sub></i> �
//!	<i>S<sub>j</sub></i> �� 35.pdf
typedef double pi_t;

//! \brief ��� ��� �������� ������� ������ � ��������
//!	<i>H<sub>spec</sub></i> � <i>H<sub>map</sub></i>  �� 35.pdf
typedef double h_t;

//!	\brief ��� ��� ��������� ����������� <i>lambda</i>. ������������
//! ��� ���������� <i>RD-������� ��������</i>
typedef double lambda_t;

//!	\brief ������������� ��������� ����������
/*!	���� ��� ����� ���� ������� ������� ���������� � ����� ������.
	��� �� ���� �������, ����� ���������� ������������� ����
	��������� �����������, � ������� ����� ����� ����������� ��������
	������ � ���������� ������������. ��������, ����� ��������� ��
	��������� ����������� ��������� ������ ������� ������ ��������
	�� ������ � �������������� ������������.

	\todo ��������, ����� ���������� ������� ���� ��� � ��������� �����,
	������� ��� ������������ ����������� � ��������������.
*/
struct p_t {
	//! \brief ���, ������� ����� ����������� ��� ���������
	typedef sz_t size_type;

	//! \brief ��������������� �����������
	/*!	\param[in] x x ����������
		\param[in] y y ����������
	*/
	inline p_t(const size_type &x, const size_type &y): x(x), y(y) {}

	//! \brief ���������� �������� ���������� x
	inline const size_type &getx() const { return x; }

	//! \brief ���������� �������� ���������� y
	inline const size_type &gety() const { return y; }

	//! \brief ���������� �������� ���������� x
	inline size_type &getx() { return x; }

	//! \brief ���������� �������� ���������� y
	inline size_type &gety() { return y; }

	//! \brief x ����������
	size_type x;

	//! \brief y ����������
	size_type y;
};



////////////////////////////////////////////////////////////////////////////////
// functions

//!	\brief ���������� ���������� ��������� � �������
/*!	\param[in] a ������
	\return ���������� ��������� � �������

	Example:
	\code
	int ar[25];
	std::cout << "items count: " << wic::items_count(ar) << std::endl;
	\endcode
*/
template <class T>
size_t items_count(const T &a) {
	return (sizeof(a) / sizeof(a[0]));
}



}	// end of namespace wic



#endif	// WIC_LIBWIC_TYPES
