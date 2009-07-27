/*!	\file     dbg_pixel.h
	\version  0.0.1
	\author   mice, ICQ: 332-292-380, mailto:wonder.mice@gmail.com
	\brief    �������� ������ wicdbg::dbg_surface - ����������� ��� �������
			  �������� �����������

	\todo     ����� �������� ������� ���� dbg_pixel.h
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
//!	\brief ���������, ������� ������������ ��� �������� ���������� ����������
//!	������� dbg_surface
/*!
*/
struct dbg_pixel
{
	// public data -------------------------------------------------------------

	//!	\brief �������� ��������������� ������������ (���������������� �
	//!	������������������)
	wic::wk_t wc;

	//!	\brief ����� ������, � ������� ����� �������������� �����������
	//!	<i>wc</i>
	wic::sz_t wc_model;

	//!	\brief ����� ������, � ������� ����� ���� ��������������� ������������
	//!	<i>wc</i>
	wic::sz_t sign_model;

	//!	\brief ������� ���������� ������
	wic::n_t n;

	//!	\brief ������ ��������������� ������, � ������� ����� �������
	//!	���������� ������ <i>n</i>
	wic::sz_t n_model;

	// public constants --------------------------------------------------------

	//! \brief ����� �������� ��� ������������� ����� ���������
	enum members
	{
		member_wc,			//!< \brief ������������� ���� wc
		member_wc_model,	//!< \brief ������������� ���� wc_model
		member_sign_model,	//!< \brief ������������� ���� sign_model
		member_n,			//!< \brief ������������� ���� n
		member_n_model,		//!< \brief ������������� ���� n_model
	};

	// public types ------------------------------------------------------------

	//! \brief ������ ��� ������ ����������� ����
	/*!	������ �������� ��� ���� �� ��� �������������� ��������������
	*/
	template <const members member>
	struct type_selector {
		typedef void result;
	};

	//! \brief ������������� ��� ���� wc
	template <>
	struct type_selector<member_wc> {
		typedef wic::wk_t result;
	};

	//! \brief ������������� ��� ���� wc_model
	template <>
	struct type_selector<member_wc_model> {
		typedef wic::sz_t result;
	};

	//! \brief ������������� ��� ���� sign_model
	template <>
	struct type_selector<member_sign_model> {
		typedef wic::sz_t result;
	};

	//! \brief ������������� ��� ���� n
	template <>
	struct type_selector<member_n> {
		typedef wic::n_t result;
	};

	//! \brief ������������� ��� ���� n_model
	template <>
	struct type_selector<member_n_model> {
		typedef wic::sz_t result;
	};

	//! \brief ��������� ���������, ������� ����� �����, ������������
	//!	�������� ���� �� ��� ��������������
	/*! \sa type_selector
	*/
	template <const members member>
	struct field {
		static typename type_selector<member>::result &get(dbg_pixel &pixel) {
			return void;
		}
	};

	//! \brief ������������� ��� ���� wc
	template <>
	struct field<member_wc> {
		static type_selector<member_wc>::result &get(dbg_pixel &pixel) {
			return pixel.wc;
		}
	};

	//! \brief ������������� ��� ���� wc_model
	template <>
	struct field<member_wc_model> {
		static type_selector<member_wc_model>::result &get(dbg_pixel &pixel) {
			return pixel.wc_model;
		}
	};

	//! \brief ������������� ��� ���� sign_model
	template <>
	struct field<member_sign_model> {
		static type_selector<member_sign_model>::result &get(dbg_pixel &pixel) {
			return pixel.sign_model;
		}
	};

	//! \brief ������������� ��� ���� n
	template <>
	struct field<member_n> {
		static type_selector<member_n>::result &get(dbg_pixel &pixel) {
			return pixel.n;
		}
	};

	//! \brief ������������� ��� ���� n_model
	template <>
	struct field<member_n_model> {
		static type_selector<member_n_model>::result &get(dbg_pixel &pixel) {
			return pixel.n_model;
		}
	};

	// public methods ----------------------------------------------------------

	//!	\brief ��������� ������� ������������ �������� ���� �� ���
	//!	��������������
	/*!	���� ���� ������� ���������� � ���������� �� ����������
		������������� ���� :^)
	*/
	template <const members member>
	typename type_selector<member>::result &get() {
		return field<member>::get(*this);
	}

	//!	\brief ��������� ������� ������������ �������� ���� �� ���
	//!	��������������
	/*!	����������� ������ ���������� �������
	*/
	template <const members member>
	const typename type_selector<member>::result &get() const {
		return field<member>::get(*(const_cast<dbg_pixel *>(this)));
	}

};



}	// end of namespace wicdbg



#endif	// WIC_LIBWICDBG_DBG_PIXEL
