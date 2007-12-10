/*!	\file     wictool.cpp
	\version  0.0.1
	\author   mice, ICQ: 332-292-380, mailto:wonder.mice@gmail.com
	\brief    ������� ���� ������� wictool

	\todo     ����� �������� ������� ���� wictool.cpp
*/


///////////////////////////////////////////////////////////////////////////////
// include

// standard C++ library headers
#include <limits.h>
#include <math.h>
#include <string.h>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>

// wavelet filters headers
#include <wavelets/cdf97/cdf97.h>

// imgs library headers
#include <imgs/img_rgb.h>
#include <imgs/bmp_file.h>

// libwic headers
#include <wic/libwic/encoder.h>
#include <wic/wicfilters/wavelets.h>

// wictool headers
#include "wictool.h"


///////////////////////////////////////////////////////////////////////////////
// public constant definitions

/*!
*/
const char WIC_SIGNATURE[3]	= {'W', 'I', 'C'};

/*!
*/
const char WIC_VERSION		= '1';


///////////////////////////////////////////////////////////////////////////////
// public function definitions

/*!	\param[in] argc ���������� ���������� ��������� � ������� <i>args</i>
	\param[in] args ������ ��������� ����������
	\param[out] channel ���������� ����������� �������� �����
	\param[out] path ���������� ����������� ���� � �����
	\param[in,out] err ��������� �� ����� ��� ������ ������. ���� �����
	<i>0</i> ����� ����������� ����������� ����� ��� ������ ������.
	\return ���������� �������������� ���������� � ������ ������, �����
	<i>-1</i>.

	������:
	\verbatim
	[-c|--channel {r|g|b|x}] <file1.bmp>[:{x|r|g|b|f}]
	\endverbatim

	�������� <i>'x'</i> ���������� �� ���������, ���� ����� ���� �� ������.
*/
int get_bmp_file_and_channel(const int argc, const char *const *const args,
							 char &channel, std::string &path,
							 std::ostream *const err)
{
	// ���������� ����� ��� ������ ������
	std::ostream &out = (0 == err)? std::cerr: (*err);

	// ����� �������� ���������
	int arg_i = 0;

	// ��������, ��� ������� ����������
	if (argc <= arg_i)
	{
		out << "Error: not enough arguments for image" << std::endl;
		return -1;
	}

	// ��������� ��������� ������
	if (0 == strcmp("-c", args[arg_i]) ||
		0 == strcmp("--channel", args[arg_i]))
	{
		// ������� � ���������� ��������� � ��������, ��� ���������� ���
		// �� ������� � �����
		if (argc <= ++arg_i)
		{
			out << "Error: not enough arguments for [-c|--channel]"
				<< std::endl;
			return -1;
		}

		// �������� ������ �������������� ������
		if (1 != strlen(args[arg_i]))
		{
			out << "Error: channel identifier must be a single character"
				<< std::endl;
			return -1;
		}

		// ��������� �������-�������������� ������
		const char ch = args[arg_i][0];

		// ���������� ����������
		channel = ch;
		++arg_i;
	}
	else channel = 'x';

	// ��������, ��� ������� ����������
	if (argc <= arg_i)
	{
		out << "Error: not enough arguments" << std::endl;
		return -1;
	}

	path = args[arg_i++];

	// �������� �� ������� ���� file.bmp:r, ��� r - �������� �����
	if (2 < path.size() && ':' == path[path.size() - 2])
	{
		channel = path[path.size() - 1];

		path = path.substr(0, path.size() - 2);
	}

	// ������� ���������� �������������� ����������
	return (arg_i);
}


/*!	\param[in] path ���� � <i>bmp</i> ����� �����������
	\param[in] channel ���������� �������� �����
	\param[in,out] err ��������� �� ����� ��� ������ ������. ���� �����
	<i>0</i> ����� ����������� ����������� ����� ��� ������ ������.
	\return ���������, ���������� ������ ��������� ������
*/
bmp_channel_bits get_bmp_channel_bits(const std::string &path,
									  const char channel,
									  std::ostream *const err)
{
	// ���������� ����� ��� ������ ������
	std::ostream &out = (0 == err)? std::cerr: (*err);

	// �������� "�������" ����������
	bmp_channel_bits result;
	memset(&result, 0, sizeof(result));

	result.channel = channel;

	// �������� ����������� �� �����
	imgs::img_rgb rgb;
	if (0 != imgs::bmp_read(rgb, path))
	{
		out << "Error: can't load image from \""
			<< path << "\"" << std::endl;

		return result;
	}

	// �������������� ������ 24-������ �����������
	if (24 != rgb.bpp())
	{
		out << "Error: only 24bit bitmaps supported" << std::endl;

		return result;
	}

	// ����������� ������ ������ ��� �������� ��������� ������
	const int pixels_count = rgb.w() * rgb.h();
	result.w = rgb.w();
	result.h = rgb.h();

	switch (channel)
	{
		case 'r':
		case 'g':
		case 'b':
			result.sz = pixels_count;
			break;

		case 'x':
			result.sz = 3 * pixels_count;
			break;

		default:
			out << "Error: invalid channel \'" << channel
				<< "\'" << std::endl;
			return result;
	}

	using imgs::img_rgb::rgb24_t;

	const rgb24_t *const rgb_bits = (rgb24_t *)rgb.bits();

	// ��������� ������� ������ ������
	result.data = new unsigned char[result.sz];

	// ����������� ������ ��������� ������
	switch (channel)
	{
		case 'r':
			for (int i = 0; pixels_count > i; ++i)
			{
				result.data[i] = rgb_bits[i].r;
			}
			break;

		case 'g':
			for (int i = 0; pixels_count > i; ++i)
			{
				result.data[i] = rgb_bits[i].g;
			}
			break;

		case 'b':
			for (int i = 0; pixels_count > i; ++i)
			{
				result.data[i] = rgb_bits[i].b;
			}
			break;

		case 'x':
			for (int i = 0, j = pixels_count, k = 2*pixels_count;
				 pixels_count > i;
				 ++i, ++j, ++k)
			{
				result.data[i] = rgb_bits[i].r;
				result.data[j] = rgb_bits[i].g;
				result.data[k] = rgb_bits[i].b;
			}
			break;

		default:
			out << "Error: invalid channel \'" << channel
				<< "\'" << std::endl;
			return result;
	}

	return result;
}


