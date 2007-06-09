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

//! \brief Арифметический кодер (енкодер и декодер)
/*!	Класс представляет собой удобную оболочку для арифметического кодера.
	Имеет следующие особенности:
	- Объединяет в себе возможности енкодера и декодера.
	- Имеет внутренний буфер для кодирования и декодирования, что позволяет
	  упростить работу с памятью.
	- Имеет набор функций для работы с моделями.

	Пример использования:
	\code
	wic::acoder acoder(1024);

	wic::acoder::models_t models;
	// populate models vector
	acoder.use(models);

	acoder.encode_start();
	acoder.put(1, 0);
	acoder.put(2, 1);
	acoder.put(3, 0);
	acoder.encode_stop();

	acoder.decode_start();
	std::cout << acoder.get_value(0);
	std::cout << acoder.get_value(1);
	std::cout << acoder.get_value(0);
	acoder.decode_stop();
	\endcode

	\todo Необходимо протестировать этот класс
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
		//!	\brief Смещение, необходимое для корректного кодирования
		//! символов из алфавита этой модели.
		/*!	\note Это поле заполняется <b>автоматически</b> при инициализации
			аркодера.
		*/
		value_type _delta;
		//!	\brief Количество символов в алфавите модели
		value_type _symbols;
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

	//!	\brief Указатель на внутренний буфер
	/*!	\return Указатель на внутренний буфер
	*/
	byte_t *buffer() const { return _buffer; }

	//!	\brief Размер внутреннего буфера
	/*!	\return Размер внутреннего буфера (в байтах)
	*/
	const sz_t buffer_sz() const { return _buffer_sz; }

	//! \brief Размер 
	// const sz_t encoded_sz() const;

	//@}

	//!	\name Работа с моделями
	//@{

	//!	\brief Устанавливает модели, используемые арифметическим кодером
	void use(const models_t &models);

	//@}

	//!	\name Функции енкодера
	//@{

	//! \brief Начинает арифметическое кодирование
	void encode_start();

	//! \brief Завершает арифметическое кодирование
	void encode_stop();

	//!	\brief Кодирует значение и помещает его в битовый поток
	void put_value(const value_type &value, const sz_t model_no);

	//!	\brief Кодирует значение и помещает его в битовый поток
	/*!	\param[in] param Значение для кодирования
		\param[in] model_no Номер модели

		Функция отличается от acoder::put_value() тем, что сама может
		производить допустимые преобразования типов.
	*/
	template <class param_t>
	void put(const param_t &param, const sz_t model_no)
	{
		assert(sizeof(param_t) <= sizeof(value_type));

		put_value(reinterpret_cast<value_type>(param), model_no);
	}

	//@}

	//!	\name Функции декодера
	//@{

	//! \brief Начинает арифметическое декодирование
	void decode_start();

	//! \brief Завершает арифметическое декодирование
	void decode_stop();

	//!	\brief Выбирает очередной символ из битового потока и
	//!	декодирует его
	value_type get_value(const sz_t model_no);

	//!	\brief Выбирает очередной символ из битового потока и
	//!	декодирует его
	/*!	\param[in] model_no Номер модели

		Функция отличается от acoder::get_value() тем, что сама может
		производить допустимые преобразования типов.
	*/
	template <class result_t>
	result_t get(const sz_t model_no)
	{
		return reinterpret_cast<result_t>(get_value(model_no));
	}

	//@}

protected:
	// protected methods -------------------------------------------------------

	//!	\name Создание моделей
	//@{

	//!	\brief Получает количество символов в алфавите по минимальному и
	//!	максимальному значениям символов в моделе
	static value_type _symbols_count(const model_t &model);

	//!	\brief Преобразует вектор моделей в описание моделей для
	//!	арифметического кодера
	static int *_mk_models(const models_t &models);

	//!	\brief Освобождает описание моделей для арифметического кодера
	static void _rm_models(const int *const models_ptr);

	//!	\brief Обновляет внутренние модели, заполняя необходимые поля
	void _refresh_models(models_t &models);

	//@}

	//!	\name Создание и удаление кодеров
	//@{

	//!	\brief Производит инициализацию моделей арифметического кодера
	void _init_models(arcoder_base *const coder_base);

	//!	\brief Создаёт арифметические енкодер и декодер
	void _mk_coders(const models_t &models);

	//!	\brief Удаляет арифметические енкодер и декодер
	void _rm_coders();

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
