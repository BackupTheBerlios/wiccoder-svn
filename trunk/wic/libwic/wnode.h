/*******************************************************************************
* file:         wnode.h                                                        *
* version:      0.1.0                                                          *
* author:       mice (mailto:my_mice@mail.ru, ICQ:332-292-380)                 *
* description:  not available                                                  *
* tests:        none                                                           *
*******************************************************************************/

#pragma once

#ifndef WIC_LIBWIC_WNODE
#define WIC_LIBWIC_WNODE

////////////////////////////////////////////////////////////////////////////////
// headers

// standard C++ library headers
// none

// libwic headers
#include <wic/libwic/types.h>


////////////////////////////////////////////////////////////////////////////////
// wic namespace
namespace wic {


////////////////////////////////////////////////////////////////////////////////
// types


////////////////////////////////////////////////////////////////////////////////
// wnode struct declaration

//! \brief Узел дерева вейвлет коэффициентов
/*!	Структура поддерживает унифицированный доступ к своим полям через
	шаблонные члены:
	\code
	// узел с некоторыми значениями
	wic::wnode node;
	node.w = 12;
	node.wc = 34;
	node.j0 = 11;

	// доступ к полям структуры производится так:
	w_t w = node.get<wnode::member_w>();
	j_t j = node.get<wnode::member_j0>();

	// или так:
	w_t wc = wnode::field<wnode::member_wc>::get(node);

	\endcode

	\sa wtree
*/
struct wnode {
	//! \brief Значение вейвлет коэффициента в узле
	w_t w;
	//! \brief Значение проквантованного вейвлет коэффициента в узле
	w_t wc;
	//!	\brief Значение откорректированного коэффициента
	wk_t wk;
	//! \brief Значение функции Лагранжа при подрезании ветви
	j_t j0;
	//! \brief Значение функции Лагранжа при сохранении ветви
	j_t j1;
	//! \brief Групповой признак подрезания ветвей
	n_t n;
	//!	\brief Флаг, указывающий, попал коэффициент в подрезанную ветвь или нет
	bool invalid;

	//! \brief Набор констант для идентификации полей структуры
	enum wnode_members {
		member_w,			//!< \brief Соответствует полю w
		member_wc,			//!< \brief Соответствует полю wc
		member_wk,			//!< \brief Соответствует полю wk
		member_j0,			//!< \brief Соответствует полю j0
		member_j1,			//!< \brief Соответствует полю j1
		member_n,			//!< \brief Соответствует полю n
		member_invalid		//!< \brief Соответствует полю invalid
	};

	//! \brief Шаблон для выбора подходящего типа
	/*!	Шаблон выбирает тип поля по его целочисленному идентификатору
	*/
	template <const wnode_members member>
	struct type_selector {
		typedef void result;
	};

	//! \brief Специализация для поля w
	template <>
	struct type_selector<member_w> {
		typedef w_t result;
	};

	//! \brief Специализация для поля wc
	template <>
	struct type_selector<member_wc> {
		typedef w_t result;
	};

	//! \brief Специализация для поля wk
	template <>
	struct type_selector<member_wk> {
		typedef wk_t result;
	};

	//! \brief Специализация для поля j0
	template <>
	struct type_selector<member_j0> {
		typedef j_t result;
	};

	//! \brief Специализация для поля j1
	template <>
	struct type_selector<member_j1> {
		typedef j_t result;
	};

	//! \brief Специализация для поля n
	template <>
	struct type_selector<member_n> {
		typedef n_t result;
	};

	//! \brief Специализация для поля wc
	template <>
	struct type_selector<member_invalid> {
		typedef bool result;
	};

	//! \brief Шаблонная структура, которая имеет метод, возвращающий
	//!	значение поля по его идентификатору
	/*! \sa type_selector
	*/
	template <const wnode_members member>
	struct field {
		static typename type_selector<member>::result &get(wnode &node) {
			return void;
		}
	};

	//! \brief Специализация для поля w
	template <>
	struct field<member_w> {
		static type_selector<member_w>::result &get(wnode &node) {
			return node.w;
		}
	};

	//! \brief Специализация для поля wc
	template <>
	struct field<member_wc> {
		static type_selector<member_wc>::result &get(wnode &node) {
			return node.wc;
		}
	};

	//! \brief Специализация для поля wk
	template <>
	struct field<member_wk> {
		static type_selector<member_wk>::result &get(wnode &node) {
			return node.wk;
		}
	};

	//! \brief Специализация для поля j0
	template <>
	struct field<member_j0> {
		static type_selector<member_j0>::result &get(wnode &node) {
			return node.j0;
		}
	};

	//! \brief Специализация для поля j1
	template <>
	struct field<member_j1> {
		static type_selector<member_j1>::result &get(wnode &node) {
			return node.j1;
		}
	};

	//! \brief Специализация для поля n
	template <>
	struct field<member_n> {
		static type_selector<member_n>::result &get(wnode &node) {
			return node.n;
		}
	};

	//! \brief Специализация для поля j0
	template <>
	struct field<member_invalid> {
		static type_selector<member_invalid>::result &get(wnode &node) {
			return node.invalid;
		}
	};

	//!	\brief Шаблонная функция возвращающая значение поля по его
	//!	идентификатору
	/*!	Ради этой функции собственно и затевалось всё безобразие
		происходившее выше :^)
	*/
	template <const wnode_members member>
	typename type_selector<member>::result &get() {
		return field<member>::get(*this);
	}


};



}	// end of namespace wic



#endif	// WIC_LIBWIC_WNODE
