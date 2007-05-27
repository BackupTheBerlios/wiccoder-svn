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
#include <new>							// for std::bad_alloc exception class
#include <assert.h>
#include <vector>

// libwic headers
#include <wic/libwic/types.h>


////////////////////////////////////////////////////////////////////////////////
// wic namespace
namespace wic {


////////////////////////////////////////////////////////////////////////////////
// basic_iterator class declaration
//! \brief Базовый класс для итераторов
/*!	Предоставляет общий интерфейс для итератора.

	В процессе итерации будет изменяться объект типа <i>ival_t</i>
*/
template <class ival_t>
class basic_iterator {
	public:
	// public types ------------------------------------------------------------

	//! \brief Псевдоним для параметра шаблона, который доступен для
	//!	пользователей класса
	typedef ival_t ival_type;

	// public methods ----------------------------------------------------------

	//!	\brief Деструктор. Виртуальный, как полагается.
	virtual ~basic_iterator() {}

	//! \brief Возвращает константную ссылку на текущее значение итератора
	/*!	\return Текущее значение итератора
	*/
	virtual const ival_t &get() const = 0;

	//! \brief Переходит к следующиму значению
	/*!	\return Новое значение итератора значение итератора

		Значение итератора может быть неверным (или не существующим), если
		достигнут конец последовательности итераций и
		basic_iterator::end() возвращает <i>true</i>. Таким образом, после
		каждого вызова basic_iterator::next() необходимо проверять
		условие завершения последовательности итераций.
	*/
	virtual const ival_t &next() = 0;

	//! \brief Проверяет, закончилась ли итереция
	/*!	\return \c true если достигли конца последовательности
	*/
	virtual const bool end() const = 0;

};



////////////////////////////////////////////////////////////////////////////////
// base_square_iterator class declaration
//! \brief Итератор по двумерной прямоугольной области
/*! Предоставляет простейшую реализацию итератора по прямоугольной двумерной
	области.

	Класс-параметр шаблона \c point_t определяет тип, который будет
	использоваться для хранения двумерных координат. Интерфейс типа должен
	поддерживать:
	- тип \c size_type, обозначающий тип, используемый для координат
	- метод size_type getx() возвращающий координату x
	- метод size_type gety() возвращающий координату y

	\todo Переименовать basic_square_iterator в basic_2d_iterator
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
		_top_left(top_left), _bottom_right(bottom_right)
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
		_top_left(src._top_left), _bottom_right(src._bottom_right)
	{
	}

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
	bool _move_left(const typename point_t::size_type &x = 1) {
		if (_top_left.getx() + x > _point.getx()) return false;

		_point.getx() -= x;

		return true;
	}

	//!	\brief Сдвигает текущие координаты вправо
	/*!	\param[in] x Смещение сдвига по оси x
		\return \c true если такой сдвиг возможен и был осуществлён, иначе
		\c false
	*/
	bool _move_right(const typename point_t::size_type &x = 1) {
		if (_point.getx() + x > _bottom_right.getx()) return false;

		_point.getx() += x;

		return true;
	}

	//!	\brief Сдвигает текущие координаты вверх
	/*!	\param[in] y Смещение сдвига по оси y
		\return \c true если такой сдвиг возможен и был осуществлён, иначе
		\c false
	*/
	bool _move_up(const typename point_t::size_type &y = 1) {
		if (_top_left.gety() + y > _point.gety()) return false;

		_point.gety() -= y;

		return true;
	}

