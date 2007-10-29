/*!	\file     encoder.cpp
	\version  0.0.1
	\author   mice, ICQ: 332-292-380, mailto:wonder.mice@gmail.com
	\brief    Реализация класса wic::encoder

	\todo     Более подробно описать файл encoder.h
*/


////////////////////////////////////////////////////////////////////////////////
// include

// standard C++ headers
// none

// libwic headers
#include <wic/libwic/encoder.h>


////////////////////////////////////////////////////////////////////////////////
// wic namespace
namespace wic {


////////////////////////////////////////////////////////////////////////////////
// encoder class public definitions

/*!	\param[in] width Ширина изображения.
	\param[in] height Высота изображения.
	\param[in] lvls Количество уровней вейвлет преобразования.
*/
encoder::encoder(const sz_t width, const sz_t height, const sz_t lvls):
	_wtree(width, height, lvls),
	_acoder(width * height * sizeof(w_t) * 4),
	_optimize_callback(0), _optimize_callback_param(0)
{
	// проверка утверждений
	assert(MINIMUM_LEVELS <= lvls);

	#ifdef LIBWIC_DEBUG
	_dbg_out_stream.open("dumps/[encoder]debug.out",
						 std::ios_base::out | std::ios_base::app);

	if (_dbg_out_stream.good())
	{
		time_t t;
		time(&t);
		_dbg_out_stream << std::endl << ctime(&t) << std::endl;
	}
	#endif
}


/*!
*/
encoder::~encoder() {
}


/*!	\param[in] callback Функция обратного вызова
	\param[in] param Пользовательский параметр, передаваемый в функцию
	обратного вызова
*/
void encoder::optimize_callback(const optimize_callback_f &callback,
								void *const param)
{
	_optimize_callback			= callback;
	_optimize_callback_param	= param;
}


/*!	\param[in] w Спектр вейвлет преобразования входного изображения для
	кодирования

	\param[in] q Квантователь. Чем больше значение (величина) квантователя,
	тем большую степень сжатия можно получить. Однако при увеличении
	квантователя качество восстановленного (декодированного) изображения
	ухудшается. Значение квантователя должно быть больше <i>1</i>.

	\param[in] lambda Параметр <i>lambda</i> используемый для
	вычисления <i>RD</i> критерия (функции Лагранжа). Представляет
	собой баланс между ошибкой и битовыми затратами. Чем больше данный
	параметр, тем больший приоритет будет отдан битовым затратам.
	Соответственно, при 0 будет учитываться только ошибка кодирования.

	\param[out] tunes Информация, необходимая для последующего
	восстановления изображения

	\return Результат проведённого кодирования

	Данный механизм кодирования применяется, когда оптимальные (или
	субоптимальные) значения параметров <i>q</i> и <i>lambda</i> известны
	заранее. Этот метод является самым быстрым так как производит операции
	оптимизации топологии и кодирования только по одному разу.

	Стоит заметить, что метод также производит квантование спектра выбранным
	квантователем <i>q</i>, поэтому его не желательно использовать в
	ситуациях когда необходимо получить результаты сжатия одного изображения
	с разным параметром <i>lambda</i>. Для таких случаев лучше использовать
	более быструю функцию cheap_encode(), которая не производит квантования
	коэффициентов спектра.

	Код функции использует макрос #OPTIMIZATION_USE_VIRTUAL_ENCODING. Если он
	определён будет производиться виртуальное кодирование коэффициентов, что
	несколько быстрее. Однако не все реализации битовых кодеров могут
	поддерживать это.

	Вся информация необходимая для успешного декодирования (кроме информации
	описывающей параметры исходного изображения, такие как его разрешение и
	количество уровней преобразования) возвращается через структуру
	tunes_t. Доступ к закодированному изображению осуществляется через объект
	арифметического кодера, ссылку на который можно получить вызвав метод
	coder().
*/
encoder::enc_result_t
encoder::encode(const w_t *const w, const q_t q, const lambda_t &lambda,
				tunes_t &tunes)
{
	// результат проведённой оптимизации
	enc_result_t result;

	// проверка входных параметров
	assert(0 != w);

	// загрузка спектра
	_wtree.load_field<wnode::member_w>(w);

	// оптимизация топологии ветвей
	result.optimization = 
		#ifdef OPTIMIZATION_USE_VIRTUAL_ENCODING
		_optimize_wtree(lambda, q, tunes.models, true);
		#else
		_optimize_wtree(lambda, q, tunes.models, false);
		#endif

	// кодирование всего дерева
	_acoder.encode_start();

	_encode_wtree();

	_acoder.encode_stop();

	// запись данных необходимых для последующего декодирования
	tunes.q = _wtree.q();

	// отчёт о проделанной работе
	result.bpp = _calc_encoded_bpp();

	// завершение кодирования
	return result;
}


/*!	\param[in]
*/
encoder::enc_result_t
encoder::encode_fixed_lambda(
						const w_t *const w, const lambda_t &lambda,
						tunes_t &tunes,
						const q_t &q_min, const q_t &q_max, const q_t &q_eps,
						const j_t &j_eps, const sz_t &max_iterations)
{
	// результат проведённой оптимизации
	enc_result_t result;

	// проверка входных параметров
	assert(0 != w);

	// загрузка спектра
	_wtree.load_field<wnode::member_w>(w);

	// минимизация RD функции Лагранжа
	result.optimization = 
		#ifdef OPTIMIZATION_USE_VIRTUAL_ENCODING
		_search_q_min_j(lambda, q_min, q_max, q_eps,
						tunes.models, j_eps, true, max_iterations);
		#else
		_search_q_min_j(lambda, q_min, q_max, q_eps,
						tunes.models, j_eps, false, max_iterations);
		#endif

	// кодирование всего дерева
	_acoder.encode_start();

	_encode_wtree();

	_acoder.encode_stop();

	// сохранение параметров, необходимых для последующего декодирования
	tunes.q = _wtree.q();

	// отчёт о проделанной работе
	result.bpp = _calc_encoded_bpp();

	return result;
}


/*!
*/
encoder::enc_result_t
encoder::encode_fixed_lambda(const w_t *const w, const lambda_t &lambda,
							 tunes_t &tunes)
{
	static const q_t q_eps = 0.5;

	const q_t q_min = 1;
	const q_t q_max = 32;

	return encode_fixed_lambda(w, lambda, tunes, q_min, q_max, q_eps);
}


/*!	\param[in] lambda
	\param[out] tunes
*/
/*
void encoder::cheap_encode(const lambda_t &lambda, tunes_t &tunes)
{
	// оптимизация топологии ветвей
	_optimize_result_t result =
	#ifdef OPTIMIZATION_USE_VIRTUAL_ENCODING
	_optimize_wtree(lambda, true);
	#else
	_optimize_wtree(lambda, false);
	#endif

	// кодирование всего дерева
	_acoder.encode_start();

	_encode_wtree();

	_acoder.encode_stop();

	tunes.q = _wtree.q();

	const h_t bpp = h_t((coder().encoded_sz() + sizeof(header)) * BITS_PER_BYTE) / h_t(_wtree.nodes_count());

	std::cout << "bpp: " << std::setprecision(2) << bpp << std::endl;
}


//-----------------------------------------------------------------
void encoder::encode_0(const q_t q, const lambda_t &lambda, header_t &header)
{
	// оптимизация топологии ветвей
	#ifdef OPTIMIZATION_USE_VIRTUAL_ENCODING
	_optimize_result_t result = 
	_optimize_wtree(lambda, q, header.models, true);
	#else
	_optimize_result_t result = 
	_optimize_wtree(lambda, q, header.models, false);
	#endif

	// кодирование всего дерева
	_acoder.encode_start();

	_encode_wtree();

	_acoder.encode_stop();

	header.q = _wtree.q();

	const h_t bpp = h_t((coder().encoded_sz() + sizeof(header)) * BITS_PER_BYTE) / h_t(_wtree.nodes_count());

	std::cout << "bpp: " << std::setprecision(2) << bpp << std::endl;
}

void encoder::encode_1(h_t &bpp, header_t &header)
{
	_search_result_t result =
	_search_q_and_lambda(bpp, header.models);

	// кодирование всего дерева
	_acoder.encode_start();

	_encode_wtree();

	_acoder.encode_stop();

	header.q = _wtree.q();

	const h_t bppr = h_t((coder().encoded_sz() + sizeof(header)) * BITS_PER_BYTE) / h_t(_wtree.nodes_count());

	std::cout << "bpp: " << std::setprecision(2) << bppr << std::endl;
	std::cout << "q: " << result.optimized.q << std::endl;
	std::cout << "lambda: " << result.optimized.lambda << std::endl;
}

void encoder::encode_2(const lambda_t &lambda, header_t &header)
{
	_search_result_t result =
	_search_q_min_j(lambda, header.models, 1.0f, 64.0f, 0.1f, 0.1f);

	// кодирование всего дерева
	_acoder.encode_start();

	_encode_wtree();

	_acoder.encode_stop();

	header.q = _wtree.q();

	const h_t bpp = h_t((coder().encoded_sz() + sizeof(header)) * BITS_PER_BYTE) / h_t(_wtree.nodes_count());

	std::cout << "bpp: " << std::setprecision(2) << bpp << std::endl;
	std::cout << "q: " << result.optimized.q << std::endl;
	std::cout << "lambda: " << result.optimized.lambda << std::endl;
}
*/


/*!	param[in] data Указатель на блок памяти, содержащий закодированное
	изображение
	param[in] data_sz Размер блока, содержащего закодированное
	изображения
	param[in] tunes Данные необходимые для восстановления изображения,
	полученные от одной из функций кодирования.

	Стоит заметить, что в текущей реализации, память для арифметического
	кодера выделяется зарание, размер которой определяется исходя из
	размеров самого изображения. Поэтому необходимо, чтобы размер
	данных <i>data_sz</i> был меньше, чем acoder::buffer_sz().
*/
void encoder::decode(const byte_t *const data, const sz_t data_sz,
					 const tunes_t &tunes)
{
	// проверка утверждений
	assert(_acoder.buffer_sz() >= data_sz);

	// копирование памяти в арифметический кодер
	memcpy(_acoder.buffer(), data, data_sz);

	// инициализация спектра перед кодированием
	_wtree.wipeout();

	// установка характеристик моделей
	_acoder.use(_mk_acoder_models(tunes.models));

	// декодирование
	_acoder.decode_start();

	_encode_wtree(true);

	_acoder.decode_stop();

	// деквантование
	_wtree.dequantize<wnode::member_wc>(tunes.q);
}



////////////////////////////////////////////////////////////////////////////////
// wtree class protected definitions

/*!	\param[in] s Значение прогнозной величины <i>S<sub>j</sub></i>
	\param[in] lvl Номер уровня разложения, из которого был взят коэффициент
	\return Номер выбираемой модели

	\note Стоит заметить, что для нулевого и первого уровней функция
	возвращает определённые значения, независимо от параметра <i>s</i>.
*/
sz_t encoder::_ind_spec(const pi_t &s, const sz_t lvl) {
	if (subbands::LVL_0 == lvl) return 0;
	if (subbands::LVL_1 == lvl) return 1;

	if (26.0  <= s) return 1;
	if ( 9.8  <= s) return 2;
	if ( 4.1  <= s) return 3;
	if ( 1.72 <= s) return 4;

	return 5;
}


/*!	\param[in] pi Значение прогнозной величины <i>P<sub>i</sub></i>
	\param[in] lvl Номер уровня разложения, из которого был взят групповой
	признак подрезания ветвей
	\return Номер выбираемой модели

	\note Стоит заметить, что если параметр <i>lvl</i> равен <i>0</i>
	функция всегда возвращает нулевую модель, независимо от параметра
	<i>pi</i>.
*/
sz_t encoder::_ind_map(const pi_t &pi, const sz_t lvl) {
	if (subbands::LVL_0 == lvl) return 0;

	if (4.0 <= pi) return 4;
	if (1.1 <= pi) return 3;
	if (0.3 <= pi) return 2;

	return 1;
}


/*!	\param[in] desc Описание моделей арифметического кодера
	\return Модели для арифметического кодера
*/
acoder::models_t encoder::_mk_acoder_models(const models_desc_t &desc)
{
	// создание моделей для кодирования
	acoder::models_t models;
	acoder::model_t model;

	// модел #0 ----------------------------------------------------------------
	model.min = desc.mdl_0_min;
	model.max = desc.mdl_0_max;
	models.push_back(model);

	// модель #1 ---------------------------------------------------------------
	model.min = desc.mdl_1_min;
	model.max = desc.mdl_1_max;
	models.push_back(model);

	// модели #2..#5 -----------------------------------------------------------
	model.min = desc.mdl_x_min;
	model.max = desc.mdl_x_max;

	models.insert(models.end(), ACODER_SPEC_MODELS_COUNT - 2, model);

	// создание моделей для кодирования групповых признаков подрезания ---------
	model.min = 0;
	model.max = 0x7;
	models.push_back(model);

	model.max = 0xF;
	models.insert(models.end(), ACODER_MAP_MODELS_COUNT - 1, model);

	// проверка утверждений
	assert(ACODER_TOTAL_MODELS_COUNT == models.size());

	return models;
}


/*!	\return Описание моделей для арифметического кодера
*/
encoder::models_desc_t encoder::_mk_acoder_smart_models()
{
	// создание моделей для кодирования
	models_desc_t desc;
	
	// модел #0 ----------------------------------------------------------------
	{
		const subbands::subband_t &sb_LL = _wtree.sb().get_LL();
		wtree::coefs_iterator i = _wtree.iterator_over_subband(sb_LL);

		wk_t lvl0_min = 0;
		wk_t lvl0_max = 0;
		_wtree.minmax<wnode::member_wc>(i, lvl0_min, lvl0_max);

		desc.mdl_0_min = short(lvl0_min);
		desc.mdl_0_max = short(lvl0_max);
	}

	// модели #1..#5 -----------------------------------------------------------
	{
		// поиск минимума и максимума на первом уровне
		wtree::coefs_cumulative_iterator i_cum;

		for (sz_t k = 0; subbands::SUBBANDS_ON_LEVEL > k; ++k)
		{
			const subbands::subband_t &sb = _wtree.sb().get(subbands::LVL_1, k);
			i_cum.add(_wtree.iterator_over_subband(sb));
		}

		wk_t lvl1_min = 0;
		wk_t lvl1_max = 0;
		_wtree.minmax<wnode::member_wc>(some_iterator_adapt(i_cum),
										lvl1_min, lvl1_max);

		// поиск минимума и максимума на уровнях начиная со второго
		wtree::coefs_cumulative_iterator j_cum;

		for (sz_t lvl = subbands::LVL_1 + subbands::LVL_NEXT;
			 _wtree.lvls() >= lvl; ++lvl)
		{
			for (sz_t k = 0; subbands::SUBBANDS_ON_LEVEL > k; ++k)
			{
				const subbands::subband_t &sb = _wtree.sb().get(lvl, k);
				j_cum.add(_wtree.iterator_over_subband(sb));
			}
		}

		wk_t lvlx_min = 0;
		wk_t lvlx_max = 0;
		_wtree.minmax<wnode::member_wc>(some_iterator_adapt(j_cum),
										lvlx_min, lvlx_max);

		// модель #1
		desc.mdl_1_min = short(std::min(lvl1_min, lvlx_min));
		desc.mdl_1_max = short(std::max(lvl1_max, lvlx_max));

		// модели #2..#5
		desc.mdl_x_min = lvlx_min;
		desc.mdl_x_max = lvlx_max;
	}

	return desc;
}


/*!	\param[in] m Номер модели для кодирования
	\param[in] wk Значение коэффициента для кодирования
	\return Битовые затраты, необходимые для кодирования коэффициента с
	использованием этой модели
*/
h_t encoder::_h_spec(const sz_t m, const wk_t &wk) {
	return _acoder.enc_entropy(wk, m);
}


/*!	\param[in] m Номер модели для кодирования
	\param[in] n Значение группового признака подрезания ветвей
	\return Битовые затраты, необходимые для кодирования группового
	признака подрезания
*/
h_t encoder::_h_map(const sz_t m, const n_t &n) {
	return _acoder.enc_entropy(n, m + ACODER_SPEC_MODELS_COUNT);
}


/*!	\param[in] m Номер модели для кодирования
	\param[in] wk Значение коэффициента для кодирования
	\param[in] virtual_encode Если <i>true</i> то будет производиться
	виртуальное кодирование (только перенастройка моделей, без помещения
	кодируемого символа в выходной поток).
*/
void encoder::_encode_spec(const sz_t m, const wk_t &wk,
						   const bool virtual_encode)
{
	_acoder.put(wk, m, virtual_encode);
}


/*!	\param[in] m Номер модели для кодирования
	\param[in] n Значение группового признака подрезания ветвей
	\param[in] virtual_encode Если <i>true</i> то будет производиться
	виртуальное кодирование (только перенастройка моделей, без помещения
	кодируемого символа в выходной поток).
*/
void encoder::_encode_map(const sz_t m, const n_t &n,
						  const bool virtual_encode)
{
	_acoder.put(n, m + ACODER_SPEC_MODELS_COUNT, virtual_encode);
}


/*!	\param[in] m Номер модели для кодирования
	\return Значение коэффициента для кодирования
*/
wk_t encoder::_decode_spec(const sz_t m)
{
	return _acoder.get<wk_t>(m);
}


/*!	\param[in] m Номер модели для кодирования
	\return Значение группового признака подрезания ветвей
*/
n_t encoder::_decode_map(const sz_t m)
{
	return _acoder.get<n_t>(m + ACODER_SPEC_MODELS_COUNT);
}


/*!	\param[in] p Предполагаемые координаты элемента (коэффициента)
	\param[in] k Откорректированное (или просто проквантованное) значение
	коэффициента
	\param[in] lambda Параметр <i>lambda</i>, отвечает за <i>Rate/Distortion</i>
	баланс при вычислении <i>RD</i> функции. Чем это значение больше, тем
	больший вклад в значение <i>RD</i> функции будут вносить битовые затраты
	на кодирование коэффициента арифметическим кодером.
	\param[in] model Номер модели арифметического кодера, которая будет
	использована для кодирования коэффициента
	\return Значения <i>RD-функции Лагрнанжа</i>

	\note Функция применима для элементов из любых саббендов.
*/
j_t encoder::_calc_rd_iteration(const p_t &p, const wk_t &k,
								const lambda_t &lambda, const sz_t &model)
{
	const wnode &node = _wtree.at(p);

	const w_t dw = (wnode::dequantize(k, _wtree.q()) - node.w);

	const double h = _h_spec(model, k);

	return (dw*dw + lambda * h);
}


/*!	\param[in] p Координаты коэффициента для корректировки
	\param[in] sb Саббенд, в котором находится коэффициент
	\param[in] lambda Параметр <i>lambda</i> используемый для
	вычисления <i>RD</i> критерия (функции Лагранжа). Представляет
	собой баланс между ошибкой и битовыми затратами.
	\return Значение откорректированного коэффициента

	\note Функция применима для коэффициентов из любых саббендов

	\sa COEF_FIX_USE_4_POINTS

	\todo Написать тест для этой функции
*/
wk_t encoder::_coef_fix(const p_t &p, const subbands::subband_t &sb,
						const lambda_t &lambda)
{
	#ifdef COEF_FIX_DISABLED
	return _wtree.at(p).wq;
	#endif

	// выбор модели и оригинального значения коэффициента
	const sz_t	model	= _ind_spec<wnode::member_wc>(p, sb);
	const wk_t	&wq		= _wtree.at(p).wq;

	// Определение набора подбираемых значений
	#ifdef COEF_FIX_USE_4_POINTS
		static const sz_t vals_count	= 4;
		const wk_t w_vals[vals_count]	= {0, wq, wq + 1, wq - 1};
	#else
		static const sz_t vals_count	= 3;
		const wk_t w_drift				= (0 <= wq)? -1: +1;
		const wk_t w_vals[vals_count]	= {0, wq, wq + w_drift};
	#endif

	// начальные значения для поиска минимума RD функции
	wk_t k_optim = w_vals[0];
	j_t j_optim = _calc_rd_iteration(p, k_optim, lambda, model);

	// поиск минимального значения RD функции
	for (int i = 1; vals_count > i; ++i) {
		const wk_t &k = w_vals[i];
		const j_t j = _calc_rd_iteration(p, k, lambda, model);
		if (j < j_optim) {
			j_optim = j;
			k_optim = k;
		}
	}

	// возврат откорректированного значения коэффициента
	return k_optim;
}


/*!	\param[in] p Координаты родительского элемента
	\param[in] sb_j Саббенд, в котором находятся дочерние элементы
	\param[in] lambda Параметр <i>lambda</i> используемый для
	вычисления <i>RD</i> критерия (функции Лагранжа). Представляет
	собой баланс между ошибкой и битовыми затратами.

	\note Функция применима только для родительских элементов не из
	<i>LL</i> саббенда.
*/
void encoder::_coefs_fix(const p_t &p, const subbands::subband_t &sb_j,
						 const lambda_t &lambda)
{
	// цикл по дочерним элементам
	for (wtree::coefs_iterator i = _wtree.iterator_over_children(p);
		 !i->end(); i->next())
	{
		const p_t &p = i->get();
		wnode &node = _wtree.at(p);
		node.wc = _coef_fix(p, sb_j, lambda);
	}
}


/*!	\param[in] p Координаты родительского элемента
	\param[in] sb_j Саббенд, в котором находятся дочерние элементы
	\param[in] lambda Параметр <i>lambda</i> используемый для
	вычисления <i>RD</i> критерия (функции Лагранжа). Представляет
	собой баланс между ошибкой и битовыми затратами.

	\note Функция применима только для родительских элементов из
	любых саббендов.
*/
void encoder::_coefs_fix_uni(const p_t &p, const subbands::subband_t &sb_j,
							 const lambda_t &lambda)
{
	// цикл по дочерним элементам
	for (wtree::coefs_iterator i = _wtree.iterator_over_children_uni(p);
		 !i->end(); i->next())
	{
		const p_t &p = i->get();
		wnode &node = _wtree.at(p);
		node.wc = _coef_fix(p, sb_j, lambda);
	}
}


/*!	\param[in] p Координаты элемента для которого будет расчитываться
	\param[in] sb Саббенд, в котором находятся коэффициенты из сохраняемой
	ветви. Другими словами, этот саббенд дочерний для того, в котором
	находится элемент с координатами <i>p</i>.
	\param[in] lambda Параметр <i>lambda</i> который участвует в вычислении
	<i>RD</i> функции и представляет собой баланс между <i>R (rate)</i> и
	<i>D (distortion)</i> частями <i>функции Лагранжа</i>.
	\return Значение <i>RD функции Лагранжа</i>.

	\note Функция не применима для элементов из <i>LL</i> саббенда.

	\sa _calc_j0_value()

	\todo Необходимо написать тест для этой функции.
*/
j_t encoder::_calc_j1_value(const p_t &p, const subbands::subband_t &sb,
							const lambda_t &lambda)
{
	// получение номера модели для кодирования коэффициентов
	const sz_t model = _ind_spec<wnode::member_wc>(p, sb);

	j_t j1 = 0;

	for (wtree::coefs_iterator i = _wtree.iterator_over_children(p);
		 !i->end(); i->next())
	{
		const wnode &node = _wtree.at(i->get());
		j1 += _calc_rd_iteration(i->get(), node.wc, lambda, model);
	}

	return j1;
}


/*!	\param[in] root Координаты корневого элемента
	\param[in] j_map Значение <i>RD-функцию Лагранжа</i> полученное
	в шаге 2.6.
	\param[in] lambda Параметр <i>lambda</i> который участвует в вычислении
	<i>RD</i> функции и представляет собой баланс между <i>R (rate)</i> и
	<i>D (distortion)</i> частями <i>функции Лагранжа</i>.
	\return Значение <i>RD-функции Лагранжа</i>.

	Функция также сохраняет полученное значение <i>RD-функции Лагранжа</i> в
	полях wnode::j0 и wnode::j1 корневого элемента.
*/
j_t encoder::_calc_jx_value(const p_t &root, const j_t &j_map,
							const lambda_t &lambda)
{
	assert(_wtree.sb().test_LL(root));

	// получаем ссылку на LL саббенд
	const subbands::subband_t &sb_LL = _wtree.sb().get_LL();

	j_t j = j_map;

	// цикл по дочерним элементам
	for (wtree::coefs_iterator i = _wtree.iterator_over_LL_children(root);
		 !i->end(); i->next())
	{
		const p_t &p = i->get();

		const wnode &node = _wtree.at(p);

		j += _calc_rd_iteration(p, node.wc, lambda,
								_ind_spec<wnode::member_wc>(p, sb_LL));
	}

	wnode &node = _wtree.at(root);

	return (node.j0 = node.j1 = j);
}


/*!	\param[in] p Координаты элемента, для которого выполняется подготовка
	значений <i>J</i> (<i>RD функция Лагранжа</i>) 
	\param[in] sb_j Саббенд в котором находятся дочерние элементы
	\param[in] lambda Параметр <i>lambda</i> используемый для
	вычисления <i>RD</i> критерия (функции Лагранжа). Представляет
	собой баланс между ошибкой и битовыми затратами.

	Функция обычно выполняется при переходе с уровня <i>lvl</i>, на
	уровень <i>lvl + subbands::LVL_PREV</i>.

	\note Функция не применима для элементов из <i>LL</i> саббенда.
*/
void encoder::_prepare_j(const p_t &p, const subbands::subband_t &sb_j,
						 const lambda_t &lambda)
{
	wnode &node = _wtree.at(p);

	node.j0 = _calc_j0_value<false>(p);
	node.j1 = _calc_j1_value(p, sb_j, lambda);
}


/*!	\param[in] p Координаты элемента, для которого выполняется подготовка
	значений <i>J</i> (<i>RD функция Лагранжа</i>) 
	\param[in] sb_j Саббенд в котором находятся дочерние элементы
	\param[in] j Значение функции Лагранжа, полученное при подборе
	оптимальной топологии ветвей
	\param[in] lambda Параметр <i>lambda</i> используемый для
	вычисления <i>RD</i> критерия (функции Лагранжа). Представляет
	собой баланс между ошибкой и битовыми затратами.

	Функция обычно выполняется при переходе с уровня <i>lvl</i>, на
	уровень <i>lvl + subbands::LVL_PREV</i>.

	\note Функция не применима для элементов из <i>LL</i> саббенда.
*/
void encoder::_prepare_j(const p_t &p, const subbands::subband_t &sb_j,
						 const j_t &j, const lambda_t &lambda)
{
	wnode &node = _wtree.at(p);

	node.j0 = _calc_j0_value<true>(p);
	node.j1 = j + _calc_j1_value(p, sb_j, lambda);
}


/*!	\param[in] branch Координаты элемента, находящегося в вершине
	ветви
	\param[in] n Групповой признак подрезания, характеризующий
	топологию ветви
	\return Значение функции Лагранжа при топологии описанной в
	<i>n</i>
*/
j_t encoder::_topology_calc_j(const p_t &branch, const n_t n)
{
	j_t j = 0;

	for (wtree::coefs_iterator i =
				_wtree.iterator_over_children(branch);
		 !i->end(); i->next())
	{
		const p_t &p = i->get();
		const wnode &node = _wtree.at(p);
		const n_t mask = _wtree.child_n_mask(p, branch);
		j += (_wtree.test_n_mask(n, mask))? node.j1: node.j0;
	}

	return j;
}


//!	определённой её топологии (ветвь из <i>LL</i> саббенда)
/*!	\param[in] branch Координаты элемента, находящегося в вершине
	ветви
	\param[in] n Групповой признак подрезания, характеризующий
	топологию ветви
	\return Значение функции Лагранжа при топологии описанной в
	<i>n</i>
*/
j_t encoder::_topology_calc_j_LL(const p_t &branch, const n_t n)
{
	// начальное значение для функции Лагранжа
	j_t j = 0;

	for (wtree::coefs_iterator i =
				_wtree.iterator_over_LL_children(branch);
		 !i->end(); i->next())
	{
		const p_t &p = i->get();
		const wnode &node = _wtree.at(p);
		const n_t mask = _wtree.child_n_mask_LL(p);
		j += (_wtree.test_n_mask(n, mask))? node.j1: node.j0;
	}

	return j;
}


/*!	\param[in] branch Координаты родительского элемента, дающего
	начало ветви.
	\param[in] sb Саббенд, содержащий элемент <i>branch</i>
	\param[in] lambda Параметр <i>lambda</i> который участвует в
	вычислении <i>RD</i> функции и представляет собой баланс между
	<i>R (rate)</i> и <i>D (distortion)</i> частями функции
	<i>Лагранжа</i>.
	\return Групповой признак подрезания ветвей

	Алгоритм оптимизации топологии подробно описан в <i>35.pdf</i>

	\note Функция применима для всех ветвей (как берущих начало в
	<i>LL</i> саббенде, так и для всех остальных).
*/
encoder::_branch_topology_t
encoder::_optimize_branch_topology(const p_t &branch,
								   const subbands::subband_t &sb,
								   const lambda_t &lambda)
{
	// получение дочернего саббенда
	const sz_t lvl_j = sb.lvl + subbands::LVL_NEXT;
	const subbands::subband_t &sb_j = _wtree.sb().get(lvl_j, sb.i);

	// выбор модели для кодирования групповых признаков подрезания
	const sz_t model = _ind_map<wnode::member_wc>(branch, sb_j);

	// поиск наиболее оптимальной топологии
	wtree::n_iterator i = _wtree.iterator_through_n(sb.lvl);

	// первая итерация цикла поиска
	_branch_topology_t optim_topology;
	optim_topology.n	= i->get();
	optim_topology.j	= _topology_calc_j_uni(branch,
											   optim_topology.n);

	// последующие итерации
	for (i->next(); !i->end(); i->next())
	{
		const n_t &n = i->get();

		const j_t j_sum = _topology_calc_j_uni(branch, n);

		const j_t j = (j_sum + lambda * _h_map(model, n));

		if (j < optim_topology.j) {
			optim_topology.j = j;
			optim_topology.n = n;
		}
	}

	return optim_topology;
}


/*!	\param[in] root Координаты корневого элемента
	\param[in] virtual_encode Если <i>true</i>, то будет производиться
	виртуальное кодирование (только перенастройка моделей, без помещения
	кодируемого символа в выходной поток).

	Функция выполняет кодирование:
	- Коэффициента при корневом элементе
	- Группового признака подрезания при корневом элементе
	- Коэффициентов принадлежащих дереву с первого уровня разложения
*/
void encoder::_encode_tree_root(const p_t &root,
								const bool virtual_encode)
{
	// получение корневого элемента
	const wnode &root_node = _wtree.at(root);

	// закодировать коэффициент с нулевого уровня
	_encode_spec(_ind_spec(0, subbands::LVL_0), root_node.wc,
				 virtual_encode);

	// закодировать групповой признак подрезания с нулевого уровня
	_encode_map(_ind_map(0, subbands::LVL_0), root_node.n,
				virtual_encode);

	// кодирование дочерних коэффициентов с первого уровня
	for (wtree::coefs_iterator i = _wtree.iterator_over_LL_children(root);
		 !i->end(); i->next())
	{
		// закодировать коэффициенты с первого уровня
		_encode_spec(_ind_spec(0, subbands::LVL_1), _wtree.at(i->get()).wc,
					 virtual_encode);
	}
}


/*!	\param[in] root Координаты корнвого элемента
	\param[in] lvl Номер уровня разложения
	\param[in] virtual_encode Если <i>true</i>, то будет производиться
	виртуальное кодирование (только перенастройка моделей, без помещения
	кодируемого символа в выходной поток).
*/
void encoder::_encode_tree_leafs(const p_t &root, const sz_t lvl,
								 const bool virtual_encode)
{
	// псевдонимы для номеров уровней
	const sz_t lvl_g = lvl;
	const sz_t lvl_j = lvl_g + subbands::LVL_PREV;
	const sz_t lvl_i = lvl_j + subbands::LVL_PREV;

	// цикл по саббендам в уровне
	for (sz_t k = 0; _wtree.sb().subbands_on_lvl(lvl) > k; ++k)
	{
		const subbands::subband_t &sb_g = _wtree.sb().get(lvl_g, k);
		const subbands::subband_t &sb_j = _wtree.sb().get(lvl_j, k);

		// кодирование коэффициентов
		for (wtree::coefs_iterator g = _wtree.iterator_over_leafs(root, sb_g);
			 !g->end(); g->next())
		{
			const p_t &p_g = g->get();

			wnode &node_g = _wtree.at(p_g);

			if (node_g.invalid) continue;

			const sz_t model = _ind_spec<wnode::member_wc>(p_g, sb_g);

			_encode_spec(model, node_g.wc, virtual_encode);
		}

		// на предпоследнем уровне нет групповых признаков подрезания
		if (_wtree.lvls() == lvl) continue;

		// кодирование групповых признаков подрезания
		for (wtree::coefs_iterator j = _wtree.iterator_over_leafs(root, sb_j);
			 !j->end(); j->next())
		{
			const p_t &p_j = j->get();

			const p_t &p_i = _wtree.prnt_uni(p_j);
			const wnode &node_i = _wtree.at(p_i);

			// маска подрезания, где текущий элемент не подрезан
			const n_t mask = _wtree.child_n_mask_uni(p_j, p_i);

			// переходим к следующему потомку, если ветвь подрезана
			if (!_wtree.test_n_mask(node_i.n, mask)) continue;

			wnode &node_j = _wtree.at(p_j);

			const sz_t model = _ind_map<wnode::member_wc>(p_j, sb_g);

			_encode_map(model, node_j.n, virtual_encode);
		}
	}
}


/*!	\param[in] root Координаты корневого элемента дерева
	\param[in] virtual_encode Если <i>true</i>, то будет производиться
	виртуальное кодирование (только перенастройка моделей, без помещения
	кодируемого символа в выходной поток).
*/
void encoder::_encode_tree(const p_t &root,
						   const bool virtual_encode)
{
	// кодирование корневого и дочерних элементов дерева
	_encode_tree_root(root, virtual_encode);

	// кодирование элементов дерева на остальных уровнях
	const sz_t first_lvl = subbands::LVL_1 + subbands::LVL_NEXT;
	const sz_t final_lvl = _wtree.lvls();

	for (sz_t lvl = first_lvl; final_lvl >= lvl; ++lvl)
	{
		_encode_tree_leafs(root, lvl, virtual_encode);
	}
}


/*!	\param[in] decode_mode Если <i>false</i> функция будет выполнять
	кодирование спектра, иначе (если <i>true</i>) будет выполнять
	декодирование спектра.
*/
void encoder::_encode_wtree_root(const bool decode_mode)
{
	// LL cаббенд
	const subbands::subband_t &sb_LL = _wtree.sb().get_LL();

	// модели для кодирования в LL саббенде
	const sz_t spec_LL_model = _ind_spec(0, sb_LL.lvl);
	const sz_t map_LL_model = _ind_map(0, sb_LL.lvl);

	// (де)кодирование коэффициентов и групповых признаков подрезания
	// из LL саббенда
	for (wtree::coefs_iterator i = _wtree.iterator_over_subband(sb_LL);
		 !i->end(); i->next())
	{
		// координаты элемента
		const p_t &p_i = i->get();

		// сам элемент из LL саббенда
		wnode &node = _wtree.at(p_i);

		if (decode_mode)
		{
			// декодирование коэффициента и признака подрезания
			node.wc = _decode_spec(spec_LL_model);
			node.n = _decode_map(map_LL_model);

			// порождение ветвей в соответствии с полученным признаком
			// подрезания
			_wtree.uncut_leafs(p_i, node.n);
		}
		else
		{
			// кодирование коэффициента и признака подрезания
			_encode_spec(spec_LL_model, node.wc);
			_encode_map(map_LL_model, node.n);
		}
	}

	// модель для кодирования коэффициентов с первого уровня
	const sz_t spec_1_model = _ind_spec(0, subbands::LVL_1);

	// (де)кодирование коэффициентов из саббендов первого уровня
	for (sz_t k = 0; subbands::SUBBANDS_ON_LEVEL > k; ++k)
	{
		// очередной саббенд с первого уровня
		const subbands::subband_t &sb = _wtree.sb().get(subbands::LVL_1, k);

		// цикл по всем элементам из саббенда с первого уровня
		for (wtree::coefs_iterator i = _wtree.iterator_over_subband(sb);
			 !i->end(); i->next())
		{
			// (де)кодирование очерендного коэффициента
			if (decode_mode)
				_wtree.at(i->get()).wc = _decode_spec(spec_1_model);
			else
				_encode_spec(spec_1_model, _wtree.at(i->get()).wc);
		}
	}
}


/*!	\param[in] lvl Номер уровня, коэффициенты на котором будут
	закодированы
	\param[in] decode_mode Если <i>false</i> функция будет выполнять
	кодирование спектра, иначе (если <i>true</i>) будет выполнять
	декодирование спектра.

	\todo Возможно не стоит делать wtree::uncut_leafs() при попадании в
	подрезанную ветвь при кодирование групповыйх признаков подрезания.
	Вместо этого можно перед декодированием проинициализировать все поля
	wnode::invalid значением <i>true</i>
*/
void encoder::_encode_wtree_level(const sz_t lvl,
								  const bool decode_mode)
{
	// определение псевдонимов для номеров уровней
	const sz_t lvl_g = lvl;
	const sz_t lvl_j = lvl_g + subbands::LVL_PREV;
	const sz_t lvl_i = lvl_j + subbands::LVL_PREV;

	// цикл по саббендам на уровне
	for (sz_t k = 0; _wtree.sb().subbands_on_lvl(lvl) > k; ++k)
	{
		// саббенд на уровне коэффициенты из которого будут
		// закодированы и родительский для для него
		const subbands::subband_t &sb_g = _wtree.sb().get(lvl_g, k);
		const subbands::subband_t &sb_j = _wtree.sb().get(lvl_j, k);

		// кодирование коэффициентов
		for (wtree::coefs_iterator g = _wtree.iterator_over_subband(sb_g);
			 !g->end(); g->next())
		{
			// координаты элемента
			const p_t &p_g = g->get();

			// сам элемент
			wnode &node_g = _wtree.at(p_g);

			// переходим к следующему, если коэффициента попал в
			// подрезанную ветвь
			if (node_g.invalid) continue;

			// выбираем модель для (де)кодирования коэффициента
			const sz_t model = _ind_spec<wnode::member_wc>(p_g, sb_g);

			// (де)кодирование коэффициента
			if (decode_mode)
				node_g.wc = _decode_spec(model);
			else
				_encode_spec(model, node_g.wc);
		}

		// на предпоследнем уровне нет групповых признаков подрезания
		if (_wtree.lvls() == lvl) continue;

		// кодирование групповых признаков подрезания
		for (wtree::coefs_iterator j = _wtree.iterator_over_subband(sb_j);
			 !j->end(); j->next())
		{
			// координаты элемента
			const p_t &p_j = j->get();

			// координаты родительского элемента
			const p_t &p_i = _wtree.prnt_uni(p_j);

			// ссылка на родительский элемент
			const wnode &node_i = _wtree.at(p_i);

			// маска подрезания, где текущий элемент не подрезан
			const n_t mask = _wtree.child_n_mask_uni(p_j, p_i);

			// переходим к следующему потомку, если ветвь подрезана
			if (!_wtree.test_n_mask(node_i.n, mask)) continue;

			// значение элемента
			wnode &node_j = _wtree.at(p_j);

			// выбор модели для кодирования группового признака подрезания
			const sz_t model = _ind_map<wnode::member_wc>(p_j, sb_g);

			if (decode_mode)
			{
				// декодирование признака подрезания
				node_j.n = _decode_map(model);

				// порождение ветвей в соответствии с полученным признаком
				// подрезания
				_wtree.uncut_leafs(p_j, node_j.n);
			}
			else
			{
				// кодирование признака подрезания
				_encode_map(model, node_j.n);
			}
		}
	}
}


/*!	\param[in] decode_mode Если <i>false</i> функция будет выполнять
	кодирование спектра, иначе (если <i>true</i>) будет выполнять
	декодирование спектра.
*/
void encoder::_encode_wtree(const bool decode_mode)
{
	// (де)кодирование корневых элементов
	_encode_wtree_root(decode_mode);

	// (де)кодирование остальных элементов
	const sz_t first_lvl = subbands::LVL_1 + subbands::LVL_NEXT;
	const sz_t final_lvl = _wtree.lvls();

	// цикл по уровням
	for (sz_t lvl = first_lvl; final_lvl >= lvl; ++lvl)
	{
		_encode_wtree_level(lvl, decode_mode);
	}
}


/*!	\param[in] root Координаты корневого элемента рассматриваемого дерева
	\param[in] lambda Параметр <i>lambda</i> используемый для
	вычисления <i>RD</i> критерия (функции Лагранжа). Представляет
	собой баланс между ошибкой и битовыми затратами.

	На первом шаге кодирования выполняется корректировка коэффициентов
	на самом последнем (с наибольшей площадью) уровне разложения и
	последующий расчёт <i>RD-функций Лагранжа</i> для вариантов сохранения
	и подрезания оконечных листьев дерева.
*/
void encoder::_optimize_tree_step_1(const p_t &root, const lambda_t &lambda)
{
	// просматриваются все узлы предпоследнего уровня
	const sz_t lvl_i = _wtree.sb().lvls() + subbands::LVL_PREV;
	const sz_t lvl_j = _wtree.sb().lvls();

	// цикл по саббендам
	for (sz_t k = 0; subbands::SUBBANDS_ON_LEVEL > k; ++k)
	{
		const subbands::subband_t &sb_i = _wtree.sb().get(lvl_i, k);
		const subbands::subband_t &sb_j = _wtree.sb().get(lvl_j, k);

		// цикл по элементам из предпоследнего уровня дерева
		for (wtree::coefs_iterator i = _wtree.iterator_over_leafs(root, sb_i);
			!i->end(); i->next())
		{
			// родительский элемент из предпоследнего уровня
			const p_t &p = i->get();

			// Шаг 1.1. Корректировка проквантованных коэффициентов
			_coefs_fix(p, sb_j, lambda);

			// Шаг 1.2. Расчет RD-функций Лагранжа для вариантов сохранения и
			// подрезания листьев
			_prepare_j(p, sb_j, lambda);
		}
	}
}


/*!	\param[in] root Координаты корневого элемента дерева
	\param[in] lambda Параметр <i>lambda</i> используемый для
	вычисления <i>RD</i> критерия (функции Лагранжа). Представляет
	собой баланс между ошибкой и битовыми затратами.
*/
void encoder::_optimize_tree_step_2(const p_t &root, const lambda_t &lambda)
{
	// Шаги 2.1 - 2.5 ----------------------------------------------------------

	// цикл по уровням
	for (sz_t lvl_i = _wtree.sb().lvls() + 2*subbands::LVL_PREV;
		 0 < lvl_i; --lvl_i)
	{
		const sz_t lvl_j = lvl_i + subbands::LVL_NEXT;

		// цикл по саббендам
		for (sz_t k = 0; subbands::SUBBANDS_ON_LEVEL > k; ++k) {

			// получение ссылок на саббенды
			const subbands::subband_t &sb_i = _wtree.sb().get(lvl_i, k);
			const subbands::subband_t &sb_j = _wtree.sb().get(lvl_j, k);

			// цикл по родительским элементам
			for (wtree::coefs_iterator i =
						_wtree.iterator_over_leafs(root, sb_i);
				 !i->end(); i->next())
			{
				const p_t &p = i->get();

				// Шаг 2.1. Определение оптимальной топологии ветвей
				const _branch_topology_t optim_topology =
						_optimize_branch_topology(p, sb_i, lambda);

				// Шаг 2.2. Изменение топологии дерева
				_wtree.cut_leafs<wnode::member_wc>(p, optim_topology.n);

				// Шаг 2.3. Корректировка проквантованных коэффициентов
				_coefs_fix(p, sb_j, lambda);

				// Шаг 2.4. Подготовка для просмотра следующего уровня
				_prepare_j(p, sb_j, optim_topology.j, lambda);
			}
		}
		// на всякий случай, если какой-нить фрик сделает sz_t беззнаковым
		// типом :^)
		// if (0 == lvl) break;
	}
}


/*!	\param[in] root Координаты корневого элемента дерева
	\param[in] lambda Параметр <i>lambda</i> используемый для
	вычисления <i>RD</i> критерия (функции Лагранжа). Представляет
	собой баланс между ошибкой и битовыми затратами.
*/
void encoder::_optimize_tree_step_3(const p_t &root, const lambda_t &lambda)
{
	const subbands::subband_t &sb_LL = _wtree.sb().get_LL();

	// Шаг 2.6. Определение оптимальной топологии ветвей
	const _branch_topology_t optim_topology =
			_optimize_branch_topology(root, sb_LL, lambda);

	// Шаг 2.7. Изменение топологии дерева
	_wtree.cut_leafs<wnode::member_wc>(root, optim_topology.n);

	// шаг 3. Вычисление RD-функции Лагранжа для всего дерева
	_calc_jx_value(root, optim_topology.j, lambda);
}


/*!	\param[in] root Координаты корневого элемента дерева
	\param[in] lambda Параметр <i>lambda</i> используемый для
	вычисления <i>RD</i> критерия (функции Лагранжа). Представляет
	собой баланс между ошибкой и битовыми затратами.
	\return Значение <i>RD-функций Лагранжа</i> для дерева

	Для корректной работы функции необходимо, чтобы в дереве все ветви
	были отмечены как подрезанные, а элементы как корректные. Значения
	функций <i>Лагранжа</i> в спектре должны быть обнулены. Арифметический
	кодер должен быть настроен на корректные модели (смотри acoder::use()).

	\note Необходимую подготовку выполняет функция wnode::filling_refresh(),
	при условии, что поля wnode::w и wnode::wq корректны.
*/
j_t encoder::_optimize_tree(const p_t &root, const lambda_t &lambda)
{
	_optimize_tree_step_1(root, lambda);
	_optimize_tree_step_2(root, lambda);
	_optimize_tree_step_3(root, lambda);

	return (_wtree.at(root).j1);
}


/*!	\param[in] lambda Параметр <i>lambda</i> используемый для
	вычисления <i>RD</i> критерия (функции Лагранжа). Представляет
	собой баланс между ошибкой и битовыми затратами.
	\param[in] virtual_encode Если <i>true</i>, то будет производиться
	виртуальное кодирование (только перенастройка моделей, без помещения
	кодируемого символа в выходной поток). При включённом виртуальном
	кодировании, поле _optimize_result_t::bpp выставляется в 0, так как
	без проведения реального кодирование невозможно оценить битовые
	затраты.
	\return Результат проведённой оптимизации

	Для корректной работы этой функции необходимо, чтобы поля wnode::w и
	wnode::wq элементов спектра были корректны. В спектре все ветви должны
	быть отмечены как подрезанные, а элементы как корректные. Значения
	функций <i>Лагранжа</i> в спектре должны быть обнулены. Арифметический
	кодер должен быть настроен на корректные модели (смотри acoder::use()).

	\note Необходимую подготовку выполняет функция wtree::filling_refresh(),
	при условии, что поля wnode::w и wnode::wq корректны.

	\note Если определён макрос #LIBWIC_DEBUG, функция будет выводить
	специальную отладочную информацию.
*/
encoder::optimize_result_t
encoder::_optimize_wtree(const lambda_t &lambda,
						 const bool virtual_encode)
{
	// инициализация возвращаемого результата
	optimize_result_t result;
	result.q		= _wtree.q();
	result.lambda	= lambda;
	result.j		= 0;
	result.bpp		= 0;

	// оптимизация топологии ветвей с кодированием
	_acoder.encode_start();

	for (wtree::coefs_iterator i =
				_wtree.iterator_over_subband(_wtree.sb().get_LL());
		 !i->end(); i->next())
	{
		// корень очередного дерева коэффициентов вейвлет преобразования
		const p_t &root = i->get();

		// оптимизация топологии отдельной ветви
		result.j += _optimize_tree(root, lambda);

		// кодирование отдельной ветви
		_encode_tree(root, virtual_encode);
	}

	_acoder.encode_stop();

	// подсчёт bpp, если производилось реальное кодирование
	if (!virtual_encode)
	{
		result.bpp = _calc_encoded_bpp();
	}

	// вывод отладочной информации
	#ifdef LIBWIC_DEBUG
	if (_dbg_out_stream.good())
	{
		_dbg_out_stream << "[OWTR]: ";
		_dbg_out_stream << "q: " << std::setw(8) << _wtree.q();
		_dbg_out_stream << " lambda: " << std::setw(8) << lambda;
		_dbg_out_stream << " j: " << std::setw(8) << result.j;
		_dbg_out_stream << " bpp: " << std::setw(8) << result.bpp;
		_dbg_out_stream << std::endl;
	}
	#endif

	// обратный вызов пользовательской функции
	if (0 != _optimize_callback)
	{
		_optimize_callback(result, _optimize_callback_param);
	}

	// возврат результата
	return result;
}


/*!	\param[in] lambda Параметр <i>lambda</i> используемый для
	вычисления <i>RD</i> критерия (функции Лагранжа). Представляет
	собой баланс между ошибкой и битовыми затратами.
	\param[in] q Квантователь
	\param[out] models Описание моделей арифметического кодера
	\param[in] virtual_encode Если <i>true</i> то будет производиться
	виртуальное кодирование (только перенастройка моделей, без помещения
	кодируемого символа в выходной поток).
	\return Результат проведённой оптимизации

	Эта версия функции <i>%encoder::_optimize_wtree()</i> сама производит
	квантование и настройку моделей арифметического кодера. Для корректной
	работы функции достаточно загруженных коэффициентов в поле wnode::w
	элементов спектра.
*/
encoder::optimize_result_t
encoder::_optimize_wtree(const lambda_t &lambda,
						 const q_t &q, models_desc_t &models,
						 const bool virtual_encode)
{
	// квантование коэффициентов
	_wtree.quantize(q);

	// определение суб-оптимальных моделей для арифметического кодера
	models = _mk_acoder_smart_models();

	// загрузка моделей в арифметический кодер
	_acoder.use(_mk_acoder_models(models));

	// оптимизация топологии ветвей
	return _optimize_wtree(lambda, virtual_encode);
}


/*!	\param[in] lambda Параметр <i>lambda</i> используемый для вычисления
	<i>RD критерия</i> (<i>функции Лагранжа<i>). Представляет собой баланс
	между ошибкой и битовыми затратами. Меньшие значения <i>lambda</i>
	соответствуют большему значению результиру<i>bpp</i>.

	\param[in] q_min Нижняя граница интервала поиска (минимальное
	значение)

	\param[in] q_max Верхняя граница интервала поиска (максимальное
	значение)

	\param[in] q_eps Необходимая погрешность определения квантователя
	<i>q</i> при котором значение <i>RD функции Лагранжа</i> минимально
	(при фиксированном параметре <i>lambda</i>).

	\param[out] models Описание моделей арифметического кодера, которое
	необходимо для последующего декодирования изображения

	\param[in] j_eps Необходимая погрешность нахождения минимума <i>RD
	функции Лагранжа</i>. Так как абсолютная величина функции <i>J</i>
	зависит от многих факторов (таких как размер изображения, его тип,
	величины параметра <i>lambda</i>), использовать это параметр
	затруднительно. Поэтому его значение по умолчанию равно <i>0</i>,
	чтобы при поиске оптимального квантователя учитывалась только
	погрешность параметра <i>q</i>.

	\param[in] virtual_encode Если <i>true</i> то будет производиться
	виртуальное кодирование (только перенастройка моделей, без помещения
	кодируемого символа в выходной поток). Виртуальное кодирование немного
	быстрее, но его использование делает невозможным определение средних
	битовых затрат на кодирование одного пиксела изображения.

	\return Результат произведённого поиска

	Данная функция использует метод золотого сечения для поиска минимума
	<i>RD функции Лагранжа</i>.

	\verbatim
	  j|___                              ______/
	   |   \_                         __/   |
	   |     \___                    /      |
	   |       | \____              /       |
	   |       |   |  \___   ______/        |
	   |       |   |      \_/     |         |
	   |       |   |       |      |         |
	---+-------+---+-------+------+---------+----------> q
	  0|       a   b     j_min    c        d
	\endverbatim

	\note Для корректной работы этой функции необходимо, чтобы поля
	wnode::w и wnode::wq элементов спектра были корректны. Для этого
	можно использовать функцию wtree::cheap_load().

	\note Если определён макрос #LIBWIC_DEBUG, функция будет выводить
	специальную отладочную информацию.
*/
encoder::optimize_result_t
encoder::_search_q_min_j(const lambda_t &lambda,
						 const q_t &q_min, const q_t &q_max,
						 const q_t &q_eps, models_desc_t &models,
						 const j_t &j_eps, const bool virtual_encode,
						 const sz_t &max_iterations)
{
	// проверка утверждений
	assert(0 <= lambda);
	assert(1 <= q_min && q_min <= q_max);
	assert(0 <= q_eps && 0 <= j_eps);

	// коэффициенты золотого сечения
	static const q_t factor_b	= (q_t(3) - sqrt(q_t(5))) / q_t(2);
	static const q_t factor_c	= (sqrt(q_t(5)) - q_t(1)) / q_t(2);

	// установка диапазона для поиска
	q_t q_a = q_min;
	q_t q_d = q_max;

	// вычисление значений в первых двух точках
	q_t q_b = q_a + factor_b * (q_d - q_a);
	q_t q_c = q_a + factor_c * (q_d - q_a);

	optimize_result_t result_b = _optimize_wtree(lambda, q_b, models,
												 virtual_encode);
	optimize_result_t result_c = _optimize_wtree(lambda, q_c, models,
												 virtual_encode);

	// запоминание предыдущего и последнего результатов
	optimize_result_t result_prev	= result_b;
	optimize_result_t result		= result_c;

	// подсчёт количества итераций
	sz_t iterations					= 0;

	// поиск оптимального значения q
	for (;;)
	{
		// проверка, достигнута ли нужная точность
		if (q_eps >= abs(q_c - q_b) ||
			j_eps >= abs(result.j - result_prev.j))
		{
			break;
		}

		if (0 < max_iterations && max_iterations <= iterations)
		{
			break;
		}

		// скоро будет получен новый результат
		result_prev = result;

		// выбор очередного значения q
		if (result_b.j < result_c.j)
		{
			q_d = q_c;
			q_c = q_b;
			q_b = q_a + factor_b*(q_d - q_a);

			result_c = result_b;

			result = result_b = _optimize_wtree(lambda, q_b,
												models, virtual_encode);
		}
		else
		{
			q_a = q_b;
			q_b = q_c;
			q_c = q_a + factor_c*(q_d - q_a);

			result_b = result_c;

			result = result_c = _optimize_wtree(lambda, q_c,
												models, virtual_encode);
		}

		// увеличение количества итераций
		++iterations;
	}

	// вывод отладочной информации
	#ifdef LIBWIC_DEBUG
	if (_dbg_out_stream.good())
	{
		_dbg_out_stream << "[SQMJ]: ";
		_dbg_out_stream << "q: " << std::setw(8) << result.q;
		_dbg_out_stream << " lambda: " << std::setw(8) << result.lambda;
		_dbg_out_stream << " j: " << std::setw(8) << result.j;
		_dbg_out_stream << " bpp: " << std::setw(8) << result.bpp;
		_dbg_out_stream << std::endl;
	}
	#endif

	// возвращение полученного результата
	return result;
}


/*! \param[in] bpp Необходимый битрейт (Bits Per Pixel), для достижения
	которого будет подбираться параметр <i>lambda</i>

	\param[in] lambda_min Нижняя граница интервала поиска (минимальное
	значение)

	\param[in] lambda_max Верхняя граница интервала поиска (максимальное
	значение)

	\param[in] bpp_eps Точность, c которой <i>bpp</i> полученнй в результате
	оптимизации топологии будет соответствовать искомому.

	\param[in] lambda_eps Точность, с которой будет подбираться параметр
	<i>lambda</i>.

	\param[in] virtual_encode Если <i>true</i> то будет производиться
	виртуальное кодирование (только перенастройка моделей, без помещения
	кодируемого символа в выходной поток).

	\return Результат проведённого поиска. Возможна ситуация, когда нужная
	<i>lambda</i> лежит вне указанного диапазона. В этом случае, функция
	подберёт такую <i>lambda</i>, которая максимально удовлетворяет условиям
	поиска.

	Поиск оптимального значения <i>lambda</i> производится дихотомией
	(методом половинного деления). Поиск будет считаться успешным, если
	найдено такое значение <i>lambda</i>, при котором полученный <i>bpp</i>
	меньше чем на <i>bpp_eps</i> отличается от заданного <i>или</i> в
	процессе поиска диапазон значений <i>lambda</i> сузился до
	<i>lambda_eps</i>.

	Меньшие значения <i>lambda</i> соответствуют большему
	значению <i>bpp</i>:
	\verbatim
	   |_____
	   |     \________
	   |       |      \_________
	bpp+-------+----------------\-----------------------
	   |       |                 \_______
	   |       |                   |     \______________
	---+-------+-------------------+--------------------
	  0|       lambda_min          lambda_max
	\endverbatim

	\note Для корректной работы этой функции необходимо, чтобы поля
	wnode::w и wnode::wq элементов спектра были корректны. Для этого
	можно использовать функцию wtree::cheap_load().
*/
/*
encoder::_search_result_t
encoder::_search_lambda(const h_t &bpp,
						const lambda_t &lambda_min,
						const lambda_t &lambda_max,
						const h_t &bpp_eps,
						const lambda_t &lambda_eps,
						const bool virtual_encode)
{
	// проверка утверждений
	assert(0 < bpp);
	assert(0 <= lambda_min && lambda_min <= lambda_max);
	assert(0 <= bpp_eps && 0 <= lambda_eps);

	// установка диапазона для поиска
	lambda_t lambda_a	= lambda_min;
	lambda_t lambda_b	= lambda_max;

	// результат последней оптимизации
	_optimize_result_t result;

	// небольшая хитрость, позволяющая использовать удобный оператор
	// break
	do {
		// вычисление значений bpp на левой границе диапазона
		_wtree.filling_refresh();
		optimize_result_t result_a = _optimize_wtree(lambda_a,
													 virtual_encode);

		// проверка на допустимость входного диапазона
		if (result_a.bpp <= (bpp + bpp_eps))
		{
			result = result_a;

			break;
		}

		// вычисление значений bpp на правой границе диапазона
		_wtree.filling_refresh();
		_optimize_result_t result_b = _optimize_wtree(lambda_b,
													  virtual_encode);

		// проверка на допустимость входного диапазона
		if (result_b.bpp >= (bpp - bpp_eps))
		{
			result = result_b;

			break;
		}

		// поиск оптимального значения lamda (дихотомия)
		for (;;) {
			// подсчёт значения bpp для середины диапазона
			const lambda_t lambda_c = (lambda_b + lambda_a) / 2;

			_wtree.filling_refresh();
			_optimize_result_t result_c = _optimize_wtree(lambda_c,
														  virtual_encode);

			// проверить, достигнута ли нужная точность по bpp
			if (bpp_eps >= abs(result_c.bpp - bpp))
			{
				result = result_c;

				break;
			}

			// сужение диапазона поиска
			// if (0 < (result_b.bpp - bpp)*(result_c.bpp - bpp))
			if (bpp > result_c.bpp) lambda_b = lambda_c;
			else lambda_a = lambda_c;

			// запоминание текущего значения
			result = result_c;

			// проверить, достигнута ли нужная точность по lambda
			if (lambda_eps >= abs(lambda_b - lambda_a)) break;
		}
	} while (false);

	// возвращение полученного результата
	_search_result_t search_result;

	search_result.optimized = result;

	// вывод отладочной информации
	#ifdef LIBWIC_DEBUG
	if (_dbg_out_stream.good())
	{
		_dbg_out_stream << "[SLFB]: ";
		_dbg_out_stream << "q: " << std::setw(8) << result.q;
		_dbg_out_stream << " lambda: " << std::setw(8) << result.lambda;
		_dbg_out_stream << " j: " << std::setw(8) << result.j;
		_dbg_out_stream << " bpp: " << std::setw(8) << result.bpp;
		_dbg_out_stream << std::endl;
	}
	#endif

	return search_result;
}
*/


/*!
*/
/*
encoder::_search_result_t
encoder::_search_q_and_lambda(const h_t &bpp,
							  models_desc_t &models)
{
	static const q_t factor_b = q_t((3.0 - sqrt(5.0)) / 2.0);
    static const q_t factor_c = q_t((sqrt(5.0) - 1.0) / 2.0);

	q_t q_a	= 4;
	q_t q_d	= 64;

	q_t q_b = q_a + factor_b * (q_d - q_a);
	q_t q_c = q_a + factor_c * (q_d - q_a);

	w_t dw_b = 0;
	_search_result_t result_b = _search_q_and_lambda_iter(bpp, q_b, models,
														  dw_b, q_d - q_a);
	w_t dw_c = 0;
	_search_result_t result_c = _search_q_and_lambda_iter(bpp, q_c, models,
														  dw_c, q_d - q_a);

	_search_result_t result = result_c;

	while (0.01 < abs(q_b - q_c))
    {
		std::cout << "+";
		// _dbg_out_stream << "\td_b: " << dw_b << "; d_c: " << dw_c << std::endl;
		// _dbg_out_stream << "\t[" << q_a << ", " << q_b << ", ";
		// _dbg_out_stream << q_c << ", " << q_d << "] -> ";

        if (dw_b <= dw_c)
        {
			// _dbg_out_stream << "{dw_b <= dw_c}" << std::endl;

            q_d = q_c;
            q_c = q_b;
            dw_c = dw_b;
			q_b = q_a + factor_b * (q_d - q_a);
			// a         b    c    d
			// a    b    c    d

			w_t tmp_dw_b = 0;
			result = result_b = _search_q_and_lambda_iter(bpp, q_b, models, dw_b,
														  q_d - q_a);
        }
        else
        {
			// _dbg_out_stream << "{dw_b > dw_c}" << std::endl;

            q_a = q_b;
            q_b = q_c;
            dw_b = dw_c;
			q_c = q_a + factor_c * (q_d - q_a);
			// a    b    c         d
			//      a    b    c    d

			result = result_c = _search_q_and_lambda_iter(bpp, q_c, models, dw_c,
														  q_d - q_a);
        }
    }

	std::cout << "!";

	w_t dw = 0;
	result = _search_q_and_lambda_iter(bpp, result.optimized.q, models, dw, -169);

	std::cout << std::endl;

	return result;
}
*/


/*!	\param[in] including_tunes Если этот параметр равен <i>true</i> в
	расчёте <i>bpp</i> будет принят во внимание размер дополнительный данных,
	необходимых для восстановления изображения, определённых в структуре
	tunes_t
	\return Среднее количество бит, затрачиваемых на кодирование одного
	пикселя
*/
h_t encoder::_calc_encoded_bpp(const bool including_tunes)
{
	const sz_t data_sz = coder().encoded_sz()
						 + ((including_tunes)? sizeof(tunes_t): 0);

	return h_t(data_sz * BITS_PER_BYTE) / h_t(_wtree.nodes_count());
}



}	// end of namespace wic
