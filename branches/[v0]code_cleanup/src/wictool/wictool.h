/*!	\file     wictool.h
	\version  0.0.1
	\author   mice, ICQ: 332-292-380, mailto:wonder.mice@gmail.com
	\brief    Главный заголовочный файл утилиты wictool

	\todo     Более подробно описать файл wictool.h
*/

#ifndef WIC_WICTOOL_WICTOOL
#define WIC_WICTOOL_WICTOOL

///////////////////////////////////////////////////////////////////////////////
// include

// standard C++ library headers
#include <string>

// libwic headers
#include <wic/libwic/types.h>


///////////////////////////////////////////////////////////////////////////////
// public data types

//!	\brief Структура описывает два bmp файла с выбранными цветовыми слоями
//!	(каналами) для последующего их сравнения
/*!	Данная структура используется для получения путей к двум файлам и
	идентификаторов цветовых слоёв (каналов). Используется как входные данные
	для функций сравнения двух изображений.

	Выбранный канал rgb изображения представляется ASCII символами:
	- 'r' - Красный
	- 'g' - Зелёный
	- 'b' - Синий
	- 'x' - Все каналы
*/
struct bmp_file_diff_src_t
{
	//!	\brief Канал первого файла
	char channel1;
	//!	\brief Путь к первому файлу
	std::string file1;
	//!	\brief Канал второго файла
	char channel2;
	//!	\brief Путь ко второму файлу
	std::string file2;
};

//!	\brief Содержит биты цветового канала изображения и его характеристики
/*!	\attention Необходимо использовать функцию free_bmp_channel_bits() для
	освобождения ресурсов занятых структурой.
*/
struct bmp_channel_bits
{
	//!	\brief Биты цветового канала изображения
	unsigned char *data;
	//!	\brief Использованный цветовой канал
	char channel;
	//!	\brief Размер данных цветового канала (в байтах)
	unsigned int sz;
	//!	\brief Ширина цветового канала изображения
	unsigned int w;
	//!	\brief Высота цветового канала изображения
	unsigned int h;
};


//!	\brief Параметры кодирования
struct encode_params_t
{
	//!	\brief Используемый квантователь
	wic::q_t q;
	//!	\brief Баланс качество / степень сжатия
	wic::lambda_t lambda;
	//!	\brief Желаемые битовые затраты
	wic::h_t bpp;
	//!	\brief Количество уровней вейвлет преобразования
	unsigned int steps;
};

//!	\brief Спектр вейвлет преобразования (коэффициенты разложения
//!	и сопутствующая информация)
struct spectre_t
{
	//!	\brief Массив коэффициентов вейвлет преобразования
	wic::w_t *data;
	//!	\brief Количество коэффициентов вейвлет преобразования
	unsigned int sz;
	//!	\brief Ширина вейвлет спектра
	unsigned int w;
	//!	\brief Высота вейвлет спектра
	unsigned int h;
};

//!	\brief Простейшая статистика цветового канала
struct simple_stat_t
{
	//!	\brief Значение минимального элемента
	unsigned char min;
	//!	\brief Значение максимального элемента
	unsigned char max;
	//!	\brief Среднее значение элементов
	unsigned char avg;
};



///////////////////////////////////////////////////////////////////////////////
// public function declarations

//!	\name Работа с цветовыми каналами
//@{

//!	\brief Обрабатывает массив параметров (как в командной строке) и получает
//!	идентификатор цветового канала и путь к bmp файлу
int get_bmp_file_and_channel(const int argc, const char *const *const args,
							 char &channel, std::string &path,
							 std::ostream *const err = 0);

//!	\brief Читает изображение и выделяет из него выбранный цветовой канал
bmp_channel_bits get_bmp_channel_bits(const std::string &path,
									  const char channel,
									  std::ostream *const err = 0);

//!	\brief Освобождает динамическую память связанную со структурой
//!	bmp_channel_bits
void free_bmp_channel_bits(bmp_channel_bits &bits);

//!	\brief Проверяет, содержится ли в структуре bmp_channel_bits
//!	полезная информация
bool is_channel_bits_good(const bmp_channel_bits &bits,
						  std::ostream *const err = 0);

//@}


//!	\name Сравнение изображений
//@{

//!	\brief Обрабатывает массив параметров (как в командной строке) и
//!	заполняет структуру, используемую другими функциями для сравнения двух
//!	изображений
int get_bmp_file_diff_src(const int argc, const char *const *const args,
						  bmp_file_diff_src_t &result,
						  std::ostream *const err = 0);

//!	\brief Проверяет правомерность сравнения двух цветовых каналов
bool is_diff_allowed(const bmp_channel_bits &bits1,
					 const bmp_channel_bits &bits2,
					 std::ostream *const err = 0);

//!	\brief Производит подсчёт оценки <i>PSNR</i> двух цветовых каналов
double psnr(const bmp_channel_bits &bits1, const bmp_channel_bits &bits2,
			std::ostream *const err = 0);

//!	\brief Выводит на экран значение оценки <i>PSNR</i> двух цветовых
//!	каналов
int psnr(const bmp_file_diff_src_t &diff_src,
		 std::ostream *const err = 0);

//!	\brief Подсчитывает простейшую статистику
simple_stat_t calc_simple_stat(const bmp_channel_bits &bits,
							   std::ostream *const err = 0);

//!	\brief Выводит статистику разницы двух цветовых каналов
int stat(const bmp_channel_bits &bits1, const bmp_channel_bits &bits2,
		 std::ostream *const out = 0, std::ostream *const err = 0);

//!	\brief Выводит статистику разницы двух цветовых каналов
int stat(const bmp_file_diff_src_t &diff_src,
		 std::ostream *const err = 0);

//@}


//!	\name Работа с вейвлет преобразованиями
//@{

//!	\brief Получает название вейвлет преобразования из командной строки
int get_wavelet_filter(const int argc, const char *const *const args,
					   std::string &result, std::ostream *const err = 0);

//!	\brief Выполняет выбранное вейвлет преобразование
spectre_t forward_transform(const std::string &filter,
							const unsigned int steps,
							const bmp_channel_bits &bits,
							std::ostream *const err = 0);

//!	\brief Освобождает память, занимаемую спектром вейвлет коэффициентов
void free_spectre(spectre_t &spectre);

//@}


//!	\name Кодирование и декодирование
//@{

//!	\brief Получает название вейвлет преобразования из командной строки
int get_encode_method(const int argc, const char *const *const args,
					  std::string &result, std::ostream *const err = 0);

//!	\brief Получает параметры кодирования
int get_encode_params(const int argc, const char *const *const args,
					  encode_params_t &params, std::ostream *const err = 0);

//!	\brief Получает путь к сжатому файлу
int get_wic_file(const int argc, const char *const *const args,
				 std::string &wic_file, std::ostream *const err = 0);

//@}


//!	\name Вспомогательные функции
//@{

//!	\brief Here's to the crazy ones.
void crazy_ones(std::ostream &out);

//!	\brief Выводит информацию о утилите
void about(std::ostream &out);

//!	\brief Выводит информацию по пользованию утилитой
int usage(std::ostream &out);

//@}



#endif	// WIC_WICTOOL_WICTOOL
