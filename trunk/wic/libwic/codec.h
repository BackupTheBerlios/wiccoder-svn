/*!	\file     codec.h
	\version  0.0.1
	\author   mice, ICQ: 332-292-380, mailto:wonder.mice@gmail.com
	\brief    ������� ������������ ����

	������ ������������ ���� �������� ����� �� ����������� ���
	������������� ���������� wiccoder.

	\todo     ����� �������� ������� ���� coder.h
*/

#pragma once

#ifndef WIC_LIBWIC_INCLUDED
#define WIC_LIBWIC_INCLUDED

////////////////////////////////////////////////////////////////////////////////
// include

// libwic headers
#include <wic/libwic/types.h>


////////////////////////////////////////////////////////////////////////////////
// wic namespace
//!	\brief Namespace for wiccoder library
/*!	\todo Add detailed description for <i>%wic</i> namespace.
*/
namespace wic {


////////////////////////////////////////////////////////////////////////////////
// doxygen documentation
/*!	\mainpage

	\section mainpage_sectAbout � �������

	<a href="http://developer.berlios.de/projects/wiccoder/">
	<b>wiccoder</b></a> - �����-������������� ����� � �������� ��������
	����� ��� ������ ����������� � ��������. ��������������� �
	<a href="http://www.miee.ru">���������� ��������� ����������� �������
	(����)</a>.
	
	<a href="http://developer.berlios.de/projects/wiccoder/">
	<b>wiccoder</b></a> ������������ ����� ���������� � ����� ������������ ���
	������� ������������ ���� � ������������ ������� ������ �����������.
	�������� ����� ���������� �������� �� ����� <i>C++</i> � �������
	�������� ����������, ��� ������ � ����� ������ � ��������� ���
	������������ ����������, �������� �� ��������������. ����������
	��������� ����������������, ��� ������ � �������� ��������� �������.

	\section mainpage_sectDocIndex �������� ������������

	����� ��������� ���������� ������� ������������ �� ����������� ��
	������ � ������ ���� �������� ��������� ����. ��� ��������� �����
	���� � �������� ������� ���� �������� � <i>wiccoder</i>. ��� �����
	��������� ���������� �� ��������� ���� ����������� � ���������������
	������� ������ ������������.

	<b>����������:</b>
	- \ref pgTerminology
		- \ref pgTerminology_sectIntroduction
		- \ref pgTerminology_sectSubbands
		- \ref pgTerminology_sectTrees_leafs_and_branches
		- \ref pgTerminology_sectElements_and_Coefs
		- \ref pgTerminology_sectGroupedCutSigns
	- \ref pgCoding
		- \ref pgCoding_sectAbout
		- \ref pgCoding_sectIterators
	- \ref pgLicense
		- \ref pgLicense_sectAbout

	\section mainpage_sectContacts ��������

	�������, ��������� � ����������� ����������� �� ����������� ��������
	���� <a href="mailto:wonder.mice@gmail.com">wonder.mice@gmail.com</i>.
*/

/*!	\page pgTerminology ������������

	\section pgTerminology_sectIntroduction ����������

	� ������ ���������� ��������� ������ ����������� �������������
	������������. ��������� ������� � ����������� ����������� ��� �������
	������ ���������� � �������� ��������� �������� ������ ������������ �� ��
	���������� ��� � ���������� ������. ��� ��, ������ ����� ��������,
	������������� ����������� ��� ������� �������� � ������ ��������� �
	������ ���������. � ���� ������ ����� ������� ����������� ����� �������,
	�������, ��� ������������� ���������� � ���������� ��� ��� ���� �����
	�������� ��������� ���� ��� ������������, ������� ������ ����� ���������
	� ���� ������. ������� ����� ����� ����� ���������� � �����������
	������������ ������������, ��� ��� ������� ������ ��� ��� �������� ����
	�� ���������� ������� � ������.

	\section pgTerminology_sectSubbands ��������

	������ <i>�������</i> (<i>subband</i>) ����� ������������ � ����
	������������ �������� � ������������ ����� ������������� ��������
	������� ����������� ������� ��������������. �� ������ ������ ����������
	���������� �� ��� �������� ����������� �������. ��� ���� �������
	(<i>LL</i>, �� <i>LowLow</i>, ����� ��������������) ������ �������������
	� ������� ����� ���� �������. ����� �������, ���������� ���������
	������� �� ����� ������� ���������� ��� <i>Sb<sub>count</sub> = 
	3*Levels<sub>count</sub> + 1</i>. ������ <a href="../35.pdf">35.pdf</a>
	��� ����� ��������� ���������� � ���������.

	\section pgTerminology_sectTrees_leafs_and_branches �������, ������ � �����

	������ <i>������</i> (<i>tree</i>) ����� ������������ � ���� �� ��
	����������. ������� ����� ���������� ��� � ���� ������� ������ (���������
	���������� ������� ��������������), ��� � �������� ������ ������ (�
	������� ��������� ����� �����������, ������ �������� �������� �������,
	������� � ����� �������������� (<i>LL</i>) ��������� �������
	(<i>��������</i>). ������ ����������� �������� ����� ������ � ������������.
	� ���������� ������� ������������ �������� <i>������</i> ������ ��� ������
	(<i>������</i>) ������������� � �� ��������, � ����� ���������, �����������
	� <i>LL</i> ��������.

	<i>�������� ��������� ������</i> (<i>root</i>) ����� �������� ����� �������
	�� <i>LL</i> ��������.

	������ <i>������</i> (<i>leafs</i>) ����� ����������� ��� �����������
	������ ������������� ������������� ������ ������ � ������� � ������� ������
	�������� ��� ���������� ��������� ������ ������ ����������. ��� �� ������
	�����������, �� ������ � ��������� ���������.

	��� <i>������</i> (<i>branch</i>) ����� �������� ����� ������������� � ��
	�������� � ����� ��������� (�� ����������� �� <i>LL</i> ��������).

	\section pgTerminology_sectElements_and_Coefs �������� � ������������

	������� ��������, ��� � ������ ���������� ���������, �������
	<i>�������</i> (<i>element</i>) � <i>�����������</i> (<i>coefficient</i>,
	<i>factor</i>) ��������. <i>�����������</i> - ��� ��������� ��������
	������� ������������. �� ����� ���� ���������������, ������������������
	��� ����� ������, �� �������, ��� ��� ������ ������������ ��������
	��������. <i>�������</i> ������������ ����� ������ �������� (��� ���������
	� �������� ����� <i>C++</i>), ����������� ����� ���������� � �������
	�������. ��� ���������������, ������������������ � ������������ ��������
	������ ������������ ������������ � <i>�������</i>.

	\section pgTerminology_sectGroupedCutSigns ��������� �������� ����������

	� ��������� � � ���� ��������� �������� ���������� ������������ ������
	<i>N</i> (��� <i>n</i>). <i>N<sub>i</sub></i> ������������ �����
	��������� ������� ���������� ������, ����������� � �������� � ��������
	<i>i</i>, � �� ����� ��� <i>n<sub>i</sub></i> ��������� � ����������
	�������� ���������� <i>i-��</i> �������� �� ������. ������������ ������
	��������� ���������� �������� ������ ��� ��������� (���� � ������ �
	����������), ������� ��� ����� ��������� ��������� �������������
	����������� � <a href="../35.pdf">35.pdf</a>.
*/

/*! \page pgCoding ����������

	\section pgCoding_sectAbout � ����������

	���������� <i>wiccoder</i> �������� �� <i>C++</i> � ������� ��������
	����������, ��� ��������� � �������� ������������ � ������������.
*/

/*!	\page pgLicense ������������ ����������

	\section pgLicense_sectAbout � ����������

	���������� <i>wiccoder</i> ����������� ��� 
	������������ ����������� <i>GNU Lesser General Public License (LGPL)</i>.

	���� ������� �����, ������� ������� �������� ����������� � �������
	��������� ����� <i>wiccoder</i>:

	\verbatim
	MIEE (C) 2007, MIEE DSP Team

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
	USA
	\endverbatim
*/


////////////////////////////////////////////////////////////////////////////////
// types

//! \brief ������� ������ ��� ��������
struct encode_in_t {
	//! \brief ������������ ������� �������������� �����������
	w_t *image;
	//! \brief ���������� ������� ������� ��������������
	sz_t lvls;
	//! \brief ������ �����������
	sz_t width;
	//! \brief ������ �����������
	sz_t height;
};

//! \brief �������� ������ ��������
struct encode_out_t {
	//! \brief ����� ��� ����� ������� �����������
	byte_t *data;
	//! \brief ������ ������
	sz_t data_sz;
	//! \brief ������ ���������� � ����� ������
	sz_t enc_sz;
};

//! \brief ������� ������ ��� ��������
struct decode_in_t {
};

//! \brief �������� ������ ��������
struct decode_out_t {
};


////////////////////////////////////////////////////////////////////////////////
// encode and decode functions declaration

//! \brief ������� ����������� (������) �����������
bool encode(const encode_in_t &in, encode_out_t &out);

//! \brief ������� ������������� (��������������) �����������
bool decode(const decode_in_t &in, decode_out_t &out);



}	// end of namespace wic



#endif	// WIC_LIBWIC_INCLUDED
