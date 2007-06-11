#include <iostream>

#include <wic/libwic/encoder.h>
#include <imgs/img_rgb.h>
#include <imgs/bmp_file.h>
#include <imgs/bmp_dump.h>
#include <wavelets/cdf97/cdf97.h>


int main() {
	// some constants to use
	static const int lvls		= 3;

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
	encoder.spectrum().quantize(6);

	encoder.spectrum().save<wic::wnode::member_wq>(image_wt);
	imgs::bmp_dump<wic::w_t, wic::sz_t>::txt_dump(image_wt,
												  rgb_image.w(), rgb_image.h(),
												  "dumps/[enc]quantized.wq");

	// кодирование
	encoder.encode(0);

	encoder.spectrum().save<wic::wnode::member_wc>(image_wt);

	imgs::bmp_dump<wic::w_t, wic::sz_t>::txt_dump(image_wt,
												  rgb_image.w(), rgb_image.h(),
												  "dumps/[enc]optimized.wc");

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

	// деквантование
	encoder.spectrum().dequantize(6);

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
