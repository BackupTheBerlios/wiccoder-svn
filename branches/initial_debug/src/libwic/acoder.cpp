/*!	\file     acoder.cpp
	\version  0.0.1
	\author   mice, ICQ: 332-292-380, mailto:wonder.mice@gmail.com
	\brief    ���������� ������ wic::acoder - ��������������� ������

	\todo     ����� �������� ������� ���� acoder.cpp
*/


////////////////////////////////////////////////////////////////////////////////
// headers

// standard C++ library headers
// none

// libwic headers
#include <wic/libwic/acoder.h>


////////////////////////////////////////////////////////////////////////////////
// wic namespace
namespace wic {


////////////////////////////////////////////////////////////////////////////////
// acoder class public definitions

/*!	\param[in] buffer_sz ������ ������ (� ������), ���������� ��� ����������
	�����
*/
acoder::acoder(const sz_t buffer_sz):
	_buffer_sz(buffer_sz), _buffer(0),
	_out_stream(0), _aencoder(0),
	_in_stream(0), _adecoder(0)
{
	assert(0 < _buffer_sz);

	// ��������� ������ ��� ���������� �����
	_buffer = new byte_t[_buffer_sz];

	if (0 == _buffer) throw std::bad_alloc();

	// �������� ������� ������� ��� ��������������� ������
	_out_stream	= new BITOutMemStream((char *)_buffer, _buffer_sz);
	_in_stream	= new BITInMemStream((char *)_buffer, _buffer_sz);

	if (0 == _out_stream || 0 == _in_stream) throw std::bad_alloc();
}


/*!	����������� ������ ���������� ���������� �������
*/
acoder::~acoder()
{
	// ����������� ������
	_rm_coders();

	// ����������� ������
	if (0 != _out_stream) delete _out_stream;
	if (0 != _out_stream) delete _in_stream;

	// ����������� �����
	if (0 != _buffer) delete[] _buffer;
}


/*!	\return ������ �������������� ������ (� ������)

	���������� <i>0</i>, ���� �������������� ������� ��� �� ������.

	\sa acoder::use()
*/
sz_t acoder::encoded_sz() const
{
	if (0 == _out_stream) return 0;

	return sz_t(_out_stream->PackedBytes());
}


/*!
*/
void acoder::use(const models_t &models)
{
	_mk_coders(models);
}


/*!
*/
void acoder::encode_start()
{
	// �������� �����������
	assert(0 != _aencoder);
	assert(0 != _out_stream);

	// ������ ��������������� �����������
	_aencoder->StartPacking();

	// ������������� �������
	_init_models(_aencoder);

	//
	_dbg_encode_out = new std::ofstream("dumps/arcoder.encode");
}


/*!
*/
void acoder::encode_stop()
{
	// �������� �����������
	assert(0 != _aencoder);
	assert(0 != _out_stream);

	// ��������� EOF ������� � �����
	_aencoder->PutEOF();

	// ���������� �����������
	_aencoder->EndPacking();

	//
	delete _dbg_encode_out;
}


/*!	\param[in] value �������� ��� �����������
	\param[in] model_no ����� ������
*/
void acoder::put_value(const value_type &value, const sz_t model_no)
{
	// �������� �����������
	assert(0 != _aencoder);
	assert(0 != _out_stream);

	assert(0 <= model_no && model_no < int(_models.size()));

	const model_t &model = _models[model_no];

	assert(model.min <= value && value <= model.max);

	// ����� ������ � ����������� (�������� ��������)
	_aencoder->model(model_no);

	(*_aencoder) << (value + model._delta);

	(*_dbg_encode_out) << "mdl: " << model_no << "; val: " << value << std::endl;
}


/*!
*/
void acoder::decode_start()
{
	// �������� �����������
	assert(0 != _adecoder);
	assert(0 != _in_stream);

	// ������ ��������������� �������������
	_adecoder->StartUnpacking();

	// ������������� �������
	_init_models(_adecoder);

	//
	_dbg_decode_out = new std::ofstream("dumps/arcoder.decode");
}


/*!
*/
void acoder::decode_stop()
{
	// �������� �����������
	assert(0 != _adecoder);
	assert(0 != _in_stream);

	// ���������� �������������
	_adecoder->EndUnpacking();

	//
	delete _dbg_decode_out;
}


/*!	\param[in] model_no ����� ������
*/
acoder::value_type acoder::get_value(const sz_t model_no)
{
	// �������� �����������
	assert(0 != _adecoder);
	assert(0 != _in_stream);

	assert(0 <= model_no && model_no < int(_models.size()));

	const model_t &model = _models[model_no];

	// ����� ������ � ������������� (�������� ��������)
	_adecoder->model(model_no);

	value_type value;
	(*_adecoder) >> value;

	(*_dbg_decode_out) << "mdl: " << model_no << "; val: " << (value - model._delta) << std::endl;

	return (value - model._delta);
}


////////////////////////////////////////////////////////////////////////////////
// acoder class protected definitions

/*!	\param[in] model ������
	\return ���������� �������� � ��������
*/
acoder::value_type acoder::_symbols_count(const model_t &model)
{
	assert(model.min <= model.max);

	return (model.max - model.min + 1);
}


/*!	\param[in] models ������
	\return ������ � ������� ��� ��������������� ������

	\attention ������� ������ ���������� ����� acoder::_refresh_models()
*/
int *acoder::_mk_models(const models_t &models)
{
	// �������� ������� �������
	assert(0 < models.size());

	// ��������� ������ ��� ������
	int *const result = new int[models.size()];

	if (0 == result) throw std::bad_alloc();

	// ����������� ������� �� �������
	int k = 0;

	for (models_t::const_iterator i = models.begin(); models.end() != i; ++i)
	{
		result[k++] = i->_symbols;
	}

	return result;
}


/*!	\param[in,out] models_ptr ������ � ������� ��� ��������������� ������
*/
void acoder::_rm_models(const int *const models_ptr)
{
	if (0 != models_ptr) delete[] models_ptr;
}


/*!	\param[in] models ������ ��������������� ������ ��� ����������

	������� ��������� ��������� ��������:
	- ������������� ���� model_t::_delta
	- ������������� ���� model_t::_symbols
*/
void acoder::_refresh_models(models_t &models)
{
	for (models_t::iterator i = models.begin(); models.end() != i; ++i)
	{
		model_t &model = (*i);
		model._delta = - model.min;
		model._symbols = _symbols_count(model);
	}
}


/*!	\param[in] coder_base ������� ����� ��� �������������� �������� �
	��������
*/
void acoder::_init_models(arcoder_base *const coder_base)
{
	// �������� �����������
	assert(0 != coder_base);
	assert(coder_base->Models().Size() == _models.size());

	// ����� ���������
	coder_base->ResetStatistics();

	// ������������� �������
	/*
	for (sz_t i = 0; coder_base->Models().Size() > i; i++)
	{
		const model_t &model = _models[i];

		coder_base->model(i);

		for (int j = 0; model._symbols > j; ++j)
		{
			coder_base->update_model(j);
		}
	}
	*/
}


/*!	\param[in] models ������

	������� ������ ������� � �������.
*/
void acoder::_mk_coders(const models_t &models)
{
	// �������� �����������
	assert(0 != _out_stream);
	assert(0 != _in_stream);

	// ������������ ������ ��������
	_rm_coders();

	// ���������� �������� �������
	_models = models;

	// ���������� �������
	_refresh_models(_models);

	// ������ ��� ��������
	const int *const models_ptr = _mk_models(_models);

	// �������� ����� �������� �������� � ��������
	_aencoder = new ArCoderWithBitCounter<BITOutMemStream>(
						int(_models.size()), models_ptr, *_out_stream);

	_adecoder = new ArDecoderWithBitCounter<BITInMemStream>(
						int(_models.size()), models_ptr, *_in_stream);

	// ������������ ������� ��� ��������
	_rm_models(models_ptr);

	// �������� ���������� ��������
	if (0 == _aencoder || 0 == _adecoder) throw std::bad_alloc();
}


/*!
*/
void acoder::_rm_coders()
{
	if (0 != _aencoder) {
		delete _aencoder;
		_aencoder = 0;
	}

	if (0 != _adecoder) {
		delete _adecoder;
		_adecoder = 0;
	}
}


/*! \param[in] coder_base ������� ����� ��� �������������� �������� �
	��������
	\param[in] value ��������, ������ �� �������� ������
	\param[in] model_no ����� ������������ ������
	\return ������� ������� �� ����������� �������
*/
double acoder::_entropy_eval(arcoder_base *const coder_base,
							 const value_type &value, const sz_t model_no)
{
	// �������� �����������
	assert(0 != coder_base);
	assert(coder_base->Models().Size() == _models.size());
	assert(0 <= model_no && model_no < int(_models.size()));

	const model_t &model = _models[model_no];

	assert(model.min <= value && value <= model.max);

	// ����� ������ � ������� ������� ������ (�������� ��������)
	coder_base->model(model_no);

	return (coder_base->entropy_eval(value + model._delta));
}



}	// namespace wic
