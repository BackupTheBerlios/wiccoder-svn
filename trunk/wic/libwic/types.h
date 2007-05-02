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

//! \brief Байт (8 бит), без знака
typedef unsigned char byte_t;

//! \brief Тип действительных вейвлет коэффициентов
typedef double w_t;

//! \brief Тип целочисленных вейвлет коэффициентов
typedef int wk_t;

//! \brief Тип для признаков подрезания ветвей
typedef unsigned char n_t;

//! \brief Тип для значений RD-функции Лагранжа
typedef double j_t;

//! \brief Тип размеров (ширина, высота, длинна)
typedef unsigned int sz_t;

//!	\\brief Тип для квантователя
typedef double q_t;

//!	\brief Двухмерные координаты
struct p_t {
	//! \brief Вспомогательный конструктор
	/*!	\param[in] x x координата
		\param[in] y y координата
	*/
	p_t(const sz_t &x, const sz_t &y): x(x), y(y) {}
	//! \brief x координата
	sz_t x;
	//! \brief y координата
	sz_t y;
};



}	// end of namespace wic



#endif	// WIC_LIBWIC_TYPES
