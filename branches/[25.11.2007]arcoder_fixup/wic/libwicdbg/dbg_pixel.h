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
	wic::sz_t m;

	// public constants --------------------------------------------------------

	//! \brief Набор констант для идентификации полей структуры
	enum dbg_pixel_members
	{
		member_wc,			//!< \brief Соответствует полю wc
		member_m			//!< \brief Соответствует полю m
	};

	// public types ------------------------------------------------------------

	//! \brief Шаблон для выбора подходящего типа
	/*!	Шаблон выбирает тип поля по его целочисленному идентификатору
	*/
	template <const dbg_pixel_members member>
	struct type_selector {
		typedef void result;
	};

	//! \brief Специализация для поля wc
	template <>
	struct type_selector<member_wc> {
		typedef wic::wk_t result;
	};

	//! \brief Специализация для поля m
	template <>
	struct type_selector<member_m> {
		typedef wic::sz_t result;
	};

	//! \brief Шаблонная структура, которая имеет метод, возвращающий
	//!	значение поля по его идентификатору
	/*! \sa type_selector
	*/
	template <const dbg_pixel_members member>
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

	//! \brief Специализация для поля m
	template <>
	struct field<member_m> {
		static type_selector<member_m>::result &get(dbg_pixel &pixel) {
			return pixel.m;
		}
	};

	// public methods ----------------------------------------------------------

	//!	\brief Шаблонная функция возвращающая значение поля по его
	//!	идентификатору
	/*!	Ради этой функции собственно и затевалось всё безобразие
		происходившее выше :^)
	*/
	template <const dbg_pixel_members member>
	typename type_selector<member>::result &get() {
		return field<member>::get(*this);
	}

	//!	\brief Шаблонная функция возвращающая значение поля по его
	//!	идентификатору
	/*!	Константная версия предыдущей функции
	*/
	template <const dbg_pixel_members member>
	const typename type_selector<member>::result &get() const {
		return field<member>::get(*(const_cast<wnode *>(this)));
	}

};



}	// end of namespace wicdbg



#endif	// WIC_LIBWICDBG_DBG_PIXEL