/*!
*/
int set_bmp_channel_bits(const std::string &path, const bmp_channel_bits &bits,
						 const char &usr_channel,
						 std::ostream *const err)
{
	// ���������� ����� ��� ������ ������
	std::ostream &out = (0 == err)? std::cerr: (*err);

	// �������� ������� ����������
	if (!is_channel_bits_good(bits, err)) return -1;

	// ������������� �����������
	imgs::img_rgb rgb;

	// �������� �����������, ���� ������������ ����� �������� ��������� �����
	if ('x' != usr_channel && 'x' != bits.channel)
	{
		if (0 != imgs::bmp_read(rgb, path))
		{
			out << "Warning: can't load image \"" << path << "\"" << std::endl;
		}
	}

	rgb.reset(bits.w, bits.h, 24);

	using imgs::img_rgb::rgb24_t;

	rgb24_t *const rgb_bits = (rgb24_t *)rgb.bits();

	const int pixels_count = rgb.w() * rgb.h();

	// �������� ������� ��������� ������
	const unsigned int required_sz = ('x' != bits.channel)? pixels_count
														  : pixels_count * 3;
	if (bits.sz != required_sz)
	{
		out << "Error: invalid channel size " << bits.sz << " for channel \""
			<< bits.channel << "\" (required size is: " << required_sz <<
			")" << std::endl;
		return -1;
	}

	// ����������� ������ ��������� ������
	switch (bits.channel)
	{
		case 'r':
		case 'g':
		case 'b':
			switch (usr_channel)
			{
				case 'x':
					for (int i = 0; pixels_count > i; ++i)
					{
						rgb_bits[i].r = bits.data[i];
						rgb_bits[i].g = bits.data[i];
						rgb_bits[i].b = bits.data[i];
					}
					break;

				case 'r':
					for (int i = 0; pixels_count > i; ++i)
					{
						rgb_bits[i].r = bits.data[i];
					}
					break;

				case 'g':
					for (int i = 0; pixels_count > i; ++i)
					{
						rgb_bits[i].g = bits.data[i];
					}
					break;

				case 'b':
					for (int i = 0; pixels_count > i; ++i)
					{
						rgb_bits[i].b = bits.data[i];
					}
					break;

				default:
					out << "Error: invalid user channel \'" << usr_channel
						<< "\'" << std::endl;
					return -1;
			}
			break;

		case 'x':
			if ('x' != usr_channel)
			{
				out << "Error: can't write source channel \"" << bits.channel
					<< "\" in user channel \"" << usr_channel << "\""
					<< std::endl;
				return -1;
			}

			for (int i = 0, j = pixels_count, k = 2*pixels_count;
				 pixels_count > i;
				 ++i, ++j, ++k)
			{
				rgb_bits[i].r = bits.data[i];
				rgb_bits[i].g = bits.data[j];
				rgb_bits[i].b = bits.data[k];
			}
			break;

		default:
			out << "Error: invalid channel \'" << bits.channel
				<< "\'" << std::endl;
			return -1;
	}

	if (0 != imgs::bmp_write(rgb, path))
	{
		out << "Error: can't save image file \"" << path << "\"" << std::endl;
		return -1;
	}

	return 0;
}


/*!	\param[in] bits ���������, ������ ��������������� � ������� �����
	�����������.
*/
void free_bmp_channel_bits(bmp_channel_bits &bits)
{
	if (0 != bits.data)
	{
		delete[] bits.data;
		bits.data = 0;
	}
}


/*!	\param[in] bits ����������� ���������
	\return true ���� ��������� �������� �������� ������
*/
bool is_channel_bits_good(const bmp_channel_bits &bits, std::ostream *const err)
{
	// ���������� ����� ��� ������ ������
	std::ostream &out = (0 == err)? std::cerr: (*err);

	// �������� ������� ������
	if (0 == bits.data || 0 >= bits.sz)
	{
		out << "Error: color channel data must not be empty or null"
			<< std::endl;

		return false;
	}

	// �������� ������������ �������������� ��������
	if (0 >= bits.w || 0 >= bits.h)
	{
		out << "Error: dimensions " << bits.w << "x" << bits.h << " are invalid"
			<< std::endl;

		return false;
	}

	// �������� ����� ������
	if ('\0' == bits.channel)
	{
		out << "Error: char(" << int(bits.channel) << ") is invalid "
			<< "channel name" << std::endl;

		return false;
	}

	// �������� �������� �������
	return true;
}


/*!	\param[in] argc ���������� ���������� ��������� � ������� <i>args</i>
	\param[in] args ������ ��������� ����������
	\param[out] result ��������� � ������� ����� ������� ��������� ���������
	\param[in,out] err ��������� �� ����� ��� ������ ������. ���� �����
	<i>0</i> ����� ����������� ����������� ����� ��� ������ ������.
	\return ���������� �������������� ���������� � ������ ������, �����
	<i>-1</i>.

	������:
	\verbatim
	[-c|--channel {r|g|b|x}] <file1.bmp> [-c|--channel {r|g|b|x}] <file2.bmp>
	\endverbatim

	�������� <i>'x'</i> ���������� �� ���������, ���� ����� ���� �� ������.

	\sa get_bmp_file_and_channel()
*/
int get_bmp_file_diff_src(const int argc, const char *const *const args,
						  bmp_file_diff_src_t &result,
						  std::ostream *const err)
{
	// ���������� ����� ��� ������ ������
	std::ostream &out = (0 == err)? std::cerr: (*err);

	// ����� �������� ���������
	int arg_i = 0;

	// ���������� �������������� ���������� ��� ��� ������
	int u = 0;

	// ��������� ���������� � ������ �����
	u = get_bmp_file_and_channel(argc - arg_i, args + arg_i,
								 result.channel1, result.file1,
								 err);
	if (0 > u) return -1;

	arg_i += u;

	// ��������� ���������� � ������ �����
	u = get_bmp_file_and_channel(argc - arg_i, args + arg_i,
								 result.channel2, result.file2,
								 err);
	if (0 > u) return -1;

	arg_i += u;

	return arg_i;
}


/*!	\param[in] bits1 ������ �������� �����
	\param[in] bits2 ������ �������� �����
	\param[in,out] err ��������� �� ����� ��� ������ ������. ���� �����
	<i>0</i> ����� ����������� ����������� ����� ��� ������ ������.
	\return <i>true</i> ���� ��������� �������� ������� ����������, �����
	<i>false</i>

	����� ��������� ���� �������� ������� ���� �����������, ����������:
	- ��� �������� ������ ������ ����� �������� ����������
	- �������������� ������� ������ ���������
	- ������� ������ ��� �������� ���������� ������� ������ ���������
*/
bool is_diff_allowed(const bmp_channel_bits &bits1,
					 const bmp_channel_bits &bits2,
					 std::ostream *const err)
{
	// ���������� ����� ��� ������ ������
	std::ostream &out = (0 == err)? std::cerr: (*err);

	// �������� ������� �������� ����������
	if (0 == bits1.data || 0 == bits2.data) 
	{
		out << "Error: color channels data must not be empty or null"
			<< std::endl;

		return false;
	}

	// �������� �������������� ��������
	if (bits1.w != bits2.w || bits1.h != bits2.h)
	{
		out << "Error: color channels dimensions must be equal"
			<< std::endl;

		return false;
	}

	// �������� ���������� ������
	if (bits1.sz != bits2.sz)
	{
		out << "Error: color channels size must be equal"
			<< std::endl;

		return false;
	}

	return true;
}


