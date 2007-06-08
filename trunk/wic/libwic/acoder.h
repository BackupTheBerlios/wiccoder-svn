/*!	\file     acoder.h
	\version  0.0.1
	\author   mice, ICQ: 332-292-380, mailto:wonder.mice@gmail.com
	\brief    Описание класса wic::acoder - арифметического кодера

	\todo     Более подробно описать файл acoder.h
*/

#pragma once

#ifndef WIC_LIBWIC_ACODER
#define WIC_LIBWIC_ACODER

////////////////////////////////////////////////////////////////////////////////
// include

// standard C++ library headers
#include <new>							// for std::bad_alloc exception class
#include <vector>
#include <assert.h>

// external library headers
#include <arcoder/arcodec.h>
#include <arcoder/arbitstream.h>

// libwic headers
#include <wic/libwic/types.h>


////////////////////////////////////////////////////////////////////////////////
// wic namespace
namespace wic {


////////////////////////////////////////////////////////////////////////////////
// types


////////////////////////////////////////////////////////////////////////////////
// acoder class declaration

//! \brief Арифметический кодер (енкодер + декодер)
/*!	
*/
class acoder {
public:
	// public types ------------------------------------------------------------

	//!	\brief Тип, которым будут оперировать арифметические енкодер и декодер
	/*!	\note Очень важно, чтобы это был тип со знаком (<i>signed</i>). Это
		делает код проще, универсальнее и облегчает отладку.
	*/
	typedef int value_type;

	//!	\brief Тип описывает модель арифметического кодера
	struct model_t {
		//!	\brief Минимальное значение символа
		value_type min;
		//!	\brief Максимальное значение символа
		value_type max;
	};

	//!	\brief Описание моделей, используемых арифметическим кодером
	typedef std::vector<model_t> models_t;

	// public constants --------------------------------------------------------

	// public methods ----------------------------------------------------------

	//!	\name Конструкторы и деструкторы
	//@{

	//!	\brief Конструкртор
	acoder(const sz_t buffer_sz);

	//!	\brief Деструктор
	~acoder();

	//@}

	//!	\name Работа с внутренним буфером
	//@{

	byte_t *get_buffer() const;
	const sz_t buffer_sz() const {}

	//@}

	//!	\name Инициализация
	//@{

	void enc_begin();
	void enc_end();

	void dec_begin();
	void dec_end();

	//@}

	//!	\name Кодирование декодирование
	//@{

	template<class result_t>
	result_t get(const sz_t model)
	{
		_adecoder->model(model);

		value_type value;
		(*_adecoder) >> value;

		return reinterpret_cast<result_t>(value);
	}

	template<class param_t>
	void put(const param_t &val, const sz_t model)
	{
		_aencoder->model(model);

		(*_aencoder) << reinterpret_cast<param_t>(val);
	}

	//@}

protected:
	// protected methods -------------------------------------------------------

	//!	\name Создание моделей для арифметического кодера
	//@{

	//!	\brief Получает количество символов в алфавите по минимальному и
	//!	максимальному значениям символов в моделе
	static value_type _symbols_count(const model_t &model);

	//!	\brief Преобразует вектор моделей в описание моделей для
	//!	арифметического кодера
	static int *_mk_models(const models_t &models);

	//!	\brief Освобождает описание моделей для арифметического кодера
	static void _rm_models(const int *const models_ptr);

	//@}

	//!	\name Создание и удаление кодеров
	//@{

	//!	\brief Удаляет арифметические енкодер и декодер
	void _rm_coders();

	//!	\brief Создаёт арифметические енкодер и декодер
	void _mk_coders(const models_t &models);

	//@}

private:
	// private data ------------------------------------------------------------

	//!	\brief Общий внутренний буфер, который будет использоваться
	//!	арифметическим енкодером и декодером
	byte_t *_buffer;

	//!	\brief Размер внутреннего буфера <i>_buffer</i> в байтах
	const sz_t _buffer_sz;

	//!	\brief Модели используемые арифметическим кодером
	models_t _models;

	//!	\brief Битовый поток для енкодера
	BITOutMemStream *_out_stream;

	//!	\brief Арифметический енкодер
    ArCoderWithBitCounter<BITOutMemStream> *_aencoder;

	//!	\brief Битовый поток для декодера
	BITInMemStream *_in_stream;

	//!	\brief Арифметический декодер
	ArDecoderWithBitCounter<BITInMemStream> *_adecoder;
};



}	// end of namespace wic



#endif	// WIC_LIBWIC_ACODER
