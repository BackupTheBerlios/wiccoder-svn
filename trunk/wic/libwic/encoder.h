/*!	\file     encoder.h
	\version  0.0.1
	\author   mice, ICQ: 332-292-380, mailto:wonder.mice@gmail.com
	\brief    Описания класса wic::encoder

	\todo     Более подробно описать файл encoder.h
*/

#pragma once

#ifndef WIC_LIBWIC_ENCODER
#define WIC_LIBWIC_ENCODER

////////////////////////////////////////////////////////////////////////////////
// include

// standard C++ library headers
#include <assert.h>
#include <algorithm>
#include <string>						// for debug output only
#include <fstream>						// for debug output only
#include <iomanip>						// for debug output only
#include <time.h>						// for debug output only

// external library header
// none

// libwic headers
#include <wic/libwic/types.h>
#include <wic/libwic/wtree.h>
#include <wic/libwic/acoder.h>
#include <wic/libwic/dpcm.h>
#include <wic/libwic/iterators.h>


////////////////////////////////////////////////////////////////////////////////
// defines

//!	\brief Если этот макрос определён, в функции encoder::_coef_fix для
//!	корректировки будут использоваться 4 значения: w, w + 1, w - 1 и 0. Если
//!	макрос не определён, будет проверенно только 3 значения: w, w +/- 1, 0.
//!	Знак операции выбирается в зависимости от значения коэффициента - плюс для
//! отрицательных и минус для положительных коэффициентов
#define	COEF_FIX_USE_4_POINTS
#undef	COEF_FIX_USE_4_POINTS

//!	\brief Если этот макрос определён, то корректировка коэффициентов
//!	производиться не будет
/*!	При включенной корректировке получаются лучшее результаты, но
	увеличивается время затрачиваемое на кодирование.
	работает
*/
#define	COEF_FIX_DISABLED
#undef	COEF_FIX_DISABLED

//!	\brief Если этот макрос определён, то во время оптимизации топологии
//!	ветвей будет использоваться виртуальное кодирование арифметическим
//!	кодером
/*!	Включение этой опции даёт прирост производительности, но иногда может
	затруднять отладку.

	\note Пока не используется
*/
#define	OPTIMIZATION_USE_VIRTUAL_ENCODING
#undef	OPTIMIZATION_USE_VIRTUAL_ENCODING

#define LIBWIC_DEBUG


////////////////////////////////////////////////////////////////////////////////
// wic namespace
namespace wic {


////////////////////////////////////////////////////////////////////////////////
// encoder class declaration

//! \brief Обёртка для енкодера. Осуществляет сжатие изображения.
/*! Класс предоставляет методы для сжатия изображения. Позволяет сжимать
	одно и то же изображение несколько раз для подбора параметров кодирования.
*/
class encoder {
public:
	// public types ------------------------------------------------------------

	//!	\brief Описания моделей арифметического кодера
	/*!	Структура содержит необходимые описания моделей арифметического кодера.
		Используя эту информацию, енкодер воссоздаёт модели, которые декодер
		использовал для кодирования.
	*/
	struct models_desc_t
	{
		//!	\brief Минимальный коэффициент для модели #0
		short mdl_0_min;
		//!	\brief Максимальный коэффициент для модели #0
		short mdl_0_max;
		//!	\brief Минимальный коэффициент для модели #1
		short mdl_1_min;
		//!	\brief Максимальный коэффициент для модели #1
		short mdl_1_max;
		//!	\brief Минимальный коэффициент для моделей #2..#5
		short mdl_x_min;
		//!	\brief Максимальный коэффициент для моделей #2..#5
		short mdl_x_max;
	};

	//!	\brief Структура для хранения информации необходимой для последующего
	//!	декодирования закодированных данных
	/*!	\todo Более подробно описать эту структуру и её использование
	*/
	struct header_t
	{
		//!	\brief Квантователь
		q_t q;
		//!	\brief Модели арифметического кодера
		models_desc_t models;
	};

	// public constants --------------------------------------------------------

	//!	\brief Минимальное допустимое количество уровней разложения
	static const sz_t MINIMUM_LEVELS			= 3;

	//!	\brief Количество моделей, используемых арифметическим кодером для
	//!	кодирования коэффициентов
	static const sz_t ACODER_SPEC_MODELS_COUNT	= 6;

	//!	\brief Количество моделей, используемых арифметическим кодером для
	//!	кодирования групповых признаков подрезания
	static const sz_t ACODER_MAP_MODELS_COUNT	= 5;

