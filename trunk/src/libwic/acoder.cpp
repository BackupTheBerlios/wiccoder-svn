/*!	\file     acoder.cpp
	\version  0.0.1
	\author   mice, ICQ: 332-292-380, mailto:wonder.mice@gmail.com
	\brief    Реализация класса wic::acoder - арифметического кодера

	\todo     Более подробно описать файл acoder.cpp
*/


////////////////////////////////////////////////////////////////////////////////
// headers

// standard C++ library headers
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

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
	_init_mode(ACODER_INIT_MODELS_MODE),
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

	#ifdef LIBWIC_DEBUG
	_dbg_out_stream.open("dumps/[acoder]models.out");
	#endif

	#ifdef LIBWIC_ACODER_LOG_VALUES
	_dbg_vals_stream.open("dumps/[acoder]values.out",
						  std::ios_base::app | std::ios_base::out);
	#endif
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


/*!	\param[in] models Описание моделей арифметического кодера
*/
void acoder::use(const models_t &models)
{
	_mk_coders(models);
}


/*!	\param[in] model_no Номер модели
	\return Минимальное значение символа, закодированного в указанной
	модели
*/
const acoder::value_type &acoder::rmin(const sz_t &model_no) const
{
	assert(0 <= model_no && model_no < sz_t(_models.size()));

	return _models[model_no]._rmin;
}


/*!	\param[in] model_no Номер модели
	\return Максимальное значение символа, закодированного в указанной
	модели
*/
const acoder::value_type &acoder::rmax(const sz_t &model_no) const
{
	assert(0 <= model_no && model_no < sz_t(_models.size()));

	return _models[model_no]._rmax;
}


/*!	\param[in] model_no Номер модели
	\return Математическое ожидание модулия кодируемой (декодируемой) величины
*/
const double &acoder::abs_average(const sz_t &model_no) const
{
	assert(0 <= model_no && model_no < sz_t(_models.size()));

	return _models[model_no]._abs_average;
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

	#ifdef LIBWIC_DEBUG
	for (models_t::iterator i = _models.begin(); _models.end() != i; ++i)
	{
		model_t &model = (*i);

		model._encoded_symbols = 0;
		model._encoded_freqs.assign(model._symbols, 0);
	}
	#endif

	#ifdef LIBWIC_ACODER_LOG_VALUES
	if (_dbg_vals_stream.good())
	{
		_dbg_vals_stream << "Start Encoding:" << std::endl;
	}
	#endif
}


/*!
*/
void acoder::encode_stop()
{
	// проверка утверждений
	assert(0 != _aencoder);
	assert(0 != _out_stream);

	// помещение EOF символа в поток
	// _aencoder->PutEOF();

	// завершение кодирования
	_aencoder->EndPacking();

	#ifdef LIBWIC_DEBUG
	if (_dbg_out_stream.good()) dbg_encoder_info(_dbg_out_stream);
	#endif

	#ifdef LIBWIC_ACODER_LOG_VALUES
	if (_dbg_vals_stream.good())
	{
		_dbg_vals_stream << "Stop Encoding." << std::endl << std::endl;
	}
	#endif

	#ifdef LIBWIC_ACODER_PRINT_STATISTIC
	for (sz_t i = 0; sz_t(_models.size()) > i; ++i)
	{
		const model_t &model = _models[i];

		std::cout << std::endl
			  << "acoder::encd::model::" << i << "::" << model._symbols
			  << "::" << model._delta << "::" << model.abs_avg
			  << std::endl;

		const std::vector<unsigned int> &freqs = model._encoded_freqs;

		for (sz_t j = 0; sz_t(freqs.size()) > j; ++j)
		{
			std::cout << "acoder::encd::value::" << i << "::" << j
					  << "::" << freqs[j] << std::endl;
		}
	}
	#endif
}


