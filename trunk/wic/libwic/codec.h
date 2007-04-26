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

//! \brief Входные данные для енкодера
struct encode_in_t {
	//! \brief Коэффициенты вейвлет преобразования изображения
	w_t *image;
	//! \brief Ширина изображения
	sz_t width;
	//! \brief Высота изображения
	sz_t height;
};

//! \brief Выходные данные енкодера
struct encode_out_t {
	//! \brief Буфер для приёма сжатого изображения
	byte_t *data;
	//! \brief Размер буфера
	sz_t data_sz;
	//! \brief Размер записанных в буфер данных
	sz_t enc_sz;
};

//! \brief Входные данные для декодера
struct decode_in_t {
};

//! \brief Выходные данные декодера
struct decode_out_t {
};


////////////////////////////////////////////////////////////////////////////////
// encode and decode functions declaration

//! \brief Функция кодирования (сжатия) изображения
bool encode(const encode_in_t &in, encode_out_t &out);

//! \brief Функция декодирования (восстановления) изображения
bool decode(const decode_in_t &in, decode_out_t &out);



}	// end of namespace wic



#endif	// WIC_LIBWIC_INCLUDED
