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
template <class value_t>
class dpcm {
public:
	// public types ------------------------------------------------------------

	//!	\brief ��� ���������� � ������������ ��������
	/*!	������ ��� ������ ������������ �������� <i>+</i>, <i>+=</i>,
		<i>+=</i> � <i>-</i>
	*/
	typedef value_t value_type;

	// public members ----------------------------------------------------------

	//!	\name ������������ � �����������
	//@{

	//!	\brief �����������
	/*!	� ���������� ������ (�������� #LIBWIC_DEBUG) ����������� ���������
		���� "dumps/[dpcm]history.out" �� ������ ��� ������ ����������
		����������.
	*/
	dpcm()
	{
		#ifdef LIBWIC_DEBUG
		_dbg_out_stream.open("dumps/[dpcm]history.out");
		#endif
	}

	//!	\brief ����������
	/*!	������ �� ������
	*/
	~dpcm()
	{
		// ������ �� ���� ������
	}

	//@}

	//!	\name ������� �����������
	//@{

	//!	\brief �������� �����������
	/*!	������ �� ������. ���������� ��� ������������ ������� ������
		�����������.
	*/
	void encode_start()
	{
	}

	//!	\brief ��������� �����������
	/*!	������� ������ �� ������ � ���������� ������ ��� ��������� �
		encode_start()
	*/
	void encode_stop()
	{
	}

	//!	\brief �������� �������� � ������ �������� ��������
	/*!	\param[in] value ���������� ��������
		\param[in] predicted ���������� ��������
		\return �������������� ��������
	*/
	value_type encode(const value_type &value, const value_type &predicted)
	{
		const value_type encoded = (value - predicted);

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

	//!	\brief �������� �������������
	/*!	������ �� ������. ���������� ��� ������������ ������� ������
		�������������.
	*/
	void decode_start()
	{
	}

	//!	\brief ��������� �������������
	/*!	������� ������ �� ������ � ���������� ������ ��� ��������� �
		decode_start()
	*/
	void decode_stop()
	{
	}

	//!	\brief ���������� �������� � ������ ��������
	/*!	\param[in] value �������������� ��������
		\return �������������� ��������
	*/
	value_type decode(const value_type &value)
	{
		const value_type predicted	= _predict(_dec_history);
		const value_type decoded	= (predicted + value);

		_history_put(decoded, _dec_history);

		#ifdef LIBWIC_DEBUG
		_dbg_out_stream << "[decoder] " << std::setw(5) << decoded;
		_dbg_out_stream << " [" << std::setw(5) << predicted << "]"; 
		_dbg_out_stream << " <<<< " << std::setw(5) << value;
		_dbg_out_stream << std::endl;
		#endif

		return decoded;
	}

	//!	\brief ���������� �������� � ������ �������� ��������
	/*!	\param[in] value ������������ ��������
		\param[in] predicted ���������� ��������
		\return �������������� ��������
	*/
	value_type decode(const value_type &value, const value_type &predicted)
	{
		const value_type decoded = (predicted + value);

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
	std::ofstream _dbg_out_stream;
	#endif
};



}	// end of wic namespace



#endif	// WIC_LIBWIC_DPCM