/*!	\param[in] bits1 ������ �������� �����
	\param[in] bits2 ������ �������� �����
	\param[in,out] err ��������� �� ����� ��� ������ ������. ���� �����
	<i>0</i> ����� ����������� ����������� ����� ��� ������ ������.
	\return ������������ �������� <i>PSNR</i> ��� <i>-1.0</i> � ������
	������. �������� <i>0.0</i> ������������ ���� ����������� ���������.
*/
double psnr(const bmp_channel_bits &bits1, const bmp_channel_bits &bits2,
			std::ostream *const err)
{
	// ���������� ����� ��� ������ ������
	std::ostream &out = (0 == err)? std::cerr: (*err);

	// �������� ������������� ���������
	if (!is_diff_allowed(bits1, bits2, err)) return -1.0;

	// ������� psnr
	double psnr_value = 0.0;

	for (unsigned int i = 0; bits1.sz > i; ++i)
	{
		const int dp = int(bits1.data[i]) - int(bits2.data[i]);
		psnr_value += (dp*dp);
	}

	if (0 == psnr_value) return (0.0);

	psnr_value /= bits1.sz;

	psnr_value = 20 * log10(double(UCHAR_MAX) / sqrt(psnr_value));

	return psnr_value;
}


/*!	\param[in] diff_src �������� ������ ��� ���������
	\param[in,out] err ��������� �� ����� ��� ������ ������. ���� �����
	<i>0</i> ����� ����������� ����������� ����� ��� ������ ������.
	\return <i>0</i> ���� �� ��������� ������� ������, ����� <i>-1</i>
*/
int psnr(const bmp_file_diff_src_t &diff_src,
		 std::ostream *const err)
{
	// ���������� ����� ��� ������ ������
	std::ostream &out = (0 == err)? std::cerr: (*err);

	// ��������� �������� �������
	bmp_channel_bits bits1 = get_bmp_channel_bits(diff_src.file1,
												  diff_src.channel1);
	bmp_channel_bits bits2 = get_bmp_channel_bits(diff_src.file2,
												  diff_src.channel2);

	// ������� PSNR
	const double psnr_value = psnr(bits1, bits2);

	// ����� ���������� �� �����
	std::cout << "psnr: " << psnr_value << "db" << std::endl;

	// ������������ ��������
	free_bmp_channel_bits(bits1);
	free_bmp_channel_bits(bits2);

	// ������� ����������
	return ((0 <= psnr_value)? 0: -1);
}


/*!	\param[in] bits �������� �����
	\return ���������� ���������� (max, min, avg) ��������� ������
*/
simple_stat_t calc_simple_stat(const bmp_channel_bits &bits,
							   std::ostream *const err)
{
	// ���������� ����� ��� ������ ������
	std::ostream &out = (0 == err)? std::cerr: (*err);

	// ������� ����������
	simple_stat_t simple_stat;
	simple_stat.min = simple_stat.max = simple_stat.avg = 0;

	// �������� ������� ������
	if (0 == bits.data || 0 >= bits.sz)
	{
		out << "Error: color channel data must not be empty or null"
			<< std::endl;

		return simple_stat;
	}

	// ��������� �������� ����������
	simple_stat.min = simple_stat.max = bits.data[0];
	double avg = bits.data[0];

	// ������ �� ����� ������
	for (unsigned int i = 1; bits.sz > i; ++i)
	{
		if (bits.data[i] < simple_stat.min)
			simple_stat.min = bits.data[i];
		else if (bits.data[i] > simple_stat.max)
			simple_stat.max = bits.data[i];

		avg += bits.data[i];
	}

	// ������� ����������
	simple_stat.avg = (unsigned char)(avg / bits.sz);

	return simple_stat;
}


/*!	\param[in] bits1 ������ �������� �����
	\param[in] bits2 ������ �������� �����
	\param[in,out] err ��������� �� ����� ��� ������ ������. ���� �����
	<i>0</i> ����� ����������� ����������� ����� ��� ������ ������.
	\return <i>0</i> � ������ ������, ����� <i>-1</i>
*/
int stat(const bmp_channel_bits &bits1, const bmp_channel_bits &bits2,
		 std::ostream *const out, std::ostream *const err)
{
	// ���������� ����� ��� ������ ����������
	std::ostream &sout = (0 == out)? std::cout: (*out);

	// ���������� ����� ��� ������ ������
	std::ostream &serr = (0 == err)? std::cerr: (*err);

	// ������� ���������� ����������
	const simple_stat_t simple_stat1 = calc_simple_stat(bits1, err);
	const simple_stat_t simple_stat2 = calc_simple_stat(bits2, err);

	// ��������� ������
	static const int hw	= 6;
	static const int fw	= 16;

	// ����� ��������� �� �����
	sout << std::setw(hw)
		 << "####" << std::setw(fw) << "image 1" << std::setw(fw) << "image 2"
		 << std::setw(fw) << "delta" << std::endl;

	sout << std::setw(hw)
		<< "dims:" << std::setw(fw) << bits1.w << "x" << bits1.h
		<< std::setw(fw) << bits2.w << "x" << bits2.h << std::endl;

	sout << std::setw(hw)
		 << "min:" << std::setw(fw) << int(simple_stat1.min)
		 << std::setw(fw) << int(simple_stat2.min) << std::setw(fw)
		 << abs(int(simple_stat2.min) - int(simple_stat1.min))
		 << std::endl;

	sout << std::setw(hw)
		 << "max:" << std::setw(fw) << int(simple_stat1.max)
		 << std::setw(fw) << int(simple_stat2.max) << std::setw(fw)
		 << abs(int(simple_stat2.max) - int(simple_stat1.max))
		 << std::endl;

	sout << std::setw(hw)
		 << "avg:" << std::setw(fw) << int(simple_stat1.avg)
		 << std::setw(fw) << int(simple_stat2.avg) << std::setw(fw)
		 << abs(int(simple_stat2.avg) - int(simple_stat1.avg))
		 << std::endl;

	sout << std::endl;

	const double psnr_value = psnr(bits1, bits2, err);

	sout << std::setw(hw) << "psnr:" << std::setw(fw)
		 << psnr_value << " dB" << std::endl;

	// �� � �������
	return 0;
}


/*!	\param[in] diff_src �������� ������ ��� ���������
	\param[in,out] err ��������� �� ����� ��� ������ ������. ���� �����
	<i>0</i> ����� ����������� ����������� ����� ��� ������ ������.
	\return <i>0</i> ���� �� ��������� ������� ������, ����� <i>-1</i>
*/
int stat(const bmp_file_diff_src_t &diff_src,
		 std::ostream *const err)
{
	// ���������� ����� ��� ������ ������
	std::ostream &out = (0 == err)? std::cerr: (*err);

	// ��������� �������� �������
	bmp_channel_bits bits1 = get_bmp_channel_bits(diff_src.file1,
												  diff_src.channel1);
	bmp_channel_bits bits2 = get_bmp_channel_bits(diff_src.file2,
												  diff_src.channel2);

	// ����� ���������� �� �����
	const int stat_result = stat(bits1, bits2);

	// ������������ ��������
	free_bmp_channel_bits(bits1);
	free_bmp_channel_bits(bits2);

	return stat_result;
}


