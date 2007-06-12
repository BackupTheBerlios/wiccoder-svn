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

/*!	\param[in] image Результат вейвлет преобразования изображения для сжатия.
	\param[in] width Ширина изображения.
	\param[in] height Высота изображения.
	\param[in] lvls Количество уровней вейвлет преобразования.
*/
encoder::encoder(const w_t *const image,
				 const sz_t width, const sz_t height, const sz_t lvls):
	_wtree(width, height, lvls),
	_acoder(width * height * sizeof(w_t) * 4)
{
	// проверка утверждений
	assert(MINIMUM_LEVELS <= lvls);

	// загрузка коэффициентов разложения в дерево
	_wtree.load(image);
}


/*!
*/
encoder::~encoder() {
}


/*!
*/
void encoder::encode(const lambda_t &lambda)
{
	_acoder.use(_mk_acoder_models<wnode::member_wq>());

	_acoder.encode_start();

	const subbands &sb = _wtree.sb();

	for (wtree::coefs_iterator i = _wtree.iterator_over_subband(sb.get_LL());
		 !i->end(); i->next())
	{
		const p_t &root = i->get();
		_optimize_tree(root, lambda);
		_encode_tree(root);
	}

	_acoder.encode_stop();

	_acoder.encode_start();

	_encode_wtree();

	_acoder.encode_stop();
}

void encoder::decode()
{
	_wtree.reset();

	_acoder.decode_start();

	_encode_wtree(true);

	_acoder.decode_stop();
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
*/
void encoder::_encode_spec(const sz_t m, const wk_t &wk) {
	_acoder.put(wk, m);
}


/*!	\param[in] m Номер модели для кодирования
	\param[in] n Значение группового признака подрезания ветвей
*/
void encoder::_encode_map(const sz_t m, const n_t &n) {
	_acoder.put(n, m + ACODER_SPEC_MODELS_COUNT);
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
	return _wtree.at(p).wq;

	// выбор модели и оригинального значения коэффициента
	const sz_t	model	= _ind_spec<wnode::member_wc>(p, sb);
	const wk_t	&wq		= _wtree.at(p).wq;

	// Определение набора подбираемых значений
	#ifdef COEF_FIX_USE_4_POINTS
		static const sz_t vals_count	= 4;
		const wk_t w_vals[vals_count] = {0, wq, wq + 1, wq - 1};
	#else
		static const sz_t vals_count	= 3;
		const wk_t w_drift = (0 <= wq)? -1: +1;
		const wk_t w_vals[vals_count] = {0, wq, wq + w_drift};
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

	Функция выполняет кодирование:
	- Коэффициента при корневом элементе
	- Группового признака подрезания при корневом элементе
	- Коэффициентов принадлежащих дереву с первого уровня разложения
	- Коэффициентов принадлежащих дереву со второго уровня разложения
	  если те не попали в подрезанные ветви
*/
void encoder::_encode_tree_root(const p_t &root)
{
	// получение корневого элемента
	const wnode &root_node = _wtree.at(root);

	// закодировать коэффициент с нулевого уровня
	_encode_spec(_ind_spec(0, subbands::LVL_0), root_node.wc);

	// закодировать групповой признак подрезания с нулевого уровня
	_encode_map(_ind_map(0, subbands::LVL_0), root_node.n);

	// кодирование дочерних коэффициентов с первого уровня
	for (wtree::coefs_iterator i = _wtree.iterator_over_LL_children(root);
		 !i->end(); i->next())
	{
		// закодировать коэффициенты с первого уровня
		_encode_spec(_ind_spec(0, subbands::LVL_1), _wtree.at(i->get()).wc);
	}

	/*
	// кодирование коэффициентов со второго уровня
	static const sz_t LVL_2 = subbands::LVL_1 + subbands::LVL_NEXT;

	for (wtree::coefs_iterator i = _wtree.iterator_over_LL_children(root);
		 !i->end(); i->next())
	{
		// координаты текущего родительского элемента с первого уровня
		const p_t &p = i->get();

		// маска подрезания, где текущий элемент не подрезан
		const n_t mask = _wtree.child_n_mask_LL(p);

		// переходим к следующему потомку, если ветвь подрезана
		if (!_wtree.test_n_mask(root_node.n, mask)) continue;

		// саббенд в котором лежит текущий родительский элемент
		const subbands::subband_t &sb_i =
						_wtree.sb().from_point(p, subbands::LVL_1);

		// саббенд в котором лежат дочерние элементы со второго уровня
		// (дочерний от sb_i)
		const subbands::subband_t &sb_j =
						_wtree.sb().get(LVL_2, sb_i.i);

		// ветвь не подрезана, кодируем дочерние коэффициенты
		for (wtree::coefs_iterator i = _wtree.iterator_over_children(p);
			 !i->end(); i->next())
		{
			// координаты текущего дочернего элемента со второго уровня
			const p_t &c = i->get();

			// кодирование коэффициента
			_encode_spec(_ind_spec<wnode::member_wc>(c, sb_j),
						 _wtree.at(c).wc);
		}
	}
	*/
}


/*!	\param[in] root Координаты корнвого элемента
	\param[in] lvl Номер уровня разложения
*/
void encoder::_encode_tree_leafs(const p_t &root, const sz_t lvl)
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

			_encode_spec(model, node_g.wc);
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

			_encode_map(model, node_j.n);
		}
	}
}


/*!	\param[in] root Координаты корневого элемента дерева
*/
void encoder::_encode_tree(const p_t &root)
{
	// кодирование корневого и дочерних элементов дерева
	_encode_tree_root(root);

	// кодирование элементов дерева на остальных уровнях
	const sz_t first_lvl = subbands::LVL_1 + subbands::LVL_NEXT;
	const sz_t final_lvl = _wtree.lvls();

	for (sz_t lvl = first_lvl; final_lvl >= lvl; ++lvl)
	{
		_encode_tree_leafs(root, lvl);
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
			if (!_wtree.test_n_mask(node_i.n, mask))
			{
				// выставление значений поля wnode::invalid в <i>true</i>
				// так как ветвь подрезана
				if (decode_mode)
					_wtree.uncut_leafs(p_j, _wtree.get_clear_n());

				continue;
			}

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
*/
j_t encoder::_optimize_tree(const p_t &root, const lambda_t &lambda)
{
	_optimize_tree_step_1(root, lambda);
	_optimize_tree_step_2(root, lambda);
	_optimize_tree_step_3(root, lambda);

	return (_wtree.at(root).j1);
}



}	// end of namespace wic
