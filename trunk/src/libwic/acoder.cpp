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

	#ifdef LIBWIC_DEBUG
	_dbg_out_stream.open("dumps/[acoder]models.out");
	#endif

	#ifdef LIBWIC_ACODER_LOG_VALUES
	_dbg_vals_stream.open("dumps/[acoder]values.out",
						  std::ios_base::app | std::ios_base::out);
	#endif
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


/*!	\param[in] models �������� ������� ��������������� ������
*/
void acoder::use(const models_t &models)
{
	_mk_coders(models);
}


/*!	\param[in] model_no ����� ������
	\return ����������� �������� �������, ��������������� � ���������
	������
*/
const acoder::value_type &acoder::rmin(const sz_t &model_no) const
{
	assert(0 <= model_no && model_no < sz_t(_models.size()));

	return _models[model_no]._rmin;
}


/*!	\param[in] model_no ����� ������
	\return ������������ �������� �������, ��������������� � ���������
	������
*/
const acoder::value_type &acoder::rmax(const sz_t &model_no) const
{
	assert(0 <= model_no && model_no < sz_t(_models.size()));

	return _models[model_no]._rmax;
}


/*!	\param[in] model_no ����� ������
	\return �������������� �������� ������� ���������� (������������) ��������
*/
const double &acoder::abs_average(const sz_t &model_no) const
{
	assert(0 <= model_no && model_no < sz_t(_models.size()));

	return _models[model_no]._abs_average;
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

	#ifdef LIBWIC_DEBUG
	for (models_t::iterator i = _models.begin(); _models.end() != i; ++i)
	{
		model_t &model = (*i);

		model._encoded_symbols = 0;
		model._encoded_freqs.assign(model._symbols, 0);
	}
	#endif

	#ifdef LIBWIC_ACODER_LOG_VALUES
	if (_dbg_vals_stream.good())
	{
		_dbg_vals_stream << "Start Encoding:" << std::endl;
	}
	#endif
}


/*!
*/
void acoder::encode_stop()
{
	// �������� �����������
	assert(0 != _aencoder);
	assert(0 != _out_stream);

	// ��������� EOF ������� � �����
	// _aencoder->PutEOF();

	// ���������� �����������
	_aencoder->EndPacking();

	#ifdef LIBWIC_DEBUG
	if (_dbg_out_stream.good()) dbg_encoder_info(_dbg_out_stream);
	#endif

	#ifdef LIBWIC_ACODER_LOG_VALUES
	if (_dbg_vals_stream.good())
	{
		_dbg_vals_stream << "Stop Encoding." << std::endl << std::endl;
	}
	#endif
}


/*!	\param[in] value �������� ��� �����������
	\param[in] model_no ����� ������
	\param[in] virtual_encode ���� <i>true</i>, �� ����� �������������
	����������� ����������� (�������� ����������� � ��������������
	������� ��� ��������� ��������� �������� � �������� �����).

	\note � ���������� ������ ������� �������� �������� ����� 
	model_t::_encoded_symbols � model_t::_encoded_freqs ��� ���������
	������, ���� ��� ����������� �����������, �������� �� ��, ���
	��������� ����������� �� ������������.
*/
void acoder::put_value(const value_type &value, const sz_t model_no,
					   const bool virtual_encode)
{
	// �������� �����������
	assert(0 != _aencoder);
	assert(0 != _out_stream);

	assert(0 <= model_no && model_no < sz_t(_models.size()));

	model_t &model = _models[model_no];

	assert(model.min <= value && value <= model.max);

	const value_type enc_val = (value + model._delta);

	assert(0 <= enc_val && enc_val < model._symbols);

	#ifdef LIBWIC_DEBUG
	++(model._encoded_symbols);
	++(model._encoded_freqs[enc_val]);
	#endif

	// ���������� ������������� � ������������ �������� ���������������
	// �������
	if (value < model._rmin) model._rmin = value;
	if (value > model._rmax) model._rmax = value;

	// ���������� �������� ��������������� �������� ���������� ��������
	++model._n;
	model._abs_average = (model._abs_average * (model._n - 1) + abs(value)) /
							model._n;

	// ����� ������ � ����������� (�������� ��������)
	_aencoder->model(model_no);

	// ��� ����������� ����������� �� ������������ �������� ������
	// � �������� �����
	if (!virtual_encode) _aencoder->put(enc_val);

	// ���������� ������ ��������������� ������
	_aencoder->update_model(enc_val);

	#ifdef LIBWIC_ACODER_LOG_VALUES
	if (_dbg_vals_stream.good())
	{
		_dbg_vals_stream << "put: " << value << " as " << enc_val
						 << " in #" << model_no << std::endl;
	}
	#endif
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

	#ifdef LIBWIC_DEBUG
	for (models_t::iterator i = _models.begin(); _models.end() != i; ++i)
	{
		model_t &model = (*i);

		model._decoded_symbols = 0;
		model._decoded_freqs.assign(model._symbols, 0);
	}
	#endif

	#ifdef LIBWIC_ACODER_LOG_VALUES
	if (_dbg_vals_stream.good())
	{
		_dbg_vals_stream << "Start Decoding:" << std::endl;
	}
	#endif
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

	#ifdef LIBWIC_DEBUG
	if (_dbg_out_stream.good()) dbg_decoder_info(_dbg_out_stream);
	#endif

	#ifdef LIBWIC_ACODER_LOG_VALUES
	if (_dbg_vals_stream.good())
	{
		_dbg_vals_stream << "Stop Decoding." << std::endl << std::endl;
	}
	#endif
}


