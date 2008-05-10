/*!	\file     wnode.h
	\version  0.0.1
	\author   mice, ICQ: 332-292-380, mailto:wonder.mice@gmail.com
	\brief    ����������� ��������� wic::wnode - �������� �������-�������

	\todo     ����� �������� ������� ���� wnode.h
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

//! \brief ���� ������ ������� ������������� (������� �������)
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
	wk_t wq;
	//!	\brief �������� ������������������� ������������
	wk_t wc;
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
	inline static wk_t round(const w_t &w) { return wk_t(floor(w + 0.5)); }

	//! \brief ���������� �����������
	/*!	\param[in] w �������� ������� ������������ ��� �����������
		\param[in] q ������������
		\return ��������������� �������� ������������
	*/
	inline static wk_t quantize(const w_t &w, const q_t &q) {
		return round(w / q);
	}

	//!	\brief ��������� ���� ���������
	/*!	\param[in] v ��������
		\return � ����������� �� �������� ��������� <i>v</i>:
		- ���� (<i>v</i> > 0) ���������� 1
		- ���� (<i>v</i> > 0) ���������� -1
		- ���� (<i>v</i> == 0) ���������� 0
	*/
	template <class T>
	inline static int sign(const T &v) {
		return (0 < v)? 1: ((0 > v)? -1: 0);
	}

	//!	\brief ��������� "�������������" ���� ���������
	/*!	\param[in] v ��������
		\return � ����������� �� �������� ��������� <i>v</i>:
		- ���� (<i>v</i> > 0) ���������� 1
		- ���� (<i>v</i> > 0) ���������� 2
		- ���� (<i>v</i> == 0) ���������� 0

		���������� sign(), �� ��� �������������� ��������� ���������� 2,
		� �� -1.
	*/
	template <class T>
	inline static int signp(const T &v) {
		return (0 < v)? 1: ((0 > v)? 2: 0);
	}

	//!	\brief ����������� ��������� ������� sign() � ��������� �������
	//!	signp() ��� ��������
	/*!	\param[in] s ���� ����������� �������� sign() ��� signp()
		\return ���� <i>s</i> ����� -1, ���������� 2. ���� <i>s</i>
		����� 2, ���������� -1. ����� ���������� <i>s</i>.
	*/
	inline static int signx(const int s) {
		switch (s) {
			case -1:	return 2;
			case 2:		return -1;
			default:	return 0;
		}
	}

	//!	\brief ����������� �������� ������������ �������� sign()
	inline static int sign_min() { return -1; }

	//!	\brief ������������ �������� ������������ �������� sign()
	inline static int sign_max() { return 1; }

	//!	\brief ����������� �������� ������������ �������� signp()
	inline static int signp_min() { return 0; }

	//!	\brief ������������ �������� ������������ �������� signp()
	inline static int signp_max() { return 2; }

	//! \brief ���������� �������������
	/*!	\param[in] wk ��������������� �������� ������� ������������
		\param[in] q ������������
		\return ��������������� (��������������) �������� ������������
	*/
	inline static w_t dequantize(const wk_t &wk, const q_t &q) {
		return w_t(wk * q);
	}

	//! \brief ����� �������� ��� ������������� ����� ���������
	enum wnode_members {
		member_w,			//!< \brief ������������� ���� w
		member_wq,			//!< \brief ������������� ���� wq
		member_wc,			//!< \brief ������������� ���� wc
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
		typedef wk_t result;
	};

	//! \brief ������������� ��� ���� wc
	template <>
	struct type_selector<member_wc> {
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

	//! \brief ������������� ��� ���� invalid
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

	//! \brief ������������� ��� ���� wc
	template <>
	struct field<member_wc> {
		static type_selector<member_wc>::result &get(wnode &node) {
			return node.wc;
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

	//! \brief ������������� ��� ���� invalid
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

	//!	\brief ��������� ������� ������������ �������� ���� �� ���
	//!	��������������
	/*!	����������� ������ ���������� �������
	*/
	template <const wnode_members member>
	const typename type_selector<member>::result &get() const {
		return field<member>::get(*(const_cast<wnode *>(this)));
	}

};



}	// end of namespace wic



#endif	// WIC_LIBWIC_WNODE
