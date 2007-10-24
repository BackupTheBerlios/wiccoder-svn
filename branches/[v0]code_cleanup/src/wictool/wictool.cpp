/*!	\file     wictool.cpp
	\version  0.0.1
	\author   mice, ICQ: 332-292-380, mailto:wonder.mice@gmail.com
	\brief    Главный файл утилиты wictool

	\todo     Более подробно описать файл wictool.cpp
*/


///////////////////////////////////////////////////////////////////////////////
// include

// standard C++ library headers
#include <limits.h>
#include <math.h>
#include <string.h>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>

// wavelet filters headers
#include <wavelets/cdf97/cdf97.h>

// imgs library headers
#include <imgs/img_rgb.h>
#include <imgs/bmp_file.h>

// libwic headers
#include <wic/libwic/encoder.h>
#include <wic/wicfilters/wavelets.h>

// wictool headers
#include "wictool.h"


///////////////////////////////////////////////////////////////////////////////
// public function definitions

/*!	\param[in] argc Количество параметров доступных в массиве <i>args</i>
	\param[in] args Массив строковых параметров
	\param[out] channel Переменная принимающая цветовой канал
	\param[out] path Переменная принимающая путь к файлу
	\param[in,out] err Указатель на поток для вывода ошибок. Если равен
	<i>0</i> будет использован стандартный поток для вывода ошибок.
	\return Количество использованных аргументов в случае успеха, иначе
	<i>-1</i>.

	Формат:
	\verbatim
	[-c|--channel {r|g|b|x}] <file1.bmp>[:{x|r|g|b|f}]
	\endverbatim

	Значение <i>'x'</i> выбирается по умолчанию, если канал явно не указан.
*/
int get_bmp_file_and_channel(const int argc, const char *const *const args,
							 char &channel, std::string &path,
							 std::ostream *const err)
{
	// определяем поток для вывода ошибок
	std::ostream &out = (0 == err)? std::cerr: (*err);

	// номер текущего аргумента
	int arg_i = 0;

	// проверка, что хватает аргументов
	if (argc <= arg_i)
	{
		out << "Error: not enough arguments for image" << std::endl;
		return -1;
	}

	// получение цветового канала
	if (0 == strcmp("-c", args[arg_i]) ||
		0 == strcmp("--channel", args[arg_i]))
	{
		// переход к следующему аргументу и проверка, что аргументов ещё
		// не подошли к концу
		if (argc <= ++arg_i)
		{
			out << "Error: not enough arguments for [-c|--channel]"
				<< std::endl;
			return -1;
		}

		// проверка длинны идентификатора канала
		if (1 != strlen(args[arg_i]))
		{
			out << "Error: channel identifier must be a single character"
				<< std::endl;
			return -1;
		}

		// получение символа-идентификатора канала
		const char ch = args[arg_i][0];

		// сохранение результата
		channel = ch;
		++arg_i;
	}
	else channel = 'x';

	// проверка, что хватает аргументов
	if (argc <= arg_i)
	{
		out << "Error: not enough arguments" << std::endl;
		return -1;
	}

	path = args[arg_i++];

	// проверка на нотацию вида file.bmp:r, где r - цветовой канал
	if (2 < path.size() && ':' == path[path.size() - 2])
	{
		channel = path[path.size() - 1];

		path = path.substr(0, path.size() - 2);
	}

	// возврат количества использованных агрументов
	return (arg_i);
}


/*!	\param[in] path Путь к <i>bmp</i> файлу изображения
	\param[in] channel Выбираемый цветовой канал
	\param[in,out] err Указатель на поток для вывода ошибок. Если равен
	<i>0</i> будет использован стандартный поток для вывода ошибок.
	\return Структура, содержащая данные цветового канала
*/
bmp_channel_bits get_bmp_channel_bits(const std::string &path,
									  const char channel,
									  std::ostream *const err)
{
	// определяем поток для вывода ошибок
	std::ostream &out = (0 == err)? std::cerr: (*err);

	// создание "пустого" результата
	bmp_channel_bits result;
	memset(&result, 0, sizeof(result));

	// загрузка изображения из файла
	imgs::img_rgb rgb;
	if (0 != imgs::bmp_read(rgb, path))
	{
		out << "Error: can't load image from \""
			<< path << "\"" << std::endl;

		return result;
	}

	// поддерживаются только 24-битные изображения
	if (24 != rgb.bpp())
	{
		out << "Error: only 24bit bitmaps supported" << std::endl;

		return result;
	}

	// определение объёма памяти для хранения цветового канала
	const int pixels_count = rgb.w() * rgb.h();
	result.w = rgb.w();
	result.h = rgb.h();

	switch (channel)
	{
		case 'r':
		case 'g':
		case 'b':
			result.sz = pixels_count;
			break;

		case 'x':
			result.sz = 3 * pixels_count;
			break;

		default:
			out << "Error: invalid channel \'" << channel
				<< "\'" << std::endl;
			return result;
	}

	using imgs::img_rgb::rgb24_t;

	const rgb24_t *const rgb_bits = (rgb24_t *)rgb.bits();

	// выделение нужного объёма памяти
	result.data = new unsigned char[result.sz];

	// копирование данных цветового канала
	switch (channel)
	{
		case 'r':
			for (int i = 0; pixels_count > i; ++i)
			{
				result.data[i] = rgb_bits[i].r;
			}
			break;

		case 'g':
			for (int i = 0; pixels_count > i; ++i)
			{
				result.data[i] = rgb_bits[i].g;
			}
			break;

		case 'b':
			for (int i = 0; pixels_count > i; ++i)
			{
				result.data[i] = rgb_bits[i].b;
			}
			break;

		case 'x':
			for (int i = 0, j = pixels_count, k = 2*pixels_count;
				 pixels_count > i;
				 ++i, ++j, ++k)
			{
				result.data[i] = rgb_bits[i].r;
				result.data[j] = rgb_bits[i].g;
				result.data[k] = rgb_bits[i].b;
			}
			break;

		default:
			out << "Error: invalid channel \'" << channel
				<< "\'" << std::endl;
			return result;
	}

	return result;
}


/*!	\param[in] bits Структура, память ассоциированная с которой будет
	освобождена.
*/
void free_bmp_channel_bits(bmp_channel_bits &bits)
{
	if (0 != bits.data)
	{
		delete[] bits.data;
		bits.data = 0;
	}
}


/*!	\param[in] bits Проверяемая структура
	\return true если структура содержит полезные данные
*/
bool is_channel_bits_good(const bmp_channel_bits &bits, std::ostream *const err)
{
	// определяем поток для вывода ошибок
	std::ostream &out = (0 == err)? std::cerr: (*err);

	// проверка наличия данных
	if (0 == bits.data || 0 >= bits.sz)
	{
		out << "Error: color channel data must not be empty or null"
			<< std::endl;

		return false;
	}

	// проверка корректности геометрических размеров
	if (0 >= bits.w || 0 >= bits.h)
	{
		out << "Error: dimensions " << bits.w << "x" << bits.h << " are invalid"
			<< std::endl;

		return false;
	}

	return true;
}


