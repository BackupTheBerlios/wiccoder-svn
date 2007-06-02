/*******************************************************************************
* file:         encoder.h                                                      *
* version:      0.1.0                                                          *
* author:       mice (mailto:my_mice@mail.ru, ICQ:332-292-380)                 *
* description:  not available                                                  *
* tests:        none                                                           *
*******************************************************************************/

#pragma once

#ifndef WIC_LIBWIC_ENCODER
#define WIC_LIBWIC_ENCODER

////////////////////////////////////////////////////////////////////////////////
// include

// standard C++ library headers
#include <assert.h>

// external library header
#include <arcoder/aencoder.h>
#include <arcoder/adecoder.h>

// libwic headers
#include <wic/libwic/types.h>
#include <wic/libwic/wtree.h>
#include <wic/libwic/iterators.h>


////////////////////////////////////////////////////////////////////////////////
// defines

//!	\brief Если этот макрос определён, в функции encoder::_coef_fix для
//!	корректировки будут использоваться 4 значения: w, w + 1, w - 1 и 0. Если
//!	макрос не определён, будет проверенно только 3 значения: w, w +/- 1, 0.
//!	Знак операции выбирается в зависимости от значения коэффициента - плюс для
//! отрицательных и минус для положительных коэффициентов
#define COEF_FIX_USE_4_POINTS


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

	//!	\brief Псевдоним для арифметического енкодера
	typedef wtc::aencoder<wk_t> aencoder;

	//!	\brief Псевдоним для арифметического декодера
	typedef wtc::adecoder<wk_t> adecoder;

	// public constants --------------------------------------------------------
	// public methods ----------------------------------------------------------

	//!	\name Конструкторы и деструкторы
	//@{

	//!	\brief Конструкртор
	encoder(const w_t *const image,
			const sz_t width, const sz_t height, const sz_t lvls);

	//!	\brief Деструктор
	~encoder();

	//@}

	//!	\name Функции кодированя
	//@{

	//!	\brief Функция осуществляющая непосредственное кодирование изображения
	void encode();

	//@}

