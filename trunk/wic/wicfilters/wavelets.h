/*!	\file     wavelets.h
	\version  0.0.1
	\author   mice, ICQ: 332-292-380, mailto:wonder.mice@gmail.com
	\brief    Описания функций и классов для работы с унифицированным
			  интерфейсом вейвлет фильтров (преобразований)
	
	\ref pgCoding_sectUnifiedWaveletFilters.
*/

/*! \page pgCoding Реализация

	\section pgCoding_sectUnifiedWaveletFilters Вейвлет фильты
*/

#pragma once

#ifndef WIC_WICFILTERS_WAVELETS_INCLUDED
#define WIC_WICFILTERS_WAVELETS_INCLUDED

////////////////////////////////////////////////////////////////////////////////
// headers

// standard C++ library headers
#include <assert.h>
#include <string>

// wavelets headers
#include <wavelets/cdf97/cdf97.h>

// libwic headers
#include <wic/libwic/types.h>


////////////////////////////////////////////////////////////////////////////////
// wic namespace
namespace wic {


////////////////////////////////////////////////////////////////////////////////
// smart_2d_matrix class declaration
//!	\brief Умная двумерная матрица
/*! Двумерная матрица, поддерживающая механизм подсчёта ссылок.
*/
template <class value_t>
class smart_2d_matrix
{
public:
	// public types ------------------------------------------------------------

	//!	\brief Тип элементов матрицы
	typedef value_t value_type;

	//!	\brief Тип самой матрицы (параметризованной)
	typedef smart_2d_matrix<value_type> this_type;

	// public constants --------------------------------------------------------

	//! \brief Начальное значение для счётчика ссылок
	static const int INIT_REF_COUNT			= 1;

	// public methods ----------------------------------------------------------

	//!	\name Конструкторы и деструкторы
	//@{

	//! \brief Конструктор
	/*!	\param[in] width Ширина матрицы
		\param[in] height Высота матрицы
	*/
	smart_2d_matrix(const int width, const int height)
	{
		_alloc_matrix(width, height);
	}

	//!	\brief Конструктор обладания
	/*!	\param[in] elements Массив элементов, который будет использоваться
		матрицей
		\param[in] width Ширина матрицы
		\param[in] height Высота матрицы
	*/
	smart_2d_matrix(value_type *const elements,
					const int width, const int height):
		_width(width), _height(height)
		_matrix(elements), _ref_count_ptr(0),
	{
		// проверка указателей
		assert(0 != _matrix);

		// проверка геометрических размеров
		assert(0 < _width && 0 < _height);
	}

	//!	\brief Конструктор с инициализацией и преобразованием элементов
	/*!	\param[in] elements Массив, преобразованные элементы которого
		будут помещены в матрицу
		\param[in] width Ширина матрицы
		\param[in] height Высота матрицы
	*/
	template <class value_from_t>
	smart_2d_matrix(value_from_t *const elements,
					const int width, const int height):
	{
		_alloc_matrix(width, height);
		_copy_elements<value_from_t>(elements);
	}

	//!	\brief Копирующий конструктор
	/*!	\param[in] src Исходная матрица

		\attention Исходная и результирующая матрицы будут использовать общие
		данные через механизм подсчёта ссылок.
	*/
	smart_2d_matrix(const this_type &src):
		_width(src._width), _height(src._height), 
		_matrix(src._matrix),
		_ref_count_ptr(src._ref_count_ptr)
	{
		// увеличить счётчик ссылок, если он используется
		if (0 != _ref_count_ptr) ++(*_ref_count_ptr);
	}

	//!	\brief Копирующий конструктор с преобразованием
	/*!	\param[in] src Исходная матрица

		\attention Исходная и результирующая матрицы будут использовать
		раздедьные данные.
	*/
	template <class value_from_t>
	smart_2d_matrix(const smart_2d_matrix<value_from_t> &src)
	{
		_alloc_matrix(src._width, src._height);
		_copy_elements<value_from_t>(src._matrix);
	}

	//!	\brief Деструктор
	/*! Уменьшает количество ссылок на матрицу, и если он становится
		равным <i>0</i> освобождает ресурсы, занимаемые матрицей.
	*/
	~smart_2d_matrix()
	{
		// если счётчик ссылок не используется, ничего освобождать не надо
		if (0 != _ref_count_ptr) return;

		// уменьшение счётчика ссылок
		--(*_ref_count_ptr);

		// проверка достижения счётчиком ссылок значения 0
		if (0 >= (*_ref_count_ptr))
		{
			// освобождение матрицы
			_free_matrix();
		}
	}