/*!	\param[in] argc ���������� ���������� ��������� � ������� <i>args</i>
	\param[in] args ������ ��������� ����������
	\param[out] result �������������� ��� (��������) ������� ��������������
	\param[in,out] err ��������� �� ����� ��� ������ ������. ���� �����
	<i>0</i> ����� ����������� ����������� ����� ��� ������ ������.
	\return ���������� �������������� ���������� � ������ ������, �����
	<i>-1</i>.

	������:
	\verbatim
	[-f|--filter <name>]
	\endverbatim

	��������� �������� ��������� <i>name</i>:
	- cdf97 (������������ �� ���������)
	- Haar
	- Daub4
	- Daub6
	- Daub8
	- Antonini
	- AntoniniSynthesis
	- Villa1810
	- Villa1810Synthesis
	- Adelson
	- Brislawn
	- BrislawnSynthesis
	- Brislawn2
	- Villa1
	- Villa2
	- Villa3
	- Villa4
	- Villa5
	- Villa6
	- Odegard
	- Petuhov1
	- Petuhov2
*/
int get_wavelet_filter(const int argc, const char *const *const args,
					   std::string &result, std::ostream *const err)
{
	// ���������� ����� ��� ������ ������
	std::ostream &out = (0 == err)? std::cerr: (*err);

	// �������� �� ���������
	static const std::string &def_result = "cdf97";

	// ����� �������� ���������
	int arg_i = 0;

	// ��������, ��� ������� ����������
	if (argc <= arg_i)
	{
		result = def_result;

		return arg_i;
	}

	// ��������� �������� ��������������
	if (0 == strcmp("-f", args[arg_i]) ||
		0 == strcmp("--filter", args[arg_i]))
	{
		// ������� � ���������� ��������� � ��������, ��� ��������� ���
		// �� ������� � �����
		if (argc <= ++arg_i)
		{
			out << "Error: not enough arguments for [-f|--filter]"
				<< std::endl;
			return -1;
		}

		// ���������� ����������
		result = args[arg_i++];
	}
	else
	{
		result = def_result;
	}

	return arg_i;
}

/*!	\param[in] w Width of image
	\param[in] h Height of image
	\param[in] steps Number of steps in wavelet transform
	\param[in,out] err ��������� �� ����� ��� ������ ������. ���� �����
	<i>0</i> ����� ����������� ����������� ����� ��� ������ ������.
	\return <i>true</i> ���� ������� ������������� ������� ��������������
*/
bool check_transform_dims(const unsigned int w, const unsigned int h,
						  const unsigned int steps,
						  std::ostream *const err)
{
	// ���������� ����� ��� ������ ������
	std::ostream &out = (0 == err)? std::cerr: (*err);

	// ������� ������ ���� ������ ����� �����
	const unsigned int factor = (1 << steps);

	if (0 != (w % factor) || 0 != (h % factor))
	{
		out << "Error: dimensions " << w << "x" << h << " must be divisible "
			<< "on " << factor << " when using " << steps << " steps transform"
			<< std::endl;
		return false;
	}

	return true;
}


/*!	\param[in] filter ��� ������������� ������� �������
	\param[in] steps ���������� ������� ������� ��������������
	\param[in] bits �������� �����, ��� ������� ���������� ���������
	������� ��������������
	\param[in,out] err ��������� �� ����� ��� ������ ������. ���� �����
	<i>0</i> ����� ����������� ����������� ����� ��� ������ ������.
	\return ���������� �������������� ���������� � ������ ������, �����
	<i>-1</i>.
	\return ������ ������� �������������
*/
spectre_t forward_transform(const std::string &filter,
							const unsigned int &steps,
							const bmp_channel_bits &bits,
							std::ostream *const err)
{
	// ���������� ����� ��� ������ ������
	std::ostream &out = (0 == err)? std::cerr: (*err);

	// ������ ������
	spectre_t spectre;
	spectre.data	= 0;
	spectre.sz		= 0;
	spectre.h		= 0;
	spectre.w		= 0;

	// �������� ������� ������
	if (filter.empty())
	{
		out << "Error: filter name \"" << filter << "\" is invalid"
			<< std::endl;

		return spectre;
	}

	if (!is_channel_bits_good(bits, err))
	{
		return spectre;
	}

	// ����������� �������������� �������� �������
	spectre.w		= bits.w;
	if ('x' == bits.channel)
		spectre.h	= 3*bits.h;
	else
		spectre.h	= bits.h;

	if (!check_transform_dims(spectre.w, spectre.h, steps, err))
	{
		return spectre;
	}

	// ��������� ������ ��� ��������� ��������������
	spectre.sz		= bits.sz;
	spectre.data	= new wic::w_t[spectre.sz];

	// ���������� ��������������
	if (0 == filter.compare("cdf97"))
	{
		for (unsigned int i = 0; spectre.sz > i; ++i)
		{
			spectre.data[i] = wic::w_t(bits.data[i]);
		}

		wt2d_cdf97_fwd(spectre.data, spectre.h, spectre.w, steps);
	}
	else
	{
		out << "Error: unknown filter \"" << filter << "\"" << std::endl;
	}

	// ������� ����������
	return spectre;
}


/*!	\param[in] spectre ������� ������
	\param[in] filter ��� ������������� �������
	\param[in] channel ��� ��������� ������
	\param[in,out] err ��������� �� ����� ��� ������ ������. ���� �����
	<i>0</i> ����� ����������� ����������� ����� ��� ������ ������.
	\return ���������� �������������� ���������� � ������ ������, �����
	<i>-1</i>.
	\return �������� ������
*/
bmp_channel_bits inverse_transform(const wic::wtree &spectre,
								   const std::string &filter,
								   const char &channel,
								   std::ostream *const err)
{
	// ���������� ����� ��� ������ ������
	std::ostream &out = (0 == err)? std::cerr: (*err);

	// ������ �����������
	bmp_channel_bits bits;
	bits.data		= 0;
	bits.sz			= 0;
	bits.w			= 0;
	bits.h			= 0;
	bits.channel	= '\0';

	// ���������� ������������� � �������
	const unsigned int coefs_count = spectre.width() * spectre.height();

	// ���������� ��������� ��������������
	if (0 == filter.compare("cdf97"))
	{
		// ��������� ������ ��� ������
		float *const coefs = new float[coefs_count];

		// ���������� ������� �������������
		spectre.save<wic::wnode::member_w>(coefs);

		// ���������� ��������� ������� ��������������
		wt2d_cdf97_inv(coefs, spectre.height(), spectre.width(),
					   spectre.sb().lvls());

		// �������� ��������� ������
		bits.w			= spectre.width();
		if ('x' == channel)
		{
			if (0 != spectre.height() % 3)
			{
				out << "Error: " << spectre.height() << " is invalid specter "
					<< "height for \"x\" channel" << std::endl;
				delete[] coefs;
				return bits;
			}

			bits.h		= spectre.height() / 3;
		}
		else
		{
			bits.h		= spectre.height();
		}

		bits.channel	= channel;
		bits.sz			= coefs_count;
		bits.data		= new unsigned char[coefs_count];

		// �������������� � ������������ �������� �����������
		for (unsigned int i = 0; coefs_count > i; ++i)
		{
			if (0.0f >= coefs[i])
				bits.data[i] = 0;
			else if (255.0f <= coefs[i])
				bits.data[i] = 255;
			else
				bits.data[i] = (unsigned char)coefs[i];
		}

		// ������������ ���������� ������
		delete[] coefs;
	}
	else
	{
		out << "Error: unknown filter \"" << filter << "\"" << std::endl;
	}

	// ������� ����������
	return bits;
}


