/*!	\file     iterators.h
	\version  0.0.1
	\author   mice, ICQ: 332-292-380, mailto:wonder.mice@gmail.com
	\brief    Определение классов и интерфейсов для итераторов

	\todo     Более подробно описать файл iterators.h
*/

#pragma once

#ifndef WIC_LIBWIC_ITERATORS_INCLUDED
#define WIC_LIBWIC_ITERATORS_INCLUDED

////////////////////////////////////////////////////////////////////////////////
// headers

// standard C++ library headers
#include <new>							// for std::bad_alloc exception class
#include <assert.h>
#include <vector>
#include <list>

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

	//!	\brief Конструктор (для порядка)
	basic_iterator() {}

	//!	\brief Деструктор. Виртуальный, как полагается.
	virtual ~basic_iterator() {}

	//! \brief Возвращает константную ссылку на текущее значение итератора
	/*!	\return Текущее значение итератора
	*/
	virtual const ival_t &get() const = 0;

	//! \brief Переходит к следующему значению
	/*!	

		Значение итератора может быть неверным (или не существующим), если
		достигнут конец последовательности итераций и
		basic_iterator::end() возвращает <i>true</i>. Таким образом, после
		каждого вызова basic_iterator::next() необходимо проверять
		условие завершения последовательности итераций.

		Не смотря на это, во многих классах итераторов <i>wiccoder</i>
		(но не во всех!) принято соглашение о
		завершении последовательности итераций. По нему не следует
		возвращать ссылку на несуществующий элемент, а вместо этого
		возвратить ссылку на предыдущий элемент, на котором закончилась
		последовательность итераций.
	*/
	virtual void next() = 0;

	//! \brief Проверяет, закончилась ли последовательность итереций
	/*!	\return <i>true</i> если достигли конца последовательности
	*/
	virtual const bool end() const = 0;

protected:
	// protected methods -------------------------------------------------------

	//! Явно запретим копирование итераторов
	basic_iterator(const basic_iterator &src) {}
};



////////////////////////////////////////////////////////////////////////////////
// basic_2d_iterator class declaration
//! \brief Итератор по двумерной прямоугольной области
/*! Предоставляет простейшую реализацию итератора по прямоугольной двумерной
	области.

	Класс-параметр шаблона \c point_t определяет тип, который будет
	использоваться для хранения двумерных координат. Интерфейс типа должен
	поддерживать:
	- тип \c size_type, обозначающий тип, используемый для координат
	- метод size_type getx() возвращающий координату x
	- метод size_type gety() возвращающий координату y
*/
template <class point_t>
class basic_2d_iterator: public basic_iterator<point_t> {
public:
	// public types ------------------------------------------------------------

	//! \brief Тип используемый для представления двумерных координат
	typedef point_t point_type;

	//!	\brief Тип используемый в <i>point_type</i> для представления
	//!	координат
	typedef typename point_t::size_type size_type;

	// public methods ----------------------------------------------------------

	//! \brief Конструктор
	/*!	\param[in] start Начальная точка, с которой начнётся
		последовательность итераций
		\param[in] top_left Координаты верхнего-левого угла ограничивающего
		прямоугольника 
		\param[in] bottom_right Координаты нижнего-правого угла
		ограничивающего прямоугольника

		Координаты указываются включительно, то есть все координаты указывают
		на реальные точки двумерной области.
	*/
	basic_2d_iterator(const point_t &start,
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
	basic_2d_iterator(const basic_2d_iterator &src):
		_point(src._point),
		_top_left(src._top_left), _bottom_right(src._bottom_right)
	{
		assert(_top_left.getx()	<= _point.getx());
		assert(_point.getx()	<= _bottom_right.getx());
		assert(_top_left.gety()	<= _point.gety());
		assert(_point.gety()	<= _bottom_right.gety());
	}

	//! \brief Определение виртуального basic_iterator::get()
	virtual const point_t &get() const {
		return _point;
	}

protected:
	// protected methods--------------------------------------------------------

	//!	\brief Сдвигает текущие координаты влево
	/*!	\param[in] dx Величина сдвига по оси x (0 <= <i>dx</i>)
		\return <i>true</i> если такой сдвиг возможен и был осуществлён, иначе
		\c <i>false</i>
	*/
	bool _move_left(const size_type &dx = 1) {
		if (_top_left.getx() + dx > _point.getx()) return false;

		_point.getx() -= dx;

		return true;
	}

	//!	\brief Сдвигает текущие координаты вправо
	/*!	\param[in] dx Величина сдвига по оси x (0 <= <i>dx</i>)
		\return <i>true</i> если такой сдвиг возможен и был осуществлён, иначе
		<i>false</i>
	*/
	bool _move_right(const size_type &dx = 1) {
		if (_point.getx() + dx > _bottom_right.getx()) return false;

		_point.getx() += dx;

		return true;
	}

	//!	\brief Сдвигает текущие координаты вверх
	/*!	\param[in] dy Величина сдвига по оси y (0 <= <i>dy</i>)
		\return <i>true</i> если такой сдвиг возможен и был осуществлён, иначе
		<i>false</i>
	*/
	bool _move_up(const typename point_t::size_type &dy = 1) {
		if (_top_left.gety() + dy > _point.gety()) return false;

		_point.gety() -= dy;

		return true;
	}

	//!	\brief Сдвигает текущие координаты вниз
	/*!	\param[in] dy Величина сдвига по оси y (0 <= <i>dy</i>)
		\return <i>true</i> если такой сдвиг возможен и был осуществлён, иначе
		<i>false</i>
	*/
	bool _move_down(const typename point_t::size_type &dy = 1) {
		if (_point.gety() + dy > _bottom_right.gety()) return false;

		_point.gety() += dy;

		return true;
	}

private:
	// private data ------------------------------------------------------------

	//! \brief Текущее значение итератора
	point_type _point;

	//! \brief Координаты левого-верхнего угла ограничивающего прямоугольника
	point_type _top_left;

	//! \brief Координаты нижнего-правого угла ограничивающего прямоугольника
	point_type _bottom_right;

};



////////////////////////////////////////////////////////////////////////////////
// snake_2d_iterator class declaration
//!	\brief Итератор, проходящий прямоугольную область змейкой
/*! Образец выполняемого прохода:
	\verbatim
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
	\endverbatim

	\sa basic_2d_iterator
*/
class snake_2d_iterator: public basic_2d_iterator<p_t> {
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