/*!	\param[in] model_no ����� ������
*/
acoder::value_type acoder::get_value(const sz_t model_no)
{
	// �������� �����������
	assert(0 != _adecoder);
	assert(0 != _in_stream);

	assert(0 <= model_no && model_no < int(_models.size()));

	model_t &model = _models[model_no];

	// ����� ������ � ������������� (�������� ��������)
	_adecoder->model(model_no);

	value_type dec_val;
	(*_adecoder) >> dec_val;

	#ifdef LIBWIC_DEBUG
	++(model._decoded_symbols);
	++(model._decoded_freqs[dec_val]);
	#endif

	dec_val -= model._delta;

	// ���������� ������������� � ������������ �������� ���������������
	// �������
	if (dec_val < model._rmin) model._rmin = dec_val;
	if (dec_val > model._rmax) model._rmax = dec_val;

	// ���������� �������� ��������������� �������� ���������� ��������
	++model._n;
	model._abs_average = (model._abs_average * (model._n - 1) + abs(dec_val)) /
							model._n;

	#ifdef LIBWIC_ACODER_LOG_VALUES
	if (_dbg_vals_stream.good())
	{
		_dbg_vals_stream << "get: " << dec_val << " as "
						 << (dec_val + model._delta)
						 << " from #" << model_no << std::endl;
	}
	#endif

	return dec_val;
}


/*!	\param[out] out ����������� ����� ��� ������
*/
void acoder::dbg_encoder_info(std::ostream &out)
{
	assert(0 != _aencoder);

	out << "[encoder models information] ----------------------------";
	out << std::endl;

	#ifdef LIBWIC_DEBUG
	unsigned int total_symbols = 0;
	#endif

	for (sz_t i = 0; sz_t(_models.size()) > i; ++i)
	{
		out << "#" << std::setw(2) << i;

		_aencoder->model(i);
		const model_t &model = _models[i];

		#ifdef LIBWIC_DEBUG
		out << "[" << std::setw(5) << model._symbols << "]";
		#endif

		const unsigned int bits = _aencoder->model()->bit_counter;

		out << ": ";
		out << std::setw(8) << bits << " bits |";
		out << std::setw(7) << (bits / 8) << " bytes";

		#ifdef LIBWIC_DEBUG
		const unsigned int encoded_symbols = model._encoded_symbols;
		total_symbols += encoded_symbols;

		out << " |" << std::setw(7) << encoded_symbols  << " symbols";
		#endif

		out << " | M = " << std::setw(7) << model._abs_average;

		out << std::endl;

		#ifdef LIBWIC_DEBUG
		_dbg_freqs_out(out, model._encoded_freqs, model, i, _aencoder);
		#endif
	}

	#ifdef LIBWIC_DEBUG
	out << "Total symbols: " << std::setw(6) << total_symbols << std::endl;
	#endif

	out << "[/encoder models information] ---------------------------";
	out << std::endl;
}


/*!	\param[in] file ��� ����� � ������� ����� ������������� �����
*/
void acoder::dbg_encoder_info(const std::string &file)
{
	std::ofstream out(file.c_str());

	if (out.good()) dbg_encoder_info(out);
}


