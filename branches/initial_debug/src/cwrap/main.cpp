#include <iostream>

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


int main() {
	// some constants to use
	static const int			lvls	= 3;

	static const wic::q_t		q		= 1;

	static const wic::lambda_t	lambda	= 10;

	// first, load the image
	imgs::img_rgb rgb_image;
	imgs::bmp_read(rgb_image, "../res/images/lena_eye_16x16.bmp");

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
	wic::encoder encoder(image_wt, rgb_image.w(), rgb_image.h(), lvls);

	// квантование коэффициентов
	encoder.spectrum().quantize(q);

	encoder.spectrum().save<wic::wnode::member_wq>(image_wt);
	imgs::bmp_dump<wic::w_t, wic::sz_t>::txt_dump(image_wt,
												  rgb_image.w(), rgb_image.h(),
												  "dumps/[enc]quantized.wq");

	encoder.spectrum().save<wic::wnode::member_n>(image_wt);
	imgs::bmp_dump<wic::w_t, wic::sz_t>::txt_dump(image_wt,
												  rgb_image.w(), rgb_image.h(),
												  "dumps/[enc]before.n");

	encoder.spectrum().save<wic::wnode::member_wc>(image_wt);
	imgs::bmp_dump<wic::w_t, wic::sz_t>::txt_dump(image_wt,
												  rgb_image.w(), rgb_image.h(),
												  "dumps/[enc]before.wc");

	// кодирование
	encoder.encode(lambda);

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

	// декодирование
	encoder.decode();

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
	encoder.spectrum().dequantize(q);

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

	imgs::bmp_write(rgb_image, "dumps/lena_eye_16x16.bmp");

	{
		std::cout << "q: " << q << std::endl;
		std::cout << "lambda: " << lambda << std::endl;
		const wic::sz_t sz = encoder.coder().encoded_sz();
		std::cout << "size: " << sz << " bytes" << std::endl;
		std::cout << "size: " << (sz / 1024) << " kbytes" << std::endl;
		std::cout << "bpp: " << double(sz * 8) / double(rgb_image.h() * rgb_image.w()) << std::endl;
		std::cout << "psnr: " << psnr("dumps/lena_eye_16x16.bmp", "../res/images/lena_eye_16x16.bmp") <<std::endl;
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