	//!	\brief Общее количество моделей, используемых арифметическим кодером
	static const sz_t ACODER_TOTAL_MODELS_COUNT	= ACODER_SPEC_MODELS_COUNT +
												  ACODER_MAP_MODELS_COUNT;

	// public methods ----------------------------------------------------------

	//!	\name Конструкторы и деструкторы
	//@{

	//!	\brief Конструкртор
	encoder(const sz_t width, const sz_t height, const sz_t lvls);

	//!	\brief Деструктор
	~encoder();

	//@}

	//!	\name Функции кодированя
	//@{

	//!	\brief Функция осуществляющая непосредственное кодирование изображения
	void encode(const w_t *const w, const q_t q, const lambda_t &lambda,
				header_t &header);

	//@}

	//!	\name Функции декодирования
	//@{

	//!	\brief Функция осуществляющая непосредственное декодирование изображения
	void decode(const byte_t *const data, const sz_t data_sz,
				const header_t &header);

	//@}

	//!	\name Доступ к внутренним элементам
	//@{

	//!	\brief Спектр вейвлет коэффициентов
	/*!	\return Константная ссылка на внутренний объект класса wic::wtree
	*/
	const wtree &spectrum() const { return _wtree; }

	//!	\brief Спектр вейвлет коэффициентов
	/*!	\return Ссылка на внутренний объект класса wic::wtree
	*/
	wtree &spectrum() { return _wtree; }

	//!	\brief Используемый арифметический кодер
	/*!	\return Константная ссылка на внутренний объект класса wic::acoder
	*/
	const acoder &coder() const { return _acoder; }

	//!	\brief Используемый арифметический кодер
	/*!	\return Ссылка на внутренний объект класса wic::acoder
	*/
	acoder &coder() { return _acoder; }

	//@}

protected:
	// protected types ---------------------------------------------------------

	//! \brief Структура описывает топологию подрезанной ветви
	struct _branch_topology_t {
		//!	\brief Групповой признак подрезания ветвей
		n_t n;
		//!	\brief Значение функции <i>Лагранжа</i>, при выполнении
		//!	подрезания ветвей, соответсвенно полю <i>n</i>
		j_t j;
	};

	//!	\brief Структура описывает результат проведённой оптимизации
	//!	топологии ветвей всего спектра
	struct _optimize_result_t {
		//!	\brief Значение квантователя, при котором была
		//!	произведена оптимизация
		q_t q;
		//!	\brief Значение параметра <i>lambda</i> используемого для
		//!	вычисления <i>RD</i> критерия (функции Лагранжа)
		lambda_t lambda;
		//!	\brief Значение функции <i>Лагранжа</i> для всего спектра
		j_t j;
		//!	\brief Среднее количество бит, затрачиваемых на кодирование
		//!	одного пиксела (элемента изображения)
		h_t bpp;
	};

	//!	\brief Структура описывает результат поиска (подбора) параметра
	//!	кодирования
	struct _search_result_t {
		//!	\brief Результат проведённой оптимизации топологии ветвей
		_optimize_result_t optimized;
	};

	//!	\brief Структура описывает результат кодирования
	struct _encode_result_t {
		//!	\brief Значение функции <i>Лагранжа</i> для всего спектра
		j_t j;
		//!	\brief Среднее количество бит, затрачиваемых на кодирование
		//!	одного пиксела (элемента изображения)
		h_t bpp;
	};

	// protected methods -------------------------------------------------------

	//!	\name Работа с моделями арифметического кодера
	//@{

	//! \brief Реализует функцию IndSpec(<i>S<sub>j</sub></i>) из 35.pdf
	sz_t _ind_spec(const pi_t &s, const sz_t lvl);

	//! \brief Реализует функцию IndSpec(<i>S<sub>j</sub></i>) из 35.pdf.
	/*!	\param[in] p Координаты элементы
		\param[in] sb Саббенд, в котором находится элемент
		\return Номер выбираемой модели

		Функция использует координаты элемента в качестве входных
		параметров.

		С помощью параметра шаблона возможно выбирать поле элемента,
		которое будет использоваться при вычислении прогнозов. Так как,
		во время декодирования нам известны только откорректированные
		коэффициенты, наиболее вероятным полем является wnode::member_wc.
	*/
	template <const wnode::wnode_members member>
	sz_t _ind_spec(const p_t &p, const subbands::subband_t &sb)
	{
		return _ind_spec(_wtree.calc_sj<member>(p.x, p.y, sb),
						 sb.lvl);
	}

	//! \brief Реализует функцию IndMap(<i>P<sub>i</sub></i>) из 35.pdf
	sz_t _ind_map(const pi_t &pi, const sz_t lvl);

