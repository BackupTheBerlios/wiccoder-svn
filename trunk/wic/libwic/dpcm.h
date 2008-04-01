/*!	\file     dpcm.h
	\version  0.0.1
	\author   mice, ICQ: 332-292-380, mailto:wonder.mice@gmail.com
	\brief    �������� ������ wic::dpcm - ���� ������

	\todo     ����� �������� ������� ���� dpcm.h
*/

#pragma once

#ifndef WIC_LIBWIC_DPCM
#define WIC_LIBWIC_DPCM

////////////////////////////////////////////////////////////////////////////////
// include

// standard C++ library headers
#include <assert.h>
#include <vector>
#include <string>						// for debug output only
#include <fstream>						// for debug output only
#include <iomanip>						// for debug output only

// external library header
// none

// libwic headers
#include <wic/libwic/defines.h>


////////////////////////////////////////////////////////////////////////////////
// wic namespace
namespace wic {


////////////////////////////////////////////////////////////////////////////////
// encoder class declaration

//! \brief ���������� ���� �����
/*! ����� ������������ ����� �������������� ��� �������, ��� � ��� �������.

	������ �������������:
	\code
	\endcode
*/
class dpcm {
public:
	// public members ----------------------------------------------------------

	//!	\name ������� �����������
	//@{

	//!	\brief �������� �������� � ������ �������� ��������
	/*!	\param[in] value ���������� ��������
		\param[in] predicted ���������� ��������
		\return �������������� ��������

		��� value_t ������������ ����� ��� ���������� � ������������ ��������.
		������ ��� ������ ������������ �������� <i>+</i>, <i>+=</i>, <i>+=</i>
		� <i>-</i>.
	*/
	template <class value_t>
	static value_t encode(const value_t &value, const value_t &predicted)
	{
		const value_t encoded = (value - predicted);

		#ifdef LIBWIC_DEBUG
		_dbg_out_stream << "[encoder] " << std::setw(5) << value;
		_dbg_out_stream << " [" << std::setw(5) << predicted << "]"; 
		_dbg_out_stream << " >>>> " << std::setw(5) << encoded;
		_dbg_out_stream << std::endl;
		#endif

		return encoded;
	}

	//!	\name ������� �������������
	//@{

	//!	\brief ���������� �������� � ������ �������� ��������
	/*!	\param[in] value ������������ ��������
		\param[in] predicted ���������� ��������
		\return �������������� ��������

		��� value_t ������������ ����� ��� ���������� � ������������ ��������.
		������ ��� ������ ������������ �������� <i>+</i>, <i>+=</i>, <i>+=</i>
		� <i>-</i>.
	*/
	template <class value_t>
	static value_t decode(const value_t &value, const value_t &predicted)
	{
		const value_t decoded = (predicted + value);

		#ifdef LIBWIC_DEBUG
		_dbg_out_stream << "[decoder] " << std::setw(5) << decoded;
		_dbg_out_stream << " [" << std::setw(5) << predicted << "]"; 
		_dbg_out_stream << " <<<< " << std::setw(5) << value;
		_dbg_out_stream << std::endl;
		#endif

		return decoded;
	}

	//@}

private:
	// private members ---------------------------------------------------------

	#ifdef LIBWIC_DEBUG
	//!	\brief ����������� �������� ����� ��� ������ ���������� �
	//!	���������� ������
	static std::ofstream _dbg_out_stream;
	#endif
};



}	// end of wic namespace



#endif	// WIC_LIBWIC_DPCM
