/*!	\file     wnode.h
	\version  0.0.1
	\author   mice, ICQ: 332-292-380, mailto:wonder.mice@gmail.com
	\brief    Определение структуры wic::wnode - элемента вейвлет-спектра

	\todo     Более подробно описать файл wnode.h
*/

#pragma once

#ifndef WIC_LIBWIC_WNODE
#define WIC_LIBWIC_WNODE

////////////////////////////////////////////////////////////////////////////////
// headers

// standard C++ library headers
#include <math.h>

// libwic headers
#include <wic/libwic/types.h>


////////////////////////////////////////////////////////////////////////////////
// wic namespace
namespace wic {


////////////////////////////////////////////////////////////////////////////////
// types


////////////////////////////////////////////////////////////////////////////////
// wnode struct declaration

//! \brief Узел дерева вейвлет коэффициентов (элемент спектра)
/*!	Структура поддерживает унифицированный доступ к своим полям через
	шаблонные члены:
	\code
	// узел с некоторыми значениями
	wic::wnode node;
	node.w = 12;
	node.wq = 34;
	node.j0 = 11;

	// доступ к полям структуры производится так:
	w_t w = node.get<wnode::member_w>();
	j_t j = node.get<wnode::member_j0>();

	// или так:
	w_t wq = wnode::field<wnode::member_wq>::get(node);

	\endcode

	\sa wtree
*/
struct wnode {
	//! \brief Значение вейвлет коэффициента в узле
	w_t w;
	//! \brief Значение проквантованного вейвлет коэффициента в узле
	wk_t wq;
	//!	\brief Значение откорректированного коэффициента
	wk_t wc;
	//! \brief Значение функции Лагранжа при подрезании ветви
	j_t j0;
	//! \brief Значение функции Лагранжа при сохранении ветви
	j_t j1;
	//! \brief Групповой признак подрезания ветвей
	n_t n;
	//!	\brief Флаг, указывающий, попал коэффициент в подрезанную ветвь или нет
	bool invalid;

	//! \brief Производит округление до ближайшего целого
	/*!	\param[in] w Число с плавающей запятой
		\return Округлённое число
	*/
	inline static wk_t round(const w_t &w) { return wk_t(floor(w + 0.5)); }

	//! \brief Производит квантование
	/*!	\param[in] w Значение вейвлет коэффициента для квантования
		\param[in] q Квантователь
		\return Проквантованное значение коэффициента
	*/
	inline static wk_t quantize(const w_t &w, const q_t &q) {
		return round(w / q);
	}

	//!	\brief Вычисляет знак аргумента
	/*!	\param[in] v Аргумент
		\return В зависимости от значения аргумента <i>v</i>:
		- если (<i>v</i> > 0) возвращает 1
		- если (<i>v</i> > 0) возвращает -1
		- если (<i>v</i> == 0) возвращает 0
	*/
	template <class T>
	inline static int sign(const T &v) {
		return (0 < v)? 1: ((0 > v)? -1: 0);
	}

	//!	\brief Вычисляет "положительный" знак аргумента
	/*!	\param[in] v Аргумент
		\return В зависимости от значения аргумента <i>v</i>:
		- если (<i>v</i> > 0) возвращает 1
		- если (<i>v</i> > 0) возвращает 2
		- если (<i>v</i> == 0) возвращает 0

		Аналогична sign(), но для отрицательного аргумента возвращает 2,
		а не -1.
	*/
	template <class T>
	inline static int signp(const T &v) {
		return (0 < v)? 1: ((0 > v)? 2: 0);
	}

	//!	\brief Преобразует результат функции sign() в результат функции
	//!	signp() или наоборот
	/*!	\param[in] s Знак вычисленный функцией sign() или signp()
		\return Если <i>s</i> равно -1, возвращает 2. Если <i>s</i>
		равно 2, возвращает -1. Иначе возвращает <i>s</i>.
	*/
	inline static int signx(const int s) {
		switch (s) {
			case -1:	return 2;
			case 2:		return -1;
			default:	return 0;
		}
	}

	//!	\brief Минимальное значение возвращаемое функцией sign()
	inline static int sign_min() { return -1; }

	//!	\brief Максимальное значение возвращаемое функцией sign()
	inline static int sign_max() { return 1; }

	//!	\brief Минимальное значение возвращаемое функцией signp()
	inline static int signp_min() { return 0; }

	//!	\brief Максимальное значение возвращаемое функцией signp()
	inline static int signp_max() { return 2; }

	//! \brief Производит деквантование
	/*!	\param[in] wk Проквантованное значение вейвлет коэффициента
		\param[in] q Квантователь
		\return Восстановленное (деквантованное) значение коэффициента
	*/
	inline static w_t dequantize(const wk_t &wk, const q_t &q) {
		return w_t(wk * q);
	}

	//! \brief Набор констант для идентификации полей структуры
	enum wnode_members {
		member_w,			//!< \brief Соответствует полю w
		member_wq,			//!< \brief Соответствует полю wq
		member_wc,			//!< \brief Соответствует полю wc
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

	//! \brief Специализация для поля wq
	template <>
	struct type_selector<member_wq> {
		typedef wk_t result;
	};

	//! \brief Специализация для поля wc
	template <>
	struct type_selector<member_wc> {
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

	//! \brief Специализация для поля invalid
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

	//! \brief Специализация для поля wq
	template <>
	struct field<member_wq> {
		static type_selector<member_wq>::result &get(wnode &node) {
			return node.wq;
		}
	};

	//! \brief Специализация для поля wc
	template <>
	struct field<member_wc> {
		static type_selector<member_wc>::result &get(wnode &node) {
			return node.wc;
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

	//! \brief Специализация для поля invalid
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

	//!	\brief Шаблонная функция возвращающая значение поля по его
	//!	идентификатору
	/*!	Константная версия предыдущей функции
	*/
	template <const wnode_members member>
	const typename type_selector<member>::result &get() const {
		return field<member>::get(*(const_cast<wnode *>(this)));
	}

};



}	// end of namespace wic



#endif	// WIC_LIBWIC_WNODE