	//!	\brief Реализует функцию IndMap(<i>P<sub>i</sub></i>) из 35.pdf
	/*!	\param[in] p Координаты элемента с кодируемым признаком подрезания
		\param[in] children_sb Саббенд, в котором находится элементы,
		дочерние от элемена с координатами <i>p</i>
		\return Номер выбираемой модели

		Функция использует координаты элемента в качестве входных
		параметров.

		С помощью параметра шаблона возможно выбирать поле элемента,
		которое будет использоваться при вычислении прогнозов. Так как,
		во время декодирования нам известны только откорректированные
		коэффициенты, наиболее вероятным полем является wnode::member_wc.
	*/
	template <const wnode::wnode_members member>
	sz_t _ind_map(const p_t &p, const subbands::subband_t &children_sb)
	{
		// подсчёт прогнозной величины Pi
		const pi_t pi_avg = _wtree.calc_pi_avg<member>(p, children_sb);

		// выбор модели для кодирования групповых признаков подрезания
		return _ind_map(pi_avg, children_sb.lvl + subbands::LVL_PREV);
	}

	//!	\brief Создаёт модели, используемые арифметическим кодером на
	//!	основе максимального и минимального элементов в спектре
	/*!	\return Модели для арифметического кодера

		Простейший метод получения моделей, однако может быть полезен,
		например, при отладке.
		\sa acoder::use()
	*/
	template <const wnode::wnode_members member>
	acoder::models_t _mk_acoder_models()
	{
		// создание моделей для кодирования
		acoder::models_t models;
		acoder::model_t model;

		// модели для коэффициентов
		// поиск минимального и максимального значений коэффициентов
		wnode::type_selector<member>::result w_min = 0;
		wnode::type_selector<member>::result w_max = 0;

		_wtree.minmax<member>(_wtree.iterator_over_wtree(), w_min, w_max);

		// +/-1 чтобы учесть корректировку
		model.min = w_min - 1;
		model.max = w_max + 1;

		models.insert(models.end(), ACODER_SPEC_MODELS_COUNT, model);

		// создание моделей для кодирования групповых признаков подрезания
		model.min = 0;
		model.max = 0x7;
		models.push_back(model);

		model.max = 0xF;
		models.insert(models.end(), ACODER_MAP_MODELS_COUNT - 1, model);

		// проверка утверждений
		assert(ACODER_TOTAL_MODELS_COUNT == models.size());

		return models;
	}

	//!	\brief Создаёт модели для арифметического кодера по их описанию
	acoder::models_t _mk_acoder_models(const models_desc_t &desc);

	//!	\brief Создаёт модели, используемые арифметическим кодером,
	//!	основываясь на специальных критериях
	encoder::models_desc_t _mk_acoder_smart_models();

	//@}

	//!	\name Поддержка арифметического кодирования
	//@{

	//! \brief Подсчитывает битовые затраты для кодирования коэффициента.
	//! Реализует функцию <i>H<sub>spec</sub></i>.
	h_t _h_spec(const sz_t m, const wk_t &wk);

	//! \brief Подсчитывает битовые затраты для кодирования группового
	//! признака подрезания ветвей. Реализует функцию <i>H<sub>map</sub></i>.
	h_t _h_map(const sz_t m, const n_t &n);

	//!	\brief Кодирует коэффициент арифметическим енкодером
	void _encode_spec(const sz_t m, const wk_t &wk,
					  const bool virtual_encode = false);

	//!	\brief Кодирует групповой признак подрезания ветвей арифметическим
	//!	енкодером
	void _encode_map(const sz_t m, const n_t &n,
					 const bool virtual_encode = false);

	//!	\brief Декодирует коэффициент арифметическим декодером
	wk_t _decode_spec(const sz_t m);

	//!	\brief Декодирует групповой признак подрезания ветвей арифметическим
	//!	декодером
	n_t _decode_map(const sz_t m);

	//@}

	//! \name Корректировка коэффициентов
	//@{

	//!	\brief Подсчитывает значение <i>RD</i> функции <i>Лагранжа</i>
	//! для значения коэффициента при кодировании его арифметическим кодером,
	//!	если бы он находился на определённых координатах.
	j_t _calc_rd_iteration(const p_t &p, const wk_t &k,
						   const lambda_t &lambda, const sz_t &model);

	//! \brief Ищет оптимальное откорректированное значение для коэффициента
	wk_t _coef_fix(const p_t &p, const subbands::subband_t &sb,
				   const lambda_t &lambda);

