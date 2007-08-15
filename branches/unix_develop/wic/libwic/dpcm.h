/*!	\file     dpcm.h
	\version  0.0.1
	\author   mice, ICQ: 332-292-380, mailto:wonder.mice@gmail.com
	\brief    Описания класса wic::dpcm - ДИКМ кодера

	\todo     Более подробно описать файл dpcm.h
*/

#pragma once

#ifndef WIC_LIBWIC_DPCM
#define WIC_LIBWIC_DPCM

////////////////////////////////////////////////////////////////////////////////
// include

// standard C++ library headers
#include <assert.h>
#include <vector>
#include <string>						// for debug output only
#include <fstream>						// for debug output only
#include <iomanip>						// for debug output only

// external library header
// none

// libwic headers
#include <wic/libwic/defines.h>


////////////////////////////////////////////////////////////////////////////////
// wic namespace
namespace wic {


////////////////////////////////////////////////////////////////////////////////
// encoder class declaration

//! \brief Простейщий ДИКМ кодер
/*! Класс одновременно может использоваться как енкодер, так и как декодер.

	Пример использования:
	\code
	\endcode
*/
template <class value_t>
class dpcm {
public:
	// public types ------------------------------------------------------------

	//!	\brief Тип кодируемых и декодируемых значений
	/*!	Данный тип должен поддерживать операции <i>+</i>, <i>+=</i>,
		<i>+=</i> и <i>-</i>
	*/
	typedef value_t value_type;

	// public constants --------------------------------------------------------

	//!	\brief Размер по умолчанию для хранимой истории енкодера / декодера
	static const int DEFAULT_HISTORY_SIZE	= 1;

	// public members ----------------------------------------------------------

	//!	\name Конструкторы и деструкторы
	//@{

	//!	\brief Конструктор
	/*!	В отладочном режиме (определён #LIBWIC_DEBUG) конструктор открывает
		файл "dumps/[dpcm]history.out" на запись для вывода отладочной
		информации.
	*/
	dpcm()
	{
		#ifdef LIBWIC_DEBUG
		_dbg_out_stream.open("dumps/[dpcm]history.out");
		#endif
	}

	//!	\brief Деструктор
	/*!	Ничего не делает
	*/
	~dpcm()
	{
		// ничего не надо делать
	}

	//@}

	//!	\name Функции кодирования
	//@{

	//!	\brief Начинает кодирование
	/*!	\param[in] history_sz Размер хранимой истории, которая будет
		использована для вычисления прогнозов
	*/
	void encode_start(const int history_sz = DEFAULT_HISTORY_SIZE)
	{
		_reset_history(_enc_history, history_sz);
	}

	//!	\brief Завершает кодирование
	/*!	Функция ничего не делает и существует только для симметрии с
		encode_start()
	*/
	void encode_stop()
	{
	}

	//!	\brief Кодирует значение с учётом прогноза
	/*!	\param[in] value Кодируемое значение
		\return Закодированное значение
	*/
	value_type encode(const value_type &value)
	{
		const value_type predicted	= _predict(_enc_history);
		const value_type encoded	= (value - predicted);

		_history_put(value, _enc_history);

		#ifdef LIBWIC_DEBUG
		_dbg_out_stream << "[encoder] " << std::setw(5) << value;
		_dbg_out_stream << " [" << std::setw(5) << predicted << "]"; 
		_dbg_out_stream << " >>>> " << std::setw(5) << encoded;
		_dbg_out_stream << std::endl;
		#endif

		return encoded;
	}

	//!	\brief Кодирует значение с учётом внешнего прогноза
	/*!	\param[in] value Кодируемое значение
		\param[in] predicted Прогнозное значение
		\return Закодированное значение
	*/
	value_type encode(const value_type &value, const value_type &predicted)
	{
		return (value - predicted);
	}

	//!	\name Функции декодирования
	//@{

