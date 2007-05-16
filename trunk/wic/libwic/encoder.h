/*******************************************************************************
* file:         encoder.h                                                      *
* version:      0.1.0                                                          *
* author:       mice (mailto:my_mice@mail.ru, ICQ:332-292-380)                 *
* description:  not available                                                  *
* tests:        none                                                           *
*******************************************************************************/

#pragma once

#ifndef WIC_LIBWIC_ENCODER
#define WIC_LIBWIC_ENCODER

////////////////////////////////////////////////////////////////////////////////
// include

// standard C++ library headers
#include <assert.h>

// external library header
#include <arcoder/aencoder.h>
#include <arcoder/adecoder.h>

// libwic headers
#include <wic/libwic/types.h>
#include <wic/libwic/wtree.h>
#include <wic/libwic/iterators.h>


////////////////////////////////////////////////////////////////////////////////
// wic namespace
namespace wic {


////////////////////////////////////////////////////////////////////////////////
// encoder class declaration

//! \brief Обёртка для енкодера. Осуществляет сжатие изображения.
/*! Класс предоставляет методы для сжатия изображения. Позволяет сжимать
	одно и то же изображение несколько раз для подбора параметров кодирования.
*/
class encoder {
public:
	// public types ------------------------------------------------------------

	//!	\brief Псевдоним для арифметического енкодера
	typedef wtc::aencoder<wk_t> aencoder;

	//!	\brief Псевдоним для арифметического декодера
	typedef wtc::adecoder<wk_t> adecoder;

	// public constants --------------------------------------------------------
	// public methods ----------------------------------------------------------

	//!	\brief Конструкртор
	encoder(const w_t *const image,
			const sz_t width, const sz_t height, const sz_t lvls);

	//!	\brief Деструктор
	~encoder();

	//!	\brief Функция осуществляющая непосредственное кодирование изображения
	void encode();

protected:
	// protected methods -------------------------------------------------------

	//!	\name Выбор модели арифметического кодера по прогнозу
	//@{

	//! \brief Реализует функцию IndSpec(<i>S<sub>j</sub></i>) из 35.pdf
	sz_t _ind_spec(const pi_t &s, const sz_t lvl);

	//! \brief Реализует функцию IndMap(<i>P<sub>i</sub></i>) из 35.pdf
	sz_t _ind_map(const pi_t &p, const bool is_LL = false);

	//@}

	//!	\name Поддержка арифметического кодирования
	//@{

	//! \brief Подсчитывает битовые затраты для кодирования коэффициента.
	//! Реализует функцию <i>H<sub>spec</sub></i>.
	h_t _h_spec(const sz_t m, const wk_t &wk);

	//! \brief Подсчитывает битовые затраты для кодирования группового
	//! признака подрезания ветвей. Реализует функцию <i>H<sub>map</sub></i>.
	h_t _h_map(const sz_t m, const n_t &n);

	//@}

	//!	\name Операции выполняемые при кодировании
	//@{

	//! \brief По координате элемента определяет направление перемещения
	/*!	\param[in] p Координаты элемента
		\return \c true, если 
	*/
	bool _going_left(const p_t &p);

	h_t _calc_h_spec(const p_t &p, const subbands::subband_t &sb);

	//! \brief Производит корректировку коэффициента
	wk_t _coef_fix(const p_t &p, const lambda_t &lambda,
				   const subbands::subband_t &sb);

	//@}

	//!	\name Шаги кодирования
	//@{

	//! \brief Шаг 1. Подготовительный.
	void _encode_step_1();

	//@}

private:
	// private data ------------------------------------------------------------

	//! \brief Рабочее дерево коэффициентов
	wtree _wtree;

	//! \brief Арифметический енкодер
	aencoder _aenc;
};



}	// end of namespace wic



#endif	// WIC_LIBWIC_ENCODER