/*!	\param[out] out ����������� ����� ��� ������
*/
void acoder::dbg_decoder_info(std::ostream &out)
{
	assert(0 != _adecoder);

	out << "[decoder models information] ----------------------------";
	out << std::endl;

	#ifdef LIBWIC_DEBUG
	unsigned int total_symbols = 0;
	#endif

	for (sz_t i = 0; sz_t(_models.size()) > i; ++i)
	{
		out << "#" << std::setw(2) << i;

		_adecoder->model(i);
		const model_t &model = _models[i];

		#ifdef LIBWIC_DEBUG
		out << "[" << std::setw(5) << model._symbols << "]";
		#endif

		const unsigned int bits = _adecoder->model()->bit_counter;

		out << ": ";
		out << std::setw(8) << bits << " bits |";
		out << std::setw(7) << (bits / 8) << " bytes";

		#ifdef LIBWIC_DEBUG
		const unsigned int decoded_symbols = model._decoded_symbols;
		total_symbols += decoded_symbols;

		out << " |" << std::setw(7) << decoded_symbols  << " symbols";
		#endif

		out << " | M = " << std::setw(7) << model._abs_average;

		out << std::endl;

		#ifdef LIBWIC_DEBUG
		_dbg_freqs_out(out, model._decoded_freqs, model, i, _adecoder);
		#endif
	}

	#ifdef LIBWIC_DEBUG
	out << "Total symbols: " << std::setw(6) << total_symbols << std::endl;
	#endif

	out << "[/decoder models information] ---------------------------";
	out << std::endl;
}


