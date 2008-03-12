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
	/*!	Ничего не делает. Существует для формализации момента начала
		кодирования.
	*/
	void encode_start()
	{
	}

	//!	\brief Завершает кодирование
	/*!	Функция ничего не делает и существует только для симметрии с
		encode_start()
	*/
	void encode_stop()
	{
	}

	//!	\brief Кодирует значение с учётом внешнего прогноза
	/*!	\param[in] value Кодируемое значение
		\param[in] predicted Прогнозное значение
		\return Закодированное значение
	*/
	value_type encode(const value_type &value, const value_type &predicted)
	{
		const value_type encoded = (value - predicted);

		#ifdef LIBWIC_DEBUG
		_dbg_out_stream << "[encoder] " << std::setw(5) << value;
		_dbg_out_stream << " [" << std::setw(5) << predicted << "]"; 
		_dbg_out_stream << " >>>> " << std::setw(5) << encoded;
		_dbg_out_stream << std::endl;
		#endif

		return encoded;
	}

	//!	\name Функции декодирования
	//@{

	//!	\brief Начинает декодирование
	/*!	Ничего не делает. Существует для формализации момента начала
		декодирования.
	*/
	void decode_start()
	{
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
		const value_type decoded = (predicted + value);

		#ifdef LIBWIC_DEBUG
		_dbg_out_stream << "[decoder] " << std::setw(5) << decoded;
		_dbg_out_stream << " [" << std::setw(5) << predicted << "]"; 
		_dbg_out_stream << " <<<< " << std::setw(5) << value;
		_dbg_out_stream << std::endl;
		#endif

		return decoded;
	}

	//@}

private:
	// private members ---------------------------------------------------------

	#ifdef LIBWIC_DEBUG
	//!	\brief Стандартный файловый поток для вывода информации в
	//!	отладочном режиме
	std::ofstream _dbg_out_stream;
	#endif
};



}	// end of wic namespace



#endif	// WIC_LIBWIC_DPCM
