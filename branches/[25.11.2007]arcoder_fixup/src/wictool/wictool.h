/*!	\file     wictool.h
	\version  0.0.1
	\author   mice, ICQ: 332-292-380, mailto:wonder.mice@gmail.com
	\brief    ������� ������������ ���� ������� wictool

	\todo     ����� �������� ������� ���� wictool.h
*/

#ifndef WIC_WICTOOL_WICTOOL
#define WIC_WICTOOL_WICTOOL

///////////////////////////////////////////////////////////////////////////////
// include

// standard C++ library headers
#include <string>

// wavelet filters headers
#include <wavelets/devis/wavelet.h>

// libwic headers
#include <wic/libwic/types.h>


///////////////////////////////////////////////////////////////////////////////
// public data types

//!	\brief ��������� ��������� ��� bmp ����� � ���������� ��������� ������
//!	(��������) ��� ������������ �� ���������
/*!	������ ��������� ������������ ��� ��������� ����� � ���� ������ �
	��������������� �������� ���� (�������). ������������ ��� ������� ������
	��� ������� ��������� ���� �����������.

	��������� ����� rgb ����������� �������������� ASCII ���������:
	- 'r' - �������
	- 'g' - ������
	- 'b' - �����
	- 'x' - ��� ������
*/
struct bmp_file_diff_src_t
{
	//!	\brief ����� ������� �����
	char channel1;
	//!	\brief ���� � ������� �����
	std::string file1;
	//!	\brief ����� ������� �����
	char channel2;
	//!	\brief ���� �� ������� �����
	std::string file2;
};

//!	\brief �������� ���� ��������� ������ ����������� � ��� ��������������
/*!	\attention ���������� ������������ ������� free_bmp_channel_bits() ���
	������������ �������� ������� ����������.
*/
struct bmp_channel_bits
{
	//!	\brief ���� ��������� ������ �����������
	unsigned char *data;
	//!	\brief �������������� �������� �����
	char channel;
	//!	\brief ������ ������ ��������� ������ (� ������)
	unsigned int sz;
	//!	\brief ������ ��������� ������ �����������
	unsigned int w;
	//!	\brief ������ ��������� ������ �����������
	unsigned int h;
};

//!	\brief ���������� ���������� ��������� ������
struct simple_stat_t
{
	//!	\brief �������� ������������ ��������
	unsigned char min;
	//!	\brief �������� ������������� ��������
	unsigned char max;
	//!	\brief ������� �������� ���������
	unsigned char avg;
};

//!	\brief ��������� �����������
struct encode_params_t
{
	//!	\brief ������������ ������������
	wic::q_t q;
	//!	\brief ������ �������� / ������� ������
	wic::lambda_t lambda;
	//!	\brief �������� ������� �������
	wic::h_t bpp;
	//!	\brief ���������� ������� ������� ��������������
	unsigned int steps;
};

//!	\brief ������ ������� �������������� (������������ ����������
//!	� ������������� ����������)
struct spectre_t
{
	//!	\brief ������ ������������� ������� ��������������
	wic::w_t *data;
	//!	\brief ���������� ������������� ������� ��������������
	unsigned int sz;
	//!	\brief ������ ������� �������
	unsigned int w;
	//!	\brief ������ ������� �������
	unsigned int h;
};

//!	\brief ��������� wic �����
struct wic_header_t
{
	//!	\brief Must be "WICX", where X - wiccoder version
	char signature[3];
	char version;
	//!	\brief ������ ����� ��������������� ������� ��������������
	unsigned char filter_sz;
	//!	\brief ���������� ������� ������� ��������������
	unsigned char steps;
	//!	\brief ��� ��������� ������
	char channel;
	//!	\brief ������ ������� �����������
	unsigned int w;
	//!	\brief ������ ������� �����������
	unsigned int h;
	//!	\brief ������ �������������� ������
	unsigned int data_sz;
	//!	\brief ��� ��������������� ������� ��������������
	// char filter[];
};


///////////////////////////////////////////////////////////////////////////////
// public constant definitions

//!	\brief ��������� ������ ������ wiccoder
extern const char WIC_SIGNATURE[3];

//!	\brief ������ ��������� ������
extern const char WIC_VERSION;


///////////////////////////////////////////////////////////////////////////////
// public function declarations

//!	\name ������ � ��������� ��������
//@{

//!	\brief ������������ ������ ���������� (��� � ��������� ������) � ��������
//!	������������� ��������� ������ � ���� � bmp �����
int get_bmp_file_and_channel(const int argc, const char *const *const args,
							 char &channel, std::string &path,
							 std::ostream *const err = 0);

//!	\brief ������ ����������� � �������� �� ���� ��������� �������� �����
bmp_channel_bits get_bmp_channel_bits(const std::string &path,
									  const char channel,
									  std::ostream *const err = 0);

//!	\brief ���������� �������� ����� � ����
int set_bmp_channel_bits(const std::string &path, const bmp_channel_bits &bits,
						 const char &usr_channel,
						 std::ostream *const err = 0);

//!	\brief ����������� ������������ ������ ��������� �� ����������
//!	bmp_channel_bits
void free_bmp_channel_bits(bmp_channel_bits &bits);

