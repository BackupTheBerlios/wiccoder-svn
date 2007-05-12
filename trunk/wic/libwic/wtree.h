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
	// public constants --------------------------------------------------------
	// public types ------------------------------------------------------------
	// public methods ----------------------------------------------------------

	//!	\brief Конструкртор
	wtree(const sz_t width, const sz_t height, const sz_t lvls);

	//!	\brief Деструктор
	~wtree();

	//! \brief Возвращает количество коэффициентов во всех деревьях
	sz_t coefs() const { return (_width * _height); }

	//! \brief Возвращает количество байт занимаемых спектром
	sz_t nodes_sz() const;

	//! \brief Загружает спектр из памяти
	void load(const w_t *const from);

	//! \brief Производит квантование
	void quantize(const q_t q = 1);

	//! \brief Возвращает информацию о саббендах
	const subbands &sb() const;

	//! \brief Получение элемента спектра по координатам
	const wnode &at(const sz_t x, const sz_t y) const;
	
	//! \brief Получение элемента спектра по координатам
	wnode &at(const sz_t x, const sz_t y);

	//! \brief Получение элемента спектра по координатам
	const wnode &operator()(const sz_t x, const sz_t y) const {
		return at(x, y);
	}
	
	//! \brief Получение элемента спектра по координатам
	wnode &operator()(const sz_t x, const sz_t y) { return at(x, y); }

	//! \brief Получение координат элемента
	p_t get_pos(const wnode &node) const;

	//!	\brief Возвращает координаты родительского элемента
	p_t prnt(const p_t &c);

	//!	\brief Возвращает координаты дочернего элемента элемента
	p_t child(const p_t &p);

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

	//! \name Группа функций связанных с подсчётом прогнозных величин
	//@{

	//! \brief Высчитывает значение прогнозной величины <i>P<sub>i</sub></i>
	/*!	\param[in] x Координата x центра маски 3x3
		\param[in] y Координата y центра маски 3x3

		См. формулу (4) из 35.pdf
	*/
	template <const wnode::wnode_members member>
	pi_t calc_pi(const sz_t x, const sz_t y) {
		const pi_t i1	=	get_safe<member>(x - 1, y) + 
							get_safe<member>(x + 1, y) +
						 	get_safe<member>(x    , y - 1) + 
							get_safe<member>(x    , y + 1);

		const pi_t i2	=	get_safe<member>(x + 1, y + 1) + 
							get_safe<member>(x + 1, y - 1) +
						 	get_safe<member>(x - 1, y + 1) + 
							get_safe<member>(x - 1, y - 1);

		return (4 * get_safe<member>(x, y) + 2 * i1 + i2) / 16;
	}

	template <const wnode::wnode_members member>
	pi_t calc_sj(const sz_t x, const sz_t y) {
		const p_t p = prnt(p_t(x, y));

		return calc_pi<member>(p.x, p.y);
	}

	//@}

protected:
	// protected methods -------------------------------------------------------

	//! \brief Сбрасывет всю информацию о деревьях в 0
	void _reset_trees_content();

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