/*!	\param[in] value Значение для кодирования
	\param[in] model_no Номер модели
	\param[in] virtual_encode Если <i>true</i>, то будет производиться
	виртуальное кодирование (имитация кодирования с перенастройкой
	моделей без реального помещения символов в выходной поток).

	\note В отладочном режиме функция изменяет значения полей 
	model_t::_encoded_symbols и model_t::_encoded_freqs для выбранной
	модели, даже при виртуальном кодировании, несмотря на то, что
	реального кодирования не производится.
*/
void acoder::put_value(const value_type &value, const sz_t model_no,
					   const bool virtual_encode)
{
	// проверка утверждений
	assert(0 != _aencoder);
	assert(0 != _out_stream);

	assert(0 <= model_no && model_no < sz_t(_models.size()));

	model_t &model = _models[model_no];

	assert(model.min <= value && value <= model.max);

	const value_type enc_val = (value + model._delta);

	assert(0 <= enc_val && enc_val < model._symbols);

	#ifdef LIBWIC_DEBUG
	++(model._encoded_symbols);
	++(model._encoded_freqs[enc_val]);
	#endif

	// обновление максимального и минимального значения закодированного
	// символа
	if (value < model._rmin) model._rmin = value;
	if (value > model._rmax) model._rmax = value;

	// обновление среднего арифметического значения кодируемых символов
	++model._n;
	model._abs_average = (model._abs_average * (model._n - 1) + abs(value)) /
							model._n;

	// выбор модели и кодирование (учитывая смещение)
	_aencoder->model(model_no);

	// при виртуальном кодировании не производится реальная запись
	// в выходной поток
	if (!virtual_encode) _aencoder->put(enc_val);

	// обновление модели арифметического кодера
	_aencoder->update_model(enc_val);

	#ifdef LIBWIC_ACODER_LOG_VALUES
	if (_dbg_vals_stream.good())
	{
		_dbg_vals_stream << "put: " << value << " as " << enc_val
						 << " in #" << model_no << std::endl;
	}
	#endif
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

	#ifdef LIBWIC_DEBUG
	for (models_t::iterator i = _models.begin(); _models.end() != i; ++i)
	{
		model_t &model = (*i);

		model._decoded_symbols = 0;
		model._decoded_freqs.assign(model._symbols, 0);
	}
	#endif

	#ifdef LIBWIC_ACODER_LOG_VALUES
	if (_dbg_vals_stream.good())
	{
		_dbg_vals_stream << "Start Decoding:" << std::endl;
	}
	#endif
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

	#ifdef LIBWIC_DEBUG
	if (_dbg_out_stream.good()) dbg_decoder_info(_dbg_out_stream);
	#endif

	#ifdef LIBWIC_ACODER_LOG_VALUES
	if (_dbg_vals_stream.good())
	{
		_dbg_vals_stream << "Stop Decoding." << std::endl << std::endl;
	}
	#endif
}


/*!	\param[in] model_no Номер модели
*/
acoder::value_type acoder::get_value(const sz_t model_no)
{
	// проверка утверждений
	assert(0 != _adecoder);
	assert(0 != _in_stream);

	assert(0 <= model_no && model_no < int(_models.size()));

	model_t &model = _models[model_no];

	// выбор модели и декодирование (учитывая смещение)
	_adecoder->model(model_no);

	value_type dec_val;
	(*_adecoder) >> dec_val;

	#ifdef LIBWIC_DEBUG
	++(model._decoded_symbols);
	++(model._decoded_freqs[dec_val]);
	#endif

	dec_val -= model._delta;

	// обновление максимального и минимального значения декодированного
	// символа
	if (dec_val < model._rmin) model._rmin = dec_val;
	if (dec_val > model._rmax) model._rmax = dec_val;

	// обновление среднего арифметического значения кодируемых символов
	++model._n;
	model._abs_average = (model._abs_average * (model._n - 1) + abs(dec_val)) /
							model._n;

	#ifdef LIBWIC_ACODER_LOG_VALUES
	if (_dbg_vals_stream.good())
	{
		_dbg_vals_stream << "get: " << dec_val << " as "
						 << (dec_val + model._delta)
						 << " from #" << model_no << std::endl;
	}
	#endif

	return dec_val;
}