	//!	\brief Сдвигает текущие координаты вниз
	/*!	\param[in] y Смещение сдвига по оси y
		\return \c true если такой сдвиг возможен и был осуществлён, иначе
		\c false
	*/
	bool _move_down(const typename point_t::size_type &y = 1) {
		if (_point.gety() + y > _bottom_right.gety()) return false;

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



////////////////////////////////////////////////////////////////////////////////
// snake_square_iterator class declaration
//!	\brief Итератор, проходящий прямоугольную область змейкой
/*! Образец выполняемого прохода:
	\code
	- - - - - - - - >
	                |
	                V
	< - - - - - - - -
	|
	V
	- - - - - - - - >
	                |
			        V
	< - - - - - - - -
	\endcode
*/
class snake_square_iterator: public basic_square_iterator<p_t> {
public:
	// public methods ----------------------------------------------------------

	//! \brief Конструктор
	/*!	\param[in] top_left Координаты верхнего-левого угла ограничивающего
		прямоугольника 
		\param[in] bottom_right Координаты нижнего-правого угла
		ограничивающего прямоугольника

		Координаты указываются включительно, т.е. все координаты указывают
		на реальные точки двумерной области.

		В качестве начальной точки будет взят верхний-левый угол.

		\sa basic_square_iterator
	*/
	snake_square_iterator(const point_type &top_left,
						  const point_type &bottom_right):
		basic_square_iterator<point_type>(top_left, top_left, bottom_right)
	{
		_points_left = (bottom_right.getx() -  top_left.getx() + 1) *
					   (bottom_right.gety() -  top_left.gety() + 1);

		_going_left = false;
	}

	//! \brief Копирующий конструктор
	/*!	\param[in] src Исходный объект
	*/
	snake_square_iterator(const snake_square_iterator &src):
		basic_square_iterator<point_type>(src), _points_left(src._points_left),
		_going_left(src._going_left)
	{
	}

	//! \brief Определение виртуального basic_iterator::next()
	virtual const point_type &next() {
		if (_going_left) {
			if (_move_left()) --_points_left;
			else if (_move_down()) {
				--_points_left;
				_going_left = false;
			} else _points_left = 0;
		} else {
			if (_move_right()) --_points_left;
			else if (_move_down()) {
				--_points_left;
				_going_left = true;
			} else _points_left = 0;
		}

		return get();
	}

	//! \brief Определение виртуального basic_iterator::end()
	virtual const bool end() const {
		return (0 >= _points_left);
	}

	//! \brief Количество оставшихся точек
	const sz_t &points_left() const { return _points_left; }

	//!	\brief Текущее направление прохода
	bool going_left() const { return _going_left; }

protected:
private:
	// private data ------------------------------------------------------------

	//!	\brief Количество итераций, оставшихся до конца
	sz_t _points_left;

	//!	\brief Текущее направление перемещения.
	bool _going_left;

};



////////////////////////////////////////////////////////////////////////////////
// snake_square_iterator class declaration
//!	\brief Итератор по дочерним элементам элемента из <i>LL</i> саббенда.
/*! Итератор по 3ём дочерним элементам элемента из <i>LL</i> саббенда.

	\note На данный момент реализация этого класса ужасна. Она использует
	динамически заполняемый std::vector и её быстродействие оставляет желать
	лучшего. Но так-как количество коэффициентов в <i>LL</i> саббенде обычно
	не так велико, оставим пока всё как есть.

	\todo Придумать более достойную реализацию этого класса
	\todo Этот класс необходимо протестировать
*/
class LL_children_iterator: public basic_iterator<p_t> {
public:
	// public constants --------------------------------------------------------

	//! \brief Количество дочерних элементов
	static const sz_t LL_CHILDREN_COUNT		= 3;

	// public methods ----------------------------------------------------------

	//! \brief Конструктор
	/*! \param[in] LL_w Ширина <i>LL</i> саббенда (в элементах)
		\param[in] LL_h Высота <i>LL</i> саббенда (в элементах)
		\param[in] p Координаты родительского элемента
	*/
	LL_children_iterator(const sz_t &LL_w, const sz_t &LL_h,
						 const p_t &p):
		_i(LL_CHILDREN_COUNT - 1)
	{
		_children.reserve(LL_CHILDREN_COUNT);
		_children.push_back(p_t(p.x + LL_w, p.y + LL_h));
		_children.push_back(p_t(p.x       , p.y + LL_h));
		_children.push_back(p_t(p.x + LL_w, p.y));
	}

	//! \brief Определение виртуального basic_iterator::get()
	virtual const p_t &get() const { return _children[_i]; }

	//! \brief Определение виртуального basic_iterator::next()
	virtual const p_t &next() {
		if (0 < _i) --_i;

		return get();
	}

	//! \brief Определение виртуального basic_iterator::end()
	virtual const bool end() const {
		return (0 == _i);
	}

protected:
	// protected types ---------------------------------------------------------

	//!	\brief Тип для массива координат дочерних элементов
	typedef std::vector<p_t> _children_t;

private:
	// private data ------------------------------------------------------------

	//!	\brief Индекс текущего дочернего элемента в массиве координат дочерних
	//!	элементов.
	_children_t::size_type _i;

	//!	\brief Массиве координат дочерних элементов.
	_children_t _children;

};



////////////////////////////////////////////////////////////////////////////////
// n_cutdown_iterator class declaration
//!	\brief Итератор по всевозможным вариантам способов подрезания ветвей
/*!	Производит постепенное увеличение количества подрезанных ветвей. В самом
	начале все ветви сохранены. Итератор использует простейший алгоритм:
	алгебраическое уменьшение группового признака подрезания.
*/
class n_cutdown_iterator: public basic_iterator<n_t>
{
public:
	// public constants --------------------------------------------------------

	//!	\brief Число вариантов подрезания и сохранения ветвей при нахождении
	//!	родительского элемента на нулевом саббенде (2^3)
	static const n_t MAX_VARIATIONS_LVL_0	= 8;

	//!	\brief Число вариантов подрезания и сохранения ветвей при нахождении
	//!	родительского элемента на ненулевом саббенде (2^4)
	static const n_t MAX_VARIATIONS_LVL_X	= 16;

	// public methods ----------------------------------------------------------

	//!	\brief Конструктор
	/*!	\param[in] lvl Уровень, на котором находится элементы с ассоциированным
		групповым признаком подрезания. Элементы с нулевого уровня имеют 3
		потомка, следовательно возможны 8 различных способов их подрезания. У
		всех остальных элементов 4 потомка и 16 различных способа подрезания.
	*/
	n_cutdown_iterator(const sz_t lvl): _end(false) {
		assert(0 <= lvl);

		_n = (0 == lvl)? (MAX_VARIATIONS_LVL_0 - 1): (MAX_VARIATIONS_LVL_X - 1);
	}

	//! \brief Определение виртуального basic_iterator::get()
	virtual const n_t &get() const { return _n; }

	//! \brief Определение виртуального basic_iterator::next()
	virtual const n_t &next() {
		(0 != _n)? --_n: _end = true;

		return _n;
	}

	//! \brief Определение виртуального basic_iterator::end()
	virtual const bool end() const { return _end; }

protected:
private:
	// private data ------------------------------------------------------------

	//!	\brief Текущее значение группового признака подрезания ветвей
	n_t _n;

	//!	\brief Признак завершения последовательности итераций
	bool _end;
};



////////////////////////////////////////////////////////////////////////////////
// some_iterator class declaration
//!	\brief Универсальная обёртка для итераторов
/*!	Этот класс предназначен для унификации кода. Функции должны возвращать
	объекты этого класса вместо самих итераторов. Если следовать этому подходу,
	переход на новый тип итераторов будет предельно простым - достаточно
	переписать функции генерирующие итераторы.

	\attention Не рекомендуется создавать итераторы в коде где они
	используются. Вместо этого лучше применять функции, генерирующие итераторы.

	\attention Класс имеет встроенный механизм подсчёта количества ссылок на
	контролируемый объект. Поэтому можно свободно пользоваться копирующим
	конструктором.

	\sa basic_iterator
*/
template <class ival_t>
class some_iterator {
public:
	// public types ------------------------------------------------------------

	//! \brief Тип контролируемого итератора
	typedef basic_iterator<ival_t> iterator_type;

	//! \brief Тип этого итератора (для удобства)
	typedef some_iterator<ival_t> self_type;

	// public constants --------------------------------------------------------

	//! \brief Начальное значение для счётчика ссылок
	static const int INIT_REF_COUNT			= 1;

	// public methods ----------------------------------------------------------

	//! \brief Конструктор
	/*!	\param[in] iterator Объект итератора над которым берётся шефство

		Функция создаёт счётчик ссылок на итератор и инициализирует его в 1
	*/
	some_iterator(iterator_type *const iterator):
		_iterator(iterator),
		_ref_count_ptr(new int(INIT_REF_COUNT))
	{
		assert(0 != _iterator);

		if (0 == _ref_count_ptr) throw std::bad_alloc();
	}

	//! \brief Копирующий конструктор
	/*! \param[in] src Исходный итератор

		Увеличивает значение счётчика ссылок на единицу
	*/
	some_iterator(const self_type &src):
		_iterator(src._iterator),
		_ref_count_ptr(src._ref_count_ptr)
	{
		assert(0 != _iterator);
		assert(0 != _ref_count_ptr);

		++(*_ref_count_ptr);
	}


	//! \brief Деструктор
	/*!	Уменьшает значение счётчика ссылок на единицу и если оно
		становится равным нулю - освобождает контролируемый итератор
		и счётчик ссылок.
	*/
	~some_iterator() {
		assert(0 != _ref_count_ptr);

		if (0 < --(*_ref_count_ptr)) return;

		if (0 != _iterator) delete _iterator;
		delete _ref_count_ptr;
	}

	//! \brief Возвращает указатель на контролируемый итератор
	iterator_type *operator->() const { return _iterator; }

	//! \brief Возвращает указатель на контролируемый итератор
	iterator_type *get() const { return _iterator; }

	//! \brief Возвращает ссылку на контролируемый итератор
	iterator_type &operator()() const { return (*_iterator); }

private:
	// private data ------------------------------------------------------------

	//! \brief Указатель на контролируемый итератор
	iterator_type *const _iterator;

	//! \brief Указатель на значение содержащее счётчик ссылок на
	//!	контролируемый итератор
	int *const _ref_count_ptr;

};



}	// wic namespace



#endif	// WIC_LIBWIC_ITERATORS_INCLUDED
