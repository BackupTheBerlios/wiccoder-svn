/*******************************************************************************
* file:         encoder.h                                                      *
* version:      0.1.0                                                          *
* author:       mice (mailto:my_mice@mail.ru, ICQ:332-292-380)                 *
* description:  not available                                                  *
* tests:        none                                                           *
*******************************************************************************/

#pragma once

#ifndef WIC_LIBWIC_ENCODER
#define WIC_LIBWIC_ENCODER

////////////////////////////////////////////////////////////////////////////////
// include

// standard C++ library headers
#include <assert.h>

// libwic headers
#include <wic/libwic/types.h>
#include <wic/libwic/wtree.h>
#include <wic/libwic/iterators.h>


////////////////////////////////////////////////////////////////////////////////
// wic namespace
namespace wic {


////////////////////////////////////////////////////////////////////////////////
// encoder class declaration

//! \brief ������ ��� ��������. ������������ ������ �����������.
/*! ����� ������������� ������ ��� ������ �����������. ��������� �������
	���� � �� �� ����������� ��������� ��� ��� ������� ���������� �����������.
*/
class encoder {
public:
	// public constants --------------------------------------------------------
	// public types ------------------------------------------------------------
	// public methods ----------------------------------------------------------

	//!	\brief ������������
	encoder(const w_t *const image,
			const sz_t width, const sz_t height, const sz_t lvls);

	//!	\brief ����������
	~encoder();

	//!	\brief ������� �������������� ���������������� ����������� �����������
	void encode();

protected:
	// protected methods -------------------------------------------------------

	//!	\name ����� ������ �� ��������
	//@{

	//! \brief ��������� ������� IndSpec(<i>S<sub>j</sub></i>) �� 35.pdf
	sz_t _ind_spec(const pi_t &s, const sz_t lvl);

	//! \brief ��������� ������� IndMap(<i>P<sub>i</sub></i>) �� 35.pdf
	sz_t _ind_map(const pi_t &p, const bool is_LL = false);

	//@}

	//!	\name ���� �����������
	//@{

	//! \brief ��� 1. ����������������.
	void _encode_step_1();

	//@}

private:
	// private data ------------------------------------------------------------

	//! \brief ������� ������ �������������
	wtree _wtree;
};



}	// end of namespace wic



#endif	// WIC_LIBWIC_ENCODER
