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

/*!	\param[in] buffer_sz Размер памяти (в байтах), выделяемой под внутренний
	буфер
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


/*!	\return Размер закодированных данных (в байтах)

	Возвращает <i>0</i>, если арифметический енкодер ещё не создан.

	\sa acoder::use()
*/
sz_t acoder::encoded_sz() const
{
	if (0 == _out_stream) return 0;

	return sz_t(_out_stream->PackedBytes());
}


/*!
*/
void acoder::use(const models_t &models)
{
	_mk_coders(models);
}


/*!
*/
void acoder::encode_start()
{
	// проверка утверждений
	assert(0 != _aencoder);
	assert(0 != _out_stream);

	// начало арифметического кодирования
	_aencoder->StartPacking();

	// инициализация моделей
	_init_models(_aencoder);

	//
	_dbg_encode_out = new std::ofstream("dumps/arcoder.encode");
}


/*!
*/
void acoder::encode_stop()
{
	// проверка утверждений
	assert(0 != _aencoder);
	assert(0 != _out_stream);

	// помещение EOF символа в поток
	_aencoder->PutEOF();

	// завершение кодирования
	_aencoder->EndPacking();

	//
	delete _dbg_encode_out;
}


/*!	\param[in] value Значение для кодирования
	\param[in] model_no Номер модели
*/
void acoder::put_value(const value_type &value, const sz_t model_no)
{
	// проверка утверждений
	assert(0 != _aencoder);
	assert(0 != _out_stream);

	assert(0 <= model_no && model_no < int(_models.size()));

	const model_t &model = _models[model_no];

	assert(model.min <= value && value <= model.max);

	// выбор модели и кодирование (учитывая смещение)
	_aencoder->model(model_no);

	(*_aencoder) << (value + model._delta);

	(*_dbg_encode_out) << "mdl: " << model_no << "; val: " << value << std::endl;
}


/*!
*/
void acoder::decode_start()
{
	// проверка утверждений
	assert(0 != _adecoder);
	assert(0 != _in_stream);

	// начало арифметического декодирования
	_adecoder->StartUnpacking();

	// инициализация моделей
	_init_models(_adecoder);

	//
	_dbg_decode_out = new std::ofstream("dumps/arcoder.decode");
}


/*!
*/
void acoder::decode_stop()
{
	// проверка утверждений
	assert(0 != _adecoder);
	assert(0 != _in_stream);

	// завершение декодирования
	_adecoder->EndUnpacking();

	//
	delete _dbg_decode_out;
}


/*!	\param[in] model_no Номер модели
*/
acoder::value_type acoder::get_value(const sz_t model_no)
{
	// проверка утверждений
	assert(0 != _adecoder);
	assert(0 != _in_stream);

	assert(0 <= model_no && model_no < int(_models.size()));

	const model_t &model = _models[model_no];

	// выбор модели и декодирование (учитывая смещение)
	_adecoder->model(model_no);

	value_type value;
	(*_adecoder) >> value;

	(*_dbg_decode_out) << "mdl: " << model_no << "; val: " << (value - model._delta) << std::endl;

	return (value - model._delta);
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

	\attention Функция должна вызываться после acoder::_refresh_models()
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
		result[k++] = i->_symbols;
	}

	return result;
}


/*!	\param[in,out] models_ptr Модели в формате для арифметического кодера
*/
void acoder::_rm_models(const int *const models_ptr)
{
	if (0 != models_ptr) delete[] models_ptr;
}


/*!	\param[in] models Модели арифметического кодера для обновления

	Функция выполняет следующие действия:
	- актуализирует поле model_t::_delta
	- актуализирует поле model_t::_symbols
*/
void acoder::_refresh_models(models_t &models)
{
	for (models_t::iterator i = models.begin(); models.end() != i; ++i)
	{
		model_t &model = (*i);
		model._delta = - model.min;
		model._symbols = _symbols_count(model);
	}
}


/*!	\param[in] coder_base Базовый класс для арифметических енкодера и
	декодера
*/
void acoder::_init_models(arcoder_base *const coder_base)
{
	// проверка утверждений
	assert(0 != coder_base);
	assert(coder_base->Models().Size() == _models.size());

	// сброс статистик
	coder_base->ResetStatistics();

	// инициализация моделей
	/*
	for (sz_t i = 0; coder_base->Models().Size() > i; i++)
	{
		const model_t &model = _models[i];

		coder_base->model(i);

		for (int j = 0; model._symbols > j; ++j)
		{
			coder_base->update_model(j);
		}
	}
	*/
}


/*!	\param[in] models Модели

	Функция создаёт енкодер и декодер.
*/
void acoder::_mk_coders(const models_t &models)
{
	// проверка утверждений
	assert(0 != _out_stream);
	assert(0 != _in_stream);

	// освобождение старых объектов
	_rm_coders();

	// сохранение настроек моделей
	_models = models;

	// обновление моделей
	_refresh_models(_models);

	// модели для аркодера
	const int *const models_ptr = _mk_models(_models);

	// создание новых объектов енкодера и декодера
	_aencoder = new ArCoderWithBitCounter<BITOutMemStream>(
						int(_models.size()), models_ptr, *_out_stream);

	_adecoder = new ArDecoderWithBitCounter<BITInMemStream>(
						int(_models.size()), models_ptr, *_in_stream);

	// освобождение моделей для аркодера
	_rm_models(models_ptr);

	// проверка полученных значений
	if (0 == _aencoder || 0 == _adecoder) throw std::bad_alloc();
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


/*! \param[in] coder_base Базовый класс для арифметических енкодера и
	декодера
	\param[in] value Значение, символ из алфавита модели
	\param[in] model_no Номер используемой модели
	\return Битовые затраты на кодирование символа
*/
double acoder::_entropy_eval(arcoder_base *const coder_base,
							 const value_type &value, const sz_t model_no)
{
	// проверка утверждений
	assert(0 != coder_base);
	assert(coder_base->Models().Size() == _models.size());
	assert(0 <= model_no && model_no < int(_models.size()));

	const model_t &model = _models[model_no];

	assert(model.min <= value && value <= model.max);

	// выбор модели и подсчёт битовых затрат (учитывая смещение)
	coder_base->model(model_no);

	return (coder_base->entropy_eval(value + model._delta));
}



}	// namespace wic