	//@}

	//!	\name Информация о матрице
	//@{

	//!	\brief Ширина матрицы
	int width() const { return _width; }

	//!	\brief Высота матрицы
	int height() const { return _height; }

	//@}

	//!	\name Доступ к элементам матрицы
	//@{

	//!	\brief Возвращает константный указател на элементы матрицы
	const value_type *data() const { return _matrix; }

	//!	\brief Возвращает указател на элементы матрицы
	value_type *data() { return _matrix; }

	//!	\brief Возвращает константную ссылку на элемент лежащий по
	//!	координатам <i>(x, y)</i>
	/*!	\param[in] x Координата <i>x</i>
		\param[in] y Координата <i>y</i>
	*/
	const value_type &at(const int x, const int y) const
	{
		return _matrix[_width * y + x];
	}

	//!	\brief Возвращает ссылку на элемент лежащий по координатам
	//!	<i>(x, y)</i>
	/*!	\param[in] x Координата <i>x</i>
		\param[in] y Координата <i>y</i>
	*/
	value_type &at(const int x, const int y)
	{
		return _matrix[_width * y + x];
	}

	//!	\brief Копирует элементы матрицы из массива используя преобразование
	//!	типов
	/*!	\param[in] elements Массив с элементами
	*/
	template <class value_from_t>
	void load(const value_from_t *const elements)
	{
		_copy_elements(elements);
	}

	//@}

protected:
	// protected methods -------------------------------------------------------

	//!	\name Работа с памятью
	//@{

	//!	\brief Выделяет память под матрицу
	/*!	\param[in] width Ширина матрицы
		\param[in] height Высота матрицы
	*/
	void _alloc_matrix(const int width, const int height)
	{
		// геометрические размеры матрицы
		_width = width;
		_height = height;

		// проверка геометрических размеров
		assert(0 < _width && 0 < _height);

		// выделение памяти под элементы матрицы
		_matrix = new value_type[_width * _height];

		// выделение памяти под счётчик ссылок
		_ref_count_ptr = new int(INIT_REF_COUNT);

		// проверка указателей
		assert(0 != _matrix);
		assert(0 != _ref_count_ptr);
	}

	//!	\brief Освобождает ресурсы, занимаемые матрицей
	/*!
	*/
	void _free_matrix()
	{
		// проверка указателей
		assert(0 != _matrix);
		assert(0 != _ref_count_ptr);

		// освобождение массива элементов
		if (0 != _matrix)
		{
			delete[] _matrix;
			_matrix = 0;
		}

		// освобождение счётчика ссылок
		if (0 != _ref_count_ptr)
		{
			delete _ref_count_ptr;
			_ref_count_ptr = 0;
		}
	}

	//!	\brief Копирует элементы матрицы из массива используя преобразование
	//!	типов
	/*!	\param[in] elements Массив с элементами
	*/
	template <class value_from_t>
	void _copy_elements(const value_from_t *const elements)
	{
		// проверка указателей и геометрических размеров
		assert(0 != elements);
		assert(0 != _matrix);
		assert(0 < _width && 0 < _height);

		// поэлементное копирование с преобразованием
		for (int i = (_width * _height - 1); 0 <= i; --i)
		{
			_matrix[i] = (value_type)(elements[i]);
		}
	}

	//@}

private:
	// private data ------------------------------------------------------------

	//!	\brief Массив элементов матрицы
	value_t *_matrix;

	//!	\brief Ширина матрицы
	int _width;

	//!	\brief Высота матрицы
	int _height;

	//!	\brief Указатель на счётчик ссылок
	int *_ref_count_ptr;
};



////////////////////////////////////////////////////////////////////////////////
// wavelets class declaration

//!	\brief 
/*!	
*/	
template <class src_t, class dest_t>
static smart_2d_matrix<dest_t>
forward_2d_transform(const src_t *const src, const std::string &filter,
					 const int width, const int height, const int levels)
{
	smart_2d_matrix<float> result(width, height);
	result.load<src_t>(src);
	wt2d_cdf97_fwd(result.data(), height, width, levels);

	return result;
}

template <class src_t, class dest_t>
static smart_2d_matrix<dest_t>
inverse_2d_transform(const src_t *const src, const std::string &filter,
					 const int width, const int height, const int levels)
{
	smart_2d_matrix<float> result(width, height);
	result.load<src_t>(src);
	wt2d_cdf97_inv(result.data(), height, width, levels);

	return result;
}



}	/// end of namespace wic



#endif	// WIC_WICFILTERS_WAVELETS_INCLUDED
