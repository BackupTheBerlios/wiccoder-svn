////////////////////////////////////////////////////////////////////////////////
// headers

// standard C++ library headers
#include <iostream>
#include <string>

// external libraries headers
#include <wic/libwic/acoder.h>

// libwic headers
// none

// qatest headers
#include "test_acoder.h"


////////////////////////////////////////////////////////////////////////////////
// public function definitions

/*!
*/
bool test_acoder_integration()
{
	std::cout << "acoder integration test" << std::endl;

	// create models
	wic::acoder::models_t models;
	wic::acoder::model_t model0;
	wic::acoder::model_t model1;
	wic::acoder::model_t model2;
    model0.min = 'a';
	model0.max = 'z';
    model1.min = 'a';
	model1.max = 'z';
    model2.min = 'a';
	model2.max = 'z';
	models.push_back(model0);
	models.push_back(model1);
	models.push_back(model2);

	// create arcoder and strings
	std::string expected = "abcdefghijklmnopqrstuvwxyz";
	std::string decoded = "";
	wic::acoder coder((wic::sz_t) expected.length());
	coder.use(models);

	// encode
	coder.encode_start();
	for (unsigned int cnt = 0; cnt < expected.length(); ++cnt)
	{
		coder.put_value(expected[cnt], cnt % 3);
	}
	coder.encode_stop();

	// decode
	coder.decode_start();
	for (unsigned int cnt = 0; cnt < expected.length(); ++cnt)
	{
		decoded.append(1, (char) coder.get_value(cnt %3));
	}
	coder.decode_stop();

	// check
	if (decoded == expected)
	{
		return true;
	}
	return false;
}