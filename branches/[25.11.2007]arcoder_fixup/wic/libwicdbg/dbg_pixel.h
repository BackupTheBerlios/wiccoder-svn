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
	wic::sz_t m;

	// public constants --------------------------------------------------------

	//! \brief ����� �������� ��� ������������� ����� ���������
	enum dbg_pixel_members
	{
		member_wc,			//!< \brief ������������� ���� wc
		member_m			//!< \brief ������������� ���� m
	};

	// public types ------------------------------------------------------------

	//! \brief ������ ��� ������ ����������� ����
	/*!	������ �������� ��� ���� �� ��� �������������� ��������������
	*/
	template <const dbg_pixel_members member>
	struct type_selector {
		typedef void result;
	};

	//! \brief ������������� ��� ���� wc
	template <>
	struct type_selector<member_wc> {
		typedef wic::wk_t result;
	};

	//! \brief ������������� ��� ���� m
	template <>
	struct type_selector<member_m> {
		typedef wic::sz_t result;
	};

	//! \brief ��������� ���������, ������� ����� �����, ������������
	//!	�������� ���� �� ��� ��������������
	/*! \sa type_selector
	*/
	template <const dbg_pixel_members member>
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

	//! \brief ������������� ��� ���� m
	template <>
	struct field<member_m> {
		static type_selector<member_m>::result &get(dbg_pixel &pixel) {
			return pixel.m;
		}
	};

	// public methods ----------------------------------------------------------

	//!	\brief ��������� ������� ������������ �������� ���� �� ���
	//!	��������������
	/*!	���� ���� ������� ���������� � ���������� �� ����������
		������������� ���� :^)
	*/
	template <const dbg_pixel_members member>
	typename type_selector<member>::result &get() {
		return field<member>::get(*this);
	}

	//!	\brief ��������� ������� ������������ �������� ���� �� ���
	//!	��������������
	/*!	����������� ������ ���������� �������
	*/
	template <const dbg_pixel_members member>
	const typename type_selector<member>::result &get() const {
		return field<member>::get(*(const_cast<wnode *>(this)));
	}

};



}	// end of namespace wicdbg



#endif	// WIC_LIBWICDBG_DBG_PIXEL