		\sa basic_2d_iterator
	*/
	snake_2d_iterator(const point_type &top_left,
					  const point_type &bottom_right):
		basic_2d_iterator<point_type>(top_left, top_left, bottom_right),
		_going_left(false)
	{
		_points_left = (bottom_right.getx() -  top_left.getx() + 1) *
					   (bottom_right.gety() -  top_left.gety() + 1);
	}

	//! \brief Копирующий конструктор
	/*!	\param[in] src Исходный объект
	*/
	snake_2d_iterator(const snake_2d_iterator &src):
		basic_2d_iterator<point_type>(src),
		_points_left(src._points_left),
		_going_left(src._going_left)
	{
	}

	//! \brief Определение виртуального basic_iterator::next()
	/*! Организует проход змейкой
	*/
	virtual void next() {
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
	}

	//! \brief Определение виртуального basic_iterator::end()
	virtual const bool end() const {
		return (0 >= _points_left);
	}

	//! \brief Возвращает количество оставшихся координат
	/*!	\return Количество оставшихся координат
	*/
	const sz_t &points_left() const { return _points_left; }

	//!	\brief Возвращает текущее направление прохода
	/*!	\return <i>true</i> если текущее направление обхода - влево,
		иначе <i>false</i>
	*/
	bool going_left() const { return _going_left; }

protected:
private:
	// private data ------------------------------------------------------------

	//!	\brief Количество итераций, оставшихся до достижения конца
	sz_t _points_left;

