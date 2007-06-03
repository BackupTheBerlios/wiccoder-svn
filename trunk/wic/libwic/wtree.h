/*!	\file     wtree.h
	\version  0.0.1
	\author   mice, ICQ: 332-292-380, mailto:wonder.mice@gmail.com
	\brief    Описания класса wic::wtree - спектра вейвлет разложения

	\todo     Более подробно описать файл wtree.h
*/


#pragma once

#ifndef WIC_LIBWIC_WTREE
#define WIC_LIBWIC_WTREE

////////////////////////////////////////////////////////////////////////////////
// include

// standard C++ library headers
#include <new>							// for std::bad_alloc exception class
#include <assert.h>

// libwic headers
#include <wic/libwic/types.h>
#include <wic/libwic/wnode.h>
#include <wic/libwic/subbands.h>
#include <wic/libwic/iterators.h>


////////////////////////////////////////////////////////////////////////////////
// wic namespace
namespace wic {


////////////////////////////////////////////////////////////////////////////////
// types


////////////////////////////////////////////////////////////////////////////////
// wtree class declaration

//! \brief Группа деревьев вейвлет коэффициентов
/*! Объекты этого класса хранят информацию, отражающую значения вейвлет
	коэффициентов в деревьях и их топологию. Для каждого дерева класс
	предоставляет следующие возможности:
	- хранение оригинальных значений вейвлет коэффициентов
	- хранение проктвантованных значений вейвлет коэффициентов
	- два значения функции лагранжа (при условии удаления вевти и
	  при условии её сохранения)
	- групповой признак подрезания ветвей
	- признак подрезания для одиночного коэффициента

	Объекты класса сами управляют обоими признаками подрезания. Квантование
	также осуществляется посредством метода класса. Значение RD-функции
	Лагранжа оставляется под усмотрение пользователя класса.

	Класс также хранит информацию о саббендах разложения.

	\note Несмотря на название, класс представляет собой все деревья
	вейвлет спектра, а не одно конкретное дерево (гроздь)

	\sa wnode, subbands
*/
class wtree {
public:
	// public types ------------------------------------------------------------

	//! \brief Псевдоним для обёртки для итераторов, использующих тип wic::p_t
	typedef some_iterator<p_t> coefs_iterator;

	//!	\brief Псевдоним для обёртки для итераторов, использующих тип wic::n_t
	typedef some_iterator<n_t> n_iterator;

	// public constants --------------------------------------------------------

	// public methods ----------------------------------------------------------

	//!	\name Конструкторы и деструкторы
	//@{

	//!	\brief Конструкртор
	wtree(const sz_t width, const sz_t height, const sz_t lvls);

	//!	\brief Деструктор
	~wtree();

	//@}

	//! \name Общая информация о дереве
	//@{

	//!	\brief Возвращает ширину спектра (в элементах)
	sz_t width() const { return _width; }

	//!	\brief Возвращает высоту спектра (в элементах)
	sz_t height() const { return _height; }

	//!	\brief Возвращает количество уровней разложения
	sz_t lvls() const { return _lvls; }

	//! \brief Возвращает количество коэффициентов во всём дереве
	sz_t coefs() const { return (_width * _height); }

	//! \brief Возвращает количество байт занимаемых спектром
	sz_t nodes_sz() const;

	//! \brief Возвращает использованный квантователь
	/*!	\return Квантователь, который был использован для квантования
		всего дерева c помощью wtree::quantize
	*/
	const q_t &q() const { return _q; }

	//@}

	//!	\name Работа с целым спектром

	//@{

	//! \brief Загружает спектр из памяти
	void load(const w_t *const from);

	//! \brief Производит квантование всего спектра
	void quantize(const q_t q = 1);

	//! \brief Обновляет дерево, восстанавливая подрезанные вевти,
	//! приравнивая значения полей wnode::wq и wnode::wk к wnode::w,
	//!	устанавливая wnode::n и обнуляя остальные поля.
	void refresh();

	//@}

	//!	\name Доступ к информации о саббендах
	//{@

	//! \brief Возвращает информацию о саббендах
	subbands &sb();

	//! \brief Возвращает информацию о саббендах
	const subbands &sb() const;

	//@}

	//! \name Доступ к элементам дерева
	//@{

	//! \brief Получение элемента спектра по координатам
	const wnode &at(const sz_t x, const sz_t y) const;
	
	//! \brief Получение элемента спектра по координатам
	wnode &at(const sz_t x, const sz_t y);

	//! \brief Получение элемента спектра по координатам
	const wnode &at(const p_t &p) const { return at(p.x, p.y); }
	
	//! \brief Получение элемента спектра по координатам
	wnode &at(const p_t &p) { return at(p.x, p.y); }

	//! \brief Получение координат элемента в спектре
	p_t get_pos(const wnode &node) const;