	//! \brief Выполняет корректировку группы дочерних элементов
	//!	(шаги 1.1 и 2.3 в алгоритме, без поддержки <i>LL</i> саббенда)
	void _coefs_fix(const p_t &p, const subbands::subband_t &sb_j,
					const lambda_t &lambda);

	//! \brief Выполняет корректировку группы дочерних элементов
	//!	(шаги 1.1 и 2.3 в алгоритме, с поддержкой <i>LL</i> саббенда)
	void _coefs_fix_uni(const p_t &p, const subbands::subband_t &sb_j,
						const lambda_t &lambda);

	//@}

	//!	\name Подготовка к просмотру следующего уровня
	//@{

	//! \brief Расчитывает значение <i>RD-функции Лагранжа</i> для варианта
	//!	подрезания ветвей
	/*!	\param[in] p Координаты элемента для которого будет расчитываться
		значение <i>RD-функции Лагранжа</i>.
		\return Значение <i>RD-функции Лагранжа</i>.

		Стоит заметить, что функция производит расчёт <i>RD-функции
		Лагранжа</i>, рассматривая подрезание ветви, начиная с дочерних
		элементов. Другими словами, сам элемент с координатами <i>p</i>
		никак не учитвается. Если установить параметр шаблона
		<i>use_node_j0</i> в <i>true</i>, то будут также учтены дочерние
		дочерних элементов (и так далее, рекурсивно до конца дерева), но
		для этого необходимо, чтобы поля wnode::j0 имели верные и актуальные
		значения.

		При подрезании вевти мы избавляемся от необходимости её кодировать,
		поэтому битовые затраты на кодирование подрезанной ветви всегда равны
		0. Следовательно, значением <i>RD-функции Лагранжа</i> является
		квадрат ошибки, которую мы получим при подрезании ветви. Параметр
		шаблона <i>use_node_j0</i> позволяет указать, следует ли при подсчёте
		ошибки учитывать ошибки полученные при убирании элементов с более
		низких уровней. Это делается путём прибавления значения поля wnode::j0
		к ошибке при подрезании коэффициентов.

		\note Функция не применима для элементов из <i>LL</i> саббенда.

		\todo <b>Необходимо написать тест для этой функции</b>
		\todo Наверное всё-же стоит что-то сделать с этим шаблонным параметром,
		который не так уж и необходим.
	*/
	template <const bool use_node_j0>
	j_t _calc_j0_value(const p_t &p)
	{
		j_t j0 = 0;

		for (wtree::coefs_iterator i = _wtree.iterator_over_children(p);
			 !i->end(); i->next())
		{
			const wnode &node = _wtree.at(i->get());

			if (use_node_j0) {
				j0 += (node.w * node.w + node.j0);
			} else {
				j0 += (node.w * node.w);
			}
		}

		return j0;
	}

	//! \brief Расчитывает <i>RD-функцию Лагранжа</i> для варианта
	//!	сохранения ветвей (без учёта истории подрезания и сохранения
	//!	ветвей)
	j_t _calc_j1_value(const p_t &p, const subbands::subband_t &sb,
					   const lambda_t &lambda);

	//!	\brief Расчитывает <i>RD-функцию Лагранжа</i> для всего дерева
	//!	(Шаг 3 алгоритма)
	j_t _calc_jx_value(const p_t &root, const j_t &j_map,
					   const lambda_t &lambda);

	//!	\brief Расчитывает значения <i>RD-функции Лагранжа</i> для вариантов
	//!	сохранения и подрезания ветвей. Осуществляет подготовку для просмотра
	//!	следующего (фактически, предыдущего) уровня дерева. Версия для самого
	//!	последнего уровня разложения (с наибольшей площадью)
	void _prepare_j(const p_t &p, const subbands::subband_t &sb,
					const lambda_t &lambda);

	//!	\brief Расчитывает значения <i>RD-функции Лагранжа</i> для вариантов
	//!	сохранения и подрезания ветвей. Осуществляет подготовку для просмотра
	//!	следующего (фактически, предыдущего) уровня дерева. Версия для всех
	//!	уровней разложения, кроме самого последнего (с наибольшей площадью)
	void _prepare_j(const p_t &p, const subbands::subband_t &sb,
					const j_t &j, const lambda_t &lambda);

	//@}

	//!	\name Оптимизация топологии дерева
	//@{

	//!	\brief Производит подсчёт функции Лагранжа для ветви при
	//!	определённой её топологии (ветвь не из <i>LL</i> саббенда)
	j_t _topology_calc_j(const p_t &branch, const n_t n);