/*!	\param[in] argc Количество параметров доступных в массиве <i>args</i>
	\param[in] args Массив строковых параметров
	\param[out] result Структура в которую будет помещён результат обработки
	\param[in,out] err Указатель на поток для вывода ошибок. Если равен
	<i>0</i> будет использован стандартный поток для вывода ошибок.
	\return Количество использованных аргументов в случае успеха, иначе
	<i>-1</i>.

	Формат:
	\verbatim
	[-c|--channel {r|g|b|x}] <file1.bmp> [-c|--channel {r|g|b|x}] <file2.bmp>
	\endverbatim

	Значение <i>'x'</i> выбирается по умолчанию, если канал явно не указан.

	\sa get_bmp_file_and_channel()
*/
int get_bmp_file_diff_src(const int argc, const char *const *const args,
						  bmp_file_diff_src_t &result,
						  std::ostream *const err)
{
	// определяем поток для вывода ошибок
	std::ostream &out = (0 == err)? std::cerr: (*err);

	// номер текущего аргумента
	int arg_i = 0;

	// количество использованных аргументов или код ошибки
	int u = 0;

	// получение информации о первом файле
	u = get_bmp_file_and_channel(argc - arg_i, args + arg_i,
								 result.channel1, result.file1,
								 err);
	if (0 > u) return -1;

	arg_i += u;

	// получение информации о втором файле
	u = get_bmp_file_and_channel(argc - arg_i, args + arg_i,
								 result.channel2, result.file2,
								 err);
	if (0 > u) return -1;

	arg_i += u;

	return arg_i;
}


/*!	\param[in] bits1 Первый цветовой канал
	\param[in] bits2 Второй цветовой канал
	\param[in,out] err Указатель на поток для вывода ошибок. Если равен
	<i>0</i> будет использован стандартный поток для вывода ошибок.
	\return <i>true</i> если сравнение цветовых каналов правомерно, иначе
	<i>false</i>

	Чтобы сравнение двух цветовых каналов было правомерным, необходимо:
	- оба цветовых канала должны иметь цветовую информацию
	- геометрические размеры должны совпадать
	- размеры памяти под цветовую информацию каналов должны совпадать
*/
bool is_diff_allowed(const bmp_channel_bits &bits1,
					 const bmp_channel_bits &bits2,
					 std::ostream *const err)
{
	// определяем поток для вывода ошибок
	std::ostream &out = (0 == err)? std::cerr: (*err);

	// проверка наличия цветовой информации
	if (0 == bits1.data || 0 == bits2.data) 
	{
		out << "Error: color channels data must not be empty or null"
			<< std::endl;

		return false;
	}

	// проверка геометрических размеров
	if (bits1.w != bits2.w || bits1.h != bits2.h)
	{
		out << "Error: color channels dimensions must be equal"
			<< std::endl;

		return false;
	}

	// проверка занимаемой памяти
	if (bits1.sz != bits2.sz)
	{
		out << "Error: color channels size must be equal"
			<< std::endl;

		return false;
	}

	return true;
}


/*!	\param[in] bits1 Первый цветовой канал
	\param[in] bits2 Второй цветовой канал
	\param[in,out] err Указатель на поток для вывода ошибок. Если равен
	<i>0</i> будет использован стандартный поток для вывода ошибок.
	\return Подсчитанное значение <i>PSNR</i> или <i>-1.0</i> в случае
	ошибки. Значение <i>0.0</i> возвращается если изображения идентичны.
*/
double psnr(const bmp_channel_bits &bits1, const bmp_channel_bits &bits2,
			std::ostream *const err)
{
	// определяем поток для вывода ошибок
	std::ostream &out = (0 == err)? std::cerr: (*err);

	// проверка правомерности сравнения
	if (!is_diff_allowed(bits1, bits2, err)) return -1.0;

	// подсчёт psnr
	double psnr_value = 0.0;

	for (unsigned int i = 0; bits1.sz > i; ++i)
	{
		const int dp = int(bits1.data[i]) - int(bits2.data[i]);
		psnr_value += (dp*dp);
	}

	if (0 == psnr_value) return (0.0);

	psnr_value /= bits1.sz;

	psnr_value = 20 * log10(double(UCHAR_MAX) / sqrt(psnr_value));

	return psnr_value;
}


/*!	\param[in] diff_src Исходные данные для сравнения
	\param[in,out] err Указатель на поток для вывода ошибок. Если равен
	<i>0</i> будет использован стандартный поток для вывода ошибок.
	\return <i>0</i> если не произошло никаких ошибок, иначе <i>-1</i>
*/
int psnr(const bmp_file_diff_src_t &diff_src,
		 std::ostream *const err)
{
	// определяем поток для вывода ошибок
	std::ostream &out = (0 == err)? std::cerr: (*err);

	// получения цветовых каналов
	bmp_channel_bits bits1 = get_bmp_channel_bits(diff_src.file1,
												  diff_src.channel1);
	bmp_channel_bits bits2 = get_bmp_channel_bits(diff_src.file2,
												  diff_src.channel2);

	// подсчёт PSNR
	const double psnr_value = psnr(bits1, bits2);

	// вывод информации на экран
	std::cout << "psnr: " << psnr_value << "db" << std::endl;

	// освобождение ресурсов
	free_bmp_channel_bits(bits1);
	free_bmp_channel_bits(bits2);

	// возврат результата
	return ((0 <= psnr_value)? 0: -1);
}


/*!	\param[in] bits Цветовой канал
	\return Простейшая статистика (max, min, avg) цветового канала
*/
simple_stat_t calc_simple_stat(const bmp_channel_bits &bits,
							   std::ostream *const err)
{
	// определяем поток для вывода ошибок
	std::ostream &out = (0 == err)? std::cerr: (*err);

	// рабочая переменная
	simple_stat_t simple_stat;
	simple_stat.min = simple_stat.max = simple_stat.avg = 0;

	// проверка входных данных
	if (0 == bits.data || 0 >= bits.sz)
	{
		out << "Error: color channel data must not be empty or null"
			<< std::endl;

		return simple_stat;
	}

	// начальные значения статистики
	simple_stat.min = simple_stat.max = bits.data[0];
	double avg = bits.data[0];

	// проход по всему каналу
	for (unsigned int i = 1; bits.sz > i; ++i)
	{
		if (bits.data[i] < simple_stat.min)
			simple_stat.min = bits.data[i];
		else if (bits.data[i] > simple_stat.max)
			simple_stat.max = bits.data[i];

		avg += bits.data[i];
	}

	// возврат результата
	simple_stat.avg = (unsigned char)(avg / bits.sz);

	return simple_stat;
}


