/*******************************************************************************
* file:         iterators.h                                                    *
* version:      0.1.0                                                          *
* author:       mice (mailto:my_mice@mail.ru, ICQ:332-292-380)                 *
* description:  not available                                                  *
* tests:        none                                                           *
*******************************************************************************/

#pragma once

#ifndef WIC_LIBWIC_ITERATORS_INCLUDED
#define WIC_LIBWIC_ITERATORS_INCLUDED

////////////////////////////////////////////////////////////////////////////////
// headers

// standard C++ library headers
#include <assert.h>

// libwic headers
#include <wic/libwic/types.h>


////////////////////////////////////////////////////////////////////////////////
// wic namespace
namespace wic {


////////////////////////////////////////////////////////////////////////////////
// basic_iterator class declaration
//! \brief Базовый класс для итераторов
/*!	Предоставляет общий интерфейс для итератора.

	В процессе итерации будет изменяться объект типа \с ival_t
*/
template <class ival_t>
class basic_iterator {
	public:
	// public types ------------------------------------------------------------

	//! \brief Псевдоним для параметра шаблона, который доступен для
	//!	пользователей класса
	typedef ival_t ival_type;

	// public methods ----------------------------------------------------------

	//! \brief Возвращает константную ссылку на текущее значение итератора
	/*!	\return Текущее значение итератора
	*/
	virtual const ival_t &get() const = 0;

	//! \brief Переходит к следующиму значению
	/*!	\return Новое значение итератора значение итератора
	*/
	virtual const ival_t &next() = 0;

	//! \brief Проверяет, закончилась ли итереция
	/*!	\return \c true если достигли конца последовательности
	*/
	virtual const bool end() const = 0;

};



////////////////////////////////////////////////////////////////////////////////
// base_square_iterator class declaration
//! \brief Итератор по двумерной квадратной области
/*! Предоставляет простейшую реализацию итератора по квадратной двумерной
	области.

	Класс-параметр шаблона \c point_t определяет тип, который будет
	использоваться для хранения двумерных координат. Интерфейс типа должен
	поддерживать:
	- тип \c size_type, обозначающий тип, используемый для координат
	- метод size_type x() возвращающий координату x
	- метод size_type y() возвращающий координату y
*/
template <class point_t>
class basic_square_iterator: public basic_iterator<point_t> {
public:
	// public types ------------------------------------------------------------

	//! \brief Тип используемый для представления двумерных координат
	typedef point_t point_type;

	// public methods ----------------------------------------------------------

	//! \brief Конструктор
	/*!	\param[in] start Начальная точка, с которой начнётся итерация
		\param[in] top_left Координаты верхнего-левого угла ограничивающего
		прямоугольника 
		\param[in] bottom_right Координаты нижнего-правого угла
		ограничивающего прямоугольника

		Координаты указываются включительно, т.е. все координаты указывают
		на реальные точки двумерной области.
	*/
	basic_square_iterator(const point_t &start,
						  const point_t &top_left,
						  const point_t &bottom_right):
		_point(start),
		_top_left(top_lef), _bottom_right(bottom_right)
	{
		assert(_top_left.getx()	<= _point.getx());
		assert(_point.getx()	<= _bottom_right.getx());
		assert(_top_left.gety()	<= _point.gety());
		assert(_point.gety()	<= _bottom_right.gety());
	}

	//! \brief Копирующий конструктор
	/*!	\param[in] src Исходный объект
	*/
	basic_square_iterator(const basic_square_iterator &src):
		_point(src._point),
		_top_left(src._top_lef), _bottom_right(src._bottom_right)
	{
	}

	//!	\brief Деструктор
	~basic_square_iterator();

	//! \brief Определение виртуального basic_iterator::get()
	virtual const point_t &get() const {
		return _point;
	}

protected:
	// protected methods--------------------------------------------------------

	//!	\brief Сдвигает текущие координаты влево
	/*!	\param[in] x Смещение сдвига по оси x
		\return \c true если такой сдвиг возможен и был осуществлён, иначе
		\c false
	*/
	bool _move_left(const typename point_t::size_type &x) {
		if (_top_left.getx() <=_point.getx() - x) return false;

		_point.getx() -= x;

		return true;
	}

	//!	\brief Сдвигает текущие координаты вправо
	/*!	\param[in] x Смещение сдвига по оси x
		\return \c true если такой сдвиг возможен и был осуществлён, иначе
		\c false
	*/
	bool _move_right(const typename point_t::size_type &x) {
		if (_point.getx() + x <= _bottom_right.getx()) return false;

		_point.getx() += x;

		return true;
	}

	//!	\brief Сдвигает текущие координаты вверх
	/*!	\param[in] y Смещение сдвига по оси y
		\return \c true если такой сдвиг возможен и был осуществлён, иначе
		\c false
	*/
	bool _move_up(const typename point_t::size_type &y) {
		if (_top_left.gety() <=_point.gety() - y) return false;

		_point.gety() -= y;

		return true;
	}

	//!	\brief Сдвигает текущие координаты вниз
	/*!	\param[in] y Смещение сдвига по оси y
		\return \c true если такой сдвиг возможен и был осуществлён, иначе
		\c false
	*/
	bool _move_down(const typename point_t::size_type &y) {
		if (_point.gety() + y <= _bottom_right.gety()) return false;

		_point.gety() += y;

		return true;
	}

private:
	// private data ------------------------------------------------------------

	//! \brief Текущее значение итератора
	point_t _point;

	//! \brief Координаты левого-верхнего угла ограничивающего прямоугольника
	point_t _top_left;

	//! \brief Координаты нижнего-правого угла ограничивающего прямоугольника
	point_t _bottom_right;

};


class snake_square_iterator: public basic_square_iterator<p_t> {
};



}	// wtc namespace



#endif	// WIC_LIBWIC_ITERATORS_INCLUDED