/*!	\param[in] spectre ������ ��� ��������
	\return <i>true</i> ���� ������ ���������� � �������� ��������� ������
*/
bool is_spectre_good(const spectre_t &spectre, std::ostream *const err)
{
	// ���������� ����� ��� ������ ������
	std::ostream &out = (0 == err)? std::cerr: (*err);

	// �������� ������� ������
	if (0 == spectre.data || 0 >= spectre.sz)
	{
		out << "Error: spectre data data must not be empty or null"
			<< std::endl;

		return false;
	}

	// �������� ������������ �������������� ��������
	if (0 >= spectre.w || 0 >= spectre.h)
	{
		out << "Error: dimensions " << spectre.w << "x" << spectre.h
			 << " are invalid for wavelet specter" << std::endl;

		return false;
	}

	// �������� �������� �������
	return true;
}


/*!	\param[in] spectre ������ ������� ��������������, ������, ����������
	������� ������� ����������
*/
void free_spectre(spectre_t &spectre)
{
	if (0 != spectre.data) delete[] spectre.data;

	spectre.data	= 0;
	spectre.sz		= 0;
	spectre.w		= 0;
	spectre.h		= 0;
}


/*!
*/
void wic_optimize_tree_callback(const wic::p_t &root,
								void *const param)
{
	/*
	if (0 == param) return;
	wic::encoder *const encoder = (wic::encoder *)param;

	static int i = 0;

	if (0 == root.getx() && 0 == root.gety()) i = 0;

	if (32 == ++i)
	{
		#ifndef LOAD_ARCODER_MODELS
		encoder->coder().save_models(wic::acoder::encoder_models,
									 "models.bin");
		#endif
	}
	*/
}


/*!
*/
void wic_optimize_callback(const wic::encoder::optimize_result_t &result,
						   void *const param)
{
	if (0 == param)
	{
		std::cout << "." << std::flush;
		/*
		std::cout << "q: " << result.q << "; bpp: " << result.bpp
				  << "; lambda: " << result.lambda << std::endl;
		*/

		return;
	}
}


/*!	\param[in] argc ���������� ���������� ��������� � ������� <i>args</i>
	\param[in] args ������ ��������� ����������
	\param[out] result �������������� ��� (��������) ������ ������
	\param[in,out] err ��������� �� ����� ��� ������ ������. ���� �����
	<i>0</i> ����� ����������� ����������� ����� ��� ������ ������.
	\return ���������� �������������� ���������� � ������ ������, �����
	<i>-1</i>.

	������:
	\verbatim
	[-m|--method <name>]
	\endverbatim

	��������� �������� ��������� <i>name</i>:
	- manual (������������ �� ���������)
	- fixed_lambda
	- fixed_q
	- fixed_bpp
*/
int get_encode_method(const int argc, const char *const *const args,
					  std::string &result, std::ostream *const err)
{
	// ���������� ����� ��� ������ ������
	std::ostream &out = (0 == err)? std::cerr: (*err);

	// �������� �� ���������
	static const std::string &def_result = "manual";

	// ����� �������� ���������
	int arg_i = 0;

	// ��������, ��� ������� ����������
	if (argc <= arg_i)
	{
		result = def_result;

		return arg_i;
	}

	// ��������� �������� ��������������
	if (0 == strcmp("-m", args[arg_i]) ||
		0 == strcmp("--method", args[arg_i]))
	{
		// ������� � ���������� ��������� � ��������, ��� ��������� ���
		// �� ������� � �����
		if (argc <= ++arg_i)
		{
			out << "Error: not enough arguments for [-m|--method]"
				<< std::endl;
			return -1;
		}

		// ���������� ����������
		result = args[arg_i++];
	}
	else
	{
		result = def_result;
	}

	return arg_i;
}


/*!	\param[in] argc ���������� ���������� ��������� � ������� <i>args</i>
	\param[in] args ������ ��������� ����������
	\param[out] params ��������� �����������, ���������� �� ��������� ������
	\param[in,out] err ��������� �� ����� ��� ������ ������. ���� �����
	<i>0</i> ����� ����������� ����������� ����� ��� ������ ������.
	\return ���������� �������������� ���������� � ������ ������, �����
	<i>-1</i>.

	������:
	\verbatim
	[-q|--quantizer value] [-l|--lambda value] [-b|--bpp bpp] [-s|--steps n]
	[-u|--quality quality]
	\endverbatim
*/
int get_encode_params(const int argc, const char *const *const args,
					  encode_params_t &params, std::ostream *const err)
{
	// ���������� ����� ��� ������ ������
	std::ostream &out = (0 == err)? std::cerr: (*err);

	// ��������� �������� �� ���������
	params.q		= 16;
	params.lambda	= 32;
	params.bpp		= 0.5;
	params.steps	= 5;

	// ����� �������� ���������
	int arg_i = 0;

	// ������ ���������� �� ��������� ������
	while (arg_i < argc)
	{
		if (0 == strcmp("-q", args[arg_i]) ||
			0 == strcmp("--quantizer", args[arg_i]))
		{
			// [-q|--quantizer value]
			if (argc <= ++arg_i)
			{
				out << "Error: not enough arguments for [-q|--quantizer]"
					<< std::endl;
				return -1;
			}

			// ��������� �������� ������������
			std::stringstream q_as_string(args[arg_i++]);
			q_as_string >> params.q;
		}
		else if (0 == strcmp("-l", args[arg_i]) ||
				 0 == strcmp("--lambda", args[arg_i]))
		{
			// [-l|--lambda value]
			if (argc <= ++arg_i)
			{
				out << "Error: not enough arguments for [-l|--lambda]"
					<< std::endl;
				return -1;
			}

			// ��������� �������� �������
			std::stringstream lambda_as_string(args[arg_i++]);
			lambda_as_string >> params.lambda;
		}
		else if (0 == strcmp("-s", args[arg_i]) ||
				 0 == strcmp("--steps", args[arg_i]))
		{
			// [-s|--steps n]
			if (argc <= ++arg_i)
			{
				out << "Error: not enough arguments for [-s|--steps]"
					<< std::endl;
				return -1;
			}

			// ��������� ���������� ����� ��������������
			std::stringstream steps_as_string(args[arg_i++]);
			steps_as_string >> params.steps;
		}
		else if (0 == strcmp("-b", args[arg_i]) ||
				 0 == strcmp("--bpp", args[arg_i]))
		{
			// [-b|--bpp value]
			if (argc <= ++arg_i)
			{
				out << "Error: not enough arguments for [-b|--bpp]"
					<< std::endl;
				return -1;
			}

			// ��������� �������� �������
			std::stringstream bpp_as_string(args[arg_i++]);
			bpp_as_string >> params.bpp;
		}
		else if (0 == strcmp("-u", args[arg_i]) ||
				 0 == strcmp("--quality", args[arg_i]))
		{
			// [-u|--quality quality]
			if (argc <= ++arg_i)
			{
				out << "Error: not enough arguments for [-u|--quality]"
					<< std::endl;
				return -1;
			}

			// ��������� ���������� ��������
			std::stringstream steps_as_string(args[arg_i++]);
			double quality = 0;
			steps_as_string >> quality;

			// �������������� ���������� �������� � lambda
			params.lambda = wic::encoder::quality_to_lambda(quality);
		}
		else
		{
			// ����� �� ����������� ����� - ������������, ��� ���������
			// ���������
			break;
		}
	}

	return arg_i;
}