	//!	\brief Возвращает координаты родительского элемента
	//!	(версия для не <i>LL</i> саббенда)
	p_t prnt(const p_t &c);

	//!	\brief Возвращает координаты родительского элемента
	//! (универсальная версия для всех саббендов)
	p_t prnt_uni(const p_t &c);

	//! \brief Позволяет получить значение элемента, проверяя его
	//!	существование. Если элемента с такими координатами не существует,
	//! будет возвращено значение по умолчанию.
	/*!	\param[in] x Координата <i>x</i> элемента
		\param[in] y Координата <i>y</i> элемента
		\param[in] def Значение по умолчанию, которое вернётся, если
		элемента с такими координатами не существует
		\return Значение элемента или значение по умолчанию, если
		таковой не существует.
	*/
	template <const wnode::wnode_members member>
	typename wnode::type_selector<member>::result get_safe(
			const sz_t x, const sz_t y,
			const typename wnode::type_selector<member>::result &def = 0)
	{
		if (0 > x || x >= _width) return def;
		if (0 > y || y >= _height) return def;

		return  _nodes[x + _width * y].get<member>();
	}

	//! \brief Позволяет получить значение элемента, проверяя его
	//!	нахождение в выбранном саббенде. Если элемент с такими
	//!	координатами находится за пределами саббенда, будет возвращено
	//!	значение по умолчанию.
	/*!	\param[in] x Координата \c x элемента
		\param[in] y Координата \c y элемента
		\param[in] sb Саббенд в котором должен находится элемент.
		\param[in] def Значение по умолчанию, которое вернётся, если
		элемента с такими координатами не существует
		\return Значение элемента или значение по умолчанию, если
		таковой находится за пределами выбранного саббенда
	*/
	template <const wnode::wnode_members member>
	typename wnode::type_selector<member>::result get_safe(
			const sz_t x, const sz_t y,
			const subbands::subband_t &sb,
			const typename wnode::type_selector<member>::result &def = 0)
	{
		if (sb.x_min > x || x > sb.x_max) return def;
		if (sb.y_min > y || y > sb.y_max) return def;

		return  _nodes[x + _width * y].get<member>();
	}

	//@}

	//! \name Подсчёт прогнозных величин
	//@{

	//! \brief Высчитывает значение прогнозной величины <i>P<sub>i</sub></i>
	/*!	\param[in] x Координата x центра маски 3x3
		\param[in] y Координата y центра маски 3x3
		\param[in] sb Ограничивающий саббенд

		Функция использует элемент <i>(x, y)</i> и его соседей для расчёта
		прогнозной величины <i>P<sub>i</sub></i>.

		Вообще, функция применима для любого саббенда, но по алгоритму,
		она никогда не используется для подсчёта прогноза в <i>LL</i>
		саббенде.

		См. формулу (4) из 35.pdf

		\todo Сделать расчёт прогноза более точным, учитывая отсутствие
		некоторых элементов на границах, что приводит к изменению
		коэффициентов взвешанной суммы.
	*/
	template <const wnode::wnode_members member>
	pi_t calc_pi(const sz_t x, const sz_t y,
				 const subbands::subband_t &sb)
	{
		const pi_t i1	= abs(get_safe<member>(x - 1, y,     sb)) + 
						  abs(get_safe<member>(x + 1, y,     sb)) +
						  abs(get_safe<member>(x    , y - 1, sb)) + 
						  abs(get_safe<member>(x    , y + 1, sb));

		const pi_t i2	= abs(get_safe<member>(x + 1, y + 1, sb)) + 
						  abs(get_safe<member>(x + 1, y - 1, sb)) +
						  abs(get_safe<member>(x - 1, y + 1, sb)) + 
						  abs(get_safe<member>(x - 1, y - 1, sb));

		return pi_t(4 * abs(get_safe<member>(x, y, sb)) + 2 * i1 + i2) / 16;
	}

	//!	\brief Вычисляет среднее по дочерним элементам значения прогнозной
	//!	величины <i>P<sub>i</sub></i>
	/*!	\param[in] prnt Координаты родительского элемента, дочерние от
		которого будут использоваться для вычисления прогнозной величины.
		\param[in] children_sb Саббенд в котором располагаются дочерние
		элементы.
		\return Среднее по дочерним элементам значения прогнозной величины
		<i>P<sub>i</sub></i>

		Функция применима только для родительских элементов не из <i>LL</i>
		саббенда. Тем не менее, функция возвращает 0 для координат из
		<i>LL</i> саббенда.

		Шаблонный параметр <i>member</i> позволяет выбирать поле элемента
		для расчёта прогноза. Обычно это будет wnode::member_wc.

		Для более подробной информации смотри <i>Шаг 2.1</i> и формулу
		<i>(5)</i> в <i>35.pdf</i>.

		\attention Следует обратить особое внимание на то, что параметр
		<i>children_sb</i> представляет собой именно саббенд, в котором
		располагаются дочерние элементы, а не саббенд, в котором находится
		сам родительский элемент с координатами <i>prnt</i>
	*/
	template <const wnode::wnode_members member>
	pi_t calc_pi_avg(const p_t &prnt, const subbands::subband_t &children_sb)
	{
		// проверка на LL саббенд
		if (sb().test_LL(prnt)) return 0;

		pi_t pi = 0;

		for (coefs_iterator i = iterator_over_children(prnt);
			!i->end(); i->next())
		{
			const p_t &c = i->get();
			pi += calc_pi<member>(c.x, c.y, children_sb);
		}

		return (pi / 4);
	}

