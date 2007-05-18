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
// defines

//!	\brief Если этот макрос определён, в функции encoder::_coef_fix для
//!	корректировки будут использоваться 4 значения: w, w + 1, w - 1 и 0. Если
//!	макрос не определён, будет проверенно только 3 значения: w, w +/- 1, 0.
//!	Знак операции выбирается в зависимости от значения коэффициента - плюс для
//! отрицательных и минус для положительных коэффициентов
#define COEF_FIX_USE_4_POINTS


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

	//!	\name Конструкторы и деструкторы
	//@{

	//!	\brief Конструкртор
	encoder(const w_t *const image,
			const sz_t width, const sz_t height, const sz_t lvls);

	//!	\brief Деструктор
	~encoder();

	//@}

	//!	\name Функции кодированя
	//@{

	//!	\brief Функция осуществляющая непосредственное кодирование изображения
	void encode();

	//@}

protected:
	// protected methods -------------------------------------------------------

	//!	\name Выбор модели арифметического кодера по прогнозу
	//@{

	//! \brief Реализует функцию IndSpec(<i>S<sub>j</sub></i>) из 35.pdf
	sz_t _ind_spec(const pi_t &s, const sz_t lvl);

	//! \brief Реализует функцию IndSpec(<i>S<sub>j</sub></i>) из 35.pdf.
	/*!	\param[in] p Координаты элементы
		\param[in] sb Саббенд, в котором находится элемент
		\return Номер выбираемой модели

		Функция использует координаты элемента в качестве входных
		параметров.

		С помощью параметра шаблона возможно выбирать поле элемента,
		которое будет использоваться при вычислении прогнозов. Так как,
		во времдя декодирования нам известны только откорректированные
		коэффициенты, наиболее вероятным полем является wnode::member_wc.
	*/
	template <const wnode::wnode_members member>
	sz_t _ind_spec(const p_t &p, const subbands::subband_t &sb) {
		return _ind_spec(_wtree.calc_sj<member>(p.x, p.y, sb), sb.lvl);
	}

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

	//!	\brief Подсчитывает значение <i>RD</i> функции <i>Лагранжа</i>
	//! для значения коэффициента при кодировании его арифметическим кодером,
	//!	если бы он находился на определённых координатах.
	j_t _calc_rd_iteration(const p_t &p, const wk_t &k,
						   const lambda_t &lambda, const sz_t &model);

	//! \brief Ищет оптимальное откорректированное значение для коэффициента
	wk_t _coef_fix(const p_t &p, const subbands::subband_t &sb,
				   const lambda_t &lambda);

	//! \brief Расчитывает <i>RD</i> функцию <i>Лагранжа</i> для варианта
	//!	подрезания ветвей
	/*!	\todo Написать тест для этой функции
	*/
	j_t _calc_j0_value(const p_t &p)
	{
		j_t j0 = 0;

		for (wtree::coefs_iterator i = _wtree.iterator_over_children(p);
			 !i->end(); i->next())
		{
			const wnode &node = _wtree.at(i->get());
			j0 += (node.wc * node.wc + node.j0);
		}

		return j0;
	}

	//! \brief Расчитывает <i>RD</i> функцию <i>Лагранжа</i> для варианта
	//!	сохранения и подрезания ветвей
	/*!	\todo Написать тест для этой функции
	*/
	j_t _calc_j1_value(const p_t &p, const subbands::subband_t &sb,
					   const lambda_t &lambda)
	{
		const sz_t model = _ind_spec<wnode::member_wc>(p, sb);

		j_t j1 = 0;

		for (wtree::coefs_iterator i = _wtree.iterator_over_children(p);
			 !i->end(); i->next())
		{
			const wnode &node = _wtree.at(i->get());
			j1 += _calc_rd_iteration(i->get(), node.wc, lambda, model);
		}

		return j1;
	}

	//@}

	//!	\name Шаги кодирования
	//@{

	//! \brief Шаг 1. Подготовительный.
	void _encode_step_1(const lambda_t &lambda);

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