/*!	\param[in] argc ���������� ���������� ��������� � ������� <i>args</i>
	\param[in] args ������ ��������� ����������
	\param[out] wic_file ���� � ������� �����
	\param[in,out] err ��������� �� ����� ��� ������ ������. ���� �����
	<i>0</i> ����� ����������� ����������� ����� ��� ������ ������.
	\return ���������� �������������� ���������� � ������ ������, �����
	<i>-1</i>.
*/
int get_wic_file(const int argc, const char *const *const args,
				 std::string &wic_file, std::ostream *const err)
{
	// ���������� ����� ��� ������ ������
	std::ostream &out = (0 == err)? std::cerr: (*err);

	// ����� �������� ���������
	int arg_i = 0;

	if (argc <= arg_i)
	{
		out << "Error: compressed file name not specified" << std::endl;
		return -1;
	}

	wic_file = args[arg_i++];

	return arg_i;
}


/*!	\param[in] argc ���������� ���������� ��������� � ������� <i>args</i>
	\param[in] args ������ ��������� ����������
	\param[out] filter ������������ ������� ������
	\param[out] method ����� �����������
	\param[out] params ��������� �����������
	\param[out] source ���� � ����������� �����
	\param[out] channel ��� ����������� ������
	\param[out] output ���� � ��������������� �����
	\param[in,out] err ��������� �� ����� ��� ������ ������. ���� �����
	<i>0</i> ����� ����������� ����������� ����� ��� ������ ������.
	\return ���������� �������������� ���������� � ������ ������, �����
	<i>-1</i>.
*/
int get_encode_options(const int argc, const char *const *const args,
					   std::string &filter, std::string &method,
					   encode_params_t &params,
					   std::string &source, char &channel,
					   std::string &output,
					   std::ostream *const err)
{
	// ���������� ����� ��� ������ ������
	std::ostream &out = (0 == err)? std::cerr: (*err);

	// ����� �������� ���������
	int argk = 0;

	// ���������� �������������� ����������
	int argx = -1;

	// ������������ ������� ��������������
	argx = get_wavelet_filter(argc - argk, args + argk, filter, err);

	if (0 > argx) return -1;
	else argk += argx;

	// ����� �����������
	argx = get_encode_method(argc - argk, args + argk, method, err);

	if (0 > argx) return -1;
	else argk += argx;

	// ��������� �����������
	argx = get_encode_params(argc - argk, args + argk, params, err);

	if (0 > argx) return -1;
	else argk += argx;

	// ���� � ����� � ������������ � ������������� ������
	argx = get_bmp_file_and_channel(argc - argk, args + argk,
									channel, source, err);

	if (0 > argx) return -1;
	else argk += argx;

	// ��������� ���� � ��������������� �����
	argx = get_wic_file(argc - argk, args + argk, output, err);

	if (0 > argx) return -1;
	else argk += argx;

	// ������� ���������� �������������� ����������
	return argk;
}


/*!	\param[in] data_sz ������ �������������� ������
	\param[in] filter ��� ������� �������
	\param[in] steps ���������� ����� ������� ��������������
	\param[in] channel �������� �����
	\param[in] w ������ �����������
	\param[in] h ������ �����������
	\param[in,out] output ����� ��� ������
	\param[in,out] err ��������� �� ����� ��� ������ ������. ���� �����
	<i>0</i> ����� ����������� ����������� ����� ��� ������ ������.
	\return ���������� �������������� ���������� � ������ ������, �����
	<i>-1</i>.
	\return ���������� ���������� ���� � ������ ������, ����� <i>-1</i>
*/
int write_wic_header(const unsigned int &data_sz, const std::string &filter,
					 const unsigned int &steps, const char &channel,
					 const unsigned int &w, const unsigned int &h,
					 std::ostream &output, std::ostream *const err)
{
	// ���������� ����� ��� ������ ������
	std::ostream &out = (0 == err)? std::cerr: (*err);

	wic_header_t header;

	// ���������� ���������
	memcpy(header.signature, WIC_SIGNATURE, sizeof(WIC_SIGNATURE));

	// ���������� ������
	header.version = WIC_VERSION;

	// ���������� ������ ����� �������
	if (0xFF < filter.size())
	{
		out << "Error: filter name \"" << filter << "\" too long" << std::endl;
		return -1;
	}

	header.filter_sz = (unsigned char)(filter.size());

	// ���������� ������ ����� �������
	if (0xFF < steps)
	{
		out << "Error: " << steps << " is too many steps" << std::endl;
		return -1;
	}

	header.steps = (unsigned char)steps;

	// ���������� ��������� ������
	header.channel = channel;

	// ���������� ������ � ������ �����������
	header.w = w;
	header.h = h;

	// ������ �������������� ������
	header.data_sz = data_sz;

	// ������ � ����� �������� ����� ���������
	output.write((const char *)&header, sizeof(header));

	// ������ ����� �������
	output.write((const char *)filter.c_str(), std::streamsize(filter.size()));

	// �������� ��������� ���������� �������� ������
	if (!output.good()) return -1;

	// ���������� ���������� ������
	return int(sizeof(header) + filter.size());
}


