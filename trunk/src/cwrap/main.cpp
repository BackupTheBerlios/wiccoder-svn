#include <iostream>

#include <wic/libwic/codec.h>
#include <wic/libwic/wtree.h>
#include <wic/libwic/subbands.h>


int main() {
	wic::subbands sb(32, 32, 2);
	for (wic::sz_t i = 0; sb.count() > i; ++i) {
		const wic::subbands::subband_t &s = sb.sb(i);
		std::cout << "(" << s.x_min << ", " << s.y_min << ", ";
		std::cout << s.x_max << ", " << s.y_max << ")" << std::endl;
	}

	std::cout << "rrr" << std::endl;

	for (wic::sz_t lvl = 0; sb.lvls() >= lvl; ++lvl) {
		for (wic::sz_t i = 0; wic::subbands::SUBBANDS_PER_LEVEL > i; ++i) {
			const wic::subbands::subband_t &s = sb.sb(lvl, i);
			std::cout << "(" << s.x_min << ", " << s.y_min << ", ";
			std::cout << s.x_max << ", " << s.y_max << ")" << std::endl;

			if (0 == lvl) break;
		}
	}

	wic::wtree tree(16, 16, 4);

	wic::encode_in_t enc_in;
	enc_in.width = 0;
	enc_in.height = 0;
	enc_in.width = 0;
	wic::encode_out_t enc_out;
	enc_out.data = 0;
	enc_out.data_sz = 0;
	enc_out.enc_sz = 0;
	wic::encode(enc_in, enc_out);

	wic::decode_in_t dec_in;
	wic::decode_out_t dec_out;
	wic::decode(dec_in, dec_out);
}
