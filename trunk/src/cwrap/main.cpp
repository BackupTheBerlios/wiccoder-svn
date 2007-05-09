#include <iostream>

#include <wic/libwic/codec.h>
#include <wic/libwic/wtree.h>
#include <wic/libwic/subbands.h>


int main() {
	wic::encode_in_t enc_in;
	enc_in.image	= 0;
	enc_in.height	= 512;
	enc_in.width	= 512;
	enc_in.lvls		= 5;
	wic::encode_out_t enc_out;
	enc_out.data = 0;
	enc_out.data_sz = 0;
	enc_out.enc_sz = 0;
	wic::encode(enc_in, enc_out);

	wic::decode_in_t dec_in;
	wic::decode_out_t dec_out;
	wic::decode(dec_in, dec_out);
}