/*!	\param[in,out] input ����� �����
	\param[out] filter ��� ��������������� �������
	\param[out] steps ���������� ������� ������� ��������������
	\param[out] channel ��� ��������� ������
	\param[out] w ������ �����������
	\param[out] h ������ �����������
	\param[in,out] err ��������� �� ����� ��� ������ ������. ���� �����
	<i>0</i> ����� ����������� ����������� ����� ��� ������ ������.
	\return ���������� �������������� ���������� � ������ ������, �����
	<i>-1</i>.
	\return ���������� ����������� ���� � ������ ������, ����� <i>-1</i>
*/
int read_wic_header(std::istream &input,
					unsigned int &data_sz, std::string &filter,
					unsigned int &steps, char &channel,
					unsigned int &w, unsigned int &h,
					std::ostream *const err)
{
	// ���������� ����� ��� ������ ������
	std::ostream &out = (0 == err)? std::cerr: (*err);

	wic_header_t header;

	// ������ ���������
	if (!input.read((char *)&header, sizeof(header)).good())
	{
		out << "Error: can't read file header" << std::endl;
		return -1;
	}

	// �������� ���������
	if (0 != memcmp(header.signature, WIC_SIGNATURE, sizeof(WIC_SIGNATURE)))
	{
		out << "Error: not a wiccoder compressed file" << std::endl;
		return -1;
	}

	// �������� ������
	if (WIC_VERSION != header.version)
	{
		out << "Error: invalid wiccoder version" << std::endl;
		return -1;
	}

	// ������ �������� ������� �������
	char filter_buf[0xFF];

	if (!input.read(filter_buf, header.filter_sz).good())
	{
		out << "Error: can't read filter name" << std::endl;
		return -1;
	}

	// ���������� �����
	filter.assign(filter_buf, header.filter_sz);
	data_sz	= header.data_sz;
	steps	= (unsigned int)header.steps;
	channel	= header.channel;
	w		= header.w;
	h		= header.h;

	// ���������� ����������� ������
	return int(sizeof(header) + filter.size());
}


/*!	\param[in,out] ����� � ������� ����� �������� ������
*/
void crazy_ones(std::ostream &out)
{
	out <<
	"Here's to the crazy ones, the misfits, the rebels, the troublemakers, the"
	<< std::endl <<
	"round pegs in the square holes... the ones who see things differently - "
	<< std::endl <<
	"they're not fond of rules... You can quote them, disagree with them, "
	<< std::endl <<
	"glorify or vilify them, but the only thing you can't do is ignore them"
	<< std::endl <<
	"because they change things... they push the human race forward, and while"
	<< std::endl <<
	"some may see them as the crazy ones, we see genius, because the ones who"
	<< std::endl <<
	"are crazy enough to think that they can change the world, are the ones who"
	<< std::endl <<
	"do."
	<< std::endl << std::endl <<
	"Apple Computers"
	<< std::endl << std::endl;
}


/*!	\param[in,out] ����� � ������� ����� �������� ���������� �
	�������
*/
void about(std::ostream &out)
{
	out << std::endl;
	out << "wictool - console wrapper for libwic (image compression algorithm "
		<< "based on" << std::endl
		<< "          encoding of tree-arranged wavelet coefficients)"
		<< std::endl;
	out << std::endl;
	out << "(C) 2007, MIEE DSP Team:" << std::endl
		<< "Sergei V. Umnyashkin" << std::endl
		<< "Dmitri M. Koplovich" << std::endl
		<< "Andrei S. Pokrovskiy" << std::endl
		<< "Andrei A. Alexandrov" << std::endl;
	out << std::endl;
	out << "Great thanks to:" << std::endl
		<< "Roman A. Kulikov" << std::endl;
	out << std::endl;
	out << "Moscow Institute of Electronic Technology"
		<< "(www.miet.ru)" << std::endl;
}


/*!	\param[in,out] out ����� � ������� ����� �������� ���������� ��
	����������� ��������
	\return ������ <i>-1</i>
*/
int usage(std::ostream &out)
{
	out << "Usage is:" << std::endl << std::endl;

	out << "wictool -a|--about" << std::endl;
	out << "wictool -h|--help" << std::endl;
	out << "wictool -e|--encode [<filter>] [<method>] [<params>] <image> "
		<< "<file.wic>" << std::endl;
	out << "wictool -d|--decode <file.wic> <image>" << std::endl;
	out << "wictool -p|--psnr <image1> <image2>" << std::endl;
	out << "wictool -s|--stat <image1> <image2>" << std::endl;
	out << std::endl;
	out << "<image>:  [-c|--channel {x|r|g|b}] <file.bmp>[:{x|r|g|b}]"
		<< std::endl;
	out << "<filter>: [-f|--filter name]" << std::endl;
	out << "<method>: [-m|--method name]" << std::endl;
	out << "<params>: [-q|--quantizer quantizer] [-l|--lambda lambda] "
		<< "[-b|--bpp bpp] " << std::endl
		<< "          [-s|--steps n] [-u|--quality k]" << std::endl;
	out << std::endl;
	out << "Filters are: cdf97 (default)" /*", Haar, Daub4, Daub6, Daub8"*/
		<< std::endl;
	out << "Methods are: manual (default), fixed_lambda, fixed_q, fixed_bpp"
		<< std::endl << std::endl;
	out << "More options:" << std::endl
		<< "          [-v|--verbose] will enable verbose output" << std::endl;

	/*
	1. lambda + q
	2. lambda
	3. bpp
	-e|--encode [-m|--mode {manual|fixed_bpp|fixed_lambda}]
		{-q|--quantizer <quantizer>}
		{-l|--lambda <lambda>}
		{-b|--bpp <bpp>}
	*/

	return -1;
}


