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
class dpcm {
public:
	// public members ----------------------------------------------------------

	//!	\name Функции кодирования
	//@{

	//!	\brief Кодирует значение с учётом внешнего прогноза
	/*!	\param[in] value Кодируемое значение
		\param[in] predicted Прогнозное значение
		\return Закодированное значение

		Тип value_t представляет собой тип кодируемых и декодируемых значений.
		Данный тип должен поддерживать операции <i>+</i>, <i>+=</i>, <i>+=</i>
		и <i>-</i>.
	*/
	template <class value_t>
	static value_t encode(const value_t &value, const value_t &predicted)
	{
		const value_t encoded = (value - predicted);

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

	//!	\brief Декодирует значение с учётом внешнего прогноза
	/*!	\param[in] value Декодируемое значение
		\param[in] predicted Прогнозное значение
		\return Декодированное значение

		Тип value_t представляет собой тип кодируемых и декодируемых значений.
		Данный тип должен поддерживать операции <i>+</i>, <i>+=</i>, <i>+=</i>
		и <i>-</i>.
	*/
	template <class value_t>
	static value_t decode(const value_t &value, const value_t &predicted)
	{
		const value_t decoded = (predicted + value);

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
	static std::ofstream _dbg_out_stream;
	#endif
};



}	// end of wic namespace



#endif	// WIC_LIBWIC_DPCM
