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

//! \brief ������ �������� ������� �������������
/*! ������� ����� ������ ������ ����������, ���������� �������� �������
	������������� � �������� � �� ���������. ��� ������� ������ �����
	������������� ��������� �����������:
	- �������� ������������ �������� ������� �������������
	- �������� ���������������� �������� ������� �������������
	- ��� �������� ������� �������� (��� ������� �������� ����� �
	  ��� ������� � ����������)
	- ��������� ������� ���������� ������
	- ������� ���������� ��� ���������� ������������

	������� ������ ���� ��������� ������ ���������� ����������. �����������
	����� �������������� ����������� ������ ������. �������� RD-�������
	�������� ����������� ��� ���������� ������������ ������.

	\sa wnode, subbands
*/
class wtree {
public:
	// public constants --------------------------------------------------------
	// public types ------------------------------------------------------------
	// public methods ----------------------------------------------------------

	//!	\brief ������������
	wtree(const sz_t width, const sz_t height, const sz_t lvls);

	//!	\brief ����������
	~wtree();

	//! \brief ���������� ���������� ������������� �� ���� ��������
	sz_t coefs() const { return (_width * _height); }

	//! \brief ���������� ���������� ���� ���������� ��������
	sz_t nodes_sz() const;

	//! \brief ��������� ������ �� ������
	void load(const w_t *const from);

	//! \brief ���������� �����������
	void quantize(const q_t q = 1);

	//! \brief ���������� ���������� � ���������
	const subbands &sb() const;

	//! \brief ��������� �������� ������� �� �����������
	const wnode &at(const sz_t x, const sz_t y) const;
	
	//! \brief ��������� �������� ������� �� �����������
	wnode &at(const sz_t x, const sz_t y);

	//! \brief ��������� �������� ������� �� �����������
	const wnode &operator()(const sz_t x, const sz_t y) const {
		return at(x, y);
	}
	
	//! \brief ��������� �������� ������� �� �����������
	wnode &operator()(const sz_t x, const sz_t y) { return at(x, y); }

	//! \brief ��������� ��������� ��������
	p_t get_pos(const wnode &node) const;

	//!	\brief ���������� ���������� ������������� ��������
	p_t prnt(const p_t &c);

	//!	\brief ���������� ���������� ��������� �������� ��������
	p_t child(const p_t &p);

	//! \brief ��������� ������ �������������
	void cut(const p_t &p);

protected:
	// protected methods -------------------------------------------------------

	//! \brief ��������� ��� ���������� � �������� � 0
	void _reset_trees_content();

private:
	// private data ------------------------------------------------------------

	//! \brief ������ ����� �������
	sz_t _width;

	//! \brief ������ ����� �������
	sz_t _height;

	//! \brief ���������� ������� ���������� �������
	sz_t _lvls;

	//!	\brief ������������, ������� ��� ����������� � ��������� ���
	q_t	_q;

	//! \brief ����� �������
	wnode *_nodes;

	//! \brief �������� ����������
	subbands *_subbands;
};



}	// end of namespace wic



#endif	// WIC_LIBWIC_WTREE