//!	\brief ����� �����
/*!	\param[in] argc ���������� ���������� ��������� ������
	\param[in] args ��������� ��������� ������
*/
int main(int argc, char **args)
{
	// ���������� �������������� ���������� ��������� ������
	int argk = 1;

	// �������� �� ������� ����������
	if (argk >= argc) return usage(std::cout);

	// ���� "��������������"
	bool verbose = false;

	// ������� ���� ��������� ���������� ��������� ������
	while (argk < argc)
	{
		// ����� ������ ������
		const std::string mode = args[argk++];

		if (0 == mode.compare("-!") || 0 == mode.compare("--crazy"))
		{
			crazy_ones(std::cout);
		}
		else if (0 == mode.compare("-v") || 0 == mode.compare("--verbose"))
		{
			verbose = true;
			// std::cout << "Verbose output enabled." << std::endl;
		}
		else if (0 == mode.compare("-a") || 0 == mode.compare("--about"))
		{
			about(std::cout);
		}
		else if (0 == mode.compare("-h") || 0 == mode.compare("--help"))
		{
			usage(std::cout);
		}
		else if (0 == mode.compare("-p") || 0 == mode.compare("--psnr"))
		{
			// -p | --psnr
			bmp_file_diff_src_t res;

			const int argx = get_bmp_file_diff_src(argc - argk , args + argk,
												   res);

			if (0 > argx) return usage(std::cout);
			else argk += argx;

			std::cout << "image 1: " << res.file1 << ":" << res.channel1
					  << std::endl;
			std::cout << "image 2: " << res.file2 << ":" << res.channel2
					  << std::endl;

			if (0 != psnr(res)) return -1;
		}
		else if (0 == mode.compare("-s") || 0 == mode.compare("--stat"))
		{
			// -s | --stat
			bmp_file_diff_src_t res;

			const int argx = get_bmp_file_diff_src(argc - argk , args + argk,
												   res);

			if (0 > argx) return usage(std::cout);
			else argk += argx;

			std::cout << "image 1: " << res.file1 << ":" << res.channel1
					  << std::endl;
			std::cout << "image 2: " << res.file2 << ":" << res.channel2
					  << std::endl;

			if (0 != stat(res)) return -1;
		}
		else if (0 == mode.compare("-e") || 0 == mode.compare("--encode"))
		{
			// ��������� ����� ��������� ������
			std::string		filter;
			std::string		method;
			encode_params_t	params;
			std::string		source;
			char			channel = '\0';
			std::string		output;

			const int argx = get_encode_options(argc - argk, args + argk,
												filter, method, params,
												source, channel, output);

			if (0 > argx) return usage(std::cout);
			else argk += argx;

			if (verbose)
			{
				std::cout << "Filter: " << filter << std::endl;
				std::cout << "Method: " << method << std::endl;
				std::cout << "Steps:  " << params.steps << std::endl;
				std::cout << "q:      " << params.q << std::endl;
				std::cout << "Lambda: " << params.lambda << std::endl;
				std::cout << "Source: \"" << source << ":"
						  << channel << "\"" << std::endl;
				std::cout << "Output: \"" << output << "\"" << std::endl;
			}

			// �������� �����������
			bmp_channel_bits img_bits = get_bmp_channel_bits(source, channel);

			if (!is_channel_bits_good(img_bits)) return -1;

			// ���������� ������� ������� ��������������
			spectre_t spectre = forward_transform(filter, params.steps,
												  img_bits);

			// ������������ ����������� (����� �������� ������ �� ��������)
			free_bmp_channel_bits(img_bits);

			// �������� ���������� ���������� ������� ��������������
			if (!is_spectre_good(spectre)) return -1;

			// ����������� �����������
			if (verbose)
			{
				std::cout << std::endl << "Encoding.." << std::flush;
			}

			wic::encoder encoder(spectre.w, spectre.h, params.steps);
			encoder.optimize_callback(wic_optimize_callback, 0);
			encoder.optimize_tree_callback(wic_optimize_tree_callback,
										   &encoder);

			wic::encoder::tunes_t tunes;
			wic::encoder::enc_result_t enc_result;

			if (0 == method.compare("manual"))
			{
				enc_result = encoder.encode(spectre.data,
											params.q, params.lambda, tunes);
			}
			else if (0 == method.compare("fixed_lambda"))
			{
				enc_result = encoder.encode_fixed_lambda(spectre.data,
														 params.lambda, tunes);
			}
			else if (0 == method.compare("fixed_q"))
			{
				enc_result = encoder.encode_fixed_q(spectre.data, params.q,
													params.bpp, tunes);
			}
			else if (0 == method.compare("fixed_bpp"))
			{
				enc_result = encoder.encode_fixed_bpp(spectre.data, params.bpp,
													  tunes);
			}
			else
			{
				std::cerr << "Error: \"" << method << "\" is invalid method"
						  << std::endl;
				free_spectre(spectre);
				return -1;
			}

			// ������������ ������� (����� �������� ������ � ���������)
			free_spectre(spectre);

			if (verbose)
			{
				std::cout << " done." << std::endl << std::endl;
				std::cout << "Optimization:" << std::endl;
				std::cout << "    q:      " << enc_result.optimization.q
						  << std::endl;
				std::cout << "    lambda: " << enc_result.optimization.lambda
						  << std::endl;
				std::cout << "    j:      " << enc_result.optimization.j
						  << std::endl;
				std::cout << "    bpp:    " << enc_result.optimization.bpp
						  << std::endl;
				std::cout << "Output:" << std::endl;
				std::cout << "    bpp:    " << enc_result.bpp << std::endl;
				std::cout << "    size:   "
						  << float(encoder.coder().encoded_sz())/1024.0f
						  << " kb" << std::endl;
			}

			// ������ � ����
			std::ofstream outstream(output.c_str(),
									std::ios_base::binary|std::ios_base::out);

			const int hdr_sz = write_wic_header(encoder.coder().encoded_sz(),
												filter, params.steps, channel,
												encoder.spectrum().width(),
												encoder.spectrum().height(),
												outstream);

			outstream.write((const char *)&tunes, sizeof(tunes));

			outstream.write((const char *)encoder.coder().buffer(),
							encoder.coder().encoded_sz());
		}
		else if (0 == mode.compare("-d") || 0 == mode.compare("--decode"))
		{
			// ��������� ����� ��������� ������
			int argx = -1;

			// ��������� ���� � ��������������� �����
			std::string wic_file;
			argx = get_wic_file(argc - argk, args + argk, wic_file);

			if (0 > argx) return usage(std::cout);
			else argk += argx;

			// ��������� ���� � ��������������� ����� � ����� ��������� ������
			char		usr_channel	= '\0';
			std::string	output;
			argx = get_bmp_file_and_channel(argc - argk, args + argk,
											usr_channel, output);

			if (0 > argx) return usage(std::cout);
			else argk += argx;

			// �������� �������� �����
			std::ifstream instream(wic_file.c_str(),
								   std::ios_base::binary|std::ios_base::in);

			std::string		filter;
			unsigned int	data_sz;
			unsigned int	steps		= 0;
			char			src_channel	= '\0';
			unsigned int	w			= 0;
			unsigned int	h			= 0;

			if (0 >= read_wic_header(instream, data_sz, filter,
									 steps, src_channel, w, h))
			{
				return -1;
			}

			wic::encoder::tunes_t tunes;
			instream.read((char *)&tunes, sizeof(tunes));

			if (verbose)
			{
				std::cout << "Filter: " << filter << std::endl;
				std::cout << "Steps:  " << steps << std::endl;
				std::cout << "q:      " << tunes.q << std::endl;
				std::cout << "Input:  \"" << wic_file << "\"" << std::endl;
				std::cout << "Output: \"" << output << ":" << usr_channel
						  << "\"" << std::endl;
			}

			wic::byte_t *const data = new wic::byte_t[data_sz];
			instream.read((char *)data, data_sz);

			if (!instream.good())
			{
				std::cerr << "Error: input file corrupted" << std::endl;
				delete[] data;
				return -1;
			}

			if (verbose)
			{
				std::cout << std::endl << "Decoding... " << std::flush;
			}

			wic::encoder decoder(w, h, steps);
			decoder.decode(data, data_sz, tunes);
			delete[] data;

			if (verbose)
			{
				std::cout << "done." << std::endl;// << std::endl;
			}

			bmp_channel_bits bits = inverse_transform(decoder.spectrum(),
													  filter, src_channel);

			if (0 != set_bmp_channel_bits(output, bits, usr_channel))
			{
				return -1;
			}
		}
		else
		{
			std::cout << "Error: unknown option \"" << mode << "\""
					  << std::endl;

			return usage(std::cout);
		}

		if (verbose)
		{
			std::cout << "--------------------------------------------------";
			std::cout << std::endl;
		}
	}

	return 0;
}