protected:
	// protected types ---------------------------------------------------------

	//! \brief Тип описывающий топологию подрезанной ветви
	struct _branch_topology_t {
		//!	\brief Групповой признак подрезания ветвей
		n_t n;
		//!	\brief Значение функции Лагранжа, при выполнении подрезания
		//!	ветвей, соответсвенно полю <i>n</i>
		j_t j;
	};

	// protected methods -------------------------------------------------------

	//!	\name Выбор модели арифметического кодера по прогнозу
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
		во времдя декодирования нам известны только откорректированные
		коэффициенты, наиболее вероятным полем является wnode::member_wc.
	*/
	template <const wnode::wnode_members member>
	sz_t _ind_spec(const p_t &p, const subbands::subband_t &sb) {
		return _ind_spec(_wtree.calc_sj<member>(p.x, p.y, sb), sb.lvl);
	}

	//! \brief Реализует функцию IndMap(<i>P<sub>i</sub></i>) из 35.pdf
	sz_t _ind_map(const pi_t &pi, const bool is_LL = false);

	//@}

	//!	\name Поддержка арифметического кодирования
	//@{

	//! \brief Подсчитывает битовые затраты для кодирования коэффициента.
	//! Реализует функцию <i>H<sub>spec</sub></i>.
	h_t _h_spec(const sz_t m, const wk_t &wk);

	//! \brief Подсчитывает битовые затраты для кодирования группового
	//! признака подрезания ветвей. Реализует функцию <i>H<sub>map</sub></i>.
	h_t _h_map(const sz_t m, const n_t &n);

	//@}

	//!	\name Операции выполняемые при кодировании
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
	//!	(шаги 1.1 и 2.3 в алгоритме)
	void _coefs_fix(const p_t &p, const subbands::subband_t &sb_j,
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

	//! \brief Расчитывает <i>RD</i> функцию <i>Лагранжа</i> для варианта
	//!	подрезания ветвей
	/*!	\param[in] p Координаты элемента для которого будет расчитываться
		значение <i>RD</i> функцию <i>Лагранжа</i>.
		\return Значение <i>RD</i> функцию <i>Лагранжа</i>.

		Стоит заметить, что функция производит расчёт <i>RD</i> функции
		<i>Лагранжа</i>, рассматривая подрезание ветви, начиная с дочерних
		элементов. Другими словами, сам элемент с координатами <i>p</i>
		никак не учитвается. Если установить параметр шаблона
		<i>use_node_j0</i> в <i>true</i>, то будут также учтены дочерние
		дочерних элементов (и так далее, рекурсивно до конца дерева), но
		для этого необходимо, чтобы поля wnode::j0 имели верные актуальные
		значения.

		При подрезании вевти мы избавляемся от необходимости её кодировать,
		поэтому битовые затраты на кодирование подрезанной ветви всегда равны
		0. Следовательно, значением <i>RD</i> функцию <i>Лагранжа</i> является
		квадрат ошибки, которую мы получим при подрезании ветви. Параметр
		шаблона <i>use_node_j0</i> позволяет указать, следует ли при подсчёте
		ошибки учитывать ошибки полученные при убирании элементов с более
		низких уровней. Это делается путём прибавления значения поля wnode::j0
		к ошибке при подрезании коэффициентов.

		\note Функция не применима для элементов из <i>LL</i> саббенда.

		\todo Необходимо написать тест для этой функции.
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
				j0 += (node.wc * node.wc + node.j0);
			} else {
				j0 += (node.wc * node.wc);
			}
		}

		return j0;
	}

	//! \brief Расчитывает <i>RDфункцию Лагранжа</i> для варианта
	//!	сохранения ветвей (без учёта истории подрезания и сохранения
	//!	ветвей)
	j_t _calc_j1_value(const p_t &p, const subbands::subband_t &sb,
					   const lambda_t &lambda);

	//!	\brief Производит подсчёт функции Лагранжа для ветви при
	//!	определённой её топологии (ветвь не из <i>LL</i> саббенда)
	/*!	\param[in] branch Координаты элемента, находящегося в вершине
		ветви
		\param[in] n Групповой признак подрезания, характеризующий
		топологию ветви
		\return Значение функции Лагранжа при топологии описанной в
		<i>n</i>
	*/
	j_t _topology_calc_j(const p_t &branch, const n_t n) {
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

	//!	\brief Производит подсчёт функции Лагранжа для ветви при
	//!	определённой её топологии (ветвь из <i>LL</i> саббенда)
	/*!	\param[in] branch Координаты элемента, находящегося в вершине
		ветви
		\param[in] n Групповой признак подрезания, характеризующий
		топологию ветви
		\return Значение функции Лагранжа при топологии описанной в
		<i>n</i>
	*/
	j_t _topology_calc_j_LL(const p_t &branch, const n_t n) {
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

	//!	\brief Производит подсчёт функции Лагранжа для ветви при
	//!	определённой её топологии (ветвь из любого саббенда)
	/*!	\param[in] branch Координаты элемента, находящегося в вершине
		ветви
		\param[in] n Групповой признак подрезания, характеризующий
		топологию ветви
		\return Значение функции Лагранжа при топологии описанной в
		<i>n</i>
	*/
	j_t _topology_calc_j_uni(const p_t &branch, const n_t n) {
		return (_wtree.sb().test_LL(branch))
				? _topology_calc_j(branch, n)
				: _topology_calc_j_LL(branch, n);
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
	_branch_topology_t _optimize_branch_topology(const p_t &branch,
												 const subbands::subband_t &sb,
												 const lambda_t &lambda)
	{
		assert(subbands::LVL_0 != sb.lvl);

		// получение дочернего саббенда
		const sz_t lvl_j = sb.lvl + subbands::LVL_NEXT;
		const subbands::subband_t &sb_j = _wtree.sb().get(lvl_j, sb.i);

		// подсчёт прогнозной величины Pi
		const pi_t pi_avg = _wtree.calc_pi_avg<wnode::member_wc>(branch, sb_j);

		// выбор модели для кодирования групповых признаков подрезания
		const sz_t model = _ind_map(pi_avg);

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

	//@}

	//!	\name Шаги кодирования
	//@{

	//! \brief Шаг 1. Подготовительный.
	void _encode_step_1(const p_t &root, const lambda_t &lambda);

	//! \brief Шаг 2. Просмотр текущего уровня с попыткой подрезания ветвей.
	void _encode_step_2(const p_t &root, const lambda_t &lambda);

	//! \brief Шаг 3. Вычисление <i>RD-функции Лагранжа</i> для всего дерева
	void _encode_step_3(const p_t &root, const lambda_t &lambda);

	//@}

private:
	// private data ------------------------------------------------------------

	//! \brief Рабочее дерево коэффициентов
	wtree _wtree;

	//! \brief Арифметический енкодер
	aencoder _aenc;
};



}	// end of namespace wic



#endif	// WIC_LIBWIC_ENCODER
