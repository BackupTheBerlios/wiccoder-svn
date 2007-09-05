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

	// public constants --------------------------------------------------------

	//!	\brief ������ �� ��������� ��� �������� ������� �������� / ��������
	static const int DEFAULT_HISTORY_SIZE	= 1;

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
	/*!	\param[in] history_sz ������ �������� �������, ������� �����
		������������ ��� ���������� ���������
	*/
	void encode_start(const int history_sz = DEFAULT_HISTORY_SIZE)
	{
		_reset_history(_enc_history, history_sz);
	}

	//!	\brief ��������� �����������
	/*!	������� ������ �� ������ � ���������� ������ ��� ��������� �
		encode_start()
	*/
	void encode_stop()
	{
	}

	//!	\brief �������� �������� � ������ ��������
	/*!	\param[in] value ���������� ��������
		\return �������������� ��������
	*/
	value_type encode(const value_type &value)
	{
		const value_type predicted	= _predict(_enc_history);
		const value_type encoded	= (value - predicted);

		_history_put(value, _enc_history);

		#ifdef LIBWIC_DEBUG
		_dbg_out_stream << "[encoder] " << std::setw(5) << value;
		_dbg_out_stream << " [" << std::setw(5) << predicted << "]"; 
		_dbg_out_stream << " >>>> " << std::setw(5) << encoded;
		_dbg_out_stream << std::endl;
		#endif

		return encoded;
	}

	//!	\brief �������� �������� � ������ �������� ��������
	/*!	\param[in] value ���������� ��������
		\param[in] predicted ���������� ��������
		\return �������������� ��������
	*/
	value_type encode(const value_type &value, const value_type &predicted)
	{
		return (value - predicted);
	}

	//!	\name ������� �������������
	//@{

	//!	\brief �������� �������������
	/*!	\param[in] history_sz ������ �������� �������, ������� �����
		������������ ��� ���������� ���������
	*/
	void decode_start(const int history_sz = DEFAULT_HISTORY_SIZE)
	{
		_reset_history(_dec_history, history_sz);
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
		return (predicted + value);
	}

	//@}

protected:
	// protected types ---------------------------------------------------------

	//!	\brief ��� ��� ������������� ���������� �������������� ���
	//!	�������������� ��������
	typedef std::vector<value_type> _history_vals_t;

	//!	\brief ������� ���� ������
	struct _history_t {
		//!	\brief ������ ���������� ��������
		_history_vals_t vals;
		//!	\brief ������� ����� ���������� ����������� � ������� ��������
		int pos;
		//!	\brief ���������� ���������� �������� ����� �������� <i>pos</i>
		int count;
	};

	// protected constants -----------------------------------------------------

	// protected methods -------------------------------------------------------

	//!	\brief ���������� ����� ������� � ��������� ���������
	/*!	\param[in] history ������������ �������
		\param[in] history_sz ������ ������� ��������
	*/
	void _reset_history(_history_t &history, const int history_sz)
	{
		history.vals.resize(history_sz);
		history.pos		= 0;
		history.count	= 0;
	}

	//!	\brief ������� ��������
	/*!	\param[in] history �������, ������� ����� ������������ ��� ����������
		��������
		\return ����������� �������� ��������
	*/
	value_type _predict(const _history_t &history)
	{
		// ���������� 0, ���� ������� �����
		if (0 == history.count) return 0;

		// ��������� ��������
		value_type numerator	= 0;

		// ����������� ��������
		int denominator			= 0;

		// ��������� ��������
		const int offset = int(history.vals.size() + history.pos - 1);

		// ���� �� ��������� � �������
		for (int factor = history.count; 0 < factor; --factor)
		{
			const int j = int(offset % history.vals.size());

			numerator	+= (factor * history.vals[j]);

			denominator	+= factor;
		}

		return (numerator / denominator);
	}

	//! \brief �������� �������� � �������
	/*!	\param[in] val ��������, ���������� � �������
		\param[in] history �������������� �������
	*/
	void _history_put(const value_type &val, _history_t &history)
	{
		// ��������� ���������� �������� ���� ��� ���� �����
		if (history.count < int(history.vals.size())) ++history.count;

		// ��������� �������� � �������
		history.vals[history.pos] = val;

		// ���������� �������� �������
		if (++history.pos == history.vals.size()) history.pos = 0;
	}

private:
	// private members ---------------------------------------------------------

	//!	\brief ������� �������������� ��������
	_history_t _enc_history;

	//!	\brief ������� �������������� ��������
	_history_t _dec_history;

	#ifdef LIBWIC_DEBUG
	//!	\brief ����������� �������� ����� ��� ������ ���������� �
	//!	���������� ������
	std::ofstream _dbg_out_stream;
	#endif
};



}	// end of wic namespace



#endif	// WIC_LIBWIC_DPCM
