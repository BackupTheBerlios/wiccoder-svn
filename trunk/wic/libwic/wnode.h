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

//! \brief ���� ������ ������� �������������
/*!	��������� ������������ ��������������� ������ � ����� ����� �����
	��������� �����:
	\code
	// ���� � ���������� ����������
	wic::wnode node;
	node.w = 12;
	node.wq = 34;
	node.j0 = 11;

	// ������ � ����� ��������� ������������ ���:
	w_t w = node.get<wnode::member_w>();
	j_t j = node.get<wnode::member_j0>();

	// ��� ���:
	w_t wq = wnode::field<wnode::member_wq>::get(node);

	\endcode

	\sa wtree
*/
struct wnode {
	//! \brief �������� ������� ������������ � ����
	w_t w;
	//! \brief �������� ���������������� ������� ������������ � ����
	w_t wq;
	//!	\brief �������� ������������������� ������������
	wk_t wk;
	//! \brief �������� ������� �������� ��� ���������� �����
	j_t j0;
	//! \brief �������� ������� �������� ��� ���������� �����
	j_t j1;
	//! \brief ��������� ������� ���������� ������
	n_t n;
	//!	\brief ����, �����������, ����� ����������� � ����������� ����� ��� ���
	bool invalid;

	//! \brief ���������� ���������� �� ���������� ������
	/*!	\param[in] w ����� � ��������� �������
		\return ���������� �����
	*/
	static wk_t round(const w_t &w) { return wk_t(floor(w + 0.5)); }

	//! \brief ����� �������� ��� ������������� ����� ���������
	enum wnode_members {
		member_w,			//!< \brief ������������� ���� w
		member_wq,			//!< \brief ������������� ���� wq
		member_wk,			//!< \brief ������������� ���� wk
		member_j0,			//!< \brief ������������� ���� j0
		member_j1,			//!< \brief ������������� ���� j1
		member_n,			//!< \brief ������������� ���� n
		member_invalid		//!< \brief ������������� ���� invalid
	};

	//! \brief ������ ��� ������ ����������� ����
	/*!	������ �������� ��� ���� �� ��� �������������� ��������������
	*/
	template <const wnode_members member>
	struct type_selector {
		typedef void result;
	};

	//! \brief ������������� ��� ���� w
	template <>
	struct type_selector<member_w> {
		typedef w_t result;
	};

	//! \brief ������������� ��� ���� wq
	template <>
	struct type_selector<member_wq> {
		typedef w_t result;
	};

	//! \brief ������������� ��� ���� wk
	template <>
	struct type_selector<member_wk> {
		typedef wk_t result;
	};

	//! \brief ������������� ��� ���� j0
	template <>
	struct type_selector<member_j0> {
		typedef j_t result;
	};

	//! \brief ������������� ��� ���� j1
	template <>
	struct type_selector<member_j1> {
		typedef j_t result;
	};

	//! \brief ������������� ��� ���� n
	template <>
	struct type_selector<member_n> {
		typedef n_t result;
	};

	//! \brief ������������� ��� ���� wq
	template <>
	struct type_selector<member_invalid> {
		typedef bool result;
	};

	//! \brief ��������� ���������, ������� ����� �����, ������������
	//!	�������� ���� �� ��� ��������������
	/*! \sa type_selector
	*/
	template <const wnode_members member>
	struct field {
		static typename type_selector<member>::result &get(wnode &node) {
			return void;
		}
	};

	//! \brief ������������� ��� ���� w
	template <>
	struct field<member_w> {
		static type_selector<member_w>::result &get(wnode &node) {
			return node.w;
		}
	};

	//! \brief ������������� ��� ���� wq
	template <>
	struct field<member_wq> {
		static type_selector<member_wq>::result &get(wnode &node) {
			return node.wq;
		}
	};

	//! \brief ������������� ��� ���� wk
	template <>
	struct field<member_wk> {
		static type_selector<member_wk>::result &get(wnode &node) {
			return node.wk;
		}
	};

	//! \brief ������������� ��� ���� j0
	template <>
	struct field<member_j0> {
		static type_selector<member_j0>::result &get(wnode &node) {
			return node.j0;
		}
	};

	//! \brief ������������� ��� ���� j1
	template <>
	struct field<member_j1> {
		static type_selector<member_j1>::result &get(wnode &node) {
			return node.j1;
		}
	};

	//! \brief ������������� ��� ���� n
	template <>
	struct field<member_n> {
		static type_selector<member_n>::result &get(wnode &node) {
			return node.n;
		}
	};

	//! \brief ������������� ��� ���� j0
	template <>
	struct field<member_invalid> {
		static type_selector<member_invalid>::result &get(wnode &node) {
			return node.invalid;
		}
	};

	//!	\brief ��������� ������� ������������ �������� ���� �� ���
	//!	��������������
	/*!	���� ���� ������� ���������� � ���������� �� ����������
		������������� ���� :^)
	*/
	template <const wnode_members member>
	typename type_selector<member>::result &get() {
		return field<member>::get(*this);
	}


};



}	// end of namespace wic



#endif	// WIC_LIBWIC_WNODE