	//!	\brief Начинает декодирование
	/*!	\param[in] history_sz Размер хранимой истории, которая будет
		использована для вычисления прогнозов
	*/
	void decode_start(const int history_sz = DEFAULT_HISTORY_SIZE)
	{
		_reset_history(_dec_history, history_sz);
	}

	//!	\brief Завершает декодирование
	/*!	Функция ничего не делает и существует только для симметрии с
		decode_start()
	*/
	void decode_stop()
	{
	}

	//!	\brief Декодирует значение с учётом прогноза
	/*!	\param[in] value Закодированное значение
		\return Декодированное значение
	*/
	value_type decode(const value_type &value)
	{
		const value_type predicted	= _predict(_dec_history);
		const value_type decoded	= (predicted + value);

		_history_put(decoded, _dec_history);

		#ifdef LIBWIC_DEBUG
		_dbg_out_stream << "[decoder] " << std::setw(5) << decoded;
		_dbg_out_stream << " [" << std::setw(5) << predicted << "]"; 
		_dbg_out_stream << " <<<< " << std::setw(5) << value;
		_dbg_out_stream << std::endl;
		#endif

		return decoded;
	}

	//!	\brief Декодирует значение с учётом внешнего прогноза
	/*!	\param[in] value Декодируемое значение
		\param[in] predicted Прогнозное значение
		\return Декодированное значение
	*/
	value_type decode(const value_type &value, const value_type &predicted)
	{
		return (predicted + value);
	}

	//@}

protected:
	// protected types ---------------------------------------------------------

	//!	\brief Тип для представления предыдущих закодированных или
	//!	декодированных значений
	typedef std::vector<value_type> _history_vals_t;

	//!	\brief История ДИКМ кодера
	struct _history_t {
		//!	\brief Массив предыдущих значений
		_history_vals_t vals;
		//!	\brief Позиция после последнего помещённого в историю значения
		int pos;
		//!	\brief Количество правильных значений перед позицией <i>pos</i>
		int count;
	};

	// protected constants -----------------------------------------------------

	// protected methods -------------------------------------------------------

	//!	\brief Производит сброс истории в начальное положение
	/*!	\param[in] history Сбрасываемая история
		\param[in] history_sz Размер истории значений
	*/
	void _reset_history(_history_t &history, const int history_sz)
	{
		history.vals.resize(history_sz);
		history.pos		= 0;
		history.count	= 0;
	}

	//!	\brief Подсчёт прогноза
	/*!	\param[in] history История, которая будет использована для построения
		прогноза
		\return Вычисленное значение прогноза
	*/
	value_type _predict(const _history_t &history)
	{
		// возвратить 0, если история пуста
		if (0 == history.count) return 0;

		// числитель прогноза
		value_type numerator	= 0;

		// знаменатель прогноза
		int denominator			= 0;

		// некоторое смещение
		const int offset = int(history.vals.size() + history.pos - 1);

		// цикл по значениям в истории
		for (int factor = history.count; 0 < factor; --factor)
		{
			const int j = int(offset % history.vals.size());

			numerator	+= (factor * history.vals[j]);

			denominator	+= factor;
		}

		return (numerator / denominator);
	}

	//! \brief Помещает значение в историю
	/*!	\param[in] val Значение, помещаемое в историю
		\param[in] history Модифицируемая история
	*/
	void _history_put(const value_type &val, _history_t &history)
	{
		// увеличить количество значений если ещё есть место
		if (history.count < int(history.vals.size())) ++history.count;

		// поместить значение в историю
		history.vals[history.pos] = val;

		// произвести смещение позиции
		if (++history.pos == history.vals.size()) history.pos = 0;
	}

private:
	// private members ---------------------------------------------------------

	//!	\brief История закодированных значений
	_history_t _enc_history;

	//!	\brief История декодированных значений
	_history_t _dec_history;

	#ifdef LIBWIC_DEBUG
	//!	\brief Стандартный файловый поток для вывода информации в
	//!	отладочном режиме
	std::ofstream _dbg_out_stream;
	#endif
};



}	// end of wic namespace



#endif	// WIC_LIBWIC_DPCM