	//! \brief Высчитывает значение прогнозной величины <i>S<sub>j</sub></i>
	/*!	\param[in] x Координата x "центра" маски 2x2
		\param[in] y Координата y "центра" маски 2x2
		\param[in] sb Ограничивающий саббенд
		\param[in] going_left <i>true</i> если выполняется проход влево,
		иначе <i>false</i>. В зависимости от этого параметра осуществляется
		выбор формы маски.
		\return Значение прогнозной величины <i>S<sub>j</sub></i>

		Функция при вычислении прогноза использует до трёх соседних элементов
		<i>(x, y)</i> и до девяти элементов соседних для родителя элемента
		<i>(x, y)</i>. По этой причине элемент <i>(x, y)</i> должен лежать
		на уровнях начиная со второго. Тем не менее функция корректно
		обрабатывает координаты элементов с первых двух уровней, возвращая
		при этом нулевое значение прогноза.

		См. формулу (6) из <i>35.pdf</i>

		\note Магические числа, использованные в функции представляют
		собой коэффициенты (веса) из <i>35.pdf</i>

		\todo Сделать расчёт прогноза более точным, учитывая отсутствие
		некоторых элементов на границах, что приводит к изменению
		коэффициентов взвешанной суммы.
	*/
	template <const wnode::wnode_members member>
	pi_t calc_sj(const sz_t x, const sz_t y,
				 const subbands::subband_t &sb,
				 const bool going_left)
	{
		// проверка на элемент из первых двух уровней
		if (subbands::LVL_1 >= sb.lvl) return 0;

		// смещение для верхних коэффициентов
		static const dsz_t	top		= (-1);
		// смещение для боковых коэффициентов
		       const dsz_t	side	= (going_left)? (+1): (-1);

		// подсчёт взвешанной суммы
		const pi_t sum = 0.4 *
						 abs(pi_t(get_safe<member>(x + side, y + top, sb))) +
						 abs(pi_t(get_safe<member>(x + side, y      , sb))) +
						 abs(pi_t(get_safe<member>(x       , y + top, sb)));

		// родительский коэффициент
		const p_t p = prnt(p_t(x, y));
		const subbands::subband_t &prnt_sb = *(sb.prnt);

		return (0.36 * pi_t(calc_pi<member>(p.x, p.y, prnt_sb)) +
				1.06 * sum);
	}

	//! \brief Высчитывает значение прогнозной величины <i>S<sub>j</sub></i>
	/*!	\param[in] x Координата x "центра" маски 2x2
		\param[in] y Координата y "центра" маски 2x2
		\param[in] sb Ограничивающий саббенд

		Автоматически определяет направление обхода, используя для этого
		функцию _going_left().

		См. формулу (6) из 35.pdf

		\return Значение прогнозной величины <i>S<sub>j</sub></i>
	*/
	template <const wnode::wnode_members member>
	pi_t calc_sj(const sz_t x, const sz_t y,
				 const subbands::subband_t &sb)
	{
		return calc_sj<member>(x, y, sb, _going_left(x, y));
	}

	//@}

	//!	\name Функции для работы с групповыми признаками подрезания
	//@{

	//!	\brief Возвращает итератор по значениям группового признака
	//!	подрезания
	n_iterator iterator_through_n(const sz_t lvl);

	//!	\brief Конструирует групповой признак подрезания ветвей, в котором
	//!	все элементы кроме одного подрезаны.
	n_t child_n_mask(const p_t &p, const p_t &branch);

	//!	\brief Конструирует групповой признак подрезания ветвей, в котором
	//!	все элементы кроме одного подрезаны. Версия для <i>LL</i> саббенда.
	n_t child_n_mask_LL(const p_t &p);