//!	\brief ���������, ���������� �� � ��������� bmp_channel_bits
//!	�������� ����������
bool is_channel_bits_good(const bmp_channel_bits &bits,
						  std::ostream *const err = 0);

//@}


//!	\name ��������� �����������
//@{

//!	\brief ������������ ������ ���������� (��� � ��������� ������) �
//!	��������� ���������, ������������ ������� ��������� ��� ��������� ����
//!	�����������
int get_bmp_file_diff_src(const int argc, const char *const *const args,
						  bmp_file_diff_src_t &result,
						  std::ostream *const err = 0);

//!	\brief ��������� ������������� ��������� ���� �������� �������
bool is_diff_allowed(const bmp_channel_bits &bits1,
					 const bmp_channel_bits &bits2,
					 std::ostream *const err = 0);

//!	\brief ���������� ������� ������ <i>PSNR</i> ���� �������� �������
double psnr(const bmp_channel_bits &bits1, const bmp_channel_bits &bits2,
			std::ostream *const err = 0);

//!	\brief ������� �� ����� �������� ������ <i>PSNR</i> ���� ��������
//!	�������
int psnr(const bmp_file_diff_src_t &diff_src,
		 std::ostream *const err = 0);

//!	\brief ������������ ���������� ����������
simple_stat_t calc_simple_stat(const bmp_channel_bits &bits,
							   std::ostream *const err = 0);

//!	\brief ������� ���������� ������� ���� �������� �������
int stat(const bmp_channel_bits &bits1, const bmp_channel_bits &bits2,
		 std::ostream *const out = 0, std::ostream *const err = 0);

//!	\brief ������� ���������� ������� ���� �������� �������
int stat(const bmp_file_diff_src_t &diff_src,
		 std::ostream *const err = 0);

//@}


//!	\name ������ � ������� ����������������
//@{

//!	\brief �������� �������� ������� �������������� �� ��������� ������
int get_wavelet_filter(const int argc, const char *const *const args,
					   std::string &result, std::ostream *const err = 0);

//!	\brief �������� ������� ������ �� ���������� devis'� �� ��� �����
FilterSet *get_devis_filter(const std::string &name);

//!	\brief ��������� ��������� ������� �� ����������� �������������
//!	������� ��������������
bool check_transform_dims(const unsigned int w, const unsigned int h,
						  const unsigned int steps,
						  std::ostream *const err = 0);

//!	\brief ��������� ������ ������� ��������������
spectre_t forward_transform(const std::string &filter,
							const unsigned int &steps,
							const bmp_channel_bits &bits,
							std::ostream *const err = 0);

//!	\brief ��������� �������� ������� ��������������
bmp_channel_bits inverse_transform(const wic::wtree &spectre,
								   const std::string &filter,
								   const char &channel,
								   std::ostream *const err = 0);

//!	\brief ��������� ������ ������� ��������������
bool is_spectre_good(const spectre_t &spectre, std::ostream *const err = 0);

//!	\brief ����������� ������, ���������� �������� ������� �������������
void free_spectre(spectre_t &spectre);

//@}


//!	\name ����������� � �������������
//@{

//!	\brief ������� ��������� ������ ��� wic::encoder
void wic_optimize_callback(const wic::encoder::optimize_result_t &result,
						   void *const param);

//!	\brief �������� �������� ������� �������������� �� ��������� ������
int get_encode_method(const int argc, const char *const *const args,
					  std::string &result, std::ostream *const err = 0);

//!	\brief �������� ��������� �����������
int get_encode_params(const int argc, const char *const *const args,
					  encode_params_t &params, std::ostream *const err = 0);

//!	\brief �������� ���� � ������� �����
int get_wic_file(const int argc, const char *const *const args,
				 std::string &wic_file, std::ostream *const err = 0);

//!	\brief �������� ����� ��������� ������, ����������� ��� �����������
int get_encode_options(const int argc, const char *const *const args,
					   std::string &filter, std::string &method,
					   encode_params_t &params,
					   std::string &source, char &channel,
					   std::string &output,
					   std::ostream *const err = 0);

//!	\brief ���������� ��������� ����������� � ����� ������
int write_wic_header(const unsigned int &data_sz, const std::string &filter,
					 const unsigned int &steps, const char &channel,
					 const unsigned int &w, const unsigned int &h,
					 std::ostream &output, std::ostream *const err = 0);

//!	\brief ������ ��������� ����������� �� ������
int read_wic_header(std::istream &input,
					unsigned int &data_sz, std::string &filter,
					unsigned int &steps, char &channel,
					unsigned int &w, unsigned int &h,
					std::ostream *const err = 0);

//@}


//!	\name ��������������� �������
//@{

//!	\brief Here's to the crazy ones.
void crazy_ones(std::ostream &out);

//!	\brief Performs several tests
void do_wic_test(std::ostream &out);

//!	\brief ������� ���������� � �������
void about(std::ostream &out);

//!	\brief ������� ���������� �� ����������� ��������
int usage(std::ostream &out);

//@}



#endif	// WIC_WICTOOL_WICTOOL