/*!	\param[in] bits1 Первый цветовой канал
	\param[in] bits2 Второй цветовой канал
	\param[in,out] err Указатель на поток для вывода ошибок. Если равен
	<i>0</i> будет использован стандартный поток для вывода ошибок.
	\return <i>0</i> в случае успеха, иначе <i>-1</i>
*/
int stat(const bmp_channel_bits &bits1, const bmp_channel_bits &bits2,
		 std::ostream *const out, std::ostream *const err)
{
	// определяем поток для вывода статистики
	std::ostream &sout = (0 == out)? std::cout: (*out);

	// определяем поток для вывода ошибок
	std::ostream &serr = (0 == err)? std::cerr: (*err);

	// подсчёт простейщей статистики
	const simple_stat_t simple_stat1 = calc_simple_stat(bits1, err);
	const simple_stat_t simple_stat2 = calc_simple_stat(bits2, err);

	// параметры вывода
	static const int hw	= 6;
	static const int fw	= 16;

	// вывод информаци на экран
	sout << std::setw(hw)
		 << "####" << std::setw(fw) << "image 1" << std::setw(fw) << "image 2"
		 << std::setw(fw) << "delta" << std::endl;

	sout << std::setw(hw)
		<< "dims:" << std::setw(fw) << bits1.w << "x" << bits1.h
		<< std::setw(fw) << bits2.w << "x" << bits2.h << std::endl;

	sout << std::setw(hw)
		 << "min:" << std::setw(fw) << int(simple_stat1.min)
		 << std::setw(fw) << int(simple_stat2.min) << std::setw(fw)
		 << abs(int(simple_stat2.min) - int(simple_stat1.min))
		 << std::endl;

	sout << std::setw(hw)
		 << "max:" << std::setw(fw) << int(simple_stat1.max)
		 << std::setw(fw) << int(simple_stat2.max) << std::setw(fw)
		 << abs(int(simple_stat2.max) - int(simple_stat1.max))
		 << std::endl;

	sout << std::setw(hw)
		 << "avg:" << std::setw(fw) << int(simple_stat1.avg)
		 << std::setw(fw) << int(simple_stat2.avg) << std::setw(fw)
		 << abs(int(simple_stat2.avg) - int(simple_stat1.avg))
		 << std::endl;

	sout << std::endl;

	const double psnr_value = psnr(bits1, bits2, err);

	sout << std::setw(hw) << "psnr:" << std::setw(fw)
		 << psnr_value << " dB" << std::endl;

	// всё в порядке
	return 0;
}


/*!	\param[in] diff_src Исходные данные для сравнения
	\param[in,out] err Указатель на поток для вывода ошибок. Если равен
	<i>0</i> будет использован стандартный поток для вывода ошибок.
	\return <i>0</i> если не произошло никаких ошибок, иначе <i>-1</i>
*/
int stat(const bmp_file_diff_src_t &diff_src,
		 std::ostream *const err)
{
	// определяем поток для вывода ошибок
	std::ostream &out = (0 == err)? std::cerr: (*err);

	// получения цветовых каналов
	bmp_channel_bits bits1 = get_bmp_channel_bits(diff_src.file1,
												  diff_src.channel1);
	bmp_channel_bits bits2 = get_bmp_channel_bits(diff_src.file2,
												  diff_src.channel2);

	// вывод статистики на экран
	const int stat_result = stat(bits1, bits2);

	// освобождение ресурсов
	free_bmp_channel_bits(bits1);
	free_bmp_channel_bits(bits2);

	return stat_result;
}


/*!	\param[in] argc Количество параметров доступных в массиве <i>args</i>
	\param[in] args Массив строковых параметров
	\param[out] result Результирующее имя (название) вейвлет преобразования
	\param[in,out] err Указатель на поток для вывода ошибок. Если равен
	<i>0</i> будет использован стандартный поток для вывода ошибок.
	\return Количество использованных аргументов в случае успеха, иначе
	<i>-1</i>.

	Формат:
	\verbatim
	[-f|--filter <name>]
	\endverbatim

	Возможные значения параметра <i>name</i>:
	- cdf97 (используется по умолчанию)
	- Haar
	- Daub4
	- Daub6
	- Daub8
	- Antonini
	- AntoniniSynthesis
	- Villa1810
	- Villa1810Synthesis
	- Adelson
	- Brislawn
	- BrislawnSynthesis
	- Brislawn2
	- Villa1
	- Villa2
	- Villa3
	- Villa4
	- Villa5
	- Villa6
	- Odegard
	- Petuhov1
	- Petuhov2
*/
int get_wavelet_filter(const int argc, const char *const *const args,
					   std::string &result, std::ostream *const err)
{
	// определяем поток для вывода ошибок
	std::ostream &out = (0 == err)? std::cerr: (*err);

	// значение по умолчанию
	static const std::string &def_result = "cdf97";

	// номер текущего аргумента
	int arg_i = 0;

	// проверка, что хватает аргументов
	if (argc <= arg_i)
	{
		result = def_result;

		return arg_i;
	}

	// получение названия преобразования
	if (0 == strcmp("-f", args[arg_i]) ||
		0 == strcmp("--filter", args[arg_i]))
	{
		// переход к следующему аргументу и проверка, что аргументы ещё
		// не подошли к концу
		if (argc <= ++arg_i)
		{
			out << "Error: not enough arguments for [-f|--filter]"
				<< std::endl;
			return -1;
		}

		// сохранение результата
		result = args[arg_i++];
	}
	else
	{
		result = def_result;
	}

	return arg_i;
}


/*!	\param[in] filter 
	\param[in] steps
	\param[in] bits 
	\param[in,out] err Указатель на поток для вывода ошибок. Если равен
	<i>0</i> будет использован стандартный поток для вывода ошибок.
	\return Количество использованных аргументов в случае успеха, иначе
	<i>-1</i>.
	\return 
*/
spectre_t forward_transform(const std::string &filter,
							const unsigned int steps,
							const bmp_channel_bits &bits,
							std::ostream *const err)
{
	// определяем поток для вывода ошибок
	std::ostream &out = (0 == err)? std::cerr: (*err);

	// пустой спектр
	spectre_t spectre;
	spectre.data	= 0;
	spectre.sz		= 0;
	spectre.h		= 0;
	spectre.w		= 0;

	// проверка входных данных
	if (filter.empty())
	{
		out << "Error: filter name \"" << filter << "\" is invalid"
			<< std::endl;

		return spectre;
	}

	if (!is_channel_bits_good(bits, err))
	{
		return spectre;
	}

	// выделение памяти под результат преобразования
	spectre.sz		= bits.w * bits.h;
	spectre.data	= new wic::w_t[spectre.sz];
	spectre.w		= bits.w;
	spectre.h		= bits.h;

	// выполнение преобразования
	if (0 == filter.compare("cdf97"))
	{
		for (unsigned int i = 0; spectre.sz > i; ++i)
		{
			spectre.data[i] = wic::w_t(bits.data[i]);
		}

		wt2d_cdf97_fwd(spectre.data, spectre.h, spectre.w, steps);
	}
	else
	{
		out << "Error: unknown filter \"" << filter << "\"" << std::endl;
	}

	return spectre;
}