	//!	\brief Проверяет, подрезанность ветвей в групповом признаке
	//!	подрезания по маске ветвей.
	/*!	\param[in] n Групповой признак подрезания ветвей
		\param[in] mask Маска дочерних ветвей, которые необходимо
		проверить на подрезаность.
		\return <i>true</i> если все запрашиваемые в <i>mask</i> ветви
		не подрезаны. <i>false</i> если хотя бы одна ветвь подрезана.
	*/
	inline bool test_n_mask(const n_t &n, const n_t &mask) {
		return (mask == (n & mask));
	}

	//@}

	//!	\name Модификация топологии дерева
	//@{

	//!	\brief Подрезает ветви в соответствии с групповым признаком подрезания
	//!	ветвей
	void cut_leafs(const p_t &branch, const n_t n);

	//@}

	//!	\name Генераторы итераторов по коэффициентам
	//@{

	//! \brief Возвращает итератор по саббенду
	coefs_iterator iterator_over_subband(const subbands::subband_t &sb);

	//! \brief Возвращает итератор по дочерним элементам (родительский
	//!	элемент не из <i>LL</i> саббенда)
	/*!	\param[in] prnt Координаты родительского элемента
		\return Итератор по дочерним элементам

		\note Данная функция не применима для родительских коэффициентов из
		<i>LL</i> саббенда.

		\sa _iterator_over_children()
	*/
	wtree::coefs_iterator iterator_over_children(const p_t &prnt)
	{
		return _iterator_over_children(prnt);
	}

	//! \brief Возвращает итератор по дочерним элементам (родительский
	//!	элемент из <i>LL</i> саббенда)
	/*!	\param[in] prnt Координаты родительского элемента
		\return Итератор по дочерним элементам

		\note Данная функция не применима для родительских коэффициентов не из
		<i>LL</i> саббенда.

		\sa _iterator_over_LL_children()
	*/
	wtree::coefs_iterator iterator_over_LL_children(const p_t &prnt)
	{
		return _iterator_over_LL_children(prnt);
	}

	//! \brief Возвращает итератор по дочерним элементам (родительский
	//!	элемент из любого саббенда)
	/*!	\param[in] prnt Координаты родительского элемента
		\return Итератор по дочерним элементам

		\note Следует помнить, что если родительский элемент из <i>LL</i>
		саббенда, дочерние элементы лежат в разных саббендах.

		\sa _iterator_over_children(), _iterator_over_LL_children()
	*/
	wtree::coefs_iterator iterator_over_children_uni(const p_t &prnt)
	{
		return (sb().test_LL(prnt))
				? _iterator_over_LL_children(prnt)
				: _iterator_over_children(prnt);
	}

	//! \brief Возвращает итератор по дочерним коэффициентам (листьям)
	coefs_iterator iterator_over_leafs(const p_t &root,
									   const subbands::subband_t &sb);

	//! \brief Возвращает итератор по дочерним коэффициентам (листьям)
	coefs_iterator iterator_over_leafs(const p_t &root,
									   const sz_t lvl, const sz_t i);

	//@}

protected:
	// protected methods -------------------------------------------------------

	//!	\name Функции работающие с целым деревом
	//@{

	//! \brief Сбрасывет всю информацию о деревьях в 0
	void _reset_trees_content();

	//@}

	//!	\name Функции для работы с отдельными коэффициентами
	//@{

	//!	\brief Возвращает координаты верхнего левого элемента из группы
	//!	дочерних элементов
	p_t _children_top_left(const p_t &prnt);

	//! \brief Определяет координаты верхнего левого элемента блока листьев
	//!	дерева
	p_t _leafs_top_left(const p_t &root, const sz_t lvl, const sz_t i);

	//!	\brief Из координат элемента получает предполагаемое направление
	//!	обхода
	bool _going_left(const sz_t x, const sz_t y);

	//@}

	//!	\name Модификация топологии дерева
	//@{

	//!	\brief Подрезает ветвь дерева
	void _cut_branch(const p_t &branch);

	//@}

	//!	\name Генераторы итераторов
	//@{

	//! \brief Возвращает итератор по дочерним элементам (родительский элемент
	//!	не из <i>LL</i> саббенда)
	basic_iterator<p_t> *_iterator_over_children(const p_t &prnt);

	//! \brief Возвращает итератор по дочерним элементам (родительский элемент
	//!	не <i>LL</i> саббенда)
	basic_iterator<p_t> *_iterator_over_LL_children(const p_t &prnt);

	//@}

private:
	// private data ------------------------------------------------------------

	//! \brief Ширина карты спектра
	sz_t _width;

	//! \brief Высота карты спектра
	sz_t _height;

	//! \brief Количество уровней разложения спектра
	sz_t _lvls;

	//!	\brief Квантователь, который был использован в последний раз
	q_t	_q;

	//! \brief Карта спектра
	wnode *_nodes;

	//! \brief Саббенды разложения
	subbands *_subbands;
};



}	// end of namespace wic



#endif	// WIC_LIBWIC_WTREE
