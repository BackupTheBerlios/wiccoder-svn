#include <iostream>
#include <string>

#include <wic/libwic/encoder.h>
#include <imgs/img_rgb.h>
#include <imgs/img_yuv.h>
#include <imgs/psnr.h>
#include <imgs/bmp_file.h>
#include <imgs/bmp_dump.h>
#include <wavelets/cdf97/cdf97.h>

double psnr(const std::string &file1, const std::string &file2) {
	imgs::img_rgb rgb1;
	imgs::img_rgb rgb2;

	imgs::bmp_read(rgb1, file1);
	imgs::bmp_read(rgb2, file2);

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


int dencode(const int argc, const char *const * const args);


int main(int argc, char **args)
{
	return dencode(argc, args);

	static const char *const in_image_file	=
	//		"../res/images/lena_eye_16x16.bmp";
			"../res/images/lena.bmp";
	//		"../res/images/lena_eye_64x64.bmp";
	static const char *const out_image_file	=
	//		"dumps/lena_eye_16x16.bmp";
			"dumps/lena.bmp";
	//		"dumps/lena_eye_64x64.bmp";

	// some constants to use
	static const int			lvls	= 5;

	static const wic::q_t		q		= 13.74849f;

	static const wic::lambda_t	lambda	= 21.6125;

	// first, load the image
	imgs::img_rgb rgb_image;
	imgs::bmp_read(rgb_image, in_image_file);

	// allocate some memory and copy converted to float image there
	const wic::sz_t image_pixels_count = rgb_image.w() * rgb_image.h();

	float *const image_wt = new float[image_pixels_count];

	imgs::img_rgb::rgb24_t *const rgb_image_bits =
		(imgs::img_rgb::rgb24_t *)(rgb_image.bits());

	for (wic::sz_t i = 0; image_pixels_count > i; ++i) {
		image_wt[i] = rgb_image_bits[i].r;
	}

	// do wavelet transform
	wt2d_cdf97_fwd(image_wt, rgb_image.h(), rgb_image.w(), lvls);

	imgs::bmp_dump<wic::w_t, wic::sz_t>::txt_dump(image_wt,
												  rgb_image.w(), rgb_image.h(),
												  "dumps/[enc]image_wt.in");

	// создание кодера
	wic::encoder encoder(rgb_image.w(), rgb_image.h(), lvls);

	/*
	// квантование коэффициентов
	encoder.spectrum().quantize(q);

	encoder.spectrum().save<wic::wnode::member_wq>(image_wt);
	imgs::bmp_dump<wic::w_t, wic::sz_t>::txt_dump(image_wt,
												  rgb_image.w(), rgb_image.h(),
												  "dumps/[enc]quantized.wq");

	imgs::bmp_dump<wic::w_t, wic::sz_t>::dump(image_wt,
												  rgb_image.w(), rgb_image.h(),
												  "dumps/[enc]quantized.bmp");

	encoder.spectrum().save<wic::wnode::member_n>(image_wt);
	imgs::bmp_dump<wic::w_t, wic::sz_t>::txt_dump(image_wt,
												  rgb_image.w(), rgb_image.h(),
												  "dumps/[enc]before.n");

	encoder.spectrum().save<wic::wnode::member_wc>(image_wt);
	imgs::bmp_dump<wic::w_t, wic::sz_t>::txt_dump(image_wt,
												  rgb_image.w(), rgb_image.h(),
												  "dumps/[enc]before.wc");
	*/

	// кодирование
	wic::encoder::header_t header;
	encoder.encode(image_wt, q, lambda, header);

	encoder.spectrum().save<wic::wnode::member_wc>(image_wt);

	imgs::bmp_dump<wic::w_t, wic::sz_t>::txt_dump(image_wt,
												  rgb_image.w(), rgb_image.h(),
												  "dumps/[enc]optimized.wc");

	encoder.spectrum().save<wic::wnode::member_invalid>(image_wt);

	imgs::bmp_dump<wic::w_t, wic::sz_t>::txt_dump(image_wt,
												  rgb_image.w(), rgb_image.h(),
												  "dumps/[enc]optimized.invalid");

	encoder.spectrum().save<wic::wnode::member_n>(image_wt);

	imgs::bmp_dump<wic::w_t, wic::sz_t>::txt_dump(image_wt,
												  rgb_image.w(), rgb_image.h(),
												  "dumps/[enc]optimized.n");

	encoder.spectrum().save<wic::wnode::member_j0>(image_wt);

	imgs::bmp_dump<wic::w_t, wic::sz_t>::txt_dump(image_wt,
												  rgb_image.w(), rgb_image.h(),
												  "dumps/[enc]optimized.j0", 10);

	encoder.spectrum().save<wic::wnode::member_j1>(image_wt);

	imgs::bmp_dump<wic::w_t, wic::sz_t>::txt_dump(image_wt,
												  rgb_image.w(), rgb_image.h(),
												  "dumps/[enc]optimized.j1", 10);

	// декодирование
	// wic::encoder decoder(rgb_image.w(), rgb_image.h(), lvls);
	encoder.decode(encoder.coder().buffer(), encoder.coder().buffer_sz(),
				   header);

	encoder.spectrum().save<wic::wnode::member_wc>(image_wt);

	imgs::bmp_dump<wic::w_t, wic::sz_t>::txt_dump(image_wt,
												  rgb_image.w(), rgb_image.h(),
												  "dumps/[dec]decoded.wc");

	encoder.spectrum().save<wic::wnode::member_n>(image_wt);

	imgs::bmp_dump<wic::w_t, wic::sz_t>::txt_dump(image_wt,
												  rgb_image.w(), rgb_image.h(),
												  "dumps/[dec]decoded.n");

	encoder.spectrum().save<wic::wnode::member_invalid>(image_wt);

	imgs::bmp_dump<wic::w_t, wic::sz_t>::txt_dump(image_wt,
												  rgb_image.w(), rgb_image.h(),
												  "dumps/[dec]decoded.invalid");

	// деквантование
	encoder.spectrum().dequantize<wic::wnode::member_wc>(q);

	// сохранение результатов
	encoder.spectrum().save<wic::wnode::member_w>(image_wt);

	imgs::bmp_dump<wic::w_t, wic::sz_t>::txt_dump(image_wt,
												  rgb_image.w(), rgb_image.h(),
												  "dumps/[dec]image_wt.in");

	wt2d_cdf97_inv(image_wt, rgb_image.h(), rgb_image.w(), lvls);

	for (wic::sz_t i = 0; image_pixels_count > i; ++i) {
		rgb_image_bits[i].r = (unsigned char)(image_wt[i]);
		rgb_image_bits[i].g = (unsigned char)(image_wt[i]);
		rgb_image_bits[i].b = (unsigned char)(image_wt[i]);
	}

	imgs::bmp_write(rgb_image, out_image_file);

	{
		std::cout << "levels: " << lvls << std::endl;
		std::cout << "q: " << q << std::endl;
		std::cout << "lambda: " << lambda << std::endl;
		const wic::sz_t sz = encoder.coder().encoded_sz();
		std::cout << "size: " << sz << " bytes" << std::endl;
		std::cout << "size: " << (sz / 1024) << " kbytes" << std::endl;
		std::cout << "bpp: " << double(sz * 8) / double(rgb_image.h() * rgb_image.w()) << std::endl;
		std::cout << "psnr: " << psnr(in_image_file, out_image_file) <<std::endl;
	}
	/*
	// what and how to encode
	wic::encode_in_t enc_in;
	enc_in.image	= image_wt;
	enc_in.height	= rgb_image.w();
	enc_in.width	= rgb_image.h();
	enc_in.lvls		= lvls;

	// place to store encoding results
	wic::encode_out_t enc_out;
	enc_out.data = 0;
	enc_out.data_sz = 0;
	enc_out.enc_sz = 0;
	wic::encode(enc_in, enc_out);
	*/

	// free memory
	delete[] image_wt;

	return 0;
}


int dencode(const int argc, const char *const * const args)
{
	// check args --------------------------------------------------------------
	if (5 != argc) {
		std::cout << "usage: " << std::endl;
		std::cout << "\tin.bmp out.bmp q lambda" << std::endl;
		std::cout << "example: " << std::endl;
		std::cout << "\tlena.bmp lena_restored.bmp 13.7 21.6" << std::endl;

		return - 1;
	}

	// some values to use ------------------------------------------------------
	static const int			lvls	= 5;

	wic::q_t q = 0;
	sscanf(args[3], "%f", &q);

	float lambda_f = 0;
	sscanf(args[4], "%f", &lambda_f);
	wic::lambda_t lambda = lambda_f;

	std::cout << "levels: " << lvls << std::endl;
	std::cout << "q: " << q << std::endl;
	std::cout << "lambda: " << lambda << std::endl;
	std::cout << "input image: " << args[1] << std::endl;
	std::cout << "output image: " << args[2] << std::endl;
	std::cout << "compressed file: " << "temp.out" << std::endl;

	// encode ------------------------------------------------------------------

	// first, load the image
	imgs::img_rgb rgb_image;
	imgs::bmp_read(rgb_image, args[1]);

	// allocate some memory and copy converted to float image there
	const wic::sz_t image_pixels_count = rgb_image.w() * rgb_image.h();

	float *const image_wt = new float[image_pixels_count];

	imgs::img_rgb::rgb24_t *const rgb_image_bits =
		(imgs::img_rgb::rgb24_t *)(rgb_image.bits());

	for (wic::sz_t i = 0; image_pixels_count > i; ++i) {
		image_wt[i] = rgb_image_bits[i].r;
	}

	// do wavelet transform
	wt2d_cdf97_fwd(image_wt, rgb_image.h(), rgb_image.w(), lvls);

	// создание кодера
	wic::encoder encoder(rgb_image.w(), rgb_image.h(), lvls);

	LARGE_INTEGER t1;
	LARGE_INTEGER t2;
	LARGE_INTEGER f;
	QueryPerformanceFrequency(&f);

	// кодирование
	std::cout << "encoding... ";
	wic::encoder::header_t header;
	QueryPerformanceCounter(&t1);
	encoder.encode(image_wt, q, lambda, header);
	QueryPerformanceCounter(&t2);
	std::cout << double(t2.QuadPart - t1.QuadPart) / double(f.QuadPart) << "s";
	std::cout << std::endl;

	// compressed file ---------------------------------------------------------
	std::ofstream compressed("temp.out", std::ios::binary|std::ios::binary);
	compressed.write((char *)&header, sizeof(&header));
	compressed.write((char *)encoder.coder().buffer(),
					 encoder.coder().encoded_sz());

	// decode ------------------------------------------------------------------

	// создание декодера
	wic::encoder decoder(rgb_image.w(), rgb_image.h(), lvls);

	// декодирование
	std::cout << "decoding... ";
	QueryPerformanceCounter(&t1);
	decoder.decode(encoder.coder().buffer(), encoder.coder().encoded_sz(),
				   header);
	QueryPerformanceCounter(&t2);
	std::cout << double(t2.QuadPart - t1.QuadPart) / double(f.QuadPart) << "s";
	std::cout << std::endl;

	// сохранение результатов
	decoder.spectrum().save<wic::wnode::member_w>(image_wt);

	wt2d_cdf97_inv(image_wt, rgb_image.h(), rgb_image.w(), lvls);

	for (wic::sz_t i = 0; image_pixels_count > i; ++i) {
		rgb_image_bits[i].r = (unsigned char)(image_wt[i]);
		rgb_image_bits[i].g = (unsigned char)(image_wt[i]);
		rgb_image_bits[i].b = (unsigned char)(image_wt[i]);
	}

	imgs::bmp_write(rgb_image, args[2]);

	// psnr calc ---------------------------------------------------------------
	const wic::sz_t sz = encoder.coder().encoded_sz();
	std::cout << "size: " << sz << " bytes" << std::endl;
	std::cout << "size: " << (sz / 1024) << " kbytes" << std::endl;
	std::cout << "bpp: " << double(sz * 8) / double(rgb_image.h() * rgb_image.w()) << std::endl;
	std::cout << "psnr: " << psnr(args[1], args[2]) <<std::endl;

	return 0;
}