/*!	\param[in] spectre Спектр вейвлет преобразования, память, занимаемую
	которым следует освободить
*/
void free_spectre(spectre_t &spectre)
{
	if (0 != spectre.data) delete[] spectre.data;

	spectre.data	= 0;
	spectre.sz		= 0;
	spectre.w		= 0;
	spectre.h		= 0;
}


/*!	\param[in] argc Количество параметров доступных в массиве <i>args</i>
	\param[in] args Массив строковых параметров
	\param[out] result Результирующее имя (название) метода сжатия
	\param[in,out] err Указатель на поток для вывода ошибок. Если равен
	<i>0</i> будет использован стандартный поток для вывода ошибок.
	\return Количество использованных аргументов в случае успеха, иначе
	<i>-1</i>.

	Формат:
	\verbatim
	[-m|--method <name>]
	\endverbatim

	Возможные значения параметра <i>name</i>:
	- manual (используется по умолчанию)
	- fixed_lambda
	- fixed_bpp
*/
int get_encode_method(const int argc, const char *const *const args,
					  std::string &result, std::ostream *const err)
{
	// определяем поток для вывода ошибок
	std::ostream &out = (0 == err)? std::cerr: (*err);

	// значение по умолчанию
	static const std::string &def_result = "manual";

	// номер текущего аргумента
	int arg_i = 0;

	// проверка, что хватает аргументов
	if (argc <= arg_i)
	{
		result = def_result;

		return arg_i;
	}

	// получение названия преобразования
	if (0 == strcmp("-m", args[arg_i]) ||
		0 == strcmp("--method", args[arg_i]))
	{
		// переход к следующему аргументу и проверка, что аргументы ещё
		// не подошли к концу
		if (argc <= ++arg_i)
		{
			out << "Error: not enough arguments for [-m|--method]"
				<< std::endl;
			return -1;
		}

		// сохранение результата
		result = args[arg_i++];
	}
	else
	{
		result = def_result;
	}

	return arg_i;
}


/*!	\param[in] argc Количество параметров доступных в массиве <i>args</i>
	\param[in] args Массив строковых параметров
	\param[out] params Параметры кодирования, полученные из командной строки
	\param[in,out] err Указатель на поток для вывода ошибок. Если равен
	<i>0</i> будет использован стандартный поток для вывода ошибок.
	\return Количество использованных аргументов в случае успеха, иначе
	<i>-1</i>.

	Формат:
	\verbatim
	[-q|--quantizer value] [-l|--lambda value] [-b|--bpp bpp] [-s|--steps n]
	\endverbatim
*/
int get_encode_params(const int argc, const char *const *const args,
					  encode_params_t &params, std::ostream *const err)
{
	// определяем поток для вывода ошибок
	std::ostream &out = (0 == err)? std::cerr: (*err);

	// присвоить значения по умолчанию
	params.q		= 16;
	params.lambda	= 32;
	params.bpp		= 0.5;
	params.steps	= 5;

	// номер текущего аргумента
	int arg_i = 0;

	// чтение параметров из командной строки
	while (arg_i < argc)
	{
		if (0 == strcmp("-q", args[arg_i]) ||
			0 == strcmp("--quantizer", args[arg_i]))
		{
			// [-q|--quantizer value]
			if (argc <= ++arg_i)
			{
				out << "Error: not enough arguments for [-q|--quantizer]"
					<< std::endl;
				return -1;
			}

			// получение значения квантователя
			std::stringstream q_as_string(args[arg_i++]);
			q_as_string >> params.q;
		}
		else if (0 == strcmp("-l", args[arg_i]) ||
				 0 == strcmp("--lambda", args[arg_i]))
		{
			// [-l|--lambda value]
			if (argc <= ++arg_i)
			{
				out << "Error: not enough arguments for [-l|--lambda]"
					<< std::endl;
				return -1;
			}

			// получение значения баланса
			std::stringstream lambda_as_string(args[arg_i++]);
			lambda_as_string >> params.lambda;
		}
		else if (0 == strcmp("-s", args[arg_i]) ||
				 0 == strcmp("--steps", args[arg_i]))
		{
			// [-s|--steps n]
			if (argc <= ++arg_i)
			{
				out << "Error: not enough arguments for [-s|--steps]"
					<< std::endl;
				return -1;
			}

			// получение количества щагов преобразования
			std::stringstream steps_as_string(args[arg_i++]);
			steps_as_string >> params.steps;
		}
		else
		{
			// Дошли до неизвестной опции - предполагаем, что параметры
			// кончились
			break;
		}
	}

	return arg_i;
}


/*!	\param[in] argc Количество параметров доступных в массиве <i>args</i>
	\param[in] args Массив строковых параметров
	\param[out] wic_file Путь к сжатому файлу
	\param[in,out] err Указатель на поток для вывода ошибок. Если равен
	<i>0</i> будет использован стандартный поток для вывода ошибок.
	\return Количество использованных аргументов в случае успеха, иначе
	<i>-1</i>.
*/
int get_wic_file(const int argc, const char *const *const args,
				 std::string &wic_file, std::ostream *const err)
{
	// определяем поток для вывода ошибок
	std::ostream &out = (0 == err)? std::cerr: (*err);

	// номер текущего аргумента
	int arg_i = 0;

	if (argc <= arg_i)
	{
		out << "Error: compressed file name not specified" << std::endl;
		return -1;
	}

	wic_file = args[arg_i++];

	return arg_i;
}


/*!	\param[in,out] Поток в который будет выведена цитата
*/
void crazy_ones(std::ostream &out)
{
	out <<
	"Here's to the crazy ones, the misfits, the rebels, the troublemakers, the"
	<< std::endl <<
	"round pegs in the square holes... the ones who see things differently - "
	<< std::endl <<
	"they're not fond of rules... You can quote them, disagree with them, "
	<< std::endl <<
	"glorify or vilify them, but the only thing you can't do is ignore them"
	<< std::endl <<
	"because they change things... they push the human race forward, and while"
	<< std::endl <<
	"some may see them as the crazy ones, we see genius, because the ones who"
	<< std::endl <<
	"are crazy enough to think that they can change the world, are the ones who"
	<< std::endl <<
	"do."
	<< std::endl << std::endl <<
	"Apple Computers"
	<< std::endl << std::endl;
}


