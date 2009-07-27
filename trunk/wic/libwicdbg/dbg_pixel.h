/*!	\file     dbg_pixel.h
	\version  0.0.1
	\author   mice, ICQ: 332-292-380, mailto:wonder.mice@gmail.com
	\brief    Описание класса wicdbg::dbg_surface - поверхности для отладки
			  процесса кодирования

	\todo     Более подробно описать файл dbg_pixel.h
*/

#ifndef WIC_LIBWICDBG_DBG_PIXEL
#define WIC_LIBWICDBG_DBG_PIXEL

///////////////////////////////////////////////////////////////////////////////
// include

// standard C++ library headers
// none

// libwic headers
#include <wic/libwic/types.h>


///////////////////////////////////////////////////////////////////////////////
// wicdbg namespace
namespace wicdbg
{


////////////////////////////////////////////////////////////////////////////////
// dbg_pixel structure declaration
//!	\brief Структура, которая используется для хранения отладочной информации
//!	классом dbg_surface
/*!
*/
struct dbg_pixel
{
	// public data -------------------------------------------------------------

	//!	\brief Значение закодированного коэффициента (проквантованного и
	//!	скорректированного)
	wic::wk_t wc;

	//!	\brief Номер модели, в которую попал закодированный коэффициент
	//!	<i>wc</i>
	wic::sz_t wc_model;

	//!	\brief Номер модели, в которую попал знак закодированного коэффициента
	//!	<i>wc</i>
	wic::sz_t sign_model;

	//!	\brief Признак подрезания ветвей
	wic::n_t n;

	//!	\brief Модель арифметического кодера, в которую попал признак
	//!	подрезания ветвей <i>n</i>
	wic::sz_t n_model;

	// public constants --------------------------------------------------------

	//! \brief Набор констант для идентификации полей структуры
	enum members
	{
		member_wc,			//!< \brief Соответствует полю wc
		member_wc_model,	//!< \brief Соответствует полю wc_model
		member_sign_model,	//!< \brief Соответствует полю sign_model
		member_n,			//!< \brief Соответствует полю n
		member_n_model,		//!< \brief Соответствует полю n_model
	};

	// public types ------------------------------------------------------------

	//! \brief Шаблон для выбора подходящего типа
	/*!	Шаблон выбирает тип поля по его целочисленному идентификатору
	*/
	template <const members member>
	struct type_selector {
		typedef void result;
	};

	//! \brief Специализация для поля wc
	template <>
	struct type_selector<member_wc> {
		typedef wic::wk_t result;
	};

	//! \brief Специализация для поля wc_model
	template <>
	struct type_selector<member_wc_model> {
		typedef wic::sz_t result;
	};

	//! \brief Специализация для поля sign_model
	template <>
	struct type_selector<member_sign_model> {
		typedef wic::sz_t result;
	};

	//! \brief Специализация для поля n
	template <>
	struct type_selector<member_n> {
		typedef wic::n_t result;
	};

	//! \brief Специализация для поля n_model
	template <>
	struct type_selector<member_n_model> {
		typedef wic::sz_t result;
	};

	//! \brief Шаблонная структура, которая имеет метод, возвращающий
	//!	значение поля по его идентификатору
	/*! \sa type_selector
	*/
	template <const members member>
	struct field {
		static typename type_selector<member>::result &get(dbg_pixel &pixel) {
			return void;
		}
	};

	//! \brief Специализация для поля wc
	template <>
	struct field<member_wc> {
		static type_selector<member_wc>::result &get(dbg_pixel &pixel) {
			return pixel.wc;
		}
	};

	//! \brief Специализация для поля wc_model
	template <>
	struct field<member_wc_model> {
		static type_selector<member_wc_model>::result &get(dbg_pixel &pixel) {
			return pixel.wc_model;
		}
	};

	//! \brief Специализация для поля sign_model
	template <>
	struct field<member_sign_model> {
		static type_selector<member_sign_model>::result &get(dbg_pixel &pixel) {
			return pixel.sign_model;
		}
	};

	//! \brief Специализация для поля n
	template <>
	struct field<member_n> {
		static type_selector<member_n>::result &get(dbg_pixel &pixel) {
			return pixel.n;
		}
	};

	//! \brief Специализация для поля n_model
	template <>
	struct field<member_n_model> {
		static type_selector<member_n_model>::result &get(dbg_pixel &pixel) {
			return pixel.n_model;
		}
	};

	// public methods ----------------------------------------------------------

	//!	\brief Шаблонная функция возвращающая значение поля по его
	//!	идентификатору
	/*!	Ради этой функции собственно и затевалось всё безобразие
		происходившее выше :^)
	*/
	template <const members member>
	typename type_selector<member>::result &get() {
		return field<member>::get(*this);
	}

	//!	\brief Шаблонная функция возвращающая значение поля по его
	//!	идентификатору
	/*!	Константная версия предыдущей функции
	*/
	template <const members member>
	const typename type_selector<member>::result &get() const {
		return field<member>::get(*(const_cast<dbg_pixel *>(this)));
	}

};



}	// end of namespace wicdbg



#endif	// WIC_LIBWICDBG_DBG_PIXEL