/*!	\param[out] out Стандартный поток для вывода
*/
void acoder::dbg_encoder_info(std::ostream &out)
{
	assert(0 != _aencoder);

	out << "[encoder models information] ----------------------------";
	out << std::endl;

	#ifdef LIBWIC_DEBUG
	unsigned int total_symbols = 0;
	#endif

	for (sz_t i = 0; sz_t(_models.size()) > i; ++i)
	{
		out << "#" << std::setw(2) << i;

		_aencoder->model(i);
		const model_t &model = _models[i];

		#ifdef LIBWIC_DEBUG
		out << "[" << std::setw(5) << model._symbols << "]";
		#endif

		const unsigned int bits = _aencoder->model()->bit_counter;

		out << ": ";
		out << std::setw(8) << bits << " bits |";
		out << std::setw(7) << (bits / 8) << " bytes";

		#ifdef LIBWIC_DEBUG
		const unsigned int encoded_symbols = model._encoded_symbols;
		total_symbols += encoded_symbols;

		out << " |" << std::setw(7) << encoded_symbols  << " symbols";
		#endif

		out << " | M = " << std::setw(7) << model._abs_average;

		out << std::endl;

		#ifdef LIBWIC_DEBUG
		_dbg_freqs_out(out, model._encoded_freqs, model, i, _aencoder);
		#endif
	}

	#ifdef LIBWIC_DEBUG
	out << "Total symbols: " << std::setw(6) << total_symbols << std::endl;
	#endif

	out << "[/encoder models information] ---------------------------";
	out << std::endl;
}


/*!	\param[in] file Имя файла в который будет производиться вывод
*/
void acoder::dbg_encoder_info(const std::string &file)
{
	std::ofstream out(file.c_str());

	if (out.good()) dbg_encoder_info(out);
}


/*!	\param[out] out Стандартный поток для вывода
*/
void acoder::dbg_decoder_info(std::ostream &out)
{
	assert(0 != _adecoder);

	out << "[decoder models information] ----------------------------";
	out << std::endl;

	#ifdef LIBWIC_DEBUG
	unsigned int total_symbols = 0;
	#endif

	for (sz_t i = 0; sz_t(_models.size()) > i; ++i)
	{
		out << "#" << std::setw(2) << i;

		_adecoder->model(i);
		const model_t &model = _models[i];

		#ifdef LIBWIC_DEBUG
		out << "[" << std::setw(5) << model._symbols << "]";
		#endif

		const unsigned int bits = _adecoder->model()->bit_counter;

		out << ": ";
		out << std::setw(8) << bits << " bits |";
		out << std::setw(7) << (bits / 8) << " bytes";

		#ifdef LIBWIC_DEBUG
		const unsigned int decoded_symbols = model._decoded_symbols;
		total_symbols += decoded_symbols;

		out << " |" << std::setw(7) << decoded_symbols  << " symbols";
		#endif

		out << " | M = " << std::setw(7) << model._abs_average;

		out << std::endl;

		#ifdef LIBWIC_DEBUG
		_dbg_freqs_out(out, model._decoded_freqs, model, i, _adecoder);
		#endif
	}

	#ifdef LIBWIC_DEBUG
	out << "Total symbols: " << std::setw(6) << total_symbols << std::endl;
	#endif

	out << "[/decoder models information] ---------------------------";
	out << std::endl;
}


