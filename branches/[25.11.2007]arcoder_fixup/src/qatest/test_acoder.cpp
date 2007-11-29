////////////////////////////////////////////////////////////////////////////////
// headers

// standard C++ library headers
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

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


/*!	Тест проверяет работоспособность функций arcoder_base::save_dummy() и
	arcoder_base::load_dummy()
*/
bool test_acoder_dummy_saves()
{
	std::cout << "acoder dummy saves test" << std::endl;

	// количество моделей, участвующих в тесте
	static const int M = 9;

	// Размеры моделей
	static const int models[M]	= {1, 10, 20, 30, 40, 50, 60, 70, 80};

	// Смещение нулевого символа в моделях
	static const int offsets[M]	= {0, 2,  16, 1,  35, 25, 12, 59, 41};

	// Генерация случайной последовательности входных данных
	static const int data_len = 102400;

	std::vector<int> data;
	std::vector<int> data_m;

	for (int i = 0; data_len > i; ++i)
	{
		const int m = (rand() % M);
		const int s = ((rand() % models[m]) - offsets[m]);
		data.push_back(s);
		data_m.push_back(m);
	}

	// Создание буфера для приёма закодированных данных
	char encode_buffer[data_len];
	BITOutMemStream encode_out(encode_buffer, data_len);

	// Создание арифметического енкодера
	ArCoderWithBitCounter<BITOutMemStream> encoder(M, models, encode_out);

	// Накопление статистики енкодером
	encoder.StartPacking();

	for (int i = 0; data_len > i; ++i)
	{
		const int m = data_m[i];
		encoder.model(m);
		encoder << (data[i] + offsets[m]);
	}

	encoder.EndPacking();

	// Сохранение моделей в поток
	std::stringstream saved_models(std::ios_base::binary |
								   std::ios_base::in |
								   std::ios_base::out);
	encoder.save_dummy(saved_models, M, offsets);

	// Создание буфера для чтения закодированных данных
	BITInMemStream decode_in(encode_buffer, data_len);

	// Создание арифметического декодера
	ArDecoderWithBitCounter<BITInMemStream> decoder(M, models, decode_in);

	// Загрузка моделей в декодер
	saved_models.seekg(0, std::ios_base::beg);
	decoder.load_dummy(saved_models, M, offsets);

	// сравнение моделей енкодера и декодера
	bool has_errors = false;

	for (int i = 0; M > i; ++i)
	{
		encoder.model(i);
		decoder.model(i);

		const int emc = encoder.EOF_SYMBOL();
		const int dmc = decoder.EOF_SYMBOL();

		if (emc != dmc)
		{
			has_errors = true;

			std::cout << "Error: models #" << i << " have different size "
					  << "(e: " << emc << ", d: " << dmc << ")"
					  << std::endl;

			continue;
		}

		const int *const em = encoder.freq();
		const int *const dm = decoder.freq();

		for (int j = 0; (emc + 1) > j; ++j)
		{
			if (em[j] == dm[j]) continue;

			has_errors = true;

			std::cout << "Error: symbols #" << j << " in model #" << i
					  << " are different "
					  << "(e: " << em[j] << ", d: " << dm[j] << ")"
					  << std::endl;
		}
	}

	return !has_errors;

	// Повторное кодирование
	encoder.StartPacking();

	for (int i = 0; data_len > i; ++i)
	{
		const int m = data_m[i];
		encoder.model(m);
		encoder << (data[i] + offsets[m]);
	}

	encoder.EndPacking();

	// Загрузка моделей в декодер
	saved_models.seekg(0, std::ios_base::beg);
	decoder.load_dummy(saved_models, M, offsets);

	decoder.StartUnpacking();

	// Декодирование с проверкой
	for (int i = 0; data_len > i; ++i)
	{
		const int m = data_m[i];

		const int d = data[i];
		int s;
		decoder >> s;
		s -= offsets[m];

		if (s == data[i]) continue;

		std::cout << "Error: at position #" << i << std::endl;

		return false;
	}

	decoder.EndUnpacking();

	return false;
}
