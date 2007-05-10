#include <iostream>

#include <wic/libwic/codec.h>
#include <wic/libwic/wtree.h>
#include <wic/libwic/subbands.h>
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

	const imgs::img_rgb::rgb24_t *const rgb_image_bits =
		(imgs::img_rgb::rgb24_t *)(rgb_image.bits());

	for (wic::sz_t i = 0; image_pixels_count > i; ++i) {
		image_wt[i] = rgb_image_bits[i].r;
	}

	// do wavelet transform
	wt2d_cdf97_fwd(image_wt, rgb_image.h(), rgb_image.w(), lvls);

	imgs::bmp_dump<wic::w_t, wic::sz_t>::txt_dump(image_wt,
												  rgb_image.w(), rgb_image.h(),
												  "dumps/[enc]image_wt.in");

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

	// free memory
	delete[] image_wt;

	return 0;

	wic::decode_in_t dec_in;
	wic::decode_out_t dec_out;
	wic::decode(dec_in, dec_out);
}
