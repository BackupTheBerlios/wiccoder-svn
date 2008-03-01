/*!	\file     encoder.cpp
	\version  0.0.1
	\author   mice, ICQ: 332-292-380, mailto:wonder.mice@gmail.com
	\brief    ���������� ������ wic::encoder

	\todo     ����� �������� ������� ���� encoder.h
*/


////////////////////////////////////////////////////////////////////////////////
// include

// standard C++ headers
#include <math.h>

// libwic headers
#include <wic/libwic/encoder.h>


////////////////////////////////////////////////////////////////////////////////
// wic namespace
namespace wic {


////////////////////////////////////////////////////////////////////////////////
// encoder class public definitions

/*!	\param[in] width ������ �����������.
	\param[in] height ������ �����������.
	\param[in] lvls ���������� ������� ������� ��������������.
*/
encoder::encoder(const sz_t width, const sz_t height, const sz_t lvls):
	_wtree(width, height, lvls),
	_acoder(width * height * sizeof(w_t) * 4),
	_optimize_tree_callback(0), _optimize_tree_callback_param(0),
	_optimize_callback(0), _optimize_callback_param(0)
	#ifdef LIBWIC_USE_DBG_SURFACE
	, _dbg_opt_surface(width, height), _dbg_enc_surface(width, height),
	_dbg_dec_surface(width, height)
	#endif
{
	// �������� �����������
	assert(MINIMUM_LEVELS <= lvls);

	#ifdef LIBWIC_DEBUG
	_dbg_out_stream.open("dumps/[encoder]debug.out",
						 std::ios_base::out | std::ios_base::app);

	if (_dbg_out_stream.good())
	{
		time_t t;
		time(&t);
		_dbg_out_stream << std::endl << ctime(&t) << std::endl;
	}
	#endif
}


/*!
*/
encoder::~encoder() {
}


/*!	\param[in] callback ������� ��������� ������
	\param[in] param ���������������� ��������, ������������ � �������
	��������� ������
*/
void encoder::optimize_tree_callback(const optimize_tree_callback_f &callback,
									 void *const param)
{
	_optimize_tree_callback			= callback;
	_optimize_tree_callback_param	= param;
}


/*!	\param[in] callback ������� ��������� ������
	\param[in] param ���������������� ��������, ������������ � �������
	��������� ������
*/
void encoder::optimize_callback(const optimize_callback_f &callback,
								void *const param)
{
	_optimize_callback			= callback;
	_optimize_callback_param	= param;
}


/*!	\param[in] w ������ ������� �������������� �������� ����������� ���
	�����������

	\param[in] q ������������. ��� ������ �������� (��������) ������������,
	��� ������� ������� ������ ����� ��������. ������ ��� ����������
	������������ �������� ���������������� (���������������) �����������
	����������. �������� ������������ ������ ���� ������ <i>1</i>.

	\param[in] lambda �������� <i>lambda</i> ������������ ���
	���������� <i>RD</i> �������� (������� ��������). ������������
	����� ������ ����� ������� � �������� ���������. ��� ������ ������
	��������, ��� ������� ��������� ����� ����� ������� ��������.
	��������������, ��� 0 ����� ����������� ������ ������ �����������.

	\param[out] tunes ����������, ����������� ��� ������������
	�������������� �����������

	\return ��������� ����������� �����������

	������ �������� ����������� �����������, ����� ����������� (���
	��������������) �������� ���������� <i>q</i> � <i>lambda</i> ��������
	�������. ���� ����� �������� ����� ������� ��� ��� ���������� ��������
	����������� ��������� � ����������� ������ �� ������ ����.

	����� ��������, ��� ����� ����� ���������� ����������� ������� ���������
	������������� <i>q</i>, ������� ��� �� ���������� ������������ �
	��������� ����� ���������� �������� ���������� ������ ������ �����������
	� ������ ���������� <i>lambda</i>. ��� ����� ������� ����� ������������
	����� ������� ������� cheap_encode(), ������� �� ���������� �����������
	������������� ������� (�� ����� ���� ��� �� ������������ =).

	��� ������� ���������� ������ #OPTIMIZATION_USE_VIRTUAL_ENCODING. ���� ��
	�������� ����� ������������� ����������� ����������� �������������, ���
	��������� �������. ������ �� ��� ���������� ������� ������� �����
	������������ ���.

	��� ���������� ����������� ��� ��������� ������������� (����� ����������
	����������� ��������� ��������� �����������, ����� ��� ��� ���������� �
	���������� ������� ��������������) ������������ ����� ���������
	tunes_t. ������ � ��������������� ����������� �������������� ����� ������
	��������������� ������, ������ �� ������� ����� �������� ������ �����
	coder().
*/
encoder::enc_result_t
encoder::encode(const w_t *const w, const q_t q, const lambda_t &lambda,
				tunes_t &tunes)
{
	// ��������� ���������� �����������
	enc_result_t result;

	// �������� ������� ����������
	assert(0 != w);

	// �������� �������
	_wtree.load_field<wnode::member_w>(w);

	// ����������� ��������� ������
	result.optimization =
		#ifdef OPTIMIZATION_USE_VIRTUAL_ENCODING
		_optimize_wtree_q(lambda, q, true, true);
		#else
		_optimize_wtree_q(lambda, q, false, true);
		#endif

	// ����������� ����� ������, ���� ����������
	if (result.optimization.real_encoded)
	{
		tunes.models = result.optimization.models;
		result.bpp = result.optimization.bpp;
	}
	else
	{
		result.bpp = _real_encode_tight(tunes.models);
	}

	// ������ ������ ����������� ��� ������������ �������������
	tunes.q = _wtree.q();

	// ���������� �����������
	return result;
}


/*!	\param[in] w ������ ������� �������������� �������� ����������� ���
	�����������

	\param[in] lambda �������� <i>lambda</i> ������������ ���
	���������� <i>RD</i> �������� (������� ��������). ������������
	����� ������ ����� ������� � �������� ���������. ��� ������ ������
	��������, ��� ������� ��������� ����� ����� ������� ��������.
	��������������, ��� 0 ����� ����������� ������ ������ �����������.

	\param[out] tunes ����������, ����������� ��� ������������
	�������������� �����������

	\param[in] q_min ������ ������� ��������� ������ (�����������
	��������)

	\param[in] q_max ������� ������� ��������� ������ (������������
	��������)

	\param[in] q_eps ����������� ����������� ����������� ������������
	<i>q</i> ��� ������� �������� <i>RD ������� ��������</i> ����������
	(��� ������������� ��������� <i>lambda</i>).

	\param[in] j_eps ����������� ����������� ���������� �������� <i>RD
	������� ��������</i>. ��� ��� ���������� �������� ������� <i>J</i>
	������� �� ������ �������� (����� ��� ������ �����������, ��� ���,
	�������� ��������� <i>lambda</i>), ������������ ��� ��������
	��������������. ������� ��� �������� �� ��������� ����� <i>0</i>,
	����� ��� ������ ������������ ������������ ����������� ������
	����������� ��������� <i>q</i>.

	\param[in] max_iterations ������������ ���������� �������� ����������
	�������� <i>RD ������� ��������</i>. ���� ��� �������� ����� <i>0</i>,
	���������� ����������� �������� �� ����������.

	\param[in] precise_bpp ������ ����������� �������� � �������
	_search_q_min_j()

	������� ���������� ����������� ����������� ��� ������������� ���������
	<i>lambda</i>, �������� �������� ����������� <i>q</i> ����� �������,
	����� �������� ������� <i>RD �������� ��������</i> ���� �����������.
	����� <i>lambda</i> ��������� ��������� ��������������� ��������. ���
	���� �������� ������, ��� ������ ����� ������� ������ �, ��������������,
	���� �������� ��������������� �����������. ��� <i>lambda</i> ������
	<i>0</i>, �������������� ����������� ����� ���������� ��������.

	� ����������� ������� ������� ������������ �� ����������� ��������
	<i>lambda</i>, � ����� ������ ���������� ��������. � ����� ���������
	����� ��������������� �������� quality_to_lambda(), �������
	����������� ���������� �������� (����� � ��������� <i>[0, 100]</i>) �
	�������������� �������� ��������� <i>lambda</i>.

	��� ������� ���������� ������ #OPTIMIZATION_USE_VIRTUAL_ENCODING. ���� ��
	�������� ����� ������������� ����������� ����������� �������������, ���
	��������� �������. ������ �� ��� ���������� ������� ������� �����
	������������ ���.

	\sa _search_q_min_j()
*/
encoder::enc_result_t
encoder::encode_fixed_lambda(
						const w_t *const w, const lambda_t &lambda,
						tunes_t &tunes,
						const q_t &q_min, const q_t &q_max, const q_t &q_eps,
						const j_t &j_eps, const sz_t &max_iterations,
						const bool precise_bpp)
{
	// ��������� ���������� �����������
	enc_result_t result;

	// �������� ������� ����������
	assert(0 != w);

	// �������� �������
	_wtree.load_field<wnode::member_w>(w);

	// ����������� RD ������� ��������
	result.optimization = 
		#ifdef OPTIMIZATION_USE_VIRTUAL_ENCODING
		_search_q_min_j(lambda, q_min, q_max, q_eps, j_eps,
						true, max_iterations, precise_bpp);
		#else
		_search_q_min_j(lambda, q_min, q_max, q_eps, j_eps,
						false, max_iterations, precise_bpp);
		#endif

	// ����������� ����� ������, ���� ����������
	if (result.optimization.real_encoded)
	{
		tunes.models = result.optimization.models;
		result.bpp = result.optimization.bpp;
	}
	else
	{
		result.bpp = _real_encode_tight(tunes.models);
	}

	// ���������� ����������, ����������� ��� ������������ �������������
	tunes.q = _wtree.q();

	return result;
}


/*!	\param[in] w ������ ������� �������������� �������� ����������� ���
	�����������

	\param[in] lambda �������� <i>lambda</i> ������������ ���
	���������� <i>RD</i> �������� (������� ��������). ������������
	����� ������ ����� ������� � �������� ���������. ��� ������ ������
	��������, ��� ������� ��������� ����� ����� ������� ��������.
	��������������, ��� 0 ����� ����������� ������ ������ �����������.

	\param[out] tunes ����������, ����������� ��� ������������
	�������������� �����������

	��� ���������� ������ ������� encode_fixed_lambda() ���������� ���������
	�������� ����������:
	- <i>q_min = 1</i>
	- <i>q_max = 64</i>
	- <i>q_eps = 0.01</i>
	- <i>j_eps = 0.0</i>
	- <i>max_iterations = 0</i>
	- <i>precise_bpp = true</i>

	��� ������� �������� ���������� ������ #OPTIMIZATION_USE_VIRTUAL_ENCODING.
	���� �� �������� ����� ������������� ����������� ����������� �������������,
	��� ��������� �������. ������ �� ��� ���������� ������� ������� �����
	������������ ���.
*/
encoder::enc_result_t
encoder::encode_fixed_lambda(const w_t *const w, const lambda_t &lambda,
							 tunes_t &tunes)
{
	static const q_t q_eps	= q_t(0.01);
	static const j_t j_eps	= j_t(0);

	static const q_t q_min	= q_t(1);
	static const q_t q_max	= q_t(64);

	static const sz_t max_iterations	= 0;

	static const bool precise_bpp		= true;

	return encode_fixed_lambda(w, lambda, tunes, q_min, q_max, q_eps,
							   j_eps, 0, precise_bpp);
}


/*!	\param[in] w ������ ������� ������������� ���������������� ��������
	����������� ��� �����������

	\param[in] q ������������ ������������

	\param[in] bpp ����������� ������� (Bits Per Pixel), ��� ����������
	�������� ����� ����������� �������� <i>lambda</i>

	\param[in] bpp_eps ����������� ��������, c ������� <i>bpp</i> ����������
	� ���������� ������ ��������� <i>lambda</i> ����� ���������������
	���������.

	\param[out] tunes ����������, ����������� ��� ������������
	�������������� �����������

	\param[in] lambda_min ������ ������� ��������� ������ (�����������
	��������)

	\param[in] lambda_max ������� ������� ��������� ������ (������������
	��������)

	\param[in] lambda_eps ��������, � ������� ����� ����������� ��������
	<i>lambda</i>.

	\param[in] max_iterations ������������ ���������� �������� ����������
	�������� <i>RD ������� ��������</i>. ���� ��� �������� ����� <i>0</i>,
	���������� ����������� �������� �� ����������.

	\param[in] precise_bpp ���� <i>true</i> ����� ������� ����� �����������
	��������� �������� ������� ������� ������������� ����� �����������
	�������� ����������� � ����������� �������� ��������������� ������ ���
	��������� ������ ������� ������.

	\return ��������� ����������� ������. �������� ��������, ����� ������
	<i>lambda</i> ����� ��� ���������� ���������. � ���� ������, �������
	������� ����� <i>lambda</i>, ������� ����������� ������������� ��������
	������.

	���������� ����������� ����������� ��� ������������� ��������� <i>q</i>,
	�������� �������� ��������� <i>lambda</i> ����� �������, ����� �������
	������� �� ����������� ����������� ���� ����������� ������ � ��������.

	��� ������� ���������� ������ #OPTIMIZATION_USE_VIRTUAL_ENCODING.
	���� �� �������� ����� ������������� ����������� ����������� �������������,
	��� ��������� �������. ������ �� ��� ���������� ������� ������� �����
	������������ ���.

	\sa _search_lambda_at_bpp
*/
encoder::enc_result_t
encoder::encode_fixed_q(const w_t *const w, const q_t &q,
						const h_t &bpp, const h_t &bpp_eps,
						tunes_t &tunes,
						const lambda_t &lambda_min,
						const lambda_t &lambda_max,
						const lambda_t &lambda_eps,
						const sz_t &max_iterations,
						const bool precise_bpp)
{
	// ��������� ���������� �����������
	enc_result_t result;

	// �������� ������� ����������
	assert(0 != w);
	assert(1 <= q);

	// �������� �������
	_wtree.cheap_load(w, q);

	// ��������� ������������� ������� ��������������� ������
	tunes.models = _setup_acoder_models();

	// ����������� RD ������� ��������
	result.optimization =
		#ifdef OPTIMIZATION_USE_VIRTUAL_ENCODING
		_search_lambda_at_bpp(bpp, bpp_eps,
							  lambda_min, lambda_max, lambda_eps,
							  true, max_iterations, precise_bpp);
		#else
		_search_lambda_at_bpp(bpp, bpp_eps,
							  lambda_min, lambda_max, lambda_eps,
							  false, max_iterations, precise_bpp);
		#endif

	// ����������� ����� ������, ���� ����������
	if (result.optimization.real_encoded)
	{
		tunes.models = result.optimization.models;
		result.bpp = result.optimization.bpp;
	}
	else
	{
		result.bpp = _real_encode_tight(tunes.models);
	}

	// ���������� ����������, ����������� ��� ������������ �������������
	tunes.q = _wtree.q();

	return result;
}


/*!	\param[in] w ������ ������� ������������� ���������������� ��������
	����������� ��� �����������

	\param[in] q ������������ ������������

	\param[in] bpp ����������� ������� (Bits Per Pixel), ��� ����������
	�������� ����� ����������� �������� <i>lambda</i>

	\param[out] tunes ����������, ����������� ��� ������������
	�������������� �����������

	\return ��������� ����������� ������.

	��� ���������� ������ ������� encode_fixed_q() ���������� ���������
	�������� ����������:
	- <i>bpp_eps = 0.001</i>
	- <i>lambda_min = 0.05*q*q</i>
	- <i>lambda_max = 0.20*q*q</i>
	- <i>lambda_eps = 0.0</i>
	- <i>max_iterations = 0</i>
	- <i>precise_bpp = true</i>

	��� ������� �������� ���������� ������ #OPTIMIZATION_USE_VIRTUAL_ENCODING.
	���� �� �������� ����� ������������� ����������� ����������� �������������,
	��� ��������� �������. ������ �� ��� ���������� ������� ������� �����
	������������ ���.
*/
encoder::enc_result_t
encoder::encode_fixed_q(const w_t *const w, const q_t &q,
						const h_t &bpp, tunes_t &tunes)
{
	static const h_t bpp_eps			= 0.001;

	static const lambda_t lambda_min	= 0.05*q*q;
	static const lambda_t lambda_max	= 0.20*q*q;
	static const lambda_t lambda_eps	= 0;

	static const sz_t max_iterations	= 0;

	static const bool precise_bpp		= true;

	return encode_fixed_q(w, q, bpp, bpp_eps, tunes,
						  lambda_min, lambda_max, lambda_eps,
						  max_iterations, precise_bpp);
}


/*!	\param[in] w ������ ������� ������������� ���������������� ��������
	����������� ��� �����������

	\param[in] bpp ����������� ������� (Bits Per Pixel), ��� ����������
	�������� ����� ����������� ��������� <i>q</i> � <i>lambda</i>.

	\param[in] bpp_eps ����������� ��������, c ������� <i>bpp</i> ����������
	� ���������� ������ ���������� <i>q</i> � <i>lambda</i> �����
	��������������� ���������.

	\param[in] q_min ������ ������� ��������� ������ (�����������
	��������)

	\param[in] q_max ������� ������� ��������� ������ (������������
	��������)

	\param[in] q_eps ����������� ����������� ����������� ������������
	<i>q</i> ��� ������� �������� <i>RD ������� ��������</i> ����������
	(��� ������������� ��������� <i>lambda</i>).

	\param[in] lambda_eps ��������, � ������� ����� ����������� ��������
	<i>lambda</i>

	\param[out] tunes ����������, ����������� ��� ������������
	�������������� �����������

	\return ��������� ����������� ������

	\sa _search_q_lambda_for_bpp()
*/
encoder::enc_result_t
encoder::encode_fixed_bpp(
						const w_t *const w,
						const h_t &bpp, const h_t &bpp_eps,
						const q_t &q_min, const q_t &q_max, const q_t &q_eps,
						const lambda_t &lambda_eps,
						tunes_t &tunes, const bool precise_bpp)
{
	// ��������� ���������� �����������
	enc_result_t result;

	// �������� ������� ����������
	assert(0 != w);

	// �������� �������
	_wtree.load_field<wnode::member_w>(w);

	// ����������� RD ������� ��������
	result.optimization = 
		#ifdef OPTIMIZATION_USE_VIRTUAL_ENCODING
		_search_q_lambda_for_bpp(bpp, bpp_eps,
								 q_min, q_max, q_eps,
								 lambda_eps, tunes.models, true, precise_bpp);
		#else
		_search_q_lambda_for_bpp(bpp, bpp_eps,
								 q_min, q_max, q_eps,
								 lambda_eps, tunes.models, false, precise_bpp);
		#endif

	// ����������� ����� ������, ���� ����������
	if (result.optimization.real_encoded)
	{
		tunes.models = result.optimization.models;
		result.bpp = result.optimization.bpp;
	}
	else
	{
		result.bpp = _real_encode_tight(tunes.models);
	}

	// ���������� ����������, ����������� ��� ������������ �������������
	tunes.q = _wtree.q();

	return result;
}


/*!	\param[in] w ������ ������� ������������� ���������������� ��������
	����������� ��� �����������

	\param[in] bpp ����������� ������� (Bits Per Pixel), ��� ����������
	�������� ����� ����������� ��������� <i>q</i> � <i>lambda</i>.

	\param[out] tunes ����������, ����������� ��� ������������
	�������������� �����������

	\return ��������� ����������� ������

	��� ���������� ������ ������� encode_fixed_bpp() ������ ���������
	���������� ��������������� ��������:
	- <i>bpp_eps = 0.001</i>
	- <i>q_min = 1</i>
	- <i>q_max = 64</i>
	- <i>q_eps = 0.01</i>
	- <i>lambda_eps = 0</i>
	- <i>precise_bpp = true</i>

	\sa _search_q_lambda_for_bpp(), encode_fixed_bpp
*/
encoder::enc_result_t
encoder::encode_fixed_bpp(const w_t *const w, const h_t &bpp,
						  tunes_t &tunes)
{
	static const h_t bpp_eps			= 0.001;

	static const q_t q_min				= q_t(1);
	static const q_t q_max				= q_t(64);
	static const q_t q_eps				= q_t(0.01);

	static const lambda_t lambda_eps	= 0.00001;

	static const bool precise_bpp		= true;

	return encode_fixed_bpp(w, bpp, bpp_eps,
							q_min, q_max, q_eps, lambda_eps, tunes,
							precise_bpp);
}


/*!	param[in] data ��������� �� ���� ������, ���������� ��������������
	�����������
	param[in] data_sz ������ �����, ����������� ��������������
	�����������
	param[in] tunes ������ ����������� ��� �������������� �����������,
	���������� �� ����� �� ������� �����������.

	����� ��������, ��� � ������� ����������, ������ ��� ���������������
	������ ���������� �������, ������ ������� ������������ ������ ��
	�������� ������ �����������. ������� ����������, ����� ������
	������ <i>data_sz</i> ��� ������, ��� acoder::buffer_sz().
*/
void encoder::decode(const byte_t *const data, const sz_t data_sz,
					 const tunes_t &tunes)
{
	// �������� �����������
	assert(_acoder.buffer_sz() >= data_sz);

	// ����������� ������ � �������������� �����
	memcpy(_acoder.buffer(), data, data_sz);

	// ������������� ������� ����� ������������
	_wtree.wipeout();

	// ��������� ������������� �������
	_acoder.use(_mk_acoder_models(tunes.models));

	// �������������
	_acoder.decode_start();

	_encode_wtree(true);

	_acoder.decode_stop();

	// �������������
	_wtree.dequantize<wnode::member_wc>(tunes.q);
}


/*!	\param[in] quality ���������� ��������
	\return �������� ��������� <i>lambda</i>, ��������������� ����������
	���������� ��������

	���������� �������� ������������ �������� �� ��������� <i>[0, 100]</i>.
	�������� <i>100</i> ������������� ������������� �������� ������, �
	�������� <i>0</i> ������������� ������������ ������� ������.

	�������������� ������������ �� �������:
	\verbatim
	lambda = pow((quality + 2.0), (102 / (quality + 2) - 1)) - 1;
	\endverbatim
*/
lambda_t encoder::quality_to_lambda(const double &quality)
{
	assert(0.0 <= quality && quality <= 100.0);

	const double d = 2.0;
	const double b	= (quality + d);
	const double p	= ((100.0 + d) / b) - 1.0;

	return (pow(b, p) - 1.0);
}


////////////////////////////////////////////////////////////////////////////////
// wtree class protected definitions

/*!	\param[in] s �������� ���������� �������� <i>S<sub>j</sub></i>
	\param[in] lvl ����� ������ ����������, �� �������� ��� ���� �����������
	\return ����� ���������� ������

	\note ����� ��������, ��� ��� �������� � ������� ������� �������
	���������� ����������� ��������, ���������� �� ��������� <i>s</i>.
*/
sz_t encoder::_ind_spec(const pi_t &s, const sz_t lvl) {
	if (subbands::LVL_0 == lvl) return 0;
	if (subbands::LVL_1 == lvl) return 1;

	if (26.0  <= s) return 1;
	if ( 9.8  <= s) return 2;
	if ( 4.1  <= s) return 3;
	if ( 1.72 <= s) return 4;

	return 5;
}


/*!	\param[in] pi �������� ���������� �������� <i>P<sub>i</sub></i>
	\param[in] lvl ����� ������ ����������, �� �������� ��� ���� ���������
	������� ���������� ������
	\return ����� ���������� ������

	\note ����� ��������, ��� ���� �������� <i>lvl</i> ����� <i>0</i>
	������� ������ ���������� ������� ������, ���������� �� ���������
	<i>pi</i>.
*/
sz_t encoder::_ind_map(const pi_t &pi, const sz_t lvl) {
	if (subbands::LVL_0 == lvl) return 0;

	if (4.0 <= pi) return 4;
	if (1.1 <= pi) return 3;
	if (0.3 <= pi) return 2;

	return 1;
}


/*!	\param[in] desc �������� ������� ��������������� ������
	\return ������ ��� ��������������� ������
*/
acoder::models_t encoder::_mk_acoder_models(const models_desc1_t &desc) const
{
	// �������� ������� ��� �����������
	acoder::models_t models;
	acoder::model_t model;

	// ����� #0 ----------------------------------------------------------------
	model.min = desc.mdl_0_min;
	model.max = desc.mdl_0_max;
	models.push_back(model);

	// ������ #1 ---------------------------------------------------------------
	model.min = desc.mdl_1_min;
	model.max = desc.mdl_1_max;
	models.push_back(model);

	// ������ #2..#5 -----------------------------------------------------------
	model.min = desc.mdl_x_min;
	model.max = desc.mdl_x_max;

	models.insert(models.end(), ACODER_SPEC_MODELS_COUNT - 2, model);

	// �������� ������� ��� ����������� ��������� ��������� ���������� ---------
	model.min = 0;
	model.max = 0x7;
	models.push_back(model);

	model.max = 0xF;
	models.insert(models.end(), ACODER_MAP_MODELS_COUNT - 1, model);

	// �������� �����������
	assert(ACODER_TOTAL_MODELS_COUNT == models.size());

	return models;
}


/*!	\param[in] desc �������� ������� ��������������� ������
	\return ������ ��� ��������������� ������
*/
acoder::models_t encoder::_mk_acoder_models(const models_desc2_t &desc) const
{
	// �������� ������� ��� �����������
	acoder::models_t models;

	for (sz_t i = 0; ACODER_TOTAL_MODELS_COUNT > i; ++i)
	{
		acoder::model_t model;
		model.min = desc.mins[i];
		model.max = desc.maxs[i];

		models.push_back(model);
	}

	// �������� �����������
	assert(ACODER_TOTAL_MODELS_COUNT == models.size());

	return models;
}


/*!	\param[in] desc �������� ������� ��������������� ������ � ���������������
	�������
	\return ������ ��� ��������������� ������
*/
acoder::models_t encoder::_mk_acoder_models(const models_desc_t &desc) const
{
	// �������� �������, ��� �� �������� �������������� �����
	acoder::models_t models;

	// ����� ������� ������������� ��������
	switch (desc.version)
	{
		case MODELS_DESC_V1:
			models = _mk_acoder_models(desc.md.v1);
			break;

		case MODELS_DESC_V2:
			models = _mk_acoder_models(desc.md.v2);
			break;

		default:
			// unsupported models description
			assert(false);
			break;
	}

	return models;
}


/*!	\return �������� ������� ��� ��������������� ������

	�������� ������� (�������� ������������� � ������������ �������� � ������)
	������������ �� ���������� ��������:
	- ��� ������ #0: ����������� ������� �� <i>LL</i> ��������, ������������
	  ������� ����� �� <i>LL</i> ��������
	- ��� ������ #1: ����������� ������� �� ���� ��������� ������� ������
	  (�������� "��������" �� <i>LL</i>), ������������ ������� ����� �� ����
	  ��������� ������� ������.
	- ��� ������� #2..#5: ����������� ������� �� ���� ���������� ���������,
	  ������������ ������� ����� �� ���� ���������� ���������
*/
encoder::models_desc1_t encoder::_mk_acoder_smart_models() const
{
	// �������� ������� ��� �����������
	models_desc1_t desc;

	// ����� #0 ----------------------------------------------------------------
	{
		const subbands::subband_t &sb_LL = _wtree.sb().get_LL();
		wtree::coefs_iterator i = _wtree.iterator_over_subband(sb_LL);

		wk_t lvl0_min = 0;
		wk_t lvl0_max = 0;
		_wtree.minmax<wnode::member_wq>(i, lvl0_min, lvl0_max);

		desc.mdl_0_min = short(lvl0_min);
		desc.mdl_0_max = short(lvl0_max);
	}

	// ������ #1..#5 -----------------------------------------------------------
	{
		// ����� �������� � ��������� �� ������ ������
		wtree::coefs_cumulative_iterator i_cum;

		for (sz_t k = 0; subbands::SUBBANDS_ON_LEVEL > k; ++k)
		{
			const subbands::subband_t &sb = _wtree.sb().get(subbands::LVL_1, k);
			i_cum.add(_wtree.iterator_over_subband(sb));
		}

		wk_t lvl1_min = 0;
		wk_t lvl1_max = 0;
		_wtree.minmax<wnode::member_wq>(some_iterator_adapt(i_cum),
										lvl1_min, lvl1_max);

		// ����� �������� � ��������� �� ������� ������� �� �������
		wtree::coefs_cumulative_iterator j_cum;

		for (sz_t lvl = subbands::LVL_1 + subbands::LVL_NEXT;
			 _wtree.lvls() >= lvl; ++lvl)
		{
			for (sz_t k = 0; subbands::SUBBANDS_ON_LEVEL > k; ++k)
			{
				const subbands::subband_t &sb = _wtree.sb().get(lvl, k);
				j_cum.add(_wtree.iterator_over_subband(sb));
			}
		}

		wk_t lvlx_min = 0;
		wk_t lvlx_max = 0;
		_wtree.minmax<wnode::member_wq>(some_iterator_adapt(j_cum),
										lvlx_min, lvlx_max);

		// ������ #1
		desc.mdl_1_min = short(std::min(lvl1_min, lvlx_min));
		desc.mdl_1_max = short(std::max(lvl1_max, lvlx_max));

		// ������ #2..#5
		desc.mdl_x_min = lvlx_min;
		desc.mdl_x_max = lvlx_max;
	}

	return desc;
}


/*!	\param[in] ac �������������� �����, ���������� �������� ����� ������������
	��� ���������� �������� �������
*/
encoder::models_desc2_t encoder::_mk_acoder_post_models(const acoder &ac) const
{
	// �������� �����������
	assert(ACODER_TOTAL_MODELS_COUNT == ac.models().size());

	encoder::models_desc2_t desc;

	for (sz_t i = 0; sz_t(ac.models().size()) > i; ++i)
	{
		desc.mins[i] = ac.rmin(i);
		desc.maxs[i] = ac.rmax(i);

		// �������� �� ������ ������ (� ������� rmin > rmax)
		// ������ �������� ����������, ���� � ������ �� ������ �� ������
		// ��������
		if (desc.mins[i] > desc.maxs[i])
		{
			desc.mins[i] = desc.maxs[i] = 0;
		}
	}

	return desc;
}


/*!	\return �������� ������� ��������������� ������, ������� ����
	������������ ���� ��������

	��� ��������� �������� ������� ������������ �������
	_mk_acoder_smart_models()
*/
encoder::models_desc_t encoder::_setup_acoder_models()
{
	// �������� ������� ��� ��������������� ������
	models_desc_t desc;

	// ������������� ������������� ������������� �������� �������
	desc.version = MODELS_DESC_V1;

	// ����������� ���-����������� ������� ��� ��������������� ������
	desc.md.v1 = _mk_acoder_smart_models();

	// �������� ������� � �������������� �����
	_acoder.use(_mk_acoder_models(desc));

	return desc;
}


/*!	\return �������� ������� ��������������� ������, ������� ����
	������������ ���� ��������

	��� ��������� �������� ������� ������������ �������
	_mk_acoder_post_models()
*/
encoder::models_desc_t encoder::_setup_acoder_post_models()
{
	// �������� ������� ��� ��������������� ������
	models_desc_t desc;

	// ������������ ������ ������ ������������� �������� �������
	// ��������������� ������
	desc.version = MODELS_DESC_V2;

	// �������� ������ �������� ������� ��������������� ������,
	// ����������� �� ���������� �����������, ���������� ���
	// ����������� ��������� �������� ������� ������� �������������
	desc.md.v2 = _mk_acoder_post_models(_acoder);

	// �������� ������� � �������������� �����
	_acoder.use(_mk_acoder_models(desc));

	return desc;
}


/*!	\param[in] result ��������� ���������� �����������, � ��������� �������
	����� ���������� ������ ��������������� ������
	\param[in] models ������������ ������ ��������������� ������, �������
	������� ����������� � ������ �� ��������� ���������� ����������� ���������
	\return <i>true</i> ���� ����������� ������ ���� ������������� �
	<i>false</i> ���� ������ �� ���������� ���������� �����������.
*/
bool encoder::_restore_spoiled_models(const optimize_result_t &result,
									  const acoder::models_t &models)
{
	// ��������, ���� �� � �������� ����������� �������� 
	if (MODELS_DESC_NONE == result.models.version) return false;

	// �������������� �������� �� ��������� ������ � ����� �������.
	// ������ �� ������������, ��� ��� � ������� ���������� �������
	// ����������� ��������� ������ �������� ������� ���������������
	// ������ � ������ �� ���������.
	// if (models == _mk_acoder_models(result.models)) return false;

	// �������������� ������� ������������ �������������� �������
	_acoder.use(models);

	return true;
}


/*!	\param[in] m ����� ������ ��� �����������
	\param[in] wk �������� ������������ ��� �����������
	\return ������� �������, ����������� ��� ����������� ������������ �
	�������������� ���� ������
*/
h_t encoder::_h_spec(const sz_t m, const wk_t &wk) {
	return _acoder.enc_entropy(wk, m);
}


/*!	\param[in] m ����� ������ ��� �����������
	\param[in] n �������� ���������� �������� ���������� ������
	\return ������� �������, ����������� ��� ����������� ����������
	�������� ����������
*/
h_t encoder::_h_map(const sz_t m, const n_t &n) {
	return _acoder.enc_entropy(n, m + ACODER_SPEC_MODELS_COUNT);
}


/*!	\param[in] m ����� ������ ��� �����������
	\param[in] wk �������� ������������ ��� �����������
	\param[in] virtual_encode ���� <i>true</i> �� ����� �������������
	����������� ����������� (������ ������������� �������, ��� ���������
	����������� ������� � �������� �����).
*/
void encoder::_encode_spec(const sz_t m, const wk_t &wk,
						   const bool virtual_encode)
{
	_acoder.put(wk, m, virtual_encode);
}


/*!	\param[in] m ����� ������ ��� �����������
	\param[in] n �������� ���������� �������� ���������� ������
	\param[in] virtual_encode ���� <i>true</i> �� ����� �������������
	����������� ����������� (������ ������������� �������, ��� ���������
	����������� ������� � �������� �����).
*/
void encoder::_encode_map(const sz_t m, const n_t &n,
						  const bool virtual_encode)
{
	_acoder.put(n, m + ACODER_SPEC_MODELS_COUNT, virtual_encode);
}


/*!	\param[in] m ����� ������ ��� �����������
	\return �������� ������������ ��� �����������
*/
wk_t encoder::_decode_spec(const sz_t m)
{
	return _acoder.get<wk_t>(m);
}


/*!	\param[in] m ����� ������ ��� �����������
	\return �������� ���������� �������� ���������� ������
*/
n_t encoder::_decode_map(const sz_t m)
{
	return _acoder.get<n_t>(m + ACODER_SPEC_MODELS_COUNT);
}


/*!	\param[in] p �������������� ���������� �������� (������������)
	\param[in] k ������������������ (��� ������ ���������������) ��������
	������������
	\param[in] lambda �������� <i>lambda</i>, �������� �� <i>Rate/Distortion</i>
	������ ��� ���������� <i>RD</i> �������. ��� ��� �������� ������, ���
	������� ����� � �������� <i>RD</i> ������� ����� ������� ������� �������
	�� ����������� ������������ �������������� �������.
	\param[in] model ����� ������ ��������������� ������, ������� �����
	������������ ��� ����������� ������������
	\return �������� <i>RD-������� ���������</i>

	\note ������� ��������� ��� ��������� �� ����� ���������.
*/
j_t encoder::_calc_rd_iteration(const p_t &p, const wk_t &k,
								const lambda_t &lambda, const sz_t &model)
{
	const wnode &node = _wtree.at(p);

	const w_t dw = (wnode::dequantize(k, _wtree.q()) - node.w);

	const double h = _h_spec(model, k);

	return (dw*dw + lambda * h);
}


/*!	\param[in] p ���������� ������������ ��� �������������
	\param[in] sb �������, � ������� ��������� �����������
	\param[in] lambda �������� <i>lambda</i> ������������ ���
	���������� <i>RD</i> �������� (������� ��������). ������������
	����� ������ ����� ������� � �������� ���������.
	\return �������� ������������������� ������������

	\note ������� ��������� ��� ������������� �� ����� ���������

	\sa COEF_FIX_USE_4_POINTS

	\todo �������� ���� ��� ���� �������
*/
wk_t encoder::_coef_fix(const p_t &p, const subbands::subband_t &sb,
						const lambda_t &lambda)
{
	#ifdef COEF_FIX_DISABLED
	return _wtree.at(p).wq;
	#endif

	// ����� ������ � ������������� �������� ������������
	const sz_t	model	= _ind_spec<wnode::member_wc>(p, sb);
	const wk_t	&wq		= _wtree.at(p).wq;

	// ����������� ������ ����������� ��������
	#ifdef COEF_FIX_USE_4_POINTS
		static const sz_t vals_count	= 4;
		const wk_t w_vals[vals_count]	= {0, wq, wq + 1, wq - 1};
	#else
		static const sz_t vals_count	= 3;
		const wk_t w_drift				= (0 <= wq)? -1: +1;
		const wk_t w_vals[vals_count]	= {0, wq, wq + w_drift};
	#endif

	// ��������� �������� ��� ������ �������� RD �������
	wk_t k_optim = w_vals[0];
	j_t j_optim = _calc_rd_iteration(p, k_optim, lambda, model);

	// ����� ������������ �������� RD �������
	for (int i = 1; vals_count > i; ++i) {
		const wk_t &k = w_vals[i];
		const j_t j = _calc_rd_iteration(p, k, lambda, model);
		if (j < j_optim) {
			j_optim = j;
			k_optim = k;
		}
	}

	// ������� ������������������� �������� ������������
	return k_optim;
}


/*!	\param[in] p ���������� ������������� ��������
	\param[in] sb_j �������, � ������� ��������� �������� ��������
	\param[in] lambda �������� <i>lambda</i> ������������ ���
	���������� <i>RD</i> �������� (������� ��������). ������������
	����� ������ ����� ������� � �������� ���������.

	\note ������� ��������� ������ ��� ������������ ��������� �� ��
	<i>LL</i> ��������.
*/
void encoder::_coefs_fix(const p_t &p, const subbands::subband_t &sb_j,
						 const lambda_t &lambda)
{
	// ���� �� �������� ���������
	for (wtree::coefs_iterator i = _wtree.iterator_over_children(p);
		 !i->end(); i->next())
	{
		const p_t &p = i->get();
		wnode &node = _wtree.at(p);
		node.wc = _coef_fix(p, sb_j, lambda);
	}
}


/*!	\param[in] p ���������� ������������� ��������
	\param[in] sb_j �������, � ������� ��������� �������� ��������
	\param[in] lambda �������� <i>lambda</i> ������������ ���
	���������� <i>RD</i> �������� (������� ��������). ������������
	����� ������ ����� ������� � �������� ���������.

	\note ������� ��������� ������ ��� ������������ ��������� ��
	����� ���������.
*/
void encoder::_coefs_fix_uni(const p_t &p, const subbands::subband_t &sb_j,
							 const lambda_t &lambda)
{
	// ���� �� �������� ���������
	for (wtree::coefs_iterator i = _wtree.iterator_over_children_uni(p);
		 !i->end(); i->next())
	{
		const p_t &p = i->get();
		wnode &node = _wtree.at(p);
		node.wc = _coef_fix(p, sb_j, lambda);
	}
}


/*!	\param[in] p ���������� �������� ��� �������� ����� �������������
	\param[in] sb �������, � ������� ��������� ������������ �� �����������
	�����. ������� �������, ���� ������� �������� ��� ����, � �������
	��������� ������� � ������������ <i>p</i>.
	\param[in] lambda �������� <i>lambda</i> ������� ��������� � ����������
	<i>RD</i> ������� � ������������ ����� ������ ����� <i>R (rate)</i> �
	<i>D (distortion)</i> ������� <i>������� ��������</i>.
	\return �������� <i>RD ������� ��������</i>.

	\note ������� �� ��������� ��� ��������� �� <i>LL</i> ��������.

	\sa _calc_j0_value()

	\todo ���������� �������� ���� ��� ���� �������.
*/
j_t encoder::_calc_j1_value(const p_t &p, const subbands::subband_t &sb,
							const lambda_t &lambda)
{
	// ��������� ������ ������ ��� ����������� �������������
	const sz_t model = _ind_spec<wnode::member_wc>(p, sb);

	j_t j1 = 0;

	for (wtree::coefs_iterator i = _wtree.iterator_over_children(p);
		 !i->end(); i->next())
	{
		const wnode &node = _wtree.at(i->get());
		j1 += _calc_rd_iteration(i->get(), node.wc, lambda, model);
	}

	return j1;
}


/*!	\param[in] root ���������� ��������� ��������
	\param[in] j_map �������� <i>RD-������� ��������</i> ����������
	� ���� 2.6.
	\param[in] lambda �������� <i>lambda</i> ������� ��������� � ����������
	<i>RD</i> ������� � ������������ ����� ������ ����� <i>R (rate)</i> �
	<i>D (distortion)</i> ������� <i>������� ��������</i>.
	\return �������� <i>RD-������� ��������</i>.

	������� ����� ��������� ���������� �������� <i>RD-������� ��������</i> �
	����� wnode::j0 � wnode::j1 ��������� ��������.
*/
j_t encoder::_calc_jx_value(const p_t &root, const j_t &j_map,
							const lambda_t &lambda)
{
	assert(_wtree.sb().test_LL(root));

	// �������� ������ ��������
	const subbands &sb = _wtree.sb();

	j_t j = j_map;

	// ���� �� �������� ���������
	for (wtree::coefs_iterator i = _wtree.iterator_over_LL_children(root);
		 !i->end(); i->next())
	{
		const p_t &p = i->get();

		const wnode &node = _wtree.at(p);

		// �������� ������ �� ������� � ������� ����� ��������������� �������
		const subbands::subband_t &sb_i = sb.from_point(p, subbands::LVL_1);

		j += _calc_rd_iteration(p, node.wc, lambda,
								_ind_spec<wnode::member_wc>(p, sb_i));
	}

	wnode &node = _wtree.at(root);

	return (node.j0 = node.j1 = j);
}


/*!	\param[in] p ���������� ��������, ��� �������� ����������� ����������
	�������� <i>J</i> (<i>RD ������� ��������</i>) 
	\param[in] sb_j ������� � ������� ��������� �������� ��������
	\param[in] lambda �������� <i>lambda</i> ������������ ���
	���������� <i>RD</i> �������� (������� ��������). ������������
	����� ������ ����� ������� � �������� ���������.

	������� ������ ����������� ��� �������� � ������ <i>lvl</i>, ��
	������� <i>lvl + subbands::LVL_PREV</i>.

	\note ������� �� ��������� ��� ��������� �� <i>LL</i> ��������.
*/
void encoder::_prepare_j(const p_t &p, const subbands::subband_t &sb_j,
						 const lambda_t &lambda)
{
	wnode &node = _wtree.at(p);

	node.j0 = _calc_j0_value<false>(p);
	node.j1 = _calc_j1_value(p, sb_j, lambda);
}


/*!	\param[in] p ���������� ��������, ��� �������� ����������� ����������
	�������� <i>J</i> (<i>RD ������� ��������</i>) 
	\param[in] sb_j ������� � ������� ��������� �������� ��������
	\param[in] j �������� ������� ��������, ���������� ��� �������
	����������� ��������� ������
	\param[in] lambda �������� <i>lambda</i> ������������ ���
	���������� <i>RD</i> �������� (������� ��������). ������������
	����� ������ ����� ������� � �������� ���������.

	������� ������ ����������� ��� �������� � ������ <i>lvl</i>, ��
	������� <i>lvl + subbands::LVL_PREV</i>.

	\note ������� �� ��������� ��� ��������� �� <i>LL</i> ��������.
*/
void encoder::_prepare_j(const p_t &p, const subbands::subband_t &sb_j,
						 const j_t &j, const lambda_t &lambda)
{
	wnode &node = _wtree.at(p);

	node.j0 = _calc_j0_value<true>(p);
	node.j1 = j + _calc_j1_value(p, sb_j, lambda);
}


/*!	\param[in] branch ���������� ��������, ������������ � �������
	�����
	\param[in] n ��������� ������� ����������, ���������������
	��������� �����
	\return �������� ������� �������� ��� ��������� ��������� �
	<i>n</i>
*/
j_t encoder::_topology_calc_j(const p_t &branch, const n_t n)
{
	j_t j = 0;

	for (wtree::coefs_iterator i =
				_wtree.iterator_over_children(branch);
		 !i->end(); i->next())
	{
		const p_t &p = i->get();
		const wnode &node = _wtree.at(p);
		const n_t mask = _wtree.child_n_mask(p, branch);
		j += (_wtree.test_n_mask(n, mask))? node.j1: node.j0;
	}

	return j;
}


//!	����������� � ��������� (����� �� <i>LL</i> ��������)
/*!	\param[in] branch ���������� ��������, ������������ � �������
	�����
	\param[in] n ��������� ������� ����������, ���������������
	��������� �����
	\return �������� ������� �������� ��� ��������� ��������� �
	<i>n</i>
*/
j_t encoder::_topology_calc_j_LL(const p_t &branch, const n_t n)
{
	// ��������� �������� ��� ������� ��������
	j_t j = 0;

	for (wtree::coefs_iterator i =
				_wtree.iterator_over_LL_children(branch);
		 !i->end(); i->next())
	{
		const p_t &p = i->get();
		const wnode &node = _wtree.at(p);
		const n_t mask = _wtree.child_n_mask_LL(p);
		j += (_wtree.test_n_mask(n, mask))? node.j1: node.j0;
	}

	return j;
}


/*!	\param[in] branch ���������� ������������� ��������, �������
	������ �����.
	\param[in] sb �������, ���������� ������� <i>branch</i>
	\param[in] lambda �������� <i>lambda</i> ������� ��������� �
	���������� <i>RD</i> ������� � ������������ ����� ������ �����
	<i>R (rate)</i> � <i>D (distortion)</i> ������� �������
	<i>��������</i>.
	\return ��������� ������� ���������� ������

	�������� ����������� ��������� �������� ������ � <i>35.pdf</i>

	\note ������� ��������� ��� ���� ������ (��� ������� ������ �
	<i>LL</i> ��������, ��� � ��� ���� ���������).
*/
encoder::_branch_topology_t
encoder::_optimize_branch_topology(const p_t &branch,
								   const subbands::subband_t &sb,
								   const lambda_t &lambda)
{
	// ��������� ��������� ��������
	const sz_t lvl_j = sb.lvl + subbands::LVL_NEXT;
	const subbands::subband_t &sb_j = _wtree.sb().get(lvl_j, sb.i);

	// ����� ������ ��� ����������� ��������� ��������� ����������
	const sz_t model = _ind_map<wnode::member_wc>(branch, sb_j);

	// ����� �������� ����������� ���������
	wtree::n_iterator i = _wtree.iterator_through_n(sb.lvl);

	// ������ �������� ����� ������
	_branch_topology_t optim_topology;
	optim_topology.n	= i->get();
	optim_topology.j	= _topology_calc_j_uni(branch,
											   optim_topology.n);

	// ����������� ��������
	for (i->next(); !i->end(); i->next())
	{
		const n_t &n = i->get();

		const j_t j_sum = _topology_calc_j_uni(branch, n);

		const j_t j = (j_sum + lambda * _h_map(model, n));

		if (j < optim_topology.j) {
			optim_topology.j = j;
			optim_topology.n = n;
		}
	}

	return optim_topology;
}


/*!	\param[in] root ���������� ��������� ��������
	\param[in] virtual_encode ���� <i>true</i>, �� ����� �������������
	����������� ����������� (������ ������������� �������, ��� ���������
	����������� ������� � �������� �����).

	������� ��������� �����������:
	- ������������ ��� �������� ��������
	- ���������� �������� ���������� ��� �������� ��������
	- ������������� ������������� ������ � ������� ������ ����������
*/
void encoder::_encode_tree_root(const p_t &root,
								const bool virtual_encode)
{
	// ��������� ��������� ��������
	const wnode &root_node = _wtree.at(root);

	// ����������� ������, ������������ ��� ����������� ������������
	const sz_t spec_model	= _ind_spec(0, subbands::LVL_0);

	// ������������ ����������� � �������� ������
	_encode_spec(spec_model, root_node.wc, virtual_encode);

	// ����������� ������ ��� ����������� �������� ����������
	const sz_t map_model	= _ind_map(0, subbands::LVL_0);

	// ������������ ��������� ������� ���������� � �������� ������
	_encode_map(map_model, root_node.n, virtual_encode);

	#ifdef LIBWIC_USE_DBG_SURFACE
	// ������ ���������� ���������� � �������������� ������������ � ��������
	// ����������
	wicdbg::dbg_pixel &dbg_pixel = _dbg_opt_surface.get(root);
	dbg_pixel.wc		= root_node.wc;
	dbg_pixel.wc_model	= spec_model;
	dbg_pixel.n			= root_node.n;
	dbg_pixel.n_model	= map_model;
	#endif

	// ����������� �������� ������������� � ������� ������
	for (wtree::coefs_iterator i = _wtree.iterator_over_LL_children(root);
		 !i->end(); i->next())
	{
		// ����������� ������, ������������ ��� ����������� ������������
		const sz_t spec_model = _ind_spec(0, subbands::LVL_1);

		// ������ �� ���������� �����������
		const wk_t &wc		= _wtree.at(i->get()).wc;

		// ������������ ������������ � ������� ������
		_encode_spec(spec_model, wc, virtual_encode);

		#ifdef LIBWIC_USE_DBG_SURFACE
		// ������ ���������� ���������� � �������������� ������������
		wicdbg::dbg_pixel &dbg_pixel = _dbg_opt_surface.get(i->get());
		dbg_pixel.wc		= wc;
		dbg_pixel.wc_model	= spec_model;
		#endif
	}
}


/*!	\param[in] root ���������� �������� ��������
	\param[in] lvl ����� ������ ����������
	\param[in] virtual_encode ���� <i>true</i>, �� ����� �������������
	����������� ����������� (������ ������������� �������, ��� ���������
	����������� ������� � �������� �����).
*/
void encoder::_encode_tree_leafs(const p_t &root, const sz_t lvl,
								 const bool virtual_encode)
{
	// ���������� ��� ������� �������
	const sz_t lvl_g = lvl;
	const sz_t lvl_j = lvl_g + subbands::LVL_PREV;
	const sz_t lvl_i = lvl_j + subbands::LVL_PREV;

	// ���� �� ��������� � ������
	for (sz_t k = 0; _wtree.sb().subbands_on_lvl(lvl) > k; ++k)
	{
		const subbands::subband_t &sb_g = _wtree.sb().get(lvl_g, k);
		const subbands::subband_t &sb_j = _wtree.sb().get(lvl_j, k);

		// ����������� �������������
		for (wtree::coefs_iterator g = _wtree.iterator_over_leafs(root, sb_g);
			 !g->end(); g->next())
		{
			const p_t &p_g = g->get();

			wnode &node_g = _wtree.at(p_g);

			if (node_g.invalid) continue;

			const sz_t model = _ind_spec<wnode::member_wc>(p_g, sb_g);

			_encode_spec(model, node_g.wc, virtual_encode);

			#ifdef LIBWIC_USE_DBG_SURFACE
			// ������ ����������� ������������ � ���������� �����������
			wicdbg::dbg_pixel &dbg_pixel = _dbg_opt_surface.get(p_g);
			dbg_pixel.wc		= node_g.wc;
			dbg_pixel.wc_model	= model;
			#endif
		}

		// �� ������������� ������ ��� ��������� ��������� ����������
		if (_wtree.lvls() == lvl) continue;

		// ����������� ��������� ��������� ����������
		for (wtree::coefs_iterator j = _wtree.iterator_over_leafs(root, sb_j);
			 !j->end(); j->next())
		{
			const p_t &p_j = j->get();

			const p_t &p_i = _wtree.prnt_uni(p_j);
			const wnode &node_i = _wtree.at(p_i);

			// ����� ����������, ��� ������� ������� �� ��������
			const n_t mask = _wtree.child_n_mask_uni(p_j, p_i);

			// ��������� � ���������� �������, ���� ����� ���������
			if (!_wtree.test_n_mask(node_i.n, mask)) continue;

			wnode &node_j = _wtree.at(p_j);

			const sz_t model = _ind_map<wnode::member_wc>(p_j, sb_g);

			_encode_map(model, node_j.n, virtual_encode);

			#ifdef LIBWIC_USE_DBG_SURFACE
			// ������ �������� ���������� � ���������� �����������
			wicdbg::dbg_pixel &dbg_pixel = _dbg_opt_surface.get(p_j);
			dbg_pixel.n			= node_j.n;
			dbg_pixel.n_model	= model;
			#endif
		}
	}
}


/*!	\param[in] root ���������� ��������� �������� ������
	\param[in] virtual_encode ���� <i>true</i>, �� ����� �������������
	����������� ����������� (������ ������������� �������, ��� ���������
	����������� ������� � �������� �����).
*/
void encoder::_encode_tree(const p_t &root,
						   const bool virtual_encode)
{
	// ����������� ��������� � �������� ��������� ������
	_encode_tree_root(root, virtual_encode);

	// ����������� ��������� ������ �� ��������� �������
	const sz_t first_lvl = subbands::LVL_1 + subbands::LVL_NEXT;
	const sz_t final_lvl = _wtree.lvls();

	for (sz_t lvl = first_lvl; final_lvl >= lvl; ++lvl)
	{
		_encode_tree_leafs(root, lvl, virtual_encode);
	}
}


/*!	\param[in] decode_mode ���� <i>false</i> ������� ����� ���������
	����������� �������, ����� (���� <i>true</i>) ����� ���������
	������������� �������.
*/
void encoder::_encode_wtree_root(const bool decode_mode)
{
	// LL c������
	const subbands::subband_t &sb_LL = _wtree.sb().get_LL();

	// ������ ��� ����������� � LL ��������
	const sz_t spec_LL_model = _ind_spec(0, sb_LL.lvl);
	const sz_t map_LL_model = _ind_map(0, sb_LL.lvl);

	// (��)����������� ������������� � ��������� ��������� ����������
	// �� LL ��������
	for (wtree::coefs_iterator i = _wtree.iterator_over_subband(sb_LL);
		 !i->end(); i->next())
	{
		// ���������� ��������
		const p_t &p_i = i->get();

		// ��� ������� �� LL ��������
		wnode &node = _wtree.at(p_i);

		if (decode_mode)
		{
			// ������������� ������������ � �������� ����������
			node.wc = _decode_spec(spec_LL_model);
			node.n = _decode_map(map_LL_model);

			// ���������� ������ � ������������ � ���������� ���������
			// ����������
			_wtree.uncut_leafs(p_i, node.n);
		}
		else
		{
			// ����������� ������������ � �������� ����������
			_encode_spec(spec_LL_model, node.wc);
			_encode_map(map_LL_model, node.n);

			#ifdef LIBWIC_USE_DBG_SURFACE
			// ������ ���������� � ���������� ������������� � ���������
			// ���������� � ���������� �����������
			wicdbg::dbg_pixel &dbg_pixel = _dbg_enc_surface.get(p_i);
			dbg_pixel.wc		= node.wc;
			dbg_pixel.wc_model	= spec_LL_model;
			dbg_pixel.n			= node.n;
			dbg_pixel.n_model	= map_LL_model;
			#endif
		}
	}

	// ������ ��� ����������� ������������� � ������� ������
	const sz_t spec_1_model = _ind_spec(0, subbands::LVL_1);

	// (��)����������� ������������� �� ��������� ������� ������
	for (sz_t k = 0; subbands::SUBBANDS_ON_LEVEL > k; ++k)
	{
		// ��������� ������� � ������� ������
		const subbands::subband_t &sb = _wtree.sb().get(subbands::LVL_1, k);

		// ���� �� ���� ��������� �� �������� � ������� ������
		for (wtree::coefs_iterator i = _wtree.iterator_over_subband(sb);
			 !i->end(); i->next())
		{
			// (��)����������� ����������� ������������
			if (decode_mode)
			{
				_wtree.at(i->get()).wc = _decode_spec(spec_1_model);
			}
			else
			{
				// ������ �� ���������� �����������
				const wk_t &wc = _wtree.at(i->get()).wc;

				// ����������� ������������
				_encode_spec(spec_1_model, wc);

				#ifdef LIBWIC_USE_DBG_SURFACE
				// ������ ���������� � ���������� ������������ � ����������
				// �����������
				wicdbg::dbg_pixel &dbg_pixel = _dbg_enc_surface.get(i->get());
				dbg_pixel.wc		= wc;
				dbg_pixel.wc_model	= spec_1_model;
				#endif
			}
		}
	}
}


/*!	\param[in] lvl ����� ������, ������������ �� ������� �����
	������������
	\param[in] decode_mode ���� <i>false</i> ������� ����� ���������
	����������� �������, ����� (���� <i>true</i>) ����� ���������
	������������� �������.

	\todo �������� �� ����� ������ wtree::uncut_leafs() ��� ��������� �
	����������� ����� ��� ����������� ���������� ��������� ����������.
	������ ����� ����� ����� �������������� ������������������� ��� ����
	wnode::invalid ��������� <i>true</i>
*/
void encoder::_encode_wtree_level(const sz_t lvl,
								  const bool decode_mode)
{
	// ����������� ����������� ��� ������� �������
	const sz_t lvl_g = lvl;
	const sz_t lvl_j = lvl_g + subbands::LVL_PREV;
	const sz_t lvl_i = lvl_j + subbands::LVL_PREV;

	// ���� �� ��������� �� ������
	for (sz_t k = 0; _wtree.sb().subbands_on_lvl(lvl) > k; ++k)
	{
		// ������� �� ������ ������������ �� �������� �����
		// ������������ � ������������ ��� ��� ����
		const subbands::subband_t &sb_g = _wtree.sb().get(lvl_g, k);
		const subbands::subband_t &sb_j = _wtree.sb().get(lvl_j, k);

		// ����������� �������������
		for (wtree::coefs_iterator g = _wtree.iterator_over_subband(sb_g);
			 !g->end(); g->next())
		{
			// ���������� ��������
			const p_t &p_g = g->get();

			// ��� �������
			wnode &node_g = _wtree.at(p_g);

			// ��������� � ����������, ���� ������������ ����� �
			// ����������� �����
			if (node_g.invalid) continue;

			// �������� ������ ��� (��)����������� ������������
			const sz_t model = _ind_spec<wnode::member_wc>(p_g, sb_g);

			// (��)����������� ������������
			if (decode_mode)
			{
				node_g.wc = _decode_spec(model);

				#ifdef LIBWIC_USE_DBG_SURFACE
				// ������ ���������� � ���������� ������������ � ����������
				// �����������
				wicdbg::dbg_pixel &dbg_pixel = _dbg_dec_surface.get(p_g);
				dbg_pixel.wc		= node_g.wc;
				dbg_pixel.wc_model	= model;
				#endif
			}
			else
			{
				_encode_spec(model, node_g.wc);

				#ifdef LIBWIC_USE_DBG_SURFACE
				// ������ ���������� � ���������� ������������ � ����������
				// �����������
				wicdbg::dbg_pixel &dbg_pixel = _dbg_enc_surface.get(p_g);
				dbg_pixel.wc		= node_g.wc;
				dbg_pixel.wc_model	= model;
				#endif
			}
		}

		// �� ������������� ������ ��� ��������� ��������� ����������
		if (_wtree.lvls() == lvl) continue;

		// ����������� ��������� ��������� ����������
		for (wtree::coefs_iterator j = _wtree.iterator_over_subband(sb_j);
			 !j->end(); j->next())
		{
			// ���������� ��������
			const p_t &p_j = j->get();

			// ���������� ������������� ��������
			const p_t &p_i = _wtree.prnt_uni(p_j);

			// ������ �� ������������ �������
			const wnode &node_i = _wtree.at(p_i);

			// ����� ����������, ��� ������� ������� �� ��������
			const n_t mask = _wtree.child_n_mask_uni(p_j, p_i);

			// ��������� � ���������� �������, ���� ����� ���������
			if (!_wtree.test_n_mask(node_i.n, mask)) continue;

			// �������� ��������
			wnode &node_j = _wtree.at(p_j);

			// ����� ������ ��� ����������� ���������� �������� ����������
			const sz_t model = _ind_map<wnode::member_wc>(p_j, sb_g);

			if (decode_mode)
			{
				// ������������� �������� ����������
				node_j.n = _decode_map(model);

				// ���������� ������ � ������������ � ���������� ���������
				// ����������
				_wtree.uncut_leafs(p_j, node_j.n);

				#ifdef LIBWIC_USE_DBG_SURFACE
				// ������ ���������� � ��������� �������� ���������� �
				// ���������� �����������
				wicdbg::dbg_pixel &dbg_pixel = _dbg_dec_surface.get(p_j);
				dbg_pixel.n			= node_j.n;
				dbg_pixel.n_model	= model;
				#endif
			}
			else
			{
				// ����������� �������� ����������
				_encode_map(model, node_j.n);

				#ifdef LIBWIC_USE_DBG_SURFACE
				// ������ ���������� � ��������� �������� ���������� �
				// ���������� �����������
				wicdbg::dbg_pixel &dbg_pixel = _dbg_enc_surface.get(p_j);
				dbg_pixel.n			= node_j.n;
				dbg_pixel.n_model	= model;
				#endif
			}
		}
	}
}


/*!	\param[in] decode_mode ���� <i>false</i> ������� ����� ���������
	����������� �������, ����� (���� <i>true</i>) ����� ���������
	������������� �������.
*/
void encoder::_encode_wtree(const bool decode_mode)
{
	#ifdef LIBWIC_USE_DBG_SURFACE
	// ������� ���������� �����������, ���� ������������ �����������
	if (!decode_mode)
	{
		_dbg_enc_surface.clear();
	}
	else
	{
		_dbg_dec_surface.clear();
	}
	#endif

	// (��)����������� �������� ���������
	_encode_wtree_root(decode_mode);

	// (��)����������� ��������� ���������
	const sz_t first_lvl = subbands::LVL_1 + subbands::LVL_NEXT;
	const sz_t final_lvl = _wtree.lvls();

	// ���� �� �������
	for (sz_t lvl = first_lvl; final_lvl >= lvl; ++lvl)
	{
		_encode_wtree_level(lvl, decode_mode);
	}

	#ifdef LIBWIC_USE_DBG_SURFACE
	// ������ ���������� ����������� � ����
	if (!decode_mode)
	{
		_dbg_enc_surface.save<wicdbg::dbg_pixel::member_wc>
			("dumps/[encoder]dbg_enc_suface.wc.bmp", true);
		_dbg_enc_surface.save<wicdbg::dbg_pixel::member_wc>
			("dumps/[encoder]dbg_enc_suface.wc.txt", false);
		_dbg_enc_surface.save<wicdbg::dbg_pixel::member_wc_model>
			("dumps/[encoder]dbg_enc_suface.wc_model.bmp", true);
		_dbg_enc_surface.save<wicdbg::dbg_pixel::member_n>
			("dumps/[encoder]dbg_enc_suface.n.bmp", true);
		_dbg_opt_surface.diff<wicdbg::dbg_pixel::member_wc_model>
			(_dbg_enc_surface, "dumps/[encoder]dbg_suface.wc_model.diff");
	}
	else
	{
		_dbg_dec_surface.save<wicdbg::dbg_pixel::member_wc>
			("dumps/[decoder]dbg_dec_suface.wc.bmp", true);
		_dbg_dec_surface.save<wicdbg::dbg_pixel::member_wc>
			("dumps/[decoder]dbg_dec_suface.wc.txt", false);
		_dbg_dec_surface.save<wicdbg::dbg_pixel::member_wc_model>
			("dumps/[decoder]dbg_dec_suface.wc_model.bmp", true);
		_dbg_dec_surface.save<wicdbg::dbg_pixel::member_n>
			("dumps/[decoder]dbg_dec_suface.n.bmp", true);
	}
	#endif
}


/*!	\param[in] root ���������� ��������� �������� ���������������� ������
	\param[in] lambda �������� <i>lambda</i> ������������ ���
	���������� <i>RD</i> �������� (������� ��������). ������������
	����� ������ ����� ������� � �������� ���������.

	�� ������ ���� ����������� ����������� ������������� �������������
	�� ����� ��������� (� ���������� ��������) ������ ���������� �
	����������� ������ <i>RD-������� ��������</i> ��� ��������� ����������
	� ���������� ��������� ������� ������.
*/
void encoder::_optimize_tree_step_1(const p_t &root, const lambda_t &lambda)
{
	// ��������������� ��� ���� �������������� ������
	const sz_t lvl_i = _wtree.sb().lvls() + subbands::LVL_PREV;
	const sz_t lvl_j = _wtree.sb().lvls();

	// ���� �� ���������
	for (sz_t k = 0; subbands::SUBBANDS_ON_LEVEL > k; ++k)
	{
		const subbands::subband_t &sb_i = _wtree.sb().get(lvl_i, k);
		const subbands::subband_t &sb_j = _wtree.sb().get(lvl_j, k);

		// ���� �� ��������� �� �������������� ������ ������
		for (wtree::coefs_iterator i = _wtree.iterator_over_leafs(root, sb_i);
			!i->end(); i->next())
		{
			// ������������ ������� �� �������������� ������
			const p_t &p = i->get();

			// ��� 1.1. ������������� ��������������� �������������
			_coefs_fix(p, sb_j, lambda);

			// ��� 1.2. ������ RD-������� �������� ��� ��������� ���������� �
			// ���������� �������
			_prepare_j(p, sb_j, lambda);
		}
	}
}


/*!	\param[in] root ���������� ��������� �������� ������
	\param[in] lambda �������� <i>lambda</i> ������������ ���
	���������� <i>RD</i> �������� (������� ��������). ������������
	����� ������ ����� ������� � �������� ���������.
*/
void encoder::_optimize_tree_step_2(const p_t &root, const lambda_t &lambda)
{
	// ���� 2.1 - 2.5 ----------------------------------------------------------

	// ���� �� �������
	for (sz_t lvl_i = _wtree.sb().lvls() + 2*subbands::LVL_PREV;
		 0 < lvl_i; --lvl_i)
	{
		const sz_t lvl_j = lvl_i + subbands::LVL_NEXT;

		// ���� �� ���������
		for (sz_t k = 0; subbands::SUBBANDS_ON_LEVEL > k; ++k) {

			// ��������� ������ �� ��������
			const subbands::subband_t &sb_i = _wtree.sb().get(lvl_i, k);
			const subbands::subband_t &sb_j = _wtree.sb().get(lvl_j, k);

			// ���� �� ������������ ���������
			for (wtree::coefs_iterator i =
						_wtree.iterator_over_leafs(root, sb_i);
				 !i->end(); i->next())
			{
				const p_t &p = i->get();

				// ��� 2.1. ����������� ����������� ��������� ������
				const _branch_topology_t optim_topology =
						_optimize_branch_topology(p, sb_i, lambda);

				// ��� 2.2. ��������� ��������� ������
				_wtree.cut_leafs<wnode::member_wc>(p, optim_topology.n);

				// ��� 2.3. ������������� ��������������� �������������
				_coefs_fix(p, sb_j, lambda);

				// ��� 2.4. ���������� ��� ��������� ���������� ������
				_prepare_j(p, sb_j, optim_topology.j, lambda);
			}
		}
		// �� ������ ������, ���� �����-���� ���� ������� sz_t �����������
		// ����� :^)
		// if (0 == lvl) break;
	}
}


/*!	\param[in] root ���������� ��������� �������� ������
	\param[in] lambda �������� <i>lambda</i> ������������ ���
	���������� <i>RD</i> �������� (������� ��������). ������������
	����� ������ ����� ������� � �������� ���������.
*/
void encoder::_optimize_tree_step_3(const p_t &root, const lambda_t &lambda)
{
	const subbands::subband_t &sb_LL = _wtree.sb().get_LL();

	// ��� 2.6. ����������� ����������� ��������� ������
	const _branch_topology_t optim_topology =
			_optimize_branch_topology(root, sb_LL, lambda);

	// ��� 2.7. ��������� ��������� ������
	_wtree.cut_leafs<wnode::member_wc>(root, optim_topology.n);

	// ��� 3. ���������� RD-������� �������� ��� ����� ������
	_calc_jx_value(root, optim_topology.j, lambda);
}


/*!	\param[in] root ���������� ��������� �������� ������
	\param[in] lambda �������� <i>lambda</i> ������������ ���
	���������� <i>RD</i> �������� (������� ��������). ������������
	����� ������ ����� ������� � �������� ���������.
	\return �������� <i>RD-������� ��������</i> ��� ������

	��� ���������� ������ ������� ����������, ����� � ������ ��� �����
	���� �������� ��� �����������, � �������� ��� ����������. ��������
	������� <i>��������</i> � ������� ������ ���� ��������. ��������������
	����� ������ ���� �������� �� ���������� ������ (������ acoder::use()).

	\note ����������� ���������� ��������� ������� wnode::filling_refresh(),
	��� �������, ��� ���� wnode::w � wnode::wq ���������.
*/
j_t encoder::_optimize_tree(const p_t &root, const lambda_t &lambda)
{
	_optimize_tree_step_1(root, lambda);
	_optimize_tree_step_2(root, lambda);
	_optimize_tree_step_3(root, lambda);

	const j_t j = _wtree.at(root).j1;

	if (0 != _optimize_tree_callback)
	{
		_optimize_tree_callback(root, _optimize_tree_callback_param);
	}

	return j;
}


/*!	\param[in] lambda �������� <i>lambda</i> ������������ ���
	���������� <i>RD</i> �������� (������� ��������). ������������
	����� ������ ����� ������� � �������� ���������.

	\param[in] refresh_wtree ���� <i>true</i>, �� ����� ������� �����������
	����� ������� ������� wtree::filling_refresh(), ������� ������� ������
	� ��������� ���������. ������������ ��� ���������� ���������� ��������
	����������� ��� ����� �������� (��������, � ����� ������� ������������
	�������� ��������� <i>lambda</i>).

	\param[in] virtual_encode ���� <i>true</i>, �� ����� �������������
	����������� ����������� (������ ������������� �������, ��� ���������
	����������� ������� � �������� �����). ��� ���������� �����������
	�����������, ���� _optimize_result_t::bpp ������������ � 0, ��� ���
	��� ���������� ��������� ����������� ���������� ������� �������
	�������.

	\param[in] precise_bpp ���� <i>true</i>, �� ����� ����� �����������
	����� ������� ���� ��������� ������� ������, ������������� � ���, ���
	������ ��������������� ������ ����� ��������� ��������
	_setup_acoder_post_models(), � ����� ����� ����������� ��������
	����������� ���� �������� ������� (� ���������������� ����������).
	��. ������� _real_encode_tight() ��� �������������� ����������. �����
	����� ��������� ���������� � ����� ���������, ��� ��� ��� �����������
	��� � <i>true</i> ������ ��������������� ������ ����������, ���
	���������� ������������� ���� ������� � ���������� ������� ����������
	�����������. ������, ����� ���� � ������������. ���� �������� ���������
	� <i>true</i>, �� ����� ���������� ����������� ����� �� �����������
	�������� �����������, ��� ��� ��� ��� ������� (� ��� ����� ���������
	���� ���������� optimize_result_t::real_encoded).

	\return ��������� ���������� �����������

	��� ���������� ������ ���� ������� ����������, ����� ���� wnode::w �
	wnode::wq ��������� ������� ���� ���������. � ������� ��� ����� ������
	���� �������� ��� �����������, � �������� ��� ����������. ��������
	������� <i>��������</i> � ������� ������ ���� ��������. ��������������
	����� ������ ���� �������� �� ���������� ������ (������ acoder::use()).

	\note ����������� ���������� ��������� ������� wtree::filling_refresh(),
	��� �������, ��� ���� wnode::w � wnode::wq ���������.

	\note ���� �������� ������ #LIBWIC_DEBUG, ������� ����� ��������
	����������� ���������� ����������.
*/
encoder::optimize_result_t
encoder::_optimize_wtree(const lambda_t &lambda,
						 const bool refresh_wtree,
						 const bool virtual_encode,
						 const bool precise_bpp)
{
	// ������������� ������������� ����������
	optimize_result_t result;
	result.q				= _wtree.q();
	result.lambda			= lambda;
	result.j				= 0;
	result.bpp				= 0;
	result.models.version	= MODELS_DESC_NONE;
	result.real_encoded		= false;

	// ���������� ������ ������� ������������� (���� ���������)
	if (refresh_wtree) _wtree.filling_refresh();

	#ifdef LIBWIC_USE_DBG_SURFACE
	// ������� ���������� �����������
	_dbg_opt_surface.clear();
	#endif

	// ����������� ��������� ������ � ������������
	_acoder.encode_start();

	for (wtree::coefs_iterator i =
				_wtree.iterator_over_subband(_wtree.sb().get_LL());
		 !i->end(); i->next())
	{
		// ������ ���������� ������ ������������� ������� ��������������
		const p_t &root = i->get();

		// ����������� ��������� ��������� �����
		result.j += _optimize_tree(root, lambda);

		// ����������� ��������� �����
		_encode_tree(root, virtual_encode);
	}

	_acoder.encode_stop();

	#ifdef LIBWIC_USE_DBG_SURFACE
	// ���������� ����� ���������� ����������� � �����
	_dbg_opt_surface.save<wicdbg::dbg_pixel::member_wc>
		("dumps/[encoder]dbg_opt_suface.wc.bmp", true);
	_dbg_opt_surface.save<wicdbg::dbg_pixel::member_wc_model>
		("dumps/[encoder]dbg_opt_suface.wc_model.bmp", true);
	_dbg_opt_surface.save<wicdbg::dbg_pixel::member_n>
		("dumps/[encoder]dbg_opt_suface.n.bmp", true);
	#endif

	// ������� bpp, ���� ������������� �������� �����������
	if (precise_bpp)
	{
		result.bpp = _real_encode_tight(result.models);

		result.real_encoded = true;
	}
	else if (!virtual_encode)
	{
		result.bpp = _calc_encoded_bpp();
	}

	// ����� ���������� ����������
	#ifdef LIBWIC_DEBUG
	if (_dbg_out_stream.good())
	{
		_dbg_out_stream << "[OWTR]:";
		_dbg_out_stream << " q: " << std::setw(8) << _wtree.q();
		_dbg_out_stream << " lambda: " << std::setw(8) << lambda;
		_dbg_out_stream << " j: " << std::setw(8) << result.j;
		_dbg_out_stream << " bpp: " << std::setw(8) << result.bpp;
		_dbg_out_stream << std::endl;
	}
	#endif

	// �������� ����� ���������������� �������
	if (0 != _optimize_callback)
	{
		_optimize_callback(result, _optimize_callback_param);
	}

	// ������� ����������
	return result;
}


/*!	\param[in] lambda �������� <i>lambda</i> ������������ ���
	���������� <i>RD</i> �������� (������� ��������). ������������
	����� ������ ����� ������� � �������� ���������.

	\param[in] models �������� ������� ��������������� ������, �������
	���������� ������������ ��� ���������� �����������

	\param[in] refresh_wtree ���� <i>true</i>, �� ����� ������� �����������
	����� ������� ������� wtree::filling_refresh(), ������� ������� ������
	� ��������� ���������. ������������ ��� ���������� ���������� ��������
	����������� ��� ����� �������� (��������, � ����� ������� ������������
	�������� ��������� <i>lambda</i>).

	\param[in] virtual_encode ���� <i>true</i>, �� ����� �������������
	����������� ����������� (������ ������������� �������, ��� ���������
	����������� ������� � �������� �����). ��� ���������� �����������
	�����������, ���� _optimize_result_t::bpp ������������ � 0, ��� ���
	��� ���������� ��������� ����������� ���������� ������� �������
	�������.

	\param[in] precise_bpp ���� <i>true</i>, �� ����� ����� �����������
	����� ������� ���� ��������� ������� ������, ������������� � ���, ���
	������ ��������������� ������ ����� ��������� ��������
	_setup_acoder_post_models(), � ����� ����� ����������� ��������
	����������� ���� �������� ������� (� ���������������� ����������).
	��. ������� _real_encode_tight() ��� �������������� ����������. �����
	����� ��������� ���������� � ����� ���������, ��� ��� ��� �����������
	��� � <i>true</i> ������ ��������������� ������ ����������, ���
	���������� ������������� ���� ������� � ���������� ������� ����������
	�����������. ������, ����� ���� � ������������. ���� �������� ���������
	� <i>true</i>, �� ����� ���������� ����������� ����� �� �����������
	�������� �����������, ��� ��� ��� ��� ������� (� ��� ����� ���������
	���� ���������� optimize_result_t::real_encoded).

	\sa _optimize_wtree()
*/
encoder::optimize_result_t
encoder::_optimize_wtree_m(const lambda_t &lambda,
						   const models_desc_t &models,
						   const bool refresh_wtree,
						   const bool virtual_encode,
						   const bool precise_bpp)
{
	// ��������� ������� ��������������� ������
	_acoder.use(_mk_acoder_models(models));

	// ���������� ����������� ���������
	return _optimize_wtree(lambda, refresh_wtree, virtual_encode, precise_bpp);
}


/*!	\param[in] lambda �������� <i>lambda</i> ������������ ���
	���������� <i>RD</i> �������� (������� ��������). ������������
	����� ������ ����� ������� � �������� ���������.

	\param[in] q ������������

	\param[in] virtual_encode ���� <i>true</i> �� ����� �������������
	����������� ����������� (������ ������������� �������, ��� ���������
	����������� ������� � �������� �����).

	\param[in] precise_bpp ���� <i>true</i>, ����� ����������� ���������
	������� ������ ����� ���������� ��������� ����������� � �������
	�������� ��������������� ������ (��. _real_encode_tight). � ���� ������,
	����� ����� ����������� ��� ����� ����������� �������� �����������, ���
	��� ��� ��� ����������� �������� _real_encode_tight. ������ �������
	�������� ��������, ��� ������� ������ ��������������� ������ �����
	���������� ������� ����������.

	\return ��������� ���������� �����������

	��� ������ ������� <i>%encoder::_optimize_wtree()</i> ���� ����������
	����������� � ��������� ������� ��������������� ������. ��� ����������
	������ ������� ���������� ����������� ������������� � ���� wnode::w
	��������� �������.
*/
encoder::optimize_result_t
encoder::_optimize_wtree_q(const lambda_t &lambda, const q_t &q,
						   const bool virtual_encode,
						   const bool precise_bpp)
{
	// ����������� �������������
	_wtree.quantize(q);

	// �������� ������� � �������������� �����
	const models_desc_t models = _setup_acoder_models();

	// ����������� ��������� ������
	optimize_result_t result = _optimize_wtree_m(lambda, models, false,
												 virtual_encode, precise_bpp);

	// ���������� �������� ������� ��������������� ������
	if (MODELS_DESC_NONE == result.models.version)
	{
		result.models = models;
	}

	// ������� ����������
	return result;
}


/*!	\param[in] lambda �������� <i>lambda</i> ������������ ��� ����������
	<i>RD ��������</i> (<i>������� ��������</i>). ������������ ����� ������
	����� ������� � �������� ���������. ������� �������� <i>lambda</i>
	������������� �������� �������� ����������<i>bpp</i>.

	\param[in] q_min ������ ������� ��������� ������ (�����������
	��������)

	\param[in] q_max ������� ������� ��������� ������ (������������
	��������)

	\param[in] q_eps ����������� ����������� ����������� ������������
	<i>q</i> ��� ������� �������� <i>RD ������� ��������</i> ����������
	(��� ������������� ��������� <i>lambda</i>).

	\param[in] j_eps ����������� ����������� ���������� �������� <i>RD
	������� ��������</i>. ��� ��� ���������� �������� ������� <i>J</i>
	������� �� ������ �������� (����� ��� ������ �����������, ��� ���,
	�������� ��������� <i>lambda</i>), ������������ ��� ��������
	��������������. ������� ��� �������� �� ��������� ����� <i>0</i>,
	����� ��� ������ ������������ ������������ ����������� ������
	����������� ��������� <i>q</i>.

	\param[in] virtual_encode ���� <i>true</i> �� ����� �������������
	����������� ����������� (������ ������������� �������, ��� ���������
	����������� ������� � �������� �����). ����������� ����������� �������
	�������, �� ��� ������������� ������ ����������� ����������� �������
	������� ������ �� ����������� ������ ������� �����������.

	\param[in] max_iterations ������������ ���������� �������� ����������
	�������� <i>RD ������� ��������</i>. ���� ��� �������� ����� <i>0</i>,
	���������� ����������� �������� �� ����������.

	\param[in] precise_bpp ���� <i>true</i>, ��� ���������� ��������
	����������� ��������� ����� ������������� ��������� ������� ������ �����
	���������� ��������� ����������� � ������� �������� ��������������� ������
	(��. _real_encode_tight). � ���� ������, ����� ����� ����������� ��� �����
	����������� �������� �����������, ��� ��� ��� ��� ����������� ��������
	_real_encode_tight.

	\return ��������� ������������� ������

	������ ������� ���������� ����� �������� ������� ��� ������ ��������
	<i>RD ������� ��������</i>:
	\verbatim
	  j|___                              ______/
	   |   \_                         __/   |
	   |     \___                    /      |
	   |       | \____              /       |
	   |       |   |  \___   ______/        |
	   |       |   |      \_/     |         |
	   |       |   |       |      |         |
	---+-------+---+-------+------+---------+----------> q
	  0|       a   b     j_min    c        d
	\endverbatim

	\note ��� ���������� ������ ���� ������� ����������, ����� ����
	wnode::w ��������� ������� ���� ���������. ��� �����
	����� ������������ ������� wtree::load_field().

	\note ���� �������� ������ #LIBWIC_DEBUG, ������� ����� ��������
	����������� ���������� ����������.
*/
encoder::optimize_result_t
encoder::_search_q_min_j(const lambda_t &lambda,
						 const q_t &q_min, const q_t &q_max,
						 const q_t &q_eps, const j_t &j_eps,
						 const bool virtual_encode,
						 const sz_t &max_iterations,
						 const bool precise_bpp)
{
	// �������� �����������
	assert(0 <= lambda);
	assert(1 <= q_min && q_min <= q_max);
	assert(0 <= q_eps && 0 <= j_eps);

	// ������������ �������� �������
	static const q_t factor_b	= (q_t(3) - sqrt(q_t(5))) / q_t(2);
	static const q_t factor_c	= (sqrt(q_t(5)) - q_t(1)) / q_t(2);

	// ��������� ��������� ��� ������
	q_t q_a = q_min;
	q_t q_d = q_max;

	// ���������� �������� � ������ ���� ������
	q_t q_b = q_a + factor_b * (q_d - q_a);
	q_t q_c = q_a + factor_c * (q_d - q_a);

	optimize_result_t result_b = _optimize_wtree_q(lambda, q_b, virtual_encode,
												   precise_bpp);
	optimize_result_t result_c = _optimize_wtree_q(lambda, q_c, virtual_encode,
												   precise_bpp);

	// ����������� ����������� � ���������� �����������
	optimize_result_t result_prev	= result_b;
	optimize_result_t result		= result_c;

	// ������� ���������� ��������
	sz_t iterations					= 0;

	// ����� ������������ �������� q
	for (;;)
	{
		// ��������, ���������� �� ������ ��������
		if (q_eps >= abs(q_c - q_b) ||
			j_eps >= abs(result.j - result_prev.j))
		{
			break;
		}

		if (0 < max_iterations && max_iterations <= iterations)
		{
			break;
		}

		// ����� ����� ������� ����� ���������
		result_prev = result;

		// ����� ���������� �������� q
		if (result_b.j < result_c.j)
		{
			q_d = q_c;
			q_c = q_b;
			q_b = q_a + factor_b*(q_d - q_a);

			result_c = result_b;

			result = result_b = _optimize_wtree_q(lambda, q_b, virtual_encode,
												  precise_bpp);
		}
		else
		{
			q_a = q_b;
			q_b = q_c;
			q_c = q_a + factor_c*(q_d - q_a);

			result_b = result_c;

			result = result_c = _optimize_wtree_q(lambda, q_c, virtual_encode,
												  precise_bpp);
		}

		// ���������� ���������� ��������
		++iterations;
	}

	// ����� ���������� ����������
	#ifdef LIBWIC_DEBUG
	if (_dbg_out_stream.good())
	{
		_dbg_out_stream << "[SQMJ]:";
		_dbg_out_stream << " q: " << std::setw(8) << result.q;
		_dbg_out_stream << " lambda: " << std::setw(8) << result.lambda;
		_dbg_out_stream << " j: " << std::setw(8) << result.j;
		_dbg_out_stream << " bpp: " << std::setw(8) << result.bpp;
		_dbg_out_stream << std::endl;
	}
	#endif

	// ����������� ����������� ����������
	return result;
}


/*! \param[in] bpp ����������� ������� (Bits Per Pixel), ��� ����������
	�������� ����� ����������� �������� <i>lambda</i>

	\param[in] bpp_eps ����������� ��������, c ������� <i>bpp</i> ����������
	� ���������� ������ ��������� <i>lambda</i> ����� ���������������
	���������.

	\param[in] lambda_min ������ ������� ��������� ������ (�����������
	��������)

	\param[in] lambda_max ������� ������� ��������� ������ (������������
	��������)

	\param[in] lambda_eps ��������, � ������� ����� ����������� ��������
	<i>lambda</i>.

	\param[in] virtual_encode ���� <i>true</i> �� ����� �������������
	����������� ����������� (������ ������������� �������, ��� ���������
	����������� ������� � �������� �����).

	\param[in] max_iterations ������������ ���������� �������� ����������
	�������� <i>RD ������� ��������</i>. ���� ��� �������� ����� <i>0</i>,
	���������� ����������� �������� �� ����������.

	\param[in] precise_bpp ���� <i>true</i>, ��� ���������� ��������
	����������� ��������� ����� ������������� ��������� ������� ������ �����
	���������� ��������� ����������� � ������� �������� ��������������� ������
	(��. _real_encode_tight). � ���� ������, ����� ����� ����������� ��� �����
	����������� �������� �����������, ��� ��� ��� ��� ����������� ��������
	_real_encode_tight.

	\return ��������� ����������� ������. �������� ��������, ����� ������
	<i>lambda</i> ����� ��� ���������� ���������. � ���� ������, �������
	������� ����� <i>lambda</i>, ������� ����������� ������������� ��������
	������.

	����� ������������ �������� <i>lambda</i> ������������ ����������
	(������� ����������� �������). ����� ����� ��������� ��������, ����
	������� ����� �������� <i>lambda</i>, ��� ������� ���������� <i>bpp</i>
	������ ��� �� <i>bpp_eps</i> ���������� �� ��������� <i>���</i> �
	�������� ������ �������� �������� <i>lambda</i> ������� ��
	<i>lambda_eps</i>.

	������� �������� <i>lambda</i> ������������� ��������
	�������� <i>bpp</i>:
	\verbatim
	   |_____
	   |     \________
	   |       |      \_________
	bpp+-------+----------------\-----------------------
	   |       |                 \_______
	   |       |                   |     \______________
	---+-------+-------------------+--------------------
	  0|       lambda_min          lambda_max
	\endverbatim

	\note ��� ���������� ������ ���� ������� ����������, ����� ����
	wnode::w � wnode::wq ��������� ������� ���� ���������. ��� �����
	����� ������������ ������� wtree::cheap_load(). ����� ����� �����,
	����� ������ ��������������� ������ ���� ������ ��������� (� ��
	��������� �������������� ���������). ��� ����� ����� ���������������
	����� ���������� ����:
	\code
	// �������� �������, ������� ����� ������������ � ���������� ���
	// �������������
	models_desc_t models;
	// �������� �������� ������� ������ �� ������������� �������
	models = _mk_acoder_smart_models();
	// ����������� �������� ������� � ������ ���������� ��� ���������������
	// ������ � ����������� ��� ������ ������������ ���������� ������ ���
	// ����������� ��� �������������
	_acoder.use(_mk_acoder_models(models));
	\endcode

	\note ���� �������� ������ #LIBWIC_DEBUG, ������� ����� ��������
	����������� ���������� ����������.
*/
encoder::optimize_result_t
encoder::_search_lambda_at_bpp(
							const h_t &bpp, const h_t &bpp_eps,
							const lambda_t &lambda_min,
							const lambda_t &lambda_max,
							const lambda_t &lambda_eps,
							const bool virtual_encode,
							const sz_t &max_iterations,
							const bool precise_bpp)
{
	// �������� �����������
	assert(0 < bpp);
	assert(0 <= lambda_min && lambda_min <= lambda_max);
	assert(0 <= bpp_eps && 0 <= lambda_eps);

	// ��������� ��������� ��� ������
	lambda_t lambda_a	= lambda_min;
	lambda_t lambda_b	= lambda_max;

	// ����������� ������� ������� ��������������� ������
	const acoder::models_t original_models = _acoder.models();

	// ��������� ��������� �����������
	optimize_result_t result;
	result.q			= 0;
	result.lambda		= 0;
	result.j			= 0;
	result.bpp			= 0;
	result.real_encoded = false;

	// ������� ��������
	sz_t iterations = 0;

	// ��������� ��������, ����������� ������������ ������� ��������
	// break
	do
	{
		// ���������� �������� bpp �� ����� ������� ���������. ��� ��� �����
		// ������������ ������� _optimize_wtree_m() ��� ��� ������
		// ��������������� ������ ��� �� ���������
		optimize_result_t result_a = _optimize_wtree(lambda_a, true,
													 virtual_encode,
													 precise_bpp);

		// �������� �� ������������ �������� ���������
		if (result_a.bpp <= (bpp + bpp_eps))
		{
			result = result_a;

			break;
		}

		// �������������� ������� ��������������� ������, ���� ��� ����
		// �������� �������� ����������� ���������
		_restore_spoiled_models(result_a, original_models);

		// ���������� �������� bpp �� ������ ������� ���������
		optimize_result_t result_b = _optimize_wtree(lambda_b, true,
													 virtual_encode,
													 precise_bpp);

		// �������� �� ������������ �������� ���������
		if (result_b.bpp >= (bpp - bpp_eps))
		{
			result = result_b;

			break;
		}

		// ��������: ����� ���������� result �� ����������� �����������������,
		// �� ���� ������ ������������������ ��� �� �������� ��� ��������
		// � �������� ���������� ��� ��� ����� �������� ��������� ����������
		// �����������, � �� ���������!!!

		// ����� ���� ���������� ��� ������ ��������� � ��������� �������,
		// ������� ����� ����� ��������� ���������� ������ ��������������
		// ������� ��������������� ������, ��� ������� ����������, ����� �
		// ���������� result ��� ��������� ��������� ���������� �����������
		result = result_b;

		// ����� ������������ �������� lamda (���������)
		for (;;)
		{
			// ������� �������� bpp ��� �������� ���������
			const lambda_t lambda_c = (lambda_b + lambda_a) / 2;

			// �������������� ������� ��������������� ������, ���� ��� ����
			// �������� �������� ����������� ���������
			_restore_spoiled_models(result, original_models);

			// ����������� ��������� ������
			result = _optimize_wtree(lambda_c, true, virtual_encode,
									 precise_bpp);

			// ���������, ���������� �� ������ �������� �� bpp
			if (bpp_eps >= abs(result.bpp - bpp)) break;

			// ���������, �� �������� �� ����� �� ���������
			if (0 < max_iterations && max_iterations <= iterations) break;

			// ������� ��������� ������
			if (bpp > result.bpp)
				lambda_b = lambda_c;
			else
				lambda_a = lambda_c;

			// ���������, ���������� �� ������ �������� �� lambda
			if (lambda_eps >= abs(lambda_b - lambda_a)) break;

			// ���������� �������� ��������
			++iterations;
		}
	}
	while (false);

	// ����� ���������� ����������
	#ifdef LIBWIC_DEBUG
	if (_dbg_out_stream.good())
	{
		_dbg_out_stream << "[SLAB]: ";
		_dbg_out_stream << "q: " << std::setw(8) << result.q;
		_dbg_out_stream << " lambda: " << std::setw(8) << result.lambda;
		_dbg_out_stream << " j: " << std::setw(8) << result.j;
		_dbg_out_stream << " bpp: " << std::setw(8) << result.bpp;
		_dbg_out_stream << std::endl;
	}
	#endif

	// ����������� ����������� ����������
	return result;
}


/*!	\param[in] q ������������, ������� ����� ����������� ��� �����������
	������� ������� �������������

	\param[in] bpp ������� ������� ������� �� ����������� �����������. ���
	������ ��������� <i>lambda</i> �������� _search_lambda_at_bpp() �����
	������������ ��� �������� � �������� ���������������� ���������.

	\param[in] bpp_eps ���������� ����������� ���������� �������� �������
	������. ��� ������ ��������� <i>lambda</i> �������� _search_lambda_at_bpp()
	����� ������������ ��� �������� � �������� ���������������� ���������.

	\param[in] lambda_eps ��������, � ������� ����� ����������� ��������
	<i>lambda</i>

	\param[out] models �������� ������� ��������������� ������, �������
	���������� ��� ������������ ������������� �����������

	\param[out] d ������������������ ���������� ����������������� �������
	�� ���������. ������ ���� ������, �������� ���������� ������.

	\param[out] deviation ���������� �� ��������� <i>bpp</i>. ����
	�������������� <i>bpp</i> ��������� � �������� ����������� <i>bpp_eps</i>
	���� �������� ������������ � <i>0</i>. ����� �������� ������������ �
	�������� �������� ����� �������� <i>bpp</i> � ���������� � �������� ������
	��������� <i>lambda</i>. ����� �������, ��������� ������������� ��������
	��������� ��������, ��� ���������� <i>bpp</i> ������ ��������� � ��������
	<i>q</i> ���������� ��������� (��� ������� ������� ������� ���������
	������ ��������� <i>lambda</i>). ���� �� �������� ������� �������������
	��������, �� �������� <i>q</i> ���������� ��������� (��� �������� ������
	������� ��������� ������ ��������� <i>lambda</i>).

	\param[in] virtual_encode ���� <i>true</i> �� ����� �������������
	����������� ����������� (������ ������������� �������, ��� ���������
	����������� ������� � �������� �����). ����������� ����������� �������
	�������, �� ��� ������������� ������ ����������� ����������� �������
	������� ������ �� ����������� ������ ������� �����������.

	\param[in] max_iterations ������������ ���������� ��������, �����������
	�������� _search_lambda_at_bpp()

	\param[in] precise_bpp ���� <i>true</i>, ��� ���������� ��������
	����������� ��������� ����� ������������� ��������� ������� ������ �����
	���������� ��������� ����������� � ������� �������� ��������������� ������
	(��. _real_encode_tight). � ���� ������, ����� ����� ����������� ��� �����
	����������� �������� �����������, ��� ��� ��� ��� ����������� ��������
	_real_encode_tight.

	\return ��������� ������������� ������

	�������� <i>lambda</i> ����� �������� � ���������
	<i>[0.05*q*q, 0.20*q*q]</i>.

	\note ��� ���������� ������, ������� ����������, ����� �������� �����
	wnode::w ���� ��������� (��������� � ���� ������������ ��������� �������).

	\sa _search_lambda_at_bpp(), _search_q_lambda_for_bpp()
*/
encoder::optimize_result_t
encoder::_search_q_lambda_for_bpp_iter(
							const q_t &q, const h_t &bpp,
							const h_t &bpp_eps, const lambda_t &lambda_eps,
							models_desc_t &models, w_t &d, h_t &deviation,
							const bool virtual_encode,
							const sz_t &max_iterations,
							const bool precise_bpp)
{
	// ����������� �������
	_wtree.quantize(q);

	// ����������� ������������� ������� ��������������� ������
	models = _setup_acoder_models();

	// ����� ��������� ������ ��������� lambda
	const lambda_t lambda_min = lambda_t(0.05f * q*q);
	const lambda_t lambda_max = lambda_t(0.20f * q*q);

	// ����� ���������� ����������
	#ifdef LIBWIC_DEBUG
	if (_dbg_out_stream.good())
	{
		_dbg_out_stream << "[SQLI]: ";
		_dbg_out_stream << "q: " << std::setw(8) << q;
		_dbg_out_stream << " lambda_min: " << std::setw(8) << lambda_min;
		_dbg_out_stream << " lambda_max: " << std::setw(8) << lambda_max;
		_dbg_out_stream << std::endl;
	}
	#endif

	// ����� ��������� lambda
	const optimize_result_t result =
				_search_lambda_at_bpp(bpp, bpp_eps,
									  lambda_min, lambda_max, lambda_eps,
									  virtual_encode, max_iterations,
									  precise_bpp);

	// ������� ������������������� ���������� (������)
	d = _wtree.distortion_wc<w_t>();

	// ��������� ��������� ����������
	if (result.bpp > bpp + bpp_eps)
		deviation = (bpp - result.bpp);	// q ���������� ��������� (-1)
	else if (result.bpp < bpp - bpp_eps)
		deviation = (bpp - result.bpp);	// q ���������� ��������� (+1)
	else
		deviation = 0;					// q ������������������

	// ����� ���������� ����������
	#ifdef LIBWIC_DEBUG
	if (_dbg_out_stream.good())
	{
		_dbg_out_stream << "[SQLI]: ";
		_dbg_out_stream << "q: " << std::setw(8) << result.q;
		_dbg_out_stream << " lambda: " << std::setw(8) << result.lambda;
		_dbg_out_stream << " j: " << std::setw(8) << result.j;
		_dbg_out_stream << " bpp: " << std::setw(8) << result.bpp;
		_dbg_out_stream << " d: " << std::setw(8) << d;
		_dbg_out_stream << " deviation: " << std::setw(8) << deviation;
		_dbg_out_stream << std::endl;
	}
	#endif

	// ������� ����������
	return result;
}


/*!	\param[in] bpp ����������� ������� (Bits Per Pixel), ��� ����������
	�������� ����� ����������� ��������� <i>q</i> � <i>lambda</i>.

	\param[in] bpp_eps ����������� ��������, c ������� <i>bpp</i> ����������
	� ���������� ������ ���������� <i>q</i> � <i>lambda</i> �����
	��������������� ���������.

	\param[in] q_min ������ ������� ��������� ������ (�����������
	��������)

	\param[in] q_max ������� ������� ��������� ������ (������������
	��������)

	\param[in] q_eps ����������� ����������� ����������� ������������
	<i>q</i> ��� ������� �������� <i>RD ������� ��������</i> ����������
	(��� ������������� ��������� <i>lambda</i>).

	\param[in] lambda_eps ��������, � ������� ����� ����������� ��������
	<i>lambda</i>

	\param[out] models �������� ������� ��������������� ������, �������
	���������� ��� ������������ ������������� �����������

	\param[in] virtual_encode ���� <i>true</i> �� ����� �������������
	����������� ����������� (������ ������������� �������, ��� ���������
	����������� ������� � �������� �����). ����������� ����������� �������
	�������, �� ��� ������������� ������ ����������� ����������� �������
	������� ������ �� ����������� ������ ������� �����������.

	\param[in] precise_bpp ���� <i>true</i>, ��� ���������� ��������
	����������� ��������� ����� ������������� ��������� ������� ������ �����
	���������� ��������� ����������� � ������� �������� ��������������� ������
	(��. _real_encode_tight). � ���� ������, ����� ����� ����������� ��� �����
	����������� �������� �����������, ��� ��� ��� ��� ����������� ��������
	_real_encode_tight.

	������� ���������� ����� �������� ������� ��� ������� ��������� <i>q</i>,
	� ��� ������� ��������� <i>lambda</i> ���������� �������
	_search_q_lambda_for_bpp_iter().

	\sa _search_q_lambda_for_bpp_iter(), _search_q_lambda_for_bpp()
*/
encoder::optimize_result_t
encoder::_search_q_lambda_for_bpp(
							const h_t &bpp, const h_t &bpp_eps,
							const q_t &q_min, const q_t &q_max,
							const q_t &q_eps, const lambda_t &lambda_eps,
							models_desc_t &models,
							const bool virtual_encode,
							const bool precise_bpp)
{
	// ������������ ���������� �������� ��� ������� ��������� lambda
	static const sz_t max_iterations	= 0;

	// ������������ �������� �������
	static const q_t factor_b = q_t((3.0 - sqrt(5.0)) / 2.0);
    static const q_t factor_c = q_t((sqrt(5.0) - 1.0) / 2.0);

	// ������� � ������ ������� ������ ��������� q
	q_t q_a	= q_min;
	q_t q_d	= q_max;

	// ���������� �������� � ������ ���� ������
	q_t q_b = q_a + factor_b * (q_d - q_a);
	q_t q_c = q_a + factor_c * (q_d - q_a);

	// ������ ��������� lambda � ������ �����
	w_t dw_b		= 0;
	h_t deviation_b	= 0;
	optimize_result_t result_b =
					_search_q_lambda_for_bpp_iter(
										q_b, bpp, bpp_eps, lambda_eps, models,
										dw_b, deviation_b, virtual_encode,
										max_iterations, precise_bpp);

	// ������ ��������� lambda �� ������ �����
	w_t dw_c		= 0;
	h_t deviation_c	= 0;
	optimize_result_t result_c =
					_search_q_lambda_for_bpp_iter(
										q_c, bpp, bpp_eps, lambda_eps, models,
										dw_c, deviation_c, virtual_encode,
										max_iterations, precise_bpp);

	// ���������� ���������� ������ ��������� lambda
	h_t deviation				= deviation_c;
	optimize_result_t result	= result_c;

	// ������� ��������� ������ ������� �������� �������
	while (q_eps < abs(q_a - q_d))
    {
        if (deviation > 0 || (0 == deviation && dw_b <= dw_c))
        {
            q_d = q_c;
            q_c = q_b;
            dw_c = dw_b;
			q_b = q_a + factor_b * (q_d - q_a);
			// a         b    c    d
			// a    b    c    d

			result = result_b =
					_search_q_lambda_for_bpp_iter(
										q_b, bpp, bpp_eps, lambda_eps, models,
										dw_b, deviation_b, virtual_encode,
										max_iterations, precise_bpp);
			deviation = deviation_b;
        }
        else
        {
            q_a = q_b;
            q_b = q_c;
            dw_b = dw_c;
			q_c = q_a + factor_c * (q_d - q_a);
			// a    b    c         d
			//      a    b    c    d

			result = result_c =
					_search_q_lambda_for_bpp_iter(
										q_c, bpp, bpp_eps, lambda_eps, models,
										dw_c, deviation_b, virtual_encode,
										max_iterations, precise_bpp);
			deviation = deviation_c;
        }
    }

	return result;
}


/*!	\return ������� ������� ������� �� ����������� ������� �������
	�������������

	������ ������� ���������� ������� ��������� ������� ��������������� ������
	� �� �������� ��.
*/
h_t encoder::_real_encode()
{
	#ifdef LIBWIC_USE_DBG_SURFACE
	#endif

	// ����������� ����� ������
	_acoder.encode_start();

	_encode_wtree();

	_acoder.encode_stop();

	// ������� � ������� ������� ������� ������
	return _calc_encoded_bpp();
}


/*!	\param[out] desc �������� ������� ��������������� ������, �������
	���� ������������� ��� �����������

	\param[in] double_path ���� <i>true</i> ����������� ����� ������������
	� ��� �������. ��� ������ ������� ����������� ������������ ���������
	�������. ������ ��� ����������� ���������� � ���� ������ (��� �������)
	��� ��� �������� �������� ������������� �� ����� ������ � ������.
	������� ��� � ���, ��� �� ����� ����������� ��� ��� �� �������� ���������
	�������� ���� ������������� � ��� ��������� �������� �����������
	���������.

	\return ������� ������� ������� �� ����������� ������� �������
	�������������

	\attention ������� ����� ��������� ������������ ��� ������� ��� ���
	��� �������� ������, ������������ �������������� �������.
*/
h_t encoder::_real_encode_tight(models_desc_t &desc, const bool double_path)
{
	// ������� ������ ������ ���� ������� ��� ������� ��������� ���
	assert(double_path);

	// ���������� ������� �������, ���� ����������
	if (double_path)
	{
		_setup_acoder_models();

		_real_encode();
	}

	// ����������� � ��������� ������� ��������������� ������
	desc = _setup_acoder_post_models();

	return _real_encode();
}


/*!	\param[in] including_tunes ���� ���� �������� ����� <i>true</i> �
	������� <i>bpp</i> ����� ������ �� �������� ������ �������������� ������,
	����������� ��� �������������� �����������, ����������� � ���������
	tunes_t
	\return ������� ���������� ���, ������������� �� ����������� ������
	�������
*/
h_t encoder::_calc_encoded_bpp(const bool including_tunes)
{
	const sz_t data_sz = coder().encoded_sz()
						 + ((including_tunes)? sizeof(tunes_t): 0);

	return h_t(data_sz * BITS_PER_BYTE) / h_t(_wtree.nodes_count());
}


/*!
*/
void encoder::_test_wc_136_0()
{
	assert(0 != _wtree.at(136, 0).wc);
}



}	// end of namespace wic