/*!	\param[in,out] Поток в который будет выведена информация о
	утилите
*/
void about(std::ostream &out)
{
	out << std::endl;
	out << "wictool - console wrapper for libwic (image compression algorithm "
		<< "based on" << std::endl
		<< "          encoding of tree-arranged wavelet coefficients)"
		<< std::endl;
	out << std::endl;
	out << "(C) 2007, MIEE DSP Team:" << std::endl
		<< "Sergei V. Umnyashkin" << std::endl
		<< "Dmitri M. Koplovich" << std::endl
		<< "Andrei S. Pokrovskiy" << std::endl
		<< "Andrei A. Alexandrov" << std::endl;
	out << std::endl;
	out << "Moscow Institute of Electronic Technology"
		<< "(www.miet.ru)" << std::endl;
}


/*!	\param[in,out] out Поток в который будет выведена информация по
	пользованию утилитой
	\return Всегда <i>-1</i>
*/
int usage(std::ostream &out)
{
	out << "Usage is:" << std::endl << std::endl;

	out << "wictool -a|--about" << std::endl;
	out << "wictool -h|--help" << std::endl;
	out << "wictool -e|--encode [<filter>] [<method>] [<params>] <image> "
		<< "<file.wic>" << std::endl;
	out << "wictool -d|--decode <file.wic> <image>" << std::endl;
	out << "wictool -p|--psnr <image1> <image2>" << std::endl;
	out << "wictool -s|--stat <image1> <image2>" << std::endl;
	out << std::endl;
	out << "<image>:  [-c|--channel {x|r|g|b|f}] <file.bmp>[:{x|r|g|b|f}]"
		<< std::endl;
	out << "<filter>: [-f|--filter name]" << std::endl;
	out << "<method>: [-m|--method name]" << std::endl;
	out << "<params>: [-q|--quantizer] quantizer [-a|--lambda] lambda "
		<< "[-b|--bpp] bpp " << std::endl
		<< "          [-l|--levels] n [-f|--filter] filter" << std::endl;
	out << std::endl;
	out << "Filters are: cdf97 (default), Haar, Daub4, Daub6, Daub8"
		<< std::endl;
	out << "Methods are: manual (default), fixed_lambda, fixed_bpp"
		<< std::endl;

	/*
	1. lambda + q
	2. lambda
	3. bpp
	-e|--encode [-m|--mode {manual|fixed_bpp|fixed_lambda}]
		{-q|--quantizer <quantizer>}
		{-l|--lambda <lambda>}
		{-b|--bpp <bpp>}
	*/

	return -1;
}