	//!	\brief Текущее направление перемещения
	/*!	\sa going_left()
	*/
	bool _going_left;

};



////////////////////////////////////////////////////////////////////////////////
// LL_children_iterator class declaration
//!	\brief Итератор по дочерним элементам элемента из <i>LL</i> саббенда.
/*! Итератор по 3ём дочерним элементам элемента из <i>LL</i> саббенда.

	\note На данный момент реализация этого класса ужасна. Она использует
	динамически заполняемый std::vector и её быстродействие оставляет желать
	лучшего. Но так-как количество коэффициентов в <i>LL</i> саббенде обычно
	не так велико, оставим пока всё как есть.

	\todo Придумать более достойную реализацию этого класса
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
		_i(LL_CHILDREN_COUNT - 1), _end(false)
	{
		_children.reserve(LL_CHILDREN_COUNT);
		_children.push_back(p_t(p.x + LL_w, p.y + LL_h));
		_children.push_back(p_t(p.x       , p.y + LL_h));
		_children.push_back(p_t(p.x + LL_w, p.y));
	}

	//! \brief Определение виртуального basic_iterator::get()
	virtual const p_t &get() const { return _children[_i]; }

	//! \brief Определение виртуального basic_iterator::next()
	virtual void next() {
		(0 < _i)? (--_i): (_end = true);
	}

	//! \brief Определение виртуального basic_iterator::end()
	virtual const bool end() const {
		return _end;
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

	//!	\brief Массив координат дочерних элементов
	_children_t _children;

	//!	\brief Признак завершения последовательности итераций
	bool _end;

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

		_n = (0 == lvl)? (MAX_VARIATIONS_LVL_0 - 1)
					   : (MAX_VARIATIONS_LVL_X - 1);
	}

	//! \brief Определение виртуального basic_iterator::get()
	virtual const n_t &get() const { return _n; }

	//! \brief Определение виртуального basic_iterator::next()
	virtual void next() {
		(0 != _n)? --_n: _end = true;
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

	//! \brief Псевдоним для параметра шаблона, который доступен для
	//!	пользователей класса
	typedef ival_t ival_type;

	//! \brief Тип контролируемого итератора
	typedef basic_iterator<ival_type> iterator_type;

	//! \brief Тип этого итератора (для удобства)
	typedef some_iterator<ival_type> self_type;

	// public constants --------------------------------------------------------

	//! \brief Начальное значение для счётчика ссылок
	static const int INIT_REF_COUNT			= 1;

	// public methods ----------------------------------------------------------

	//! \brief Конструктор
	/*!	\param[in] iterator Объект итератора над которым берётся шефство
		\param[in] no_own Если <i>true</i> Счётчик ссылок увеличится на 1 и
		контролируемый объект итератора гарантированно не будет уничтожен
		при вызове деструктора.

		Функция создаёт счётчик ссылок на итератор и инициализирует его в 1
	*/
	some_iterator(iterator_type *const iterator, const bool no_own = false):
		_iterator(iterator),
		_ref_count_ptr(new int(INIT_REF_COUNT))
	{
		assert(0 != _iterator);

		assert(0 != _ref_count_ptr);	// для отладки

		if (0 == _ref_count_ptr) throw std::bad_alloc();

		if (no_own) ++(*_ref_count_ptr);
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
	const iterator_type *operator->() const { return _iterator; }

	//! \brief Возвращает указатель на контролируемый итератор
	iterator_type *operator->() { return _iterator; }

	//! \brief Возвращает указатель на контролируемый итератор
	const iterator_type *get() const { return _iterator; }

	//! \brief Возвращает указатель на контролируемый итератор
	iterator_type *get() { return _iterator; }

	//! \brief Возвращает ссылку на контролируемый итератор
	const iterator_type &operator()() const { return (*_iterator); }

	//! \brief Возвращает ссылку на контролируемый итератор
	iterator_type &operator()() { return (*_iterator); }

	static self_type adapt(iterator_type &iterator) {
		return self_type(&iterator, true);
	}

private:
	// private data ------------------------------------------------------------

	//! \brief Указатель на контролируемый итератор
	iterator_type *const _iterator;

	//! \brief Указатель на значение содержащее счётчик ссылок на
	//!	контролируемый итератор
	int *const _ref_count_ptr;

};


////////////////////////////////////////////////////////////////////////////////
// some_iterator related functions

//!	\brief Производит приобразование basic_iterator к some_iterator
/*!	\param[in] i Преобразуемый итератор
	\return Объект класса some_iterator

	\sa some_iterator::adapt
*/
template <class T>
static some_iterator<T> some_iterator_adapt(basic_iterator<T> &i)
{
	return some_iterator<T>::adapt(i);
}



////////////////////////////////////////////////////////////////////////////////
// cumulative_iterator class declaration
//!	\brief Кумулятивный итератор
/*!	Объекты класса служат для объединения итераторов. Объединяемые итераторы
	должны быть класса some_iterator (или дочерних от него классов). После
	объединения итератор последовательно проходит по значениям всех итераторов
	в порядке из добавления. Проверка на повторные значения не производится.

	\warning Данный класс не подчиняется соглашению о завершении
	последовательности итераций, описанного в basic_iterator::next() и
	работает больше как обычный итератор. Поэтому перед каждым вызовом
	cumulative_iterator::next() или cumulative_iterator::get() необходимо
	проверять cumulative_iterator::end()

	\sa some_iterator, basic_iterator
*/
template <class ival_t>
class cumulative_iterator: public basic_iterator<ival_t> {
public:
	// public types ------------------------------------------------------------

	//!	\brief Тип объединяемых итераторов
	typedef some_iterator<ival_t> iterator_type;

	// public methods ----------------------------------------------------------

	//!	\name Конструкторы и деструкторы
	//@{

	//! \brief Конструктор
	cumulative_iterator()
	{
	}

	//! \brief Деструктор
	~cumulative_iterator()
	{
	}

	//@}

	//!	\name Добавление итераторов
	//@{

	//!	\brief Добавление нового итератора
	/*!	\param[in,out] iterator Добавляемый итератор
	*/
	void add(iterator_type &iterator)
	{
		_iterators.push_back(iterator);
	}

	//@}

	//!	\name Виртуальные методы basic_iterator
	//@{

	//! \brief Определение виртуального basic_iterator::get()
	virtual const ival_type &get() const
	{
		assert(!_iterators.empty());

		return _iterators.front()->get();
	}

	//! \brief Определение виртуального basic_iterator::next()
	virtual void next()
	{
		assert(!_iterators.empty());

		if (_iterators.front()->end()) _iterators.pop_front();
		else _iterators.front()->next();
	}

	//! \brief Определение виртуального basic_iterator::end()
	virtual const bool end() const { return _iterators.empty(); }

	//@}

protected:
	// protected types ---------------------------------------------------------

	//!	\brief Тип для списка итераторов
	typedef std::list<iterator_type> _iterators_t;

private:
	// private data ------------------------------------------------------------

	//! \brief Список итераторов
	_iterators_t _iterators;

};



}	// wic namespace



#endif	// WIC_LIBWIC_ITERATORS_INCLUDED
