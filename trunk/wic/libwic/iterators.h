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
	использоваться для хранения двемерных координат. Тип должен поддерживать:
	- тип \c size_type, обозначающий тип, используемый для координат
	- поле x
*/
template <class point_t>
class basic_square_iterator: public basic_iterator<point_t> {
public:
	// public types ------------------------------------------------------------

	//! \brief Тип используемый для представления двумерных координат
	typedef point_t point_type;

	// public methods ----------------------------------------------------------

	//! \brief Конструктор
	/*!	\param[in] top_left
		\param[in] bottom_right
	*/
	basic_square_iterator(const point_t &top_left,
						  const point_t &bottom_right):
		top_left.get_x()
	{

	}

	//! \brief Копирующий конструктор
	basic_square_iterator(const basic_square_iterator &src):
		_point(src._point)
	{}

	//!	\brief Деструктор
	~basic_square_iterator();

	//! \brief Определение виртуального basic_iterator::get()
	virtual const ival_t &get() const = 0;

	//! \brief Определение виртуального basic_iterator::next()
	virtual const ival_t &next() = 0;

	//! \brief Определение виртуального basic_iterator::end()
	virtual const bool end() const = 0;

protected:
private:
	// private data ------------------------------------------------------------

	//! \brief Текущее значение итератора
	point_t _point;

};



}	// wtc namespace



#endif	// WIC_LIBWIC_ITERATORS_INCLUDED
