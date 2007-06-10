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

//! \brief �������������� ����� (������� � �������)
/*!	����� ������������ ����� ������� �������� ��� ��������������� ������.
	����� ��������� �����������:
	- ���������� � ���� ����������� �������� � ��������.
	- ����� ���������� ����� ��� ����������� � �������������, ��� ���������
	  ��������� ������ � �������.
	- ����� ����� ������� ��� ������ � ��������.

	������ �������������:
	\code
	wic::acoder acoder(1024);

	wic::acoder::models_t models;
	// populate models vector
	acoder.use(models);

	acoder.encode_start();
	acoder.put(1, 0);
	acoder.put(2, 1);
	acoder.put(3, 0);
	acoder.encode_stop();

	acoder.decode_start();
	std::cout << acoder.get_value(0);
	std::cout << acoder.get_value(1);
	std::cout << acoder.get_value(0);
	acoder.decode_stop();
	\endcode

	\todo ���������� �������������� ���� �����
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
		//!	\brief ��������, ����������� ��� ����������� �����������
		//! �������� �� �������� ���� ������.
		/*!	\note ��� ���� ����������� <b>�������������</b> ��� �������������
			��������.
		*/
		value_type _delta;
		//!	\brief ���������� �������� � �������� ������
		value_type _symbols;
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

	//!	\brief ��������� �� ���������� �����
	/*!	\return ��������� �� ���������� �����
	*/
	byte_t *buffer() const { return _buffer; }

	//!	\brief ������ ����������� ������
	/*!	\return ������ ����������� ������ (� ������)
	*/
	const sz_t buffer_sz() const { return _buffer_sz; }

	//! \brief ������ �������������� ������
	sz_t encoded_sz() const;

	//@}

	//!	\name ������ � ��������
	//@{

	//!	\brief ������������� ������, ������������ �������������� �������
	void use(const models_t &models);

	//@}

	//!	\name ������� ��������
	//@{

	//! \brief �������� �������������� �����������
	void encode_start();

	//! \brief ��������� �������������� �����������
	void encode_stop();

	//!	\brief �������� �������� � �������� ��� � ������� �����
	void put_value(const value_type &value, const sz_t model_no);

	//!	\brief �������� �������� � �������� ��� � ������� �����
	/*!	\param[in] param �������� ��� �����������
		\param[in] model_no ����� ������

		������� ���������� �� acoder::put_value() ���, ��� ���� �����
		����������� ���������� �������������� �����.
	*/
	template <class param_t>
	void put(const param_t &param, const sz_t model_no)
	{
		assert(sizeof(param_t) <= sizeof(value_type));

		put_value(reinterpret_cast<const value_type &>(param), model_no);
	}

	//!	\brief ���������� ������� ������� ������, ����������� ��� �����������
	//!	�������
	/*!	\param[in] param ������ �� �������� ������
		\param[in] model_no ����� ������������ ������
		\return ������� ������� �� ����������� �������
	*/
	template <class param_t>
	double enc_entropy(const param_t &param, const sz_t model_no)
	{
		assert(sizeof(param_t) <= sizeof(value_type));

		return _entropy_eval(_aencoder,
							 reinterpret_cast<const value_type &>(param),
							 model_no);
	}

	//@}

	//!	\name ������� ��������
	//@{

	//! \brief �������� �������������� �������������
	void decode_start();

	//! \brief ��������� �������������� �������������
	void decode_stop();

	//!	\brief �������� ��������� ������ �� �������� ������ �
	//!	���������� ���
	value_type get_value(const sz_t model_no);

	//!	\brief �������� ��������� ������ �� �������� ������ �
	//!	���������� ���
	/*!	\param[in] model_no ����� ������

		������� ���������� �� acoder::get_value() ���, ��� ���� �����
		����������� ���������� �������������� �����.
	*/
	template <class result_t>
	result_t get(const sz_t model_no)
	{
		return reinterpret_cast<result_t>(get_value(model_no));
	}

	//@}

protected:
	// protected methods -------------------------------------------------------

	//!	\name �������� �������
	//@{

	//!	\brief �������� ���������� �������� � �������� �� ������������ �
	//!	������������� ��������� �������� � ������
	static value_type _symbols_count(const model_t &model);

	//!	\brief ����������� ������ ������� � �������� ������� ���
	//!	��������������� ������
	static int *_mk_models(const models_t &models);

	//!	\brief ����������� �������� ������� ��� ��������������� ������
	static void _rm_models(const int *const models_ptr);

	//!	\brief ��������� ���������� ������, �������� ����������� ����
	static void _refresh_models(models_t &models);

	//@}

	//!	\name �������� � �������� �������
	//@{

	//!	\brief ���������� ������������� ������� ��������������� ������
	void _init_models(arcoder_base *const coder_base);

	//!	\brief ������ �������������� ������� � �������
	void _mk_coders(const models_t &models);

	//!	\brief ������� �������������� ������� � �������
	void _rm_coders();

	//@}

	//!	\name �������������� �������
	//@{

	//!	\brief ������� ������� ������ �� ����������� �������
	double _entropy_eval(arcoder_base *const coder_base,
						 const value_type &value, const sz_t model_no);

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