	//!	\brief Производит подсчёт функции Лагранжа для ветви при
	//!	определённой её топологии (ветвь из <i>LL</i> саббенда)
	j_t _topology_calc_j_LL(const p_t &branch, const n_t n);

	//!	\brief Производит подсчёт функции Лагранжа для ветви при
	//!	определённой её топологии (ветвь из любого саббенда)
	/*!	\param[in] branch Координаты элемента, находящегося в вершине
		ветви
		\param[in] n Групповой признак подрезания, характеризующий
		топологию ветви
		\return Значение функции Лагранжа при топологии описанной в
		<i>n</i>
	*/
	inline j_t _topology_calc_j_uni(const p_t &branch, const n_t n)
	{
		return (_wtree.sb().test_LL(branch))
				? _topology_calc_j_LL(branch, n)
				: _topology_calc_j(branch, n);
	}

	//!	\brief Производит оптимизацию топологии ветви, путём подрезания
	//!	дочерних ветвей.
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
	_branch_topology_t
	_optimize_branch_topology(const p_t &branch,
							  const subbands::subband_t &sb,
							  const lambda_t &lambda);

	//@}

	//!	\name Кодирование отдельных деревьев
	//@{

	//!	\brief Выполняет кодирование корневого элемента и элементов
	//!	с первого уровня разложения.
	void _encode_tree_root(const p_t &root,
						   const bool virtual_encode = false);

	//!	\brief Выполняет кодирование уровня дерева
	void _encode_tree_leafs(const p_t &root, const sz_t lvl,
							const bool virtual_encode = false);

	//!	\brief Выполняет кодирование отдельного дерева
	void _encode_tree(const p_t &root,
					  const bool virtual_encode = false);

	//@}

	//!	\name Кодирование всего спектра
	//@{

	//!	\brief Выполняет кодирование/декодирование корневых элементов
	//!	спектра
	void _encode_wtree_root(const bool decode_mode = false);

	//!	\brief Выполняет кодирование/декодирование не корневых элементов
	//!	спектра всего уровня
	void _encode_wtree_level(const sz_t lvl,
							 const bool decode_mode = false);

	//!	\brief Выполняет кодирование/декодирование всего спектра
	void _encode_wtree(const bool decode_mode = false);

	//@}

	//!	\name Оптимизация топологии отдельного дерева
	//@{

	//! \brief Шаг 1. Подготовительный.
	void _optimize_tree_step_1(const p_t &root, const lambda_t &lambda);

	//! \brief Шаг 2. Просмотр текущего уровня с попыткой подрезания ветвей.
	void _optimize_tree_step_2(const p_t &root, const lambda_t &lambda);

	//! \brief Шаг 3. Вычисление <i>RD-функции Лагранжа</i> для всего дерева
	void _optimize_tree_step_3(const p_t &root, const lambda_t &lambda);

	//!	\brief Последовательно выполняет шаги оптимизации топологи дерева
	j_t _optimize_tree(const p_t &root, const lambda_t &lambda);

	//@}

	//!	\name Оптимизация параметров кодирования
	//@{

	//!	\brief Производит оптимизацию топологии всех ветвей в спектре
	_optimize_result_t _optimize_wtree(const lambda_t &lambda,
									   const bool virtual_encode = false);

	//!	\brief Производит оптимизацию топологии всех ветвей в спектре
	//! с предварительным квантованием и настройкой арифметического
	//!	кодера.
	_optimize_result_t _optimize_wtree(const lambda_t &lambda,
									   const q_t &q, models_desc_t &models,
									   const bool virtual_encode = false);

	//!	\brief Производит поиск параметра <i>lambda</i>, подбирая его
	//! под битрейт <i>bpp</i>
	_search_result_t _search_lambda(const h_t &bpp,
									const lambda_t &lambda_min,
									const lambda_t &lambda_max,
									const h_t &bpp_eps,
									const lambda_t &lambda_eps);

	_search_result_t _search_q_min_j(const lambda_t &lambda,
									 models_desc_t &models,
									 const q_t &q_min, const q_t &q_max,
									 const q_t &j_eps, const q_t &q_eps);

	//@}

private:
	// private data ------------------------------------------------------------

	//! \brief Рабочее дерево коэффициентов
	wtree _wtree;

	//! \brief Арифметический кодер
	acoder _acoder;

	#ifdef LIBWIC_DEBUG
	//!	\brief Стандартный файловый поток для вывода информации в
	//!	отладочном режиме
	std::ofstream _dbg_out_stream;
	#endif
};



}	// end of namespace wic



#endif	// WIC_LIBWIC_ENCODER
