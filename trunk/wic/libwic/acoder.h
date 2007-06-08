/*!	\file     acoder.h
	\version  0.0.1
	\author   mice, ICQ: 332-292-380, mailto:wonder.mice@gmail.com
	\brief    �������� ������ wic::acoder - ��������������� ������

	\todo     ����� �������� ������� ���� acoder.h
*/

#pragma once

#ifndef WIC_LIBWIC_ACODER
#define WIC_LIBWIC_ACODER

////////////////////////////////////////////////////////////////////////////////
// include

// standard C++ library headers
#include <new>							// for std::bad_alloc exception class
#include <vector>
#include <assert.h>

// external library headers
#include <arcoder/arcodec.h>
#include <arcoder/arbitstream.h>

// libwic headers
#include <wic/libwic/types.h>


////////////////////////////////////////////////////////////////////////////////
// wic namespace
namespace wic {


////////////////////////////////////////////////////////////////////////////////
// types


////////////////////////////////////////////////////////////////////////////////
// acoder class declaration

//! \brief �������������� ����� (������� + �������)
/*!	
*/
class acoder {
public:
	// public types ------------------------------------------------------------

	//!	\brief ���, ������� ����� ����������� �������������� ������� � �������
	/*!	\note ����� �����, ����� ��� ��� ��� �� ������ (<i>signed</i>). ���
		������ ��� �����, ������������� � ��������� �������.
	*/
	typedef int value_type;

	//!	\brief ��� ��������� ������ ��������������� ������
	struct model_t {
		//!	\brief ����������� �������� �������
		value_type min;
		//!	\brief ������������ �������� �������
		value_type max;
	};

	//!	\brief �������� �������, ������������ �������������� �������
	typedef std::vector<model_t> models_t;

	// public constants --------------------------------------------------------

	// public methods ----------------------------------------------------------

	//!	\name ������������ � �����������
	//@{

	//!	\brief ������������
	acoder(const sz_t buffer_sz);

	//!	\brief ����������
	~acoder();

	//@}

	//!	\name ������ � ���������� �������
	//@{

	byte_t *get_buffer() const;
	const sz_t buffer_sz() const {}

	//@}

	//!	\name �������������
	//@{

	void enc_begin();
	void enc_end();

	void dec_begin();
	void dec_end();

	//@}

	//!	\name ����������� �������������
	//@{

	template<class result_t>
	result_t get(const sz_t model)
	{
		_adecoder->model(model);

		value_type value;
		(*_adecoder) >> value;

		return reinterpret_cast<result_t>(value);
	}

	template<class param_t>
	void put(const param_t &val, const sz_t model)
	{
		_aencoder->model(model);

		(*_aencoder) << reinterpret_cast<param_t>(val);
	}

	//@}

protected:
	// protected methods -------------------------------------------------------

	//!	\name �������� ������� ��� ��������������� ������
	//@{

	//!	\brief �������� ���������� �������� � �������� �� ������������ �
	//!	������������� ��������� �������� � ������
	static value_type _symbols_count(const model_t &model);

	//!	\brief ����������� ������ ������� � �������� ������� ���
	//!	��������������� ������
	static int *_mk_models(const models_t &models);

	//!	\brief ����������� �������� ������� ��� ��������������� ������
	static void _rm_models(const int *const models_ptr);

	//@}

	//!	\name �������� � �������� �������
	//@{

	//!	\brief ������� �������������� ������� � �������
	void _rm_coders();

	//!	\brief ������ �������������� ������� � �������
	void _mk_coders(const models_t &models);

	//@}

private:
	// private data ------------------------------------------------------------

	//!	\brief ����� ���������� �����, ������� ����� ��������������
	//!	�������������� ��������� � ���������
	byte_t *_buffer;

	//!	\brief ������ ����������� ������ <i>_buffer</i> � ������
	const sz_t _buffer_sz;

	//!	\brief ������ ������������ �������������� �������
	models_t _models;

	//!	\brief ������� ����� ��� ��������
	BITOutMemStream *_out_stream;

	//!	\brief �������������� �������
    ArCoderWithBitCounter<BITOutMemStream> *_aencoder;

	//!	\brief ������� ����� ��� ��������
	BITInMemStream *_in_stream;

	//!	\brief �������������� �������
	ArDecoderWithBitCounter<BITInMemStream> *_adecoder;
};



}	// end of namespace wic



#endif	// WIC_LIBWIC_ACODER