//!	\brief Точка входа
/*!	\param[in] argc Количество параметров командной строки
	\param[in] args Параметры командной строки
*/
int main(int argc, char **args)
{
	// количество использованных аргументов командной строки
	int argk = 1;

	// проверка на наличие агрументов
	if (argk >= argc) return usage(std::cout);

	// Флаг "многословности"
	bool verbose = false;

	// Главный цикл обработки параметров командной строки
	while (argk < argc)
	{
		// выбор режима работы
		const std::string mode = args[argk++];

		if (0 == mode.compare("-!") || 0 == mode.compare("--crazy"))
		{
			crazy_ones(std::cout);
		}
		else if (0 == mode.compare("-v") || 0 == mode.compare("--verbose"))
		{
			verbose = true;
			// std::cout << "Verbose output enabled." << std::endl;
		}
		else if (0 == mode.compare("-a") || 0 == mode.compare("--about"))
		{
			about(std::cout);
		}
		else if (0 == mode.compare("-h") || 0 == mode.compare("--help"))
		{
			usage(std::cout);
		}
		else if (0 == mode.compare("-p") || 0 == mode.compare("--psnr"))
		{
			// -p | --psnr
			bmp_file_diff_src_t res;

			const int argx = get_bmp_file_diff_src(argc - argk , args + argk,
												   res);

			if (0 > argx) return usage(std::cout);
			else argk += argx;

			std::cout << "image 1: " << res.file1 << ":" << res.channel1
					  << std::endl;
			std::cout << "image 2: " << res.file2 << ":" << res.channel2
					  << std::endl;

			if (0 != psnr(res)) return -1;
		}
		else if (0 == mode.compare("-s") || 0 == mode.compare("--stat"))
		{
			// -s | --stat
			bmp_file_diff_src_t res;

			const int argx = get_bmp_file_diff_src(argc - argk , args + argk,
												   res);

			if (0 > argx) return usage(std::cout);
			else argk += argx;

			std::cout << "image 1: " << res.file1 << ":" << res.channel1
					  << std::endl;
			std::cout << "image 2: " << res.file2 << ":" << res.channel2
					  << std::endl;

			if (0 != stat(res)) return -1;
		}
		else if (0 == mode.compare("-e") || 0 == mode.compare("--encode"))
		{
			// -e | --encode
			int argx = -1;

			// Используемое вейвлет преобразование
			std::string filter_name;
			argx = get_wavelet_filter(argc - argk, args + argk, filter_name);

			if (0 > argx) return usage(std::cout);
			else argk += argx;

			if (verbose)
			{
				std::cout << "Filter: " << filter_name << std::endl;
			}

			// Метод кодирования
			std::string method_name;
			argx = get_encode_method(argc - argk, args + argk, method_name);

			if (0 > argx) return usage(std::cout);
			else argk += argx;

			if (verbose)
			{
				std::cout << "Method: " << method_name << std::endl;
			}

			// Параметры кодирования
			encode_params_t params;
			argx = get_encode_params(argc - argk, args + argk, params);

			if (0 > argx) return usage(std::cout);
			else argk += argx;

			if (verbose)
			{
				std::cout << "Steps:  " << params.steps << std::endl;
				std::cout << "Q:      " << params.q << std::endl;
				std::cout << "Lambda: " << params.lambda << std::endl;
			}

			// Путь к файлу с изображением и идентификатор канала
			char channel = 'x';
			std::string img_path;
			argx = get_bmp_file_and_channel(argc - argk, args + argk,
											channel, img_path);

			if (0 > argx) return usage(std::cout);
			else argk += argx;

			if (verbose)
			{
				std::cout << "Image:  \"" << img_path << ":"
						  << channel << "\"" << std::endl;
			}

			// Получение пути к закодированному файлу
			std::string wic_path;
			argx = get_wic_file(argc - argk, args + argk, wic_path);

			if (0 > argx) return usage(std::cout);
			else argk += argx;

			if (verbose)
			{
				std::cout << "Output: \"" << wic_path << "\"" << std::endl;
			}

			// Загрузка изображения
			bmp_channel_bits img_bits = get_bmp_channel_bits(img_path, channel);

			// Выполнение вейвлет преобразования
			spectre_t spectre = forward_transform(filter_name, params.steps,
												  img_bits);

			// Кодирование изображения
			if (verbose)
			{
				std::cout << std::endl << "Encoding... " << std::flush;
			}

			wic::encoder encoder(spectre.w, spectre.h, params.steps);

			wic::encoder::tunes_t tunes;
			const wic::encoder::enc_result_t enc_result	= 
								encoder.encode(spectre.data,
											   params.q, params.lambda, tunes);

			if (verbose)
			{
				std::cout << "done." << std::endl << std::endl;
				std::cout << "Optimization:" << std::endl;
				std::cout << "    q:      " << enc_result.optimization.q
						  << std::endl;
				std::cout << "    lambda: " << enc_result.optimization.lambda
						  << std::endl;
				std::cout << "    j:      " << enc_result.optimization.j
						  << std::endl;
				std::cout << "    bpp:    " << enc_result.optimization.bpp
						  << std::endl;
				std::cout << "Output:" << std::endl;
				std::cout << "    bpp:    " << enc_result.bpp << std::endl;
				std::cout << "    size:   "
						  << float(encoder.coder().encoded_sz())/1024.0f
						  << " kb" << std::endl;
			}

			// Запись в файл
			std::ofstream output(wic_path.c_str(),
								 std::ios_base::binary|std::ios_base::out);
			output.write("WIC0", 4);
			output.write((char *)&tunes, sizeof(tunes));
			output.write((char *)encoder.coder().buffer(),
						 encoder.coder().encoded_sz());

			// Освобождение занятых ресурсов
			free_spectre(spectre);
			free_bmp_channel_bits(img_bits);
		}
		else if (0 == mode.compare("-d") || 0 == mode.compare("--decode"))
		{
			std::string wf_name;
			const int argx = get_wavelet_filter(argc - argk, args + argk, wf_name);

			if (0 > argx) return usage(std::cout);
			else argk += argx;

			std::cout << "wf_name: " << wf_name << std::endl;
		}
		else
		{
			std::cout << "Error: unknown option \"" << mode << "\""
					  << std::endl;

			return usage(std::cout);
		}
	}

	return 0;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////



// standard C++ library headers
#include <assert.h>
#include <string>
#include <fstream>
#include <iostream>

// external library header
#include <imgs/img_rgb.h>
#include <imgs/img_yuv.h>
#include <imgs/psnr.h>
#include <imgs/bmp_file.h>
#include <imgs/bmp_dump.h>
#include <wavelets/cdf97/cdf97.h>

// libwic headers
#include <wic/libwic/encoder.h>



///////////////////////////////////////////////////////////////////////////////
// types declaration

#pragma pack(push, 1)
struct compressed_hdr_t {
	unsigned short version;
	unsigned int width;
	unsigned int height;
	unsigned short lvls;
	unsigned int data_sz;
};
#pragma pack(pop)



///////////////////////////////////////////////////////////////////////////////
// constants definition

//!	\brief Версия утилиты
static const unsigned short WIC_VERSION		= 1;



///////////////////////////////////////////////////////////////////////////////
// functions declaration

//!	\brief Созадаёт кодировщик
wic::encoder *mk_encoder(const std::string &src_path,
						 const int lvls,
						 const int channel = 0);

//!	\brief  Осуществляет запись закодированного в файл
int encode(const wic::encoder &encoder,
		   const wic::encoder::tunes_t &header,
		   const std::string &result_path);

//!	\brief Осуществляет декодирование файла
int decode(const std::string &compressed_path,
		   const std::string &result_path);



///////////////////////////////////////////////////////////////////////////////
// functions definitions

/*!	\param[in] src_path Путь к файлу с исходным изображением
	\param[in] lvls Количество уровней разложения
	\param[in] channel Используемый цветовой канал:
	- 0 Red
	- 1 Green
	- 2 Blue
	\return Объект класса wic::encoder
*/
wic::encoder *mk_encoder(const std::string &src_path,
						 const int lvls,
						 const int channel)
{
	// load source image
	imgs::img_rgb rgb_image;
	if (0 != imgs::bmp_read(rgb_image, src_path.c_str()))
	{
		std::cout << "Error: can't read bitmap file \"" << src_path
				  << "\"" << std::endl;
		return 0;
	}

	// test levels factor of dimensions
	if (rgb_image.w() != rgb_image.h())
	{
		std::cout << "Error: only square pictures supported" << std::endl;
		return 0;
	}

	{
		const unsigned int factor = (1 << lvls);

		if (0 != (rgb_image.w() % factor) &&
			0 != (rgb_image.h() % factor))
		{
			std::cout << "Error: " << rgb_image.w() << "x"
					  << rgb_image.h() << " is invalid image "
					  << "dimensions for " << lvls << " levels"
					  << std::endl;
			return 0;
		}
	}

	// allocate some memory and copy converted to float image there
	const wic::sz_t image_pixels_count = rgb_image.w() * rgb_image.h();

	float *const image_wt = new float[image_pixels_count];

	imgs::img_rgb::rgb24_t *const rgb_image_bits =
		(imgs::img_rgb::rgb24_t *)(rgb_image.bits());

	if (0 == channel)
	{
		for (wic::sz_t i = 0; image_pixels_count > i; ++i) {
			image_wt[i] = rgb_image_bits[i].r;
		}
	}
	else if (1 == channel)
	{
		for (wic::sz_t i = 0; image_pixels_count > i; ++i) {
			image_wt[i] = rgb_image_bits[i].g;
		}
	}
	else
	{
		for (wic::sz_t i = 0; image_pixels_count > i; ++i) {
			image_wt[i] = rgb_image_bits[i].b;
		}
	}

	// do wavelet transform
	wt2d_cdf97_fwd(image_wt, rgb_image.h(), rgb_image.w(), lvls);

	// создание кодера
	wic::encoder *const encoder = new wic::encoder(rgb_image.w(),
												   rgb_image.h(),
												   lvls);

	// load spectrum
	encoder->spectrum().cheap_load(image_wt);

	// free spectrum
	delete[] image_wt;

	// return encoder
	return encoder;
}


/*!	\param[in] encoder Объект класса хwic::encoder, который уже
	содержит закодированные данные
	\param[in] header Заголовок закодированных данных
	\param[in] result_path Путь к файлу в который будут записанны
	закодированные данные
*/
int encode(const wic::encoder &encoder,
		   const wic::encoder::tunes_t &header,
		   const std::string &result_path)
{
	// open file for writing
	std::ofstream compressed(result_path.c_str(),
							 std::ios::out|std::ios::binary);

	if (!compressed.good())
	{
		std::cout << "Error: can't open file \"" << result_path << "\" for "
				  << "writing" << std::endl;
		return -1;
	}

	// create headers
	compressed_hdr_t compressed_hdr;
	compressed_hdr.version	= WIC_VERSION;
	compressed_hdr.width	= encoder.spectrum().width();
	compressed_hdr.height	= encoder.spectrum().height();
	compressed_hdr.lvls		= encoder.spectrum().lvls();
	compressed_hdr.data_sz	= encoder.coder().encoded_sz() + sizeof(header);

	// write data
	compressed.write((char *)&compressed_hdr, sizeof(compressed_hdr));
	compressed.write((char *)&header, sizeof(header));
	compressed.write((char *)(encoder.coder().buffer()),
					 encoder.coder().encoded_sz());

	if (!compressed.good())
	{
		std::cout << "Error: can't write to file \"" << result_path
				  << "\"";
		return -1;
	}

	std::cout << "Total file size: " << compressed.tellp() << " bytes ("
			  << (compressed.tellp() / 1024.0) << " kbytes)" << std::endl;

	return 0;
}


/*!	\param[in] compressed_path Путь к закодированному файлу
	\param[in] result_path Путь к результирующему файлу
	\return 0 если файл декодирован успешно
*/
int decode(const std::string &compressed_path,
		   const std::string &result_path)
{
	std::cout << "decoding..." << std::endl;

	// open compressed file
	std::ifstream compressed(compressed_path.c_str(),
							 std::ios::in|std::ios::binary);
	if (!compressed.good())
	{
		std::cout << "Error: can't open compressed file \""
				  << compressed_path << "\" for reading" << std::endl;
		return -1;
	}

	// read compressed header information
	compressed_hdr_t compressed_hdr;
	compressed.read((char *)&compressed_hdr, sizeof(compressed_hdr));
	if (!compressed.good())
	{
		std::cout << "Error: can't read compressed header information"
				  << "from compressed file" << std::endl;
		return -1;
	}

	std::cout << "image: " << compressed_hdr.width << "x"
			  << compressed_hdr.height << ", "<< compressed_hdr.lvls
			  << " levels" << std::endl;

	// test wictool version
	if (WIC_VERSION != compressed_hdr.version)
	{
		std::cout << "Error: compressed file of version \""
				  << compressed_hdr.version << "\" can't be decompressed "
					 "with wictool of version \"" << WIC_VERSION << "\""
				  << std::endl;
		return -1;
	}

	std::cout << "file version: " << compressed_hdr.version << std::endl;

	// test levels factor of dimensions
	{
		const unsigned int factor = (1 << compressed_hdr.lvls);

		if (0 != (compressed_hdr.width % factor) &&
			0 != (compressed_hdr.height % factor))
		{
			std::cout << "Error: " << compressed_hdr.width << "x"
					  << compressed_hdr.height << " is invalid image "
					  << "dimensions for " << compressed_hdr.lvls << " levels"
					  << std::endl;
			return -1;
		}
	}

	// create decoder
	wic::encoder encoder(compressed_hdr.width, compressed_hdr.height,
						 compressed_hdr.lvls);

	// allocate buffer for compressed data
	char *const data = new char[compressed_hdr.data_sz];

	if (0 == data)
	{
		std::cout << "Error: can't allocate memory to read file" << std::endl;
		return -1;
	}

	// read compressed data
	compressed.read(data, compressed_hdr.data_sz);
	if (!compressed.good())
	{
		std::cout << "Error: can't read compressed data from file"
				  << std::endl;
		delete[] data;
		return -1;
	}

	// get header
	const wic::encoder::tunes_t &header = 
						*((wic::encoder::tunes_t *)data);

	std::cout << "q: " << header.q << std::endl;

	// decode
	encoder.decode((wic::byte_t *)(data) + sizeof(header),
				   compressed_hdr.data_sz - sizeof(header),
				   header);

	delete[] data;

	// allocate buffer for spectrum
	wic::w_t *const image_wt = new wic::w_t[encoder.spectrum().nodes_count()];
	if (0 == image_wt)
	{
		std::cout << "Error: can't allocate buffer for spectrum" << std::endl;
		return -1;
	}

	// save decompressed data in local spectrum
	encoder.spectrum().save<wic::wnode::member_w>(image_wt);

	// do inverse wavelet transform
	wt2d_cdf97_inv(image_wt, compressed_hdr.height, compressed_hdr.width,
				   compressed_hdr.lvls);

	imgs::img_rgb rgb_image;
	rgb_image.reset(compressed_hdr.height, compressed_hdr.width, 24);

	imgs::img_rgb::rgb24_t *const rgb_image_bits =
		(imgs::img_rgb::rgb24_t *)(rgb_image.bits());

	for (wic::sz_t i = 0; encoder.spectrum().nodes_count() > i; ++i)
	{
		const wic::w_t &wval = image_wt[i];

		const unsigned char val	= ((0 >= wval)? 0:
										((0xFF <= wval)? 0xFF:
												(unsigned char)wval));

		rgb_image_bits[i].r = val;
		rgb_image_bits[i].g = val;
		rgb_image_bits[i].b = val;
	}

	delete[] image_wt;

	// write bmp to resulting file
	if (0 != imgs::bmp_write(rgb_image, result_path))
	{
		std::cout << "Error: can't write decompressed bitmap to \""
				  << result_path << "\"" << std::endl;
		return -1;
	}

	return 0;
}



///////////////////////////////////////////////////////////////////////////////
// main() function definition
int main1(int argc, char **args)
{
	// количество использованных аргументов командной строки
	int argk = 1;

	// проверка на наличие агрументов
	if (argk >= argc) return usage(std::cout);

	while (argk < argc)
	{
		// выбор режима работы
		const std::string mode = args[argk++];

		if (0 == mode.compare("-a") || 0 == mode.compare("--about"))
		{
			about(std::cout);
		}
		else if (0 == mode.compare("-h") || 0 == mode.compare("--help"))
		{
			usage(std::cout);
		}
		else if (0 == mode.compare("-p") || 0 == mode.compare("--psnr"))
		{
			// -p | --psnr
			bmp_file_diff_src_t res;

			const int argx = get_bmp_file_diff_src(argc - argk , args + argk,
												   res);

			if (0 > argx) return usage(std::cout);
			else argk += argx;

			std::cout << "image 1: " << res.file1 << ":" << res.channel1
					  << std::endl;
			std::cout << "image 2: " << res.file2 << ":" << res.channel2
					  << std::endl;

			if (0 != psnr(res)) return -1;
		}
		else if (0 == mode.compare("-s") || 0 == mode.compare("--stat"))
		{
			// -s | --stat
			bmp_file_diff_src_t res;

			const int argx = get_bmp_file_diff_src(argc - argk , args + argk,
												   res);

			if (0 > argx) return usage(std::cout);
			else argk += argx;

			std::cout << "image 1: " << res.file1 << ":" << res.channel1
					  << std::endl;
			std::cout << "image 2: " << res.file2 << ":" << res.channel2
					  << std::endl;

			if (0 != stat(res)) return -1;
		}
		else if (0 == mode.compare("-e") || 0 == mode.compare("--encode"))
		{
			std::string wf_name;
			const int argx = get_wavelet_filter(argc - argk, args + argk,
												   wf_name);

			if (0 > argx) return usage(std::cout);
			else argk += argx;

			std::cout << "wf_name: " << wf_name << std::endl;
		}
		else if (0 == mode.compare("-d") || 0 == mode.compare("--decode"))
		{
			std::string wf_name;
			const int argx = get_wavelet_filter(argc - argk, args + argk,
												   wf_name);

			if (0 > argx) return usage(std::cout);
			else argk += argx;

			std::cout << "wf_name: " << wf_name << std::endl;
		}
		else
		{
			return usage(std::cout);
		}
	}

	return 0;

	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////

	bmp_file_diff_src_t res;
	if (0 > get_bmp_file_diff_src(argc -1 , args + 1, res))
		return usage(std::cout);

	std::cout << res.channel1 << ":" << res.file1 << std::endl;
	std::cout << res.channel2 << ":" << res.file2 << std::endl;

	psnr(res);

	return 0;

	std::cout << "Image compression algorithm based on encoding of tree-arranged wavelet coefficients. Demo version." << std::endl;
	std::cout << "(C) Sergei V. Umnyashkin, Dmitri M. Koplovich, Andrei S. Pokrovskiy, Andrei A. Alexandrov, 2007" << std::endl;
	std::cout << "Moscow Institute of Electronic Technology" << std::endl;
	std::cout << "www.miet.ru" << std::endl;
	std::cout << std::endl;

	if (1 >= argc) return 0;

	if (0 == strcmp(args[1], "-p") || 0 == strcmp(args[1], "--psnr"))
	{
		if (4 > argc) return 0;

		std::cout << "PSNR: " << 0 << " dB" << std::endl;

		return 0;
	}

	if (0 == strcmp(args[1], "-d") || 0 == strcmp(args[1], "--decode"))
	{
		if (4 > argc) return 0;

		return decode(args[2], args[3]);
	}

	// декодирование
	int arg_i = 1;

	if (0 == strcmp(args[arg_i], "-e") ||
		0 == strcmp(args[arg_i], "--encode"))
	{
		if (argc <= ++arg_i) return 0;
	}

	// метод сжатия
	int method = 0;

	if (0 == strcmp(args[arg_i], "-m") ||
		0 == strcmp(args[arg_i], "--method"))
	{
		if (argc <= ++arg_i) return 0;

		if (0 == strcmp(args[arg_i], "manual") ||
			0 == strcmp(args[arg_i], "0"))
		{
			method = 0;
		} else if (0 == strcmp(args[arg_i], "fixed_bpp") ||
				   0 == strcmp(args[arg_i], "1"))
		{
			method = 1;
		}
		else if (0 == strcmp(args[arg_i], "fixed_lambda") ||
				 0 == strcmp(args[arg_i], "2"))
		{
			method = 2;
		}
		else return 0;

		if (argc <= ++arg_i) return 0;
	}

	// количество уровней разложения
	int lvls = 5;

	if (0 == strcmp(args[arg_i], "-v") ||
		0 == strcmp(args[arg_i], "--levels"))
	{
		if (argc <= ++arg_i) return 0;

		lvls = atoi(args[arg_i]);

		if (argc <= ++arg_i) return 0;
	}

	// цветовой канал
	int channel = 0;

	if (0 == strcmp(args[arg_i], "-c") ||
		0 == strcmp(args[arg_i], "--channel"))
	{
		if (argc <= ++arg_i) return 0;

		channel = atoi(args[arg_i]);

		if (argc <= ++arg_i) return 0;
	}

	// способы оптимизации
	wic::q_t q = 8;
	wic::lambda_t lambda = 10;

	// ручной
	if (0 == method)
	{
		if (0 == strcmp(args[arg_i], "-q") ||
			0 == strcmp(args[arg_i], "--q"))
		{
			if (argc <= ++arg_i) return 0;

			q = wic::q_t(atof(args[arg_i]));

			if (argc <= ++arg_i) return 0;
		}

		if (0 == strcmp(args[arg_i], "-l") ||
			0 == strcmp(args[arg_i], "--lambda"))
		{
			if (argc <= ++arg_i) return 0;

			lambda = atof(args[arg_i]);

			if (argc <= ++arg_i) return 0;
		}
	}

	// фиксированный bpp
	wic::h_t bpp = 0.5;

	if (1 == method)
	{
		if (0 == strcmp(args[arg_i], "-b") ||
			0 == strcmp(args[arg_i], "--bpp"))
		{
			if (argc <= ++arg_i) return 0;

			bpp = atof(args[arg_i]);

			if (argc <= ++arg_i) return 0;
		}
	}

	// фиксированная lambda

	if (2 == method)
	{
		if (0 == strcmp(args[arg_i], "-l") ||
			0 == strcmp(args[arg_i], "--lambda"))
		{
			if (argc <= ++arg_i) return 0;

			lambda = atof(args[arg_i]);

			if (argc <= ++arg_i) return 0;
		}
	}

	// пути исходного и сжатого файла
	const std::string srt_path = args[arg_i];
	if (argc <= ++arg_i) return 0;

	const std::string result_path = args[arg_i];

	// создание енкодера
	wic::encoder *const encoder = mk_encoder(srt_path, lvls, channel);

	if (0 == encoder)
	{
		std::cout << "Error: can't create encoder" << std::endl;
		return -1;
	}

	wic::encoder::tunes_t header;
	if (0 == method) 
	{
		std::cout << "Operation mode: manual" << std::endl;
		std::cout << "q: " << q << std::endl;
		std::cout << "lambda: " << lambda << std::endl;
		// encoder->encode_0(q, lambda, header);
	}
	else if (1 == method)
	{
		std::cout << "Operation mode: fixed bpp" << std::endl;
		std::cout << "bpp: " << bpp << std::endl;
		// encoder->encode_1(bpp, header);
	}
	else if (2 == method)
	{
		std::cout << "Operation mode: fixed lambda" << std::endl;
		std::cout << "lambda: " << lambda << std::endl;
		// encoder->encode_2(lambda, header);
	}
	else return 0;

	// запись результат в файл
	encode(*encoder, header, result_path);

	return 0;
}
