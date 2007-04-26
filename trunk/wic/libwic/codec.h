/*******************************************************************************
* file:         codec.h                                                        *
* version:      0.1.0                                                          *
* author:       mice (mailto:my_mice@mail.ru, ICQ:332-292-380)                 *
* description:  not available                                                  *
* tests:        none                                                           *
*******************************************************************************/

#pragma once

#ifndef WIC_LIBWIC_INCLUDED
#define WIC_LIBWIC_INCLUDED

////////////////////////////////////////////////////////////////////////////////
// include

// libwic headers
#include <wic/libwic/types.h>


////////////////////////////////////////////////////////////////////////////////
// wic namespace
namespace wic {


////////////////////////////////////////////////////////////////////////////////
// types

//! \brief ������� ������ ��� ��������
struct encode_in_t {
	//! \brief ������������ ������� �������������� �����������
	w_t *image;
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
