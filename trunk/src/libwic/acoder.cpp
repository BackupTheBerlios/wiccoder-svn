/*!	\file     acoder.cpp
	\version  0.0.1
	\author   mice, ICQ: 332-292-380, mailto:wonder.mice@gmail.com
	\brief    Реализация класса wic::acoder - арифметического кодера

	\todo     Более подробно описать файл acoder.cpp
*/


////////////////////////////////////////////////////////////////////////////////
// headers

// standard C++ library headers
// none

// libwic headers
#include <wic/libwic/acoder.h>


////////////////////////////////////////////////////////////////////////////////
// wic namespace
namespace wic {


////////////////////////////////////////////////////////////////////////////////
// acoder class public definitions

/*!	\param[in] Размер памяти (в байтах), выделяемой под внутренний буфер
*/
acoder::acoder(const sz_t buffer_sz):
	_buffer_sz(buffer_sz), _buffer(0),
	_out_stream(0), _aencoder(0),
	_in_stream(0), _adecoder(0)
{
	assert(0 < _buffer_sz);

	// выделение памяти под внутренний буфер
	_buffer = new byte_t[_buffer_sz];

	if (0 == _buffer) throw std::bad_alloc();

	// создание битовых потоков для арифметического кодера
	_out_stream	= new BITOutMemStream((char *)_buffer, _buffer_sz);
	_in_stream	= new BITInMemStream((char *)_buffer, _buffer_sz);

	if (0 == _out_stream || 0 == _in_stream) throw std::bad_alloc();
}


/*!	Освобождает память занимаемую внтуренним буфером
*/
acoder::~acoder()
{
	// освобождаем кодеры
	_rm_coders();

	// освобождаем потоки
	if (0 != _out_stream) delete _out_stream;
	if (0 != _out_stream) delete _in_stream;

	// освобождаем буфер
	if (0 != _buffer) delete[] _buffer;
}


////////////////////////////////////////////////////////////////////////////////
// acoder class protected definitions

/*!	\param[in] model Модель
	\return Количество символов в алфавите
*/
acoder::value_type acoder::_symbols_count(const model_t &model)
{
	assert(model.min <= model.max);

	return (model.max - model.min + 1);
}


/*!	\param[in] models Модели
	\return Модели в формате для арифметического кодера
*/
int *acoder::_mk_models(const models_t &models)
{
	// проверка наличия моделей
	assert(0 < models.size());

	// выделение памяти под модели
	int *const result = new int[models.size()];

	if (0 == result) throw std::bad_alloc();

	// копирование моделей из вектора
	int k = 0;

	for (models_t::const_iterator i = models.begin(); models.end() != i; ++i)
	{
		result[k++] = _symbols_count(*i);
	}

	return result;
}


/*!	\param[in,out] models_ptr Модели в формате для арифметического кодера
*/
void acoder::_rm_models(const int *const models_ptr)
{
	if (0 != models_ptr) delete[] models_ptr;
}


/*!
*/
void acoder::_rm_coders()
{
	if (0 != _aencoder) {
		delete _aencoder;
		_aencoder = 0;
	}

	if (0 != _adecoder) {
		delete _adecoder;
		_adecoder = 0;
	}
}


/*!	\param[in] models Модели
*/
void acoder::_mk_coders(const models_t &models)
{
	// освобождение старых объектов
	_rm_coders();

	// сохранение настроек моделей
	_models = models;

	const int *const models_ptr = _mk_models(_models);

	// создание новых объектов
	_aencoder = new ArCoderWithBitCounter<BITOutMemStream>(
						int(_models.size()), models_ptr, *_out_stream);

	_adecoder = new ArDecoderWithBitCounter<BITInMemStream>(
						int(_models.size()), models_ptr, *_in_stream);

	_rm_models(models_ptr);

	// проверка полученных значений
	if (0 == _aencoder || 0 == _adecoder) throw std::bad_alloc();
}



}	// namespace wic
