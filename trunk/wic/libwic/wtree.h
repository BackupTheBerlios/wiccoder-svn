/*******************************************************************************
* file:         wtree.h                                                        *
* version:      0.1.0                                                          *
* author:       mice (mailto:my_mice@mail.ru, ICQ:332-292-380)                 *
* description:  not available                                                  *
* tests:        none                                                           *
*******************************************************************************/

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

	\sa wnode, subbands
*/
class wtree {
public:
	// public types ------------------------------------------------------------

	//! \brief Псевдоним для обёртки для итераторов, использующих тип wic::p_t
	typedef some_iterator<p_t> coefs_iterator;

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

	//!	\name Работа с целым деревом

	//@{

	//! \brief Загружает спектр из памяти
	void load(const w_t *const from);

	//! \brief Производит квантование
	void quantize(const q_t q = 1);

	//! \brief Обновляет дерево, восстанавливая подрезанные вевти,
	//! приравнивая значения полей wnode::wq и wnode::wk к wnode::w
	//! и обнуляя остальные поля.
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

	//! \brief Получение координат элемента
	p_t get_pos(const wnode &node) const;

	//!	\brief Возвращает координаты родительского элемента
	p_t prnt(const p_t &c);

	//! \brief Подрезает дерево коэффициентов
	void cut(const p_t &p);

	//! \brief Позволяет получить значение элемента, проверяя его
	//!	существование. Если элемента с такими координатами не существует,
	//! будет возвращено значение по умолчанию.
	/*!	\param[in] x Координата \c x элемента
		\param[in] y Координата \c y элемента
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
		таковой не существует.
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

		См. формулу (4) из 35.pdf
	*/
	template <const wnode::wnode_members member>
	pi_t calc_pi(const sz_t x, const sz_t y,
				 const subbands::subband_t &sb)
	{
		const pi_t i1	=	get_safe<member>(x - 1, y,     sb) + 
							get_safe<member>(x + 1, y,     sb) +
						 	get_safe<member>(x    , y - 1, sb) + 
							get_safe<member>(x    , y + 1, sb);

		const pi_t i2	=	get_safe<member>(x + 1, y + 1, sb) + 
							get_safe<member>(x + 1, y - 1, sb) +
						 	get_safe<member>(x - 1, y + 1, sb) + 
							get_safe<member>(x - 1, y - 1, sb);

		return pi_t(4 * get_safe<member>(x, y, sb) + 2 * i1 + i2) / 16;
	}

	//! \brief Высчитывает значение прогнозной величины <i>S<sub>j</sub></i>
	/*!	\param[in] x Координата x "центра" маски 2x2
		\param[in] y Координата y "центра" маски 2x2
		\param[in] sb Ограничивающий саббенд
		\param[in] going_left \c true если выполняется проход влево,
		иначе \c false. В зависимости от этого параметра осуществляется выбор
		формы маски.

		См. формулу (6) из 35.pdf
	*/
	template <const wnode::wnode_members member>
	pi_t calc_sj(const sz_t x, const sz_t y,
				 const subbands::subband_t &sb,
				 const bool going_left)
	{
		// смещение для верхних коэффициентов
		static const dsz_t	top		= (-1);
		// смещение для боковых коэффициентов
		       const dsz_t	side	= (going_left)? (+1): (-1);

		// подсчёт взвешанной суммы
		const pi_t sum = 0.4 * pi_t(get_safe<member>(x + side, y + top, sb)) +
							   pi_t(get_safe<member>(x + side, y      , sb)) +
							   pi_t(get_safe<member>(x       , y + top, sb));

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
	*/
	template <const wnode::wnode_members member>
	pi_t calc_sj(const sz_t x, const sz_t y,
				 const subbands::subband_t &sb)
	{
		return calc_sj<member>(x, y, sb, _going_left(x, y));
	}

	//@}

	//!	\name Генераторы итераторов
	//@{

	//! \brief Возвращает итератор по саббенду
	coefs_iterator iterator_over_subband(const subbands::subband_t &sb);

	//! \brief Возвращает итератор по дочерним коэффициентам
	coefs_iterator iterator_over_children(const p_t &prnt);

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

	//!	\brief Из координат элемента получает предполагаемое направление
	//!	обхода
	bool _going_left(const sz_t x, const sz_t y);

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