/*!	\param[in] file ��� ����� � ������� ����� ������������� �����
*/
void acoder::dbg_decoder_info(const std::string &file)
{
	std::ofstream out(file.c_str());

	if (out.good()) dbg_decoder_info(out);
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
	- ������������� ���� model_t::_rmin
	- ������������� ���� model_t::_rmax
*/
void acoder::_refresh_models(models_t &models)
{
	for (models_t::iterator i = models.begin(); models.end() != i; ++i)
	{
		model_t &model		= (*i);
		model._delta		= - model.min;
		model._symbols		= _symbols_count(model);

		model._rmin			= model.max;
		model._rmax			= model.min;
		model._n			= 0;
		model._abs_average	= 0;
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

	// ���������� ���������� ���������� �������
	_refresh_models(_models);

	// ����� ���������
	coder_base->ResetStatistics();

	// ������������� �������
	coder_base->model(0);

	for (int j = 0; _models[0]._symbols > j; ++j)
	{
		coder_base->update_model(j);
	}

	for (unsigned int i = 1; 6/*_models.size()*/ > i; ++i)
	{
		const model_t &model = _models[i];

		int k0 = std::max(-model.abs_avg-1, model.min);
		int k1 = std::min(+model.abs_avg+1, model.max);

		//printf("%i - [%i, %i], delta: %i, avg: %i; {%i, %i}\n",
		// 	   i, model.min, model.max, model._delta, model.abs_avg,
		//	   k0, k1);

		coder_base->model(i);

		for (int j = k0; k1 > j; ++j)
		{
			coder_base->update_model(j + model._delta);
		}

		/*
		for (int r = 0; 10 > r; ++r)
		{
			coder_base->update_model(model._delta);
		}
		*/
	}
	/*
	const model_t &model0 = _models[0];

	coder_base->model(0);

	for (int j = 0; model0._symbols > j; ++j)
	{
		coder_base->update_model(j);
	}

	const model_t &model1 = _models[1];

	coder_base->model(1);

	for (int j = 0; model1._symbols > j; ++j)
	{
		coder_base->update_model(j);
	}

	const int d1 = model1._symbols / (2*5);

	for (int j = 0; 4*d1 > j; ++j)
	{
		coder_base->update_model(model1._delta);
	}

	for (int j = 1; d1 > j; ++j)
	{
		for (int k = 0; 2*(d1 - j) > k; ++k)
		{
			coder_base->update_model(model1._delta + j);
			coder_base->update_model(model1._delta - j);
		}
	}

	// model 2 -----------------------------------------------------------------
	const model_t &model2 = _models[2];

	coder_base->model(2);

	for (int j = 0; model2._symbols > j; ++j)
	{
		coder_base->update_model(j);
	}

	const int d2 = model2._symbols / (2*4);

	for (int j = 0; 25*d2 > j; ++j)
	{
		coder_base->update_model(model2._delta);
	}

	for (int j = 1; d2 > j; ++j)
	{
		for (int k = 0; 25*(d2 - j) > k; ++k)
		{
			coder_base->update_model(model2._delta + j);
			coder_base->update_model(model2._delta - j);
		}
	}

	// model 3 -----------------------------------------------------------------
	const model_t &model3 = _models[3];

	coder_base->model(3);

	for (int j = 0; model3._symbols > j; ++j)
	{
		coder_base->update_model(j);
	}

	const int d3 = model3._symbols / (2*6);

	for (int j = 0; 200*d3 > j; ++j)
	{
		coder_base->update_model(model3._delta);
	}

	for (int j = 1; d3 > j; ++j)
	{
		for (int k = 0; 200*(d3 - j) > k; ++k)
		{
			coder_base->update_model(model3._delta + j);
			coder_base->update_model(model3._delta - j);
		}
	}
	*/

	/*
	for (sz_t i = 0; 4 > i; i++)
	{
		const model_t &model = _models[i];

		coder_base->model(i);

		for (int j = 0; model._symbols > j; ++j)
		{
			coder_base->update_model(j);
		}
	}

	coder_base->model(4);
	const model_t &model4 = _models[4];
	coder_base->update_model(model4._delta);
	*/

	/*
	coder_base->model(6);
	const model_t &model6 = _models[6];

	coder_base->update_model(7);
	coder_base->update_model(7);
	coder_base->update_model(7);
	coder_base->update_model(7);
	coder_base->update_model(7);
	coder_base->update_model(7);
	coder_base->update_model(7);

	coder_base->model(7);
	coder_base->update_model(0);
	coder_base->update_model(0);
	coder_base->update_model(0);
	coder_base->update_model(0);
	coder_base->update_model(0);
	coder_base->update_model(0);
	coder_base->update_model(0);
	coder_base->update_model(0);

	coder_base->model(8);
	coder_base->update_model(0);
	coder_base->update_model(0);
	coder_base->update_model(0);
	coder_base->update_model(0);
	coder_base->update_model(0);
	coder_base->update_model(0);
	coder_base->update_model(0);
	coder_base->update_model(0);

	coder_base->model(9);
	coder_base->update_model(0xf);
	coder_base->update_model(0xf);
	coder_base->update_model(0xf);
	coder_base->update_model(0xf);
	coder_base->update_model(0xf);
	coder_base->update_model(0xf);
	coder_base->update_model(0xf);
	coder_base->update_model(0xf);

	coder_base->model(10);
	coder_base->update_model(0xf);
	coder_base->update_model(0xf);
	coder_base->update_model(0xf);
	coder_base->update_model(0xf);
	coder_base->update_model(0xf);
	coder_base->update_model(0xf);
	coder_base->update_model(0xf);
	coder_base->update_model(0xf);
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

	const double entropy = coder_base->entropy_eval(value + model._delta);

	return entropy;
}


#ifdef LIBWIC_DEBUG
/*!	\param[out] out ����������� ����� ��� ������
	\param[in] freqs ������ ������ ��������
	\param[in] model ������������ ������
	\param[in] model_no ����� ������������ ������
	\param[in] coder_base ������� ����� ������������� ��������������� ������

	\note ������� ������ ������� ������ ��������������� ������ �� ������
	� ������� model_no.
*/
void acoder::_dbg_freqs_out(std::ostream &out,
							const std::vector<unsigned int> &freqs,
							const model_t &model, const sz_t model_no,
							arcoder_base *const coder_base)
{
	// ����� ������������ �������
	unsigned int freq_max = 0;

	for (sz_t i = 0; sz_t(freqs.size()) > i; ++i)
	{
		if (freq_max < freqs[i]) freq_max = freqs[i];
	}

	// ������������ ������� ������
	if (0 != coder_base) coder_base->model(model_no);

	// ����� ������
	for (sz_t i = 0; sz_t(freqs.size()) > i; ++i)
	{
		const unsigned int freq = freqs[i];

		if (0 == freq) continue;

		// ����� ���������
		out << "\t" << std::setw(5) << (i - model._delta) << "|";

		for (sz_t k = (36 * freq / freq_max); 0 < k;  --k) out << '+';

		// �������� �������� �������
		out << std::setw(7) << freq;

		// �������� �������� ��������
		out << "; e=" << std::setprecision(5) << coder_base->entropy_eval(i);

		out << std::endl;
	}
}
#endif



}	// namespace wic
