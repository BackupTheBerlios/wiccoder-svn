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

// code configuration headers
#include <wic/libwic/defines.h>

// standard C++ library headers
#include <new>							// for std::bad_alloc exception class
#include <vector>
#include <string>
#include <assert.h>
#include <fstream>						// for debug output only
#include <iomanip>						// for debug output only

// external library headers
#include <arcoder/arcodec.h>
#include <arcoder/arbitstream.h>

// libwic headers
#include <wic/libwic/types.h>


////////////////////////////////////////////////////////////////////////////////
// defines


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
		/*!	\note Это поле заполняется <b>автоматически</b> при инициализации
			аркодера.
		*/
		value_type _symbols;
		//!	\brief Минимальное значение символа, который реально был
		//!	закодирован или декодирован кодером
		/*!	Значения _rmin и _rmax используются в паре, и являются корректными
			только если выполняется неравенство <i>(_rmin <= _rmax)</i>

			\note Это поле заполняется <b>автоматически</b> при инициализации
			аркодера.
		*/
		value_type _rmin;
		//!	\brief Максимальное значение символа, который реально был
		//!	закодирован или декодирован кодером
		/*!	\note Это поле заполняется <b>автоматически</b> при инициализации
			аркодера.
		*/
		value_type _rmax;

		#ifdef LIBWIC_DEBUG
		//!	\name Поля для отладки
		//@{

		//!	\brief Количество символов, закодированных с использованием этой
		//!	модели
		/*!	\note Счётчик обнуляется при каждом вызове acoder::encode_start()
		*/
		unsigned int _encoded_symbols;

		//!	\brief Количество символов, декодированных в этой модели
		/*!	\note Счётчик обнуляется при каждом вызове acoder::decode_start()
		*/
		unsigned int _decoded_symbols;

		//!	\brief Частоты появления кодируемых символов
		/*!	Вектор содержит количество появлений всех символов из алфавита
			\note Обнуляется при каждом вызове acoder::encode_start()
		*/
		std::vector<unsigned int> _encoded_freqs;

		//!	\brief Частоты появления декодируемых символов
		/*!	Вектор содержит количество появлений всех символов из алфавита
			\note Обнуляется при каждом вызове acoder::decode_start()
		*/
		std::vector<unsigned int> _decoded_freqs;

		//@}
		#endif
	};

	//!	\brief Описание моделей, используемых арифметическим кодером
	typedef std::vector<model_t> models_t;

	// public constants --------------------------------------------------------

	//!	\brief Константы соответствующие различным группам моделей
	//!	арифметического кодера
	/*!	Данные константы могут быть использованы, например, в функциях загрузки
		и сохранения моделей арифметического кодера.

		\note Сейчас не используется
	*/
	enum models_e
	{
		//!	\brief Модели арифметического енкодера
		encoder_models	= 0x01,
		//!	\brief Модели арифметического декодера
		decoder_models	= 0x02
	};

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

	//! \brief Размер закодированных данных
	sz_t encoded_sz() const;

	//@}

	//!	\name Работа с моделями
	//@{

	//!	\brief Устанавливает модели, используемые арифметическим кодером
	void use(const models_t &models);

	//!	\brief Модели, используемые кодером (енкодером и декодером)
	/*!	\return Ссылка на вектор описаний моделей арифметического кодера
	*/
	const models_t &models() const { return _models; }

	//!	\brief Возвращает минимальное значение символа закодированного или
	//!	декодированного кодером
	const value_type &rmin(const sz_t &model_no) const;

	//!	\brief Возвращает максимальное значение символа закодированного или
	//!	декодированного кодером
	const value_type &rmax(const sz_t &model_no) const;

	//@}

	//!	\name Функции енкодера
	//@{

	//! \brief Начинает арифметическое кодирование
	void encode_start();

	//! \brief Завершает арифметическое кодирование
	void encode_stop();

	//!	\brief Кодирует значение и помещает его в битовый поток
	void put_value(const value_type &value, const sz_t model_no,
				   const bool virtual_encode = false);

	//!	\brief Кодирует значение и помещает его в битовый поток
	/*!	\param[in] param Значение для кодирования
		\param[in] model_no Номер модели
		\param[in] virtual_encode Если <i>true</i>, то будет производиться
		виртуальное кодирование (имитация кодирования с перенастройкой
		моделей без реального помещения символов в выходной поток).
	*/
	template <class param_t>
	void put(const param_t &param, const sz_t model_no,
			 const bool virtual_encode = false)
	{
		assert(sizeof(param_t) <= sizeof(value_type));

		put_value(static_cast<const value_type &>(param), model_no,
				  virtual_encode);
	}

	//!	\brief Производит подсчёт битовых затрат, необходимых для кодирования
	//!	символа
	/*!	\param[in] param Символ из алфавита модели
		\param[in] model_no Номер используемой модели
		\return Битовые затраты на кодирование символа
	*/
	template <class param_t>
	double enc_entropy(const param_t &param, const sz_t model_no)
	{
		assert(sizeof(param_t) <= sizeof(value_type));

		return _entropy_eval(_aencoder,
							 static_cast<const value_type &>(param),
							 model_no);
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
		return static_cast<result_t>(get_value(model_no));
	}

	//@}

	//!	\name Отладочные функции
	//@{

	//!	\brief Выводит отладочную информацию о енкодере
	void dbg_encoder_info(std::ostream &out);

	//!	\brief Выводит отладочную информацию о енкодере
	void dbg_encoder_info(const std::string &file);

	//!	\brief Выводит отладочную информацию о декодере
	void dbg_decoder_info(std::ostream &out);

	//!	\brief Выводит отладочную информацию о декодере
	void dbg_decoder_info(const std::string &file);

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
	static void _refresh_models(models_t &models);

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

	//!	\name Статистические функции
	//@{

	//!	\brief Подсчёт битовых затрат на кодирование символа
	double _entropy_eval(arcoder_base *const coder_base,
						 const value_type &value, const sz_t model_no);

	//@}

	//!	\name Отладочные функции
	//@{

	#ifdef LIBWIC_DEBUG
	//!	\brief Выводит информацию о накопленной статистике
	static void _dbg_freqs_out(std::ostream &out,
							   const std::vector<unsigned int> &freqs,
							   const model_t &model, const sz_t model_no,
							   arcoder_base *const coder_base = 0);
	#endif

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

	#ifdef LIBWIC_DEBUG
	//!	\brief Стандартный файловый поток для вывода информации в
	//!	отладочном режиме
	std::ofstream _dbg_out_stream;
	#endif
};



}	// end of namespace wic



#endif	// WIC_LIBWIC_ACODER
