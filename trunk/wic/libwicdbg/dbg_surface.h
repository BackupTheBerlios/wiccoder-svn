/*!	\file     dbg_surface.h
	\version  0.0.1
	\author   mice, ICQ: 332-292-380, mailto:wonder.mice@gmail.com
	\brief    Описание класса wicdbg::dbg_surface - поверхности для отладки
			  процесса кодирования

	\todo     Более подробно описать файл dbg_surface.h
*/

#ifndef WIC_LIBWICDBG_DBG_SURFACE
#define WIC_LIBWICDBG_DBG_SURFACE

///////////////////////////////////////////////////////////////////////////////
// include

// standard C++ library headers
#include <assert.h>
#include <math.h>
#include <string>
#include <fstream>
#include <iomanip>

// imgs headers
#include <imgs/bmp_file.h>
#include <imgs/bmp_dump.h>

// libwic headers
#include <wic/libwic/types.h>

// libwicdbg headers
#include <wic/libwicdbg/dbg_pixel.h>


///////////////////////////////////////////////////////////////////////////////
// wicdbg namespace
namespace wicdbg
{


////////////////////////////////////////////////////////////////////////////////
// dbg_surface class declaration
//!	\brief Отладочная поверхность, которая позволяет сохранять дополнительную
//!	информацию о кодируемых коэффициентах и представлять её в удобной форме.
/*!
*/
class dbg_surface
{
public:
	// public types ------------------------------------------------------------

	//!	\brief Псевдоним для типа imgs::img_rgb::rgb24_t, чтобы упростить его
	//!	использование
	typedef imgs::img_rgb::rgb24_t rgb24_t;

	// public constants --------------------------------------------------------

	//!	\brief Номер не существующей модели арифметического кодера
	static const wic::sz_t INVALID_MODEL	= wic::MAX_ACODER_MODELS;

	// public methods ----------------------------------------------------------

	//!	\name Конструкторы и деструкторы
	//@{

	//!	\brief Конструктор
	dbg_surface(const wic::sz_t w, const wic::sz_t h);

	//!	\brief Деструктор
	~dbg_surface();

	//@}

	//!	\brief Информация о поверхности
	//@{

	//!	\bref Возвращает ширину поверхности
	wic::sz_t width() const { return _w; }

	//!	\bref Возвращает высоту поверхности
	wic::sz_t height() const { return _h; }

	//!	\bref Возвращает общее количество элементов в поверхности
	wic::sz_t size() const { return _sz; }

	//@}

	//!	\brief Работа с поверхностью
	//@{

	//!	\brief Производит очистку поверхности, заполняя все поля элементов
	//! изначальными значениями
	void clear();

	//!	\brief Возвращает константную ссылку на элемент с определённым
	//!	смещением
	const dbg_pixel &get(const wic::sz_t i) const;

	//!	\brief Возвращает ссылку на элемент с определёнными смещением
	dbg_pixel &get(const wic::sz_t i);

	//!	\brief Возвращает константную ссылку на элемент с определёнными
	//!	координатами
	const dbg_pixel &get(const wic::sz_t x, const wic::sz_t y) const;

	//!	\brief Возвращает ссылку на элемент с определёнными координатами
	dbg_pixel &get(const wic::sz_t x, const wic::sz_t y);

	//!	\brief Возвращает константную ссылку на элемент с определёнными
	//!	координатами
	const dbg_pixel &get(const wic::p_t &p) const;

	//!	\brief Возвращает ссылку на элемент с определёнными координатами
	dbg_pixel &get(const wic::p_t &p);

	//@}

	//!	\brief Экспорт поверхности в файл
	//@{

	//!	\brief
	/*!
	*/
	template <const dbg_pixel::members member>
	void save(const std::string &path, const bool as_bmp = false) const
	{
		// проверка утверждений
		assert(0 < _w && 0 < _h && 0 < _sz);
		assert(0 != _surface);

		// псевдоним для типа используемого члена
		typedef dbg_pixel::type_selector<member>::result member_type;

		// создание временного массива
		member_type *const data = new member_type[_sz];

		// копирование значения выбранного члена во временный масив
		for (wic::sz_t i = 0; _sz > i; ++i)
		{
			data[i] = _surface[i].get<member>();
		}

		// сохранение в выбранном формате
		if (as_bmp)
		{
			imgs::bmp_dump<member_type, wic::sz_t>::dump(data, _w, _h,
														 path);
		}
		else
		{
			imgs::bmp_dump<member_type, wic::sz_t>::txt_dump(data, _w, _h,
															 path);
		}

		// освобождение временного массива
		delete[] data;
	}

	//!	\ brief
	/*!
	*/
	template <const dbg_pixel::members member>
	void diff(const dbg_surface &another, const std::string &diff_path)
	{
		// проверка утверждений
		assert(size()	== another.size());
		assert(width()	== another.width());
		assert(height()	== another.height());

		// поток для вывода информации о различиях
		std::ofstream diff_out(diff_path.c_str());

		// тип члена с которым производится работа
		typedef dbg_pixel::type_selector<member>::result member_type;

		// количество найденных различий
		wic::sz_t diffs_count = 0;

		// цикл по всей поверхности
		for (wic::sz_t y = 0; height() > y; ++y)
		{
			for (wic::sz_t x = 0; width() > x; ++x)
			{
				// получение значений членов различных поверхностей
				const member_type a = get(x, y).get<member>();
				const member_type b = another.get(x, y).get<member>();

				// сравнение значений
				if (a == b) continue;

				// увеличение количества найденных различий
				++diffs_count;

				// запись информации о различии в файл
				diff_out << std::setw(5) << diffs_count << " - ["
						 << std::setw(4) << x << ", "
						 << std::setw(4) << y << "]: a = "
						 << a << ", b = " << b << std::endl;
			}
		}
	}

	//@}

	//!	\name Статические методы
	//@{

	template <class T>
	static bool diff_dump(const T *const src1, const T *const src2,
						  const std::string &path,
						  const wic::sz_t w, const wic::sz_t h,
						  const bool as_bmp = false)
	{
		// Указатели на источники должны быть не нулевыми
		assert(0 != src1 && 0 != src2);

		// Количество элементов в каждом источнике
		const wic::sz_t sz = w * h;

		// Выделение памяти под разность
		T *const diff = (T *)malloc(sz * sizeof(T));

		if (0 == diff) return false;

		// Подсчёт разницы
		for (int i = 0; sz > i; ++i)
		{
			diff[i] = (src1[i] - src2[i]);
		}

		// сохранение в выбранном формате
		if (as_bmp)
		{
			imgs::bmp_dump<T, wic::sz_t>::dump(diff, w, h, path);
		}
		else
		{
			imgs::bmp_dump<T, wic::sz_t>::txt_dump(diff, w, h, path, 10);
		}

		free(diff);

		return true;
	}

	//@}

protected:
	// protected members -------------------------------------------------------

	//!	\brief Возвращает смещение элемента по его координатам
	wic::sz_t _offset(const wic::sz_t x, const wic::sz_t y) const;

private:
	// private data ------------------------------------------------------------

	//!	\brief Ширина поверхности
	const wic::sz_t _w;

	//!	\brief Высота поверхности
	const wic::sz_t _h;

	//!	\brief Количество элементов в поверхности
	const wic::sz_t _sz;

	//!	\brief Содержание поверхности
	dbg_pixel *_surface;

};



}	// end of namespace wicdbg



#endif	// WIC_LIBWICDBG_DBG_SURFACE