/*!	\param[in] file Имя файла в который будет производиться вывод
*/
void acoder::dbg_decoder_info(const std::string &file)
{
	std::ofstream out(file.c_str());

	if (out.good()) dbg_decoder_info(out);
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
	- актуализирует поле model_t::_rmin
	- актуализирует поле model_t::_rmax
*/
void acoder::_refresh_models(models_t &models)
{
	for (models_t::iterator i = models.begin(); models.end() != i; ++i)
	{
		model_t &model		= (*i);
		model._delta		= - model.min;
		model._symbols		= _symbols_count(model);

		model._rmin			= model.max;
		model._rmax			= model.min;
		model._n			= 0;
		model._abs_average	= 0;
	}
}


/*!	\param[in] factor Масштабный коэффициент, на который будет умножена
	получаемая вероятность
	\param[in] sigma Среднеквадратичное отклонение (параметр нормального
	распределения)
	\param[in,out] coder_base Указатель на объект используемого
	арифметического кодера
	\param[in] i Номер используемой модели
	\param[in] purge Если <i>true</i> инициализация модели производится не
	будет

	Производит инициализацию текущей модели арифметического кодера
	нормальным распределением.
*/
void acoder::_init_normal_distribution(const float &factor, const float &sigma,
									   arcoder_base *const coder_base,
									   const sz_t &i, const bool purge)
{
	// Проверим входные параметры
	assert(0 != coder_base);
	assert(0 < sigma);
	assert(0 != factor);

	// Установка текущей модели
	coder_base->model(i);

	// Подсчёт часто используемых значений
	const float sigmaScrt2Pi	= float(sigma * sqrt(2*M_PI));
	const float sigma2x2		= float(2* sigma * sigma);

	// Ссылка на описание используемой модели
	const model_t &model = _models[i];

	#ifdef LIBWIC_DEBUG
	std::vector<unsigned int> freqs(model._symbols);
	#endif

	#ifdef LIBWIC_ACODER_PRINT_STATISTIC
	std::cout << std::endl
			  << "acoder::init::model::" << i << "::" << model._symbols
			  << "::" << model._delta  << "::" << model.abs_avg
			  << std::endl;
	#endif

	// Вычисление вероятности для каждого символа в модели
	for (value_type x = model.min; model.max >= x; ++x)
	{
		// Вероятность умноженая на масштабный коэффициент
		const float p = factor*exp(-x*x / sigma2x2) / sigmaScrt2Pi;

		// Символ из модели аркодера (не отритцательный)
		const value_type smb = x + model._delta;

		// Проверка не отритцательности симвлоа
		assert(0 <= smb);

		// Целочисленное значение полученной вероятности
		const unsigned int ip = (unsigned int)floor(p + 0.5);

		#ifdef LIBWIC_DEBUG
		freqs[smb] = ip;
		#endif

		#ifdef LIBWIC_ACODER_PRINT_STATISTIC
		std::cout << "acoder::init::value::" << i << "::" << smb
				  << "::" << ip << std::endl;
		#endif

		if (purge) continue;

		// Инициализация модели аркодера
		for (unsigned int k = ip; 0 < k; --k) coder_base->update_model(smb);
	}

	if (!purge) return;

	#ifdef LIBWIC_DEBUG
	_dbg_out_stream << "#" << std::setw(2) << i;

	_dbg_out_stream << "[" << std::setw(5) << model._symbols << "]";

	_dbg_out_stream << " | abs_avg = " << std::setw(7) << model._abs_average;
	_dbg_out_stream << " | sigma = " << std::setw(7) << sigma;

	_dbg_out_stream << std::endl;

	_dbg_out_stream << "[_init_normal_distribution]" << std::endl;
	_dbg_freqs_out(_dbg_out_stream, freqs, model, i, coder_base);
	_dbg_out_stream << "[/_init_normal_distribution]" << std::endl;
	#endif
}



/*!	\param[in] lambda Параметр lambda распределения лапласа
	\param[in] power Степень в котору будет возведён аргумент x в
	распределении лапласа.
	\param[in] factor Множитель, на который будет умножено значение
	вероятности по формуле распределения Лапласа. Полученное значение будет
	использоваться как количество вызовов функции update_model()
	арифметического кодера.
	\param[in] part Часть диапазона модели, которая будет инициализирована.
	\param[in,out] coder_base Указатель на объект используемого
	арифметического кодера
	\param[in] i Номер используемой модели
	\param[in] purge Если <i>true</i> инициализация модели производится не
	будет
*/
void acoder::_init_laplace_distribution(const float &lambda, const float &power,
										const float &factor, const float &part,
										arcoder_base *const coder_base,
										const sz_t &i, const bool purge)
{
	// Проверим входные параметры
	assert(0 != coder_base);
	assert(0 != lambda);
	assert(0 != factor);
	assert(1 <= part);

	// Установка текущей модели
	coder_base->model(i);

	// Ссылка на описание используемой модели
	const model_t &model = _models[i];

	#ifdef LIBWIC_DEBUG
	std::vector<unsigned int> freqs(model._symbols);
	#endif

	#ifdef LIBWIC_ACODER_PRINT_STATISTIC
	std::cout << std::endl
			  << "acoder::init::model::" << i << "::" << model._symbols
			  << "::" << model._delta << "::" << model.abs_avg
			  << std::endl;
	#endif

	// вычисление предела инициализации
	value_type limit;

	if (0 <= model.min && 0 <= model.max)
	{
		const float min_d = float(model.max - model.min);
		limit = (value_type)floor(min_d / part + 0.5f) - model._delta;
	}
	else
	{
		const float min_d = float(std::min(abs(model.min), abs(model.max)));
		limit = (value_type)floor(min_d / part + 0.5f);
	}

	// Вычисление вероятности для каждого символа в модели
	for (value_type x = model.min; model.max >= x; ++x)
	{
		// Вероятность появления символаи
		const float p = _laplace_func(lambda, x, power);

		// Символ из модели аркодера (не отритцательный)
		const value_type smb = x + model._delta;

		// Проверка не отритцательности симвлоа
		assert(0 <= smb);

		// Целочисленное значение полученной вероятности
		const unsigned int ip = (abs(x) <= limit)
									? (unsigned int)floor(factor*p + 0.5f) : 0;

		#ifdef LIBWIC_DEBUG
		freqs[smb] = ip;
		#endif

		#ifdef LIBWIC_ACODER_PRINT_STATISTIC
		std::cout << "acoder::init::value::" << i << "::" << smb
				  << "::" << ip << std::endl;
		#endif

		if (purge) continue;

		// Инициализация модели аркодера
		for (unsigned int k = ip; 0 < k; --k) coder_base->update_model(smb);
	}

	if (!purge) return;

	#ifdef LIBWIC_DEBUG
	_dbg_out_stream << "#" << std::setw(2) << i;

	_dbg_out_stream << "[" << std::setw(5) << model._symbols << "]";

	_dbg_out_stream << " | abs_avg = " << std::setw(7) << model._abs_average;
	_dbg_out_stream << " | lambda = " << std::setw(7) << lambda;

	_dbg_out_stream << std::endl;

	_dbg_out_stream << "[_init_laplace_distribution]" << std::endl;
	_dbg_freqs_out(_dbg_out_stream, freqs, model, i, coder_base);
	_dbg_out_stream << "[/_init_laplace_distribution]" << std::endl;
	#endif
}


/*!	\param[in] coder_base Базовый класс для арифметических енкодера и
	декодера
*/
void acoder::_init_models(arcoder_base *const coder_base)
{
	// проверка утверждений
	assert(0 != coder_base);
	assert(coder_base->Models().Size() == _models.size());

	// обновление внутренней информации моделей
	_refresh_models(_models);

	// сброс статистик
	coder_base->ResetStatistics();

	// инициализация моделей ---------------------------------------------------

	// для init_mode_none инициализацию проводить не требуется вообще
	if (init_mode_none == _init_mode) return;

	// для всех остальных способов нулевая модель инициализируется равномерно
	coder_base->model(0);

	for (int j = 0; _models[0]._symbols > j; ++j)
	{
		coder_base->update_model(j);
	}

	if (init_mode_geek == _init_mode)
	{
		// init_mode_geek ------------------------------------------------------

		// model 1
		coder_base->model(1);
		const model_t &model1 = _models[1];

		for (int j = 0; model1._symbols > j; ++j) coder_base->update_model(j);

		const int d1 = model1._symbols / (2*5);
		for (int j = 0; 4*d1 > j; ++j) coder_base->update_model(model1._delta);

		for (int j = 1; d1 > j; ++j)
			for (int k = 0; 2*(d1 - j) > k; ++k)
			{
				coder_base->update_model(model1._delta + j);
				coder_base->update_model(model1._delta - j);
			}

		// model 2
		coder_base->model(2);
		const model_t &model2 = _models[2];

		for (int j = 0; model2._symbols > j; ++j) coder_base->update_model(j);

		const int d2 = model2._symbols / (2*4);
		for (int j = 0; 25*d2 > j; ++j) coder_base->update_model(model2._delta);

		for (int j = 1; d2 > j; ++j)
			for (int k = 0; 25*(d2 - j) > k; ++k)
			{
				coder_base->update_model(model2._delta + j);
				coder_base->update_model(model2._delta - j);
			}

		// model 3
		coder_base->model(3);
		const model_t &model3 = _models[3];

		for (int j = 0; model3._symbols > j; ++j) coder_base->update_model(j);

		const int d3 = model3._symbols / (2*6);
		for (int j = 0; 200*d3 > j; ++j) coder_base->update_model(model3._delta);

		for (int j = 1; d3 > j; ++j)
			for (int k = 0; 200*(d3 - j) > k; ++k)
			{
				coder_base->update_model(model3._delta + j);
				coder_base->update_model(model3._delta - j);
			}
	}
	else if (init_mode_uniform == _init_mode)
	{
		// init_mode_uniform ---------------------------------------------------
		assert(!"Not supported yet");
	}
	else if (init_mode_normal == _init_mode)
	{
		// init_mode_normal ----------------------------------------------------
		static const float scale		= 725.0f;
		static const float sigmas[6]	= {0.0f, 8.2f, 4.1f, 1.9f, 0.8f, 0.4f};
		static const float factors[6]	= {1.0f, 1.2f, 1.7f, 2.3f, 1.0f, 1.1f};

		for (unsigned int i = 1; 6 > i; ++i)
		{
			const model_t &model = _models[i];

			const float sigma = (0 < model.abs_avg)? float(model.abs_avg)
												   : sigmas[i];

			_init_normal_distribution(scale, factors[i]*sigma,
									  coder_base, i);
		}
	}
	else if (init_mode_laplace == _init_mode)
	{
		// init_mode_laplace ---------------------------------------------------
		static const float scales[6]	= {0.0f, 250.0f,  455.0f, 891.0f, 1400.0f, 1500.0f};
		static const float powers[6]	= {0.0f, 1.0f,	  1.1f,   1.3f,   1.5f,    2.0f};
		static const float lambdas[6]	= {0.0f, 0.05f,   0.2f,   0.6f,   1.0f,    1.5f};

		for (unsigned int i = 1; 6 > i; ++i)
		{
			const model_t &model = _models[i];

			const float lambda = (0 < model.abs_avg)
									? 1.0f / float(abs(model.abs_avg) + 1)
									: lambdas[i];

			_init_laplace_distribution(lambda, powers[i], 1024, 2.0f,
									   coder_base, i);
		}
	}

	// map models --------------------------------------------------------------

	/*
	if (init_mode_geek == _init_mode)
	{
		coder_base->model(6);
		for (int i = 0; 8 > i; ++i) coder_base->update_model(7);
		for (int i = 0; 8 > i; ++i) coder_base->update_model(0);

		coder_base->model(8);
		for (int i = 0; 8 > i; ++i) coder_base->update_model(0);
	
		coder_base->model(9);
		for (int i = 0; 8 > i; ++i) coder_base->update_model(0x0f);

		coder_base->model(10);
		for (int i = 0; 8 > i; ++i) coder_base->update_model(0x0f);
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

	const double entropy = coder_base->entropy_eval(value + model._delta);

	return entropy;
}


#ifdef LIBWIC_DEBUG
/*!	\param[out] out Стандартный поток для вывода
	\param[in] freqs Вектор частот символов
	\param[in] model Используемая модель
	\param[in] model_no Номер используемой модели
	\param[in] coder_base Базовый класс используемого арифметического кодера

	\note Функция меняет текущую модель арифметического кодера на модель
	с номером model_no.
*/
void acoder::_dbg_freqs_out(std::ostream &out,
							const std::vector<unsigned int> &freqs,
							const model_t &model, const sz_t model_no,
							arcoder_base *const coder_base)
{
	// поиск максимальной частоты
	unsigned int freq_max = 0;

	for (sz_t i = 0; sz_t(freqs.size()) > i; ++i)
	{
		if (freq_max < freqs[i]) freq_max = freqs[i];
	}

	// переключение текущей модели
	if (0 != coder_base) coder_base->model(model_no);

	// вывод частот
	for (sz_t i = 0; sz_t(freqs.size()) > i; ++i)
	{
		const unsigned int freq = freqs[i];

		if (0 == freq) continue;

		// вывод диаграммы
		out << "\t" << std::setw(5) << (i - model._delta) << "|";

		for (sz_t k = (36 * freq / freq_max); 0 < k;  --k) out << '+';

		// числовое значение частоты
		out << std::setw(7) << freq;

		// числовое значение энтропии
		if (0 != coder_base)
		{
			out << "; e=" << std::setprecision(5)
				<< coder_base->entropy_eval(i);
		}

		out << std::endl;
	}
}
#endif



}	// namespace wic
