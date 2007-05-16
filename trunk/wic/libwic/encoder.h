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

	//! \brief Реализует функцию IndSpec(<i>S<sub>j</sub></i>) из 35.pdf.
	/*!	\param[in] p Координаты элементы
		\param[in] sb Саббенд, в котором находится элемент
		\return Номер выбираемой модели

		Функция использует координаты элемента в качестве входных
		параметров. С помощью параметра шаблона возможно выбирать
		поле элемента, которое будет использоваться в вычислениях.
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

	//! \brief Производит корректировку коэффициента
	/*!	\param[in] p Координаты коэффициента для корректировки
		\param[in] sb Саббенд, в котором находится коэффициент
		\param[in] lambda Параметр <i>lambda</i> используемый для
		вычисления <i>RD</i> критерия (функции Лагранжа). Представляет
		собой баланс между ошибкой и битовыми затратами.
		\return Значение откорректированного коэффициента

		Параметр шаблона позволяет выбирать поле элемента для корректировки.

		\todo Реализовать эту функцию
		\todo Решить, нужно ли, чтобы она была шаблонной
		\todo Написать вспомогательную фукцию для подсчёта RD критерия
	*/
	template <const wnode::wnode_members member>
	typename wnode::type_selector<member>::result _coef_fix(
			const p_t &p, const subbands::subband_t &sb,
			const lambda_t &lambda)
	{
		// types
		typedef wnode::type_selector<member>::result member_t;

		// get reference to node
		const wnode &node = _wtree.at(p);

		// Коэффициент для корректировки
		const member_t &w = node.get<member>();

		// Квантователь
		const q_t q = _wtree.q();

		// Подбираемые значения
		#ifdef COEF_FIX_USE_4_POINTS
			static const sz_t vals_count	= 4;
			const member_t w_vals[vals_count] = {w, w + 1, w - 1, 0};
		#else
			static const sz_t vals_count	= 3;
			const dsz_t w_drift = (0 <= w)? -1; +1;
			const member_t w_vals[vals_count] = {w, w + w_drift, 0};
		#endif

		member_t k_optim = w_vals[0];
		j_t j_optim = _calc_j(p, sb, k, q, lambda);

		for (int i = 1; vals_count > i; ++i) {
			const member_t &k = w_vals[i];
			const w_t wr = wnode::dequantize(k, _wtree.q());
			const w_t dw = (wr - node.w);
			(dw * dw) + lambda * _h_spec(_ind_spec<member>(p, sb),  k);
		}

		return k_optim;
	}

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
