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
#include <iostream>
#include <iomanip>

#include <imgs/img_rgb.h>
#include <imgs/bmp_file.h>

// wictool headers
#include "wictool.h"


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
	[-c|--channel {r|g|b|x}] <file1.bmp>
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
		out << "Error: not enough arguments" << std::endl;
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
	[-w|--wavelet <name>]
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
int get_wavelet_transform(const int argc, const char *const *const args,
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
	if (0 == strcmp("-w", args[arg_i]) ||
		0 == strcmp("--wavelet", args[arg_i]))
	{
		// ������� � ���������� ��������� � ��������, ��� ��������� ���
		// �� ������� � �����
		if (argc <= ++arg_i)
		{
			out << "Error: not enough arguments for [-w|--wavelet]"
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


/*!	\param[in,out] ����� � ������� ����� �������� ���������� �
	�������
*/
void about(std::ostream &out)
{
	out << std::endl;
	out << "wictool - console wrapper for libwic (image compression algorithm"
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
	out << "wictool -e|--encode [-l|--lambda] [-b|-bpp] <image> <file.wic>" << std::endl;
	out << "wictool -d|--decode <file.wic> <image>" << std::endl;
	out << "wictool -p|--psnr <image1> <image2>" << std::endl;
	out << "wictool -s|--stat <image1> <image2>" << std::endl;
	out << "\t<image>: [-c|--channel {x|r|g|b|f}] <file.bmp>[:{x|r|g|b|f}]"
		<< std::endl;

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



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////



// standard C++ library headers
#include <assert.h>
#include <string>
#include <fstream>
#include <iostream>

// external library header
#include <imgs/img_rgb.h>
#include <imgs/img_yuv.h>
#include <imgs/psnr.h>
#include <imgs/bmp_file.h>
#include <imgs/bmp_dump.h>
#include <wavelets/cdf97/cdf97.h>

// libwic headers
#include <wic/libwic/encoder.h>



///////////////////////////////////////////////////////////////////////////////
// types declaration

#pragma pack(push, 1)
struct compressed_hdr_t {
	unsigned short version;
	unsigned int width;
	unsigned int height;
	unsigned short lvls;
	unsigned int data_sz;
};
#pragma pack(pop)



///////////////////////////////////////////////////////////////////////////////
// constants definition

//!	\brief ������ �������
static const unsigned short WIC_VERSION		= 1;



///////////////////////////////////////////////////////////////////////////////
// functions declaration

//!	\brief ������� ����������
wic::encoder *mk_encoder(const std::string &src_path,
						 const int lvls,
						 const int channel = 0);

//!	\brief  ������������ ������ ��������������� � ����
int encode(const wic::encoder &encoder,
		   const wic::encoder::header_t &header,
		   const std::string &result_path);

//!	\brief ������������ ������������� �����
int decode(const std::string &compressed_path,
		   const std::string &result_path);



///////////////////////////////////////////////////////////////////////////////
// functions definitions




double psnr(const std::string &file1, const std::string &file2) {
	imgs::img_rgb rgb1;
	imgs::img_rgb rgb2;

	if (0 != imgs::bmp_read(rgb1, file1)) return -1.0;
	if (0 != imgs::bmp_read(rgb2, file2)) return -1.0;

	imgs::img_yuv yuv1;
	imgs::img_yuv yuv2;

	// imgs::rgb2yuv(rgb1, yuv1, imgs::img_yuv::YUV_FRMT_444);
	yuv1.reset(rgb1.w(), rgb1.h(), imgs::img_yuv::YUV_FRMT_444);
	for (int i = 0; yuv1.w() * yuv1.h() > i; ++i) {
		yuv1.y()[i] = ((imgs::img_rgb::rgb24_t *)rgb1.bits())[i].r;
	}
	// imgs::rgb2yuv(rgb2, yuv2, imgs::img_yuv::YUV_FRMT_444);
	yuv2.reset(rgb2.w(), rgb2.h(), imgs::img_yuv::YUV_FRMT_444);
	for (int i = 0; yuv2.w() * yuv2.h() > i; ++i) {
		yuv2.y()[i] = ((imgs::img_rgb::rgb24_t *)rgb2.bits())[i].r;
	}

	int y_sz = yuv1.w() * yuv1.h();

	return imgs::psnr<imgs::img_yuv::sample_t>::ptr(yuv1.y(), yuv2.y(), y_sz);
}


/*!	\param[in] argc Number of arguments
	\param[in] args Arguments list
	\return <i>-1</i>, �����������, ��� ��������� ��������� �� ���������
*/
int usage1(int argc, char **args)
{
	std::cout << "Usage:" << std::endl << std::endl;

	std::cout << "Encoding:" << std::endl;
	std::cout << "wictool -e -m manual -q <quantizer> -l <lambda> lena.bmp lena.wic" << std::endl;
	std::cout << "wictool -e -m fixed_bpp -b <bpp> lena.bmp lena.wic" << std::endl;
	std::cout << "wictool -e -m fixed_lambda -l <lambda> lena.bmp lena.wic" << std::endl;
	std::cout << std::endl;
	std::cout << "Decoding:" << std::endl;
	std::cout << "wictool -d lena.wic lena.bmp" << std::endl;
	std::cout << std::endl;
	std::cout << "Measuring compression quality (PSNR, dB):" << std::endl;
	std::cout << "wictool -p lena_source.bmp lena_decoded.bmp" << std::endl;
	std::cout << std::endl;

	return (-1);
}


/*!	\param[in] src_path ���� � ����� � �������� ������������
	\param[in] lvls ���������� ������� ����������
	\param[in] channel ������������ �������� �����:
	- 0 Red
	- 1 Green
	- 2 Blue
	\return ������ ������ wic::encoder
*/
wic::encoder *mk_encoder(const std::string &src_path,
						 const int lvls,
						 const int channel)
{
	// load source image
	imgs::img_rgb rgb_image;
	if (0 != imgs::bmp_read(rgb_image, src_path.c_str()))
	{
		std::cout << "Error: can't read bitmap file \"" << src_path
				  << "\"" << std::endl;
		return 0;
	}

	// test levels factor of dimensions
	if (rgb_image.w() != rgb_image.h())
	{
		std::cout << "Error: only square pictures supported" << std::endl;
		return 0;
	}

	{
		const unsigned int factor = (1 << lvls);

		if (0 != (rgb_image.w() % factor) &&
			0 != (rgb_image.h() % factor))
		{
			std::cout << "Error: " << rgb_image.w() << "x"
					  << rgb_image.h() << " is invalid image "
					  << "dimensions for " << lvls << " levels"
					  << std::endl;
			return 0;
		}
	}

	// allocate some memory and copy converted to float image there
	const wic::sz_t image_pixels_count = rgb_image.w() * rgb_image.h();

	float *const image_wt = new float[image_pixels_count];

	imgs::img_rgb::rgb24_t *const rgb_image_bits =
		(imgs::img_rgb::rgb24_t *)(rgb_image.bits());

	if (0 == channel)
	{
		for (wic::sz_t i = 0; image_pixels_count > i; ++i) {
			image_wt[i] = rgb_image_bits[i].r;
		}
	}
	else if (1 == channel)
	{
		for (wic::sz_t i = 0; image_pixels_count > i; ++i) {
			image_wt[i] = rgb_image_bits[i].g;
		}
	}
	else
	{
		for (wic::sz_t i = 0; image_pixels_count > i; ++i) {
			image_wt[i] = rgb_image_bits[i].b;
		}
	}

	// do wavelet transform
	wt2d_cdf97_fwd(image_wt, rgb_image.h(), rgb_image.w(), lvls);

	// �������� ������
	wic::encoder *const encoder = new wic::encoder(rgb_image.w(),
												   rgb_image.h(),
												   lvls);

	// load spectrum
	encoder->spectrum().cheap_load(image_wt);

	// free spectrum
	delete[] image_wt;

	// return encoder
	return encoder;
}


/*!	\param[in] encoder ������ ������ �wic::encoder, ������� ���
	�������� �������������� ������
	\param[in] header ��������� �������������� ������
	\param[in] result_path ���� � ����� � ������� ����� ���������
	�������������� ������
*/
int encode(const wic::encoder &encoder,
		   const wic::encoder::header_t &header,
		   const std::string &result_path)
{
	// open file for writing
	std::ofstream compressed(result_path.c_str(),
							 std::ios::out|std::ios::binary);

	if (!compressed.good())
	{
		std::cout << "Error: can't open file \"" << result_path << "\" for "
				  << "writing" << std::endl;
		return -1;
	}

	// create headers
	compressed_hdr_t compressed_hdr;
	compressed_hdr.version	= WIC_VERSION;
	compressed_hdr.width	= encoder.spectrum().width();
	compressed_hdr.height	= encoder.spectrum().height();
	compressed_hdr.lvls		= encoder.spectrum().lvls();
	compressed_hdr.data_sz	= encoder.coder().encoded_sz() + sizeof(header);

	// write data
	compressed.write((char *)&compressed_hdr, sizeof(compressed_hdr));
	compressed.write((char *)&header, sizeof(header));
	compressed.write((char *)(encoder.coder().buffer()),
					 encoder.coder().encoded_sz());

	if (!compressed.good())
	{
		std::cout << "Error: can't write to file \"" << result_path
				  << "\"";
		return -1;
	}

	std::cout << "Total file size: " << compressed.tellp() << " bytes ("
			  << (compressed.tellp() / 1024.0) << " kbytes)" << std::endl;

	return 0;
}


/*!	\param[in] compressed_path ���� � ��������������� �����
	\param[in] result_path ���� � ��������������� �����
	\return 0 ���� ���� ����������� �������
*/
int decode(const std::string &compressed_path,
		   const std::string &result_path)
{
	std::cout << "decoding..." << std::endl;

	// open compressed file
	std::ifstream compressed(compressed_path.c_str(),
							 std::ios::in|std::ios::binary);
	if (!compressed.good())
	{
		std::cout << "Error: can't open compressed file \""
				  << compressed_path << "\" for reading" << std::endl;
		return -1;
	}

	// read compressed header information
	compressed_hdr_t compressed_hdr;
	compressed.read((char *)&compressed_hdr, sizeof(compressed_hdr));
	if (!compressed.good())
	{
		std::cout << "Error: can't read compressed header information"
				  << "from compressed file" << std::endl;
		return -1;
	}

	std::cout << "image: " << compressed_hdr.width << "x"
			  << compressed_hdr.height << ", "<< compressed_hdr.lvls
			  << " levels" << std::endl;

	// test wictool version
	if (WIC_VERSION != compressed_hdr.version)
	{
		std::cout << "Error: compressed file of version \""
				  << compressed_hdr.version << "\" can't be decompressed "
					 "with wictool of version \"" << WIC_VERSION << "\""
				  << std::endl;
		return -1;
	}

	std::cout << "file version: " << compressed_hdr.version << std::endl;

	// test levels factor of dimensions
	{
		const unsigned int factor = (1 << compressed_hdr.lvls);

		if (0 != (compressed_hdr.width % factor) &&
			0 != (compressed_hdr.height % factor))
		{
			std::cout << "Error: " << compressed_hdr.width << "x"
					  << compressed_hdr.height << " is invalid image "
					  << "dimensions for " << compressed_hdr.lvls << " levels"
					  << std::endl;
			return -1;
		}
	}

	// create decoder
	wic::encoder encoder(compressed_hdr.width, compressed_hdr.height,
						 compressed_hdr.lvls);

	// allocate buffer for compressed data
	char *const data = new char[compressed_hdr.data_sz];

	if (0 == data)
	{
		std::cout << "Error: can't allocate memory to read file" << std::endl;
		return -1;
	}

	// read compressed data
	compressed.read(data, compressed_hdr.data_sz);
	if (!compressed.good())
	{
		std::cout << "Error: can't read compressed data from file"
				  << std::endl;
		delete[] data;
		return -1;
	}

	// get header
	const wic::encoder::header_t &header = 
						*((wic::encoder::header_t *)data);

	std::cout << "q: " << header.q << std::endl;

	// decode
	encoder.decode((wic::byte_t *)(data) + sizeof(header),
				   compressed_hdr.data_sz - sizeof(header),
				   header);

	delete[] data;

	// allocate buffer for spectrum
	wic::w_t *const image_wt = new wic::w_t[encoder.spectrum().nodes_count()];
	if (0 == image_wt)
	{
		std::cout << "Error: can't allocate buffer for spectrum" << std::endl;
		return -1;
	}

	// save decompressed data in local spectrum
	encoder.spectrum().save<wic::wnode::member_w>(image_wt);

	// do inverse wavelet transform
	wt2d_cdf97_inv(image_wt, compressed_hdr.height, compressed_hdr.width,
				   compressed_hdr.lvls);

	imgs::img_rgb rgb_image;
	rgb_image.reset(compressed_hdr.height, compressed_hdr.width, 24);

	imgs::img_rgb::rgb24_t *const rgb_image_bits =
		(imgs::img_rgb::rgb24_t *)(rgb_image.bits());

	for (wic::sz_t i = 0; encoder.spectrum().nodes_count() > i; ++i)
	{
		const wic::w_t &wval = image_wt[i];

		const unsigned char val	= ((0 >= wval)? 0:
										((0xFF <= wval)? 0xFF:
												(unsigned char)wval));

		rgb_image_bits[i].r = val;
		rgb_image_bits[i].g = val;
		rgb_image_bits[i].b = val;
	}

	delete[] image_wt;

	// write bmp to resulting file
	if (0 != imgs::bmp_write(rgb_image, result_path))
	{
		std::cout << "Error: can't write decompressed bitmap to \""
				  << result_path << "\"" << std::endl;
		return -1;
	}

	return 0;
}



///////////////////////////////////////////////////////////////////////////////
// main() function definition
int main(int argc, char **args)
{
	// ���������� �������������� ���������� ��������� ������
	int argk = 1;

	// �������� �� ������� ����������
	if (argk >= argc) return usage(std::cout);

	while (argk < argc)
	{
		// ����� ������ ������
		const std::string mode = args[argk++];

		if (0 == mode.compare("-a") || 0 == mode.compare("--about"))
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
			std::string wf_name;
			const int argx = get_wavelet_transform(argc - argk, args + argk,
												   wf_name);

			if (0 > argx) return usage(std::cout);
			else argk += argx;

			std::cout << "wf_name: " << wf_name << std::endl;
		}
		else if (0 == mode.compare("-d") || 0 == mode.compare("--decode"))
		{
			std::string wf_name;
			const int argx = get_wavelet_transform(argc - argk, args + argk,
												   wf_name);

			if (0 > argx) return usage(std::cout);
			else argk += argx;

			std::cout << "wf_name: " << wf_name << std::endl;
		}
		else
		{
			return usage(std::cout);
		}
	}

	return 0;

	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////

	bmp_file_diff_src_t res;
	if (0 > get_bmp_file_diff_src(argc -1 , args + 1, res))
		return usage(std::cout);

	std::cout << res.channel1 << ":" << res.file1 << std::endl;
	std::cout << res.channel2 << ":" << res.file2 << std::endl;

	psnr(res);

	return 0;

	std::cout << "Image compression algorithm based on encoding of tree-arranged wavelet coefficients. Demo version." << std::endl;
	std::cout << "(C) Sergei V. Umnyashkin, Dmitri M. Koplovich, Andrei S. Pokrovskiy, Andrei A. Alexandrov, 2007" << std::endl;
	std::cout << "Moscow Institute of Electronic Technology" << std::endl;
	std::cout << "www.miet.ru" << std::endl;
	std::cout << std::endl;

	if (1 >= argc) return usage1(argc, args);

	if (0 == strcmp(args[1], "-p") || 0 == strcmp(args[1], "--psnr"))
	{
		if (4 > argc) return usage1(argc, args);

		std::cout << "PSNR: " << psnr(args[2], args[3]) << " dB" << std::endl;

		return 0;
	}

	if (0 == strcmp(args[1], "-d") || 0 == strcmp(args[1], "--decode"))
	{
		if (4 > argc) return usage1(argc, args);

		return decode(args[2], args[3]);
	}

	// �������������
	int arg_i = 1;

	if (0 == strcmp(args[arg_i], "-e") ||
		0 == strcmp(args[arg_i], "--encode"))
	{
		if (argc <= ++arg_i) return usage1(argc, args);
	}

	// ����� ������
	int method = 0;

	if (0 == strcmp(args[arg_i], "-m") ||
		0 == strcmp(args[arg_i], "--method"))
	{
		if (argc <= ++arg_i) return usage1(argc, args);

		if (0 == strcmp(args[arg_i], "manual") ||
			0 == strcmp(args[arg_i], "0"))
		{
			method = 0;
		} else if (0 == strcmp(args[arg_i], "fixed_bpp") ||
				   0 == strcmp(args[arg_i], "1"))
		{
			method = 1;
		}
		else if (0 == strcmp(args[arg_i], "fixed_lambda") ||
				 0 == strcmp(args[arg_i], "2"))
		{
			method = 2;
		}
		else return usage1(argc, args);

		if (argc <= ++arg_i) return usage1(argc, args);
	}

	// ���������� ������� ����������
	int lvls = 5;

	if (0 == strcmp(args[arg_i], "-v") ||
		0 == strcmp(args[arg_i], "--levels"))
	{
		if (argc <= ++arg_i) return usage1(argc, args);

		lvls = atoi(args[arg_i]);

		if (argc <= ++arg_i) return usage1(argc, args);
	}

	// �������� �����
	int channel = 0;

	if (0 == strcmp(args[arg_i], "-c") ||
		0 == strcmp(args[arg_i], "--channel"))
	{
		if (argc <= ++arg_i) return usage1(argc, args);

		channel = atoi(args[arg_i]);

		if (argc <= ++arg_i) return usage1(argc, args);
	}

	// ������� �����������
	wic::q_t q = 8;
	wic::lambda_t lambda = 10;

	// ������
	if (0 == method)
	{
		if (0 == strcmp(args[arg_i], "-q") ||
			0 == strcmp(args[arg_i], "--q"))
		{
			if (argc <= ++arg_i) return usage1(argc, args);

			q = wic::q_t(atof(args[arg_i]));

			if (argc <= ++arg_i) return usage1(argc, args);
		}

		if (0 == strcmp(args[arg_i], "-l") ||
			0 == strcmp(args[arg_i], "--lambda"))
		{
			if (argc <= ++arg_i) return usage1(argc, args);

			lambda = atof(args[arg_i]);

			if (argc <= ++arg_i) return usage1(argc, args);
		}
	}

	// ������������� bpp
	wic::h_t bpp = 0.5;

	if (1 == method)
	{
		if (0 == strcmp(args[arg_i], "-b") ||
			0 == strcmp(args[arg_i], "--bpp"))
		{
			if (argc <= ++arg_i) return usage1(argc, args);

			bpp = atof(args[arg_i]);

			if (argc <= ++arg_i) return usage1(argc, args);
		}
	}

	// ������������� lambda

	if (2 == method)
	{
		if (0 == strcmp(args[arg_i], "-l") ||
			0 == strcmp(args[arg_i], "--lambda"))
		{
			if (argc <= ++arg_i) return usage1(argc, args);

			lambda = atof(args[arg_i]);

			if (argc <= ++arg_i) return usage1(argc, args);
		}
	}

	// ���� ��������� � ������� �����
	const std::string srt_path = args[arg_i];
	if (argc <= ++arg_i) return usage1(argc, args);

	const std::string result_path = args[arg_i];

	// �������� ��������
	wic::encoder *const encoder = mk_encoder(srt_path, lvls, channel);

	if (0 == encoder)
	{
		std::cout << "Error: can't create encoder" << std::endl;
		return -1;
	}

	wic::encoder::header_t header;
	if (0 == method) 
	{
		std::cout << "Operation mode: manual" << std::endl;
		std::cout << "q: " << q << std::endl;
		std::cout << "lambda: " << lambda << std::endl;
		encoder->encode_0(q, lambda, header);
	}
	else if (1 == method)
	{
		std::cout << "Operation mode: fixed bpp" << std::endl;
		std::cout << "bpp: " << bpp << std::endl;
		encoder->encode_1(bpp, header);
	}
	else if (2 == method)
	{
		std::cout << "Operation mode: fixed lambda" << std::endl;
		std::cout << "lambda: " << lambda << std::endl;
		encoder->encode_2(lambda, header);
	}
	else return usage1(argc, args);

	// ������ ��������� � ����
	encode(*encoder, header, result_path);

	return 0;
}