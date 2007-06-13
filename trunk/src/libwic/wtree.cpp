/*!	\file     wtree.cpp
	\version  0.0.1
	\author   mice, ICQ: 332-292-380, mailto:wonder.mice@gmail.com
	\brief    Реализация класса wic::wtree - спектра вейвлет разложения

	\todo     Более подробно описать файл wtree.cpp
*/


////////////////////////////////////////////////////////////////////////////////
// include

// standard C++ headers
#include <memory.h>

// libwic headers
#include <wic/libwic/wtree.h>


////////////////////////////////////////////////////////////////////////////////
// wic namespace
namespace wic {


////////////////////////////////////////////////////////////////////////////////
// wtree class public definitions

/*!	Введён просто так, без хитрого умыслу.
*/
const q_t wtree::DEFAULT_Q		= q_t(1);


/*!	\param[in] width Ширина вейвлет спектра (в элементах)
	\param[in] height Высота изображения (в элементах)
	\param[in] lvls Количество уровней разложения
*/
wtree::wtree(const sz_t width, const sz_t height, const sz_t lvls):
	_width(width), _height(height), _lvls(lvls),
	_nodes_count(width * height),
	_q(0),
	_nodes(0), _subbands(0)
{
	// проверка утверждений
	assert(0 < _width);
	assert(0 < _height);
	assert(0 < _lvls);
	assert(_nodes_count == (_width * _height));
	#ifdef LIBWIC_DEBUG
	{
		const sz_t factor = (1 << _lvls);
		assert(0 == (_width  % factor));
		assert(0 == (_height % factor));
	}
	#endif

	// выделение памяти и создание объектов
	try
	{
		// создание информации о саббендах
		_subbands = new subbands(_width, _height, _lvls);

		if (0 == _subbands) throw std::bad_alloc();

		// выделение памяти под карту вейвлет коэффициентов (спектр)
		_nodes = new wnode[nodes_count()];

		if (0 == _nodes) throw std::bad_alloc();
	}
	catch (const std::exception &e)
	{
		// Освобождение информации о саббендах
		if (0 != _subbands) delete _subbands;

		// Освобождение памяти под карту вейвлет коэффициентов (спектр)
		if (0 != _nodes) delete[] _nodes;

		throw e;
	}

	#ifdef LIBWIC_DEBUG
	// сброс всех значений
	memset(_nodes, 0, nodes_size());
	#endif
}


/*!	Освобождает занимаемую память
*/
wtree::~wtree() {
	// Освобождение информации о саббендах
	if (0 != _subbands) delete _subbands;

	// Освобождение памяти под карту вейвлет коэффициентов (спектр)
	if (0 != _nodes) delete[] _nodes;
}


/*!	\param[in] q Квантователь

	Функция производит квантование коэффициентов. Исходные коэффициенты
	берутся из поля wnode::w, результат квантования помещается в
	поле wnode::wq и wnode::wc.

	\sa _filling_quantize
*/
void wtree::quantize(const q_t q)
{
	_filling_quantize(q);
}


/*!	\param[in] x X координата
	\param[in] y Y координата
	\return Константная ссылка на значение элемента спектра
*/
const wnode &wtree::at(const sz_t x, const sz_t y) const {
	assert(0 <= x && x < _width && 0 <= y && y < _height);
	return _nodes[x + y*_width];
}


/*!	\param[in] x X координата
	\param[in] y Y координата
	\return Ссылка на значение элемента спектра
*/
wnode &wtree::at(const sz_t x, const sz_t y) {
	assert(0 <= x && x < _width && 0 <= y && y < _height);
	return _nodes[x + y*_width];
}


/*!	\param[in] node Ссылка на элемент
	\return Координаты элемента в спектре

	\note Элемент должен быть обязательно из этого спектра
*/
p_t wtree::get_pos(const wnode &node) const
{
	assert(_nodes <= &node);

	const sz_t offset = sz_t(&node - _nodes);

	assert(offset < nodes_count());

	return p_t(offset % _width, offset / _height);
}


/*!	\param[in] c Координаты элемента
	\return Координаты родительского элемента

	\note Данная функция не применима для элементов, родители которых
	располагаются в <i>LL</i> саббенде
*/
p_t wtree::prnt(const p_t &c)
{
	assert(!sb().test_LL(c));

	return p_t(c.x / 2, c.y / 2);
}


/*!	\param[in] c Координаты элемента
	\return Координаты родительского элемента

	\note Функция довольно медленная, т.к. делает много проверок
*/
p_t wtree::prnt_uni(const p_t &c)
{
	assert(c.x > sb().get_LL().x_max ||
		   c.y > sb().get_LL().y_max);
	assert(c.x < _width && c.y < _height);

	const subbands::subband_t &sb_HH =
					sb().get(subbands::LVL_1, subbands::SUBBAND_HH);

	// проверка, лежит ли родительский элемент в LL саббенде
	if (c.x <= sb_HH.x_max && c.y <= sb_HH.y_max)
	{
		const subbands::subband_t &sb_LL = sb().get_LL();

		if (c.y <= sb_LL.y_max) return p_t(c.x - sb_LL.width, c.y);
		if (c.x <= sb_LL.x_max) return p_t(c.x, c.y - sb_LL.height);

		return p_t(c.x - sb_LL.width, c.y - sb_LL.height);
	}

	return p_t(c.x / 2, c.y / 2);
}


/*!	\param[in] lvl Уровень вейвлет разложения, на котором находится
	элемент с ассоциированным групповым признаком подрезания
	\return Итератор по всевозможным значениям для данного уровня группового
	признака подрезания ветвей
*/
wtree::n_iterator wtree::iterator_through_n(const sz_t lvl) const {
	return new n_cutdown_iterator(lvl);
}


/*!	\param[in] p Координаты элемента, который будет оставлен неподрезанным
	в групповом признаке подрезания ветвей.
	\param[in] branch Координаты родительского элемента, дающего начало ветки.
	\return Групповой признак подрезания ветвей, сконструированный так, что
	только одна ветвь (которой принадлежит элемент с координатами <i>p</i>)
	осталась неподрезанной.
*/
n_t wtree::child_n_mask(const p_t &p, const p_t &branch) const
{
	const p_t ctl = _children_top_left(branch);

	const sz_t shift = (p.x - ctl.x) + 2*(p.y - ctl.y);

	return n_t(1 << shift);
}


/*!	\param[in] p Координаты элемента, который будет оставлен неподрезанным
	в групповом признаке подрезания ветвей.
	\return Групповой признак подрезания ветвей, сконструированный так, что
	только одна ветвь (которой принадлежит элемент с координатами <i>p</i>)
	осталась неподрезанной.

	\note Данная функция работает только с элементами, являющимися прямыми
	потомками элементов из <i>LL</i> саббенда (которые лежат в <i>1ом</i>
	уровне разложения).
*/
n_t wtree::child_n_mask_LL(const p_t &p) const 
{
	assert(subbands::LVL_1 == sb().from_point(p).lvl);

	const subbands::subband_t &sb_LL = sb().get_LL();

	if (p.y <= sb_LL.y_max) return (1 << 0);
	if (p.x <= sb_LL.x_max) return (1 << 1);

	return (1 << 2);
}


/*!	\param[in] p Координаты элемента, который будет оставлен неподрезанным
	в групповом признаке подрезания ветвей.
	\param[in] branch Координаты родительского элемента, дающего начало ветки.
	\return Групповой признак подрезания ветвей, сконструированный так, что
	только одна ветвь (которой принадлежит элемент с координатами <i>p</i>)
	осталась неподрезанной.
*/
n_t wtree::child_n_mask_uni(const p_t &p, const p_t &branch) const {
	if (sb().test_LL(branch)) return child_n_mask_LL(p);

	return child_n_mask(p, branch);
}


/*!	\param[in] branch Координаты элемента, образующего ветвь
	\param[in] n Групповой признак подрезания ветвей

	По сути, данная функция это единственный правильный способ порождения
	ветвей (замены подрезанных ветвей на сохранённые).

	Функция не выполняется рекурсивно вниз, как wtree::cut_leafs().
*/
void wtree::uncut_leafs(const p_t &branch, const n_t n)
{
	// проверка утверждений
	assert(lvls() + subbands::LVL_PREV > sb().from_point(branch).lvl);

	const bool is_LL = sb().test_LL(branch);

	for (coefs_iterator i = iterator_over_children_uni(branch);
		 !i->end(); i->next())
	{
		const p_t &p = i->get();

		const n_t mask = (is_LL)? child_n_mask_LL(p)
								: child_n_mask(p, branch);

		if (test_n_mask(n, mask)) _uncut_branch(p);
		// _uncut_branch(p, !test_n_mask(n, mask));
	}

	at(branch).n = n;
}


/*!	\param[in] sb Саббенд по которому будет происходить обход с помощью
	возвращённого итератора
	\return Итератор по саббенду
*/
wtree::coefs_iterator wtree::iterator_over_subband(
		const subbands::subband_t &sb) const
{
	return new snake_2d_iterator(p_t(sb.x_min, sb.y_min),
								 p_t(sb.x_max, sb.y_max));
}


/*!	\param[in] root Координаты корневого элемента
	\param[in] sb Саббенд, листья из которого будут рассматриваться во время
	процесса итерации.
	\return Итератор по листьям

	\todo Написать функцию получения саббенда в котором находится коэффициент

	\sa _leafs_top_left()
*/
wtree::coefs_iterator wtree::iterator_over_leafs(
		const p_t &root, const subbands::subband_t &sb) const
{
	const p_t b(_leafs_top_left(root, sb.lvl, sb.i));
	const p_t e(b.x + sb.tree_w - 1, b.y + sb.tree_h - 1);

	return new snake_2d_iterator(b, e);
}


/*!	\param[in] root Координаты корневого элемента
	\param[in] lvl Уровень, листья с которого будут рассматриваться во время
	процесса итерации
	\param[in] i Индекс саббенда внутри уровня, листья из которого буду
	рассматриваться
	\return Итератор по листьям

	\sa _leafs_top_left()
*/
wtree::coefs_iterator wtree::iterator_over_leafs(
		const p_t &root, const sz_t lvl, const sz_t i) const
{
	return iterator_over_leafs(root, sb().get(lvl, i));
}

/*!	\return Итератор по всему спектру
*/
wtree::coefs_iterator wtree::iterator_over_wtree() const
{
	assert(0 < _width && 0 < _height);

	return new snake_2d_iterator(p_t(0, 0), p_t(_width - 1, _height - 1));
}


////////////////////////////////////////////////////////////////////////////////
// wtree class protected definitions

/*!	\param[in] q Используемый квантователь

	Функция проводит квантование полей wnode::w и помещает результат в поля
	wnode::wq
*/
void wtree::_quantize(const q_t q)
{
	assert(1 <= q);

	_q = q;

	for (sz_t i = 0; nodes_count() > i; ++i)
	{
		wnode &node = _nodes[i];
		node.wq = wnode::quantize(node.w, q);
	}
}


/*!	\param[in] q Используемый квантователь

	Функция проводит квантование полей wnode::w и помещает результат в поля
	wnode::wq. Также производятся следующие действия:
	- Полученное значение из поля wnode::wq копируется в поле wnode::wc.
	- Поля wnode::j0 и wnode::j1 сбрасываются в 0.
	- Все ветви помечаются как подрезанные (wnode::n сбрасываются в 0)
	- Все элементы помечаются как существующие (wnode::invalid
	  устанавливаются в <i>false</i>)
*/
void wtree::_filling_quantize(const q_t q)
{
	assert(1 <= q);

	_q = q;

	for (sz_t i = 0; nodes_count() > i; ++i)
	{
		wnode &node		= _nodes[i];
		node.wc			= node.wq = wnode::quantize(node.w, q);
		node.j0			= node.j1 = 0;
		node.n			= 0;
		node.invalid	= false;
	}
}


/*!	Выполняет следующие действия:
	- Значение из поля wnode::wq копируется в поле wnode::wc.
	- Поля wnode::j0 и wnode::j1 сбрасываются в 0.
	- Все ветви помечаются как подрезанные (wnode::n сбрасываются в 0)
	- Все элементы помечаются как существующие (wnode::invalid
	  устанавливаются в <i>false</i>)
*/
void wtree::_filling_refresh()
{
	for (sz_t i = 0; nodes_count() > i; ++i)
	{
		wnode &node		= _nodes[i];
		node.wc			= node.wq;
		node.j0			= node.j1 = 0;
		node.n			= 0;
		node.invalid	= false;
	}
}


/*!	\param[in] prnt Координаты родительского элемента
	\return Координаты верхнего левого дочернего элемента из группы
	дочерних элементов

	\warning Функция не пригодна для использования с координатами
	родительских элементов из <i>LL</i> саббенда, так как в этом случае
	дочерние элементы оказываются в разных саббендах.
*/
p_t wtree::_children_top_left(const p_t &prnt) const {
	return p_t(2 * prnt.x, 2 * prnt.y);
}


/*!	\param[in] root Координаты корневого элемента дерева
	\param[in] lvl Уровень на котором находится требуемый блок листьев
	\param[in] i Индекс саббенда на уровне (аналогично subbands::_get())
	\return Координаты верхнего левого элемента блока листьев данного
	дерева.

	\note Эта функция корректно работает для любого значения параметра
	<i>lvl</i> так как номер саббенда и уровень разложения указываются
	явно.

	\todo Написать более подробное описание
*/
p_t wtree::_leafs_top_left(const p_t &root, const sz_t lvl, const sz_t i) const
{
	// будем часто обращаться к LL саббенду
	const subbands::subband_t &sb_LL = sb().get_LL();

	// удостоверимся, что корневой элемент из LL саббенда
	assert(sb_LL.x_min <= root.x && root.x <= sb_LL.x_max);
	assert(sb_LL.y_min <= root.y && root.y <= sb_LL.y_max);

	// если нулевой уровень, то независимо от индекса саббенда возвращаем
	// корневой элемент
	if (0 == lvl) return root;

	// находим координаты элемента из саббенда первого уровня с нужным
	// индексом
	const subbands::subband_t &sb_1 =  sb().get(subbands::LVL_1, i);

	const p_t leaf_1(sb_1.x_min + root.x, sb_1.y_min + root.y);

	// определяем множитель, на который необходимо домножить координаты
	// для перехода на требуемый уровень (это будет 2^(lvl - 1))
	const sz_t factor = 1 << (lvl - 1);

	return p_t(factor * leaf_1.x, factor * leaf_1.y);
}


/*!	\param[in] x x координата элемента (не используется в текущей реализации)
	\param[in] y y координата элемента
	\return \c true, если предполагаемое направление обхода - влево,
	иначе - \c false.

	Функция предполагает, что используется обход "змейка", при котором
	действуют следующие правила:
	- проход вправо осуществляется по чётным строкам
	- проход влево осуществляется по нечётным строкам

	Используется в
	calc_sj(const sz_t x, const sz_t y, const subbands::subband_t &sb)
	для автоматического распознавания направления обхода.
*/
bool wtree::_going_left(const sz_t x, const sz_t y) {
	// this is to prevent gcc warning, that variable x not used
	(void)x;

	return (0 != y % 2);
}


/*!	\param[in] branch Координаты элемента, порождающего ветвь

	Функция просто устанавливает поле wnode::invalid для дочерних
	от <i>branch</i> элементов в значение <i>false</i>.

	\note Элемент <i>branch</i> не может быть из <i>LL</i> саббенда
	или с саббенда находящегося на последнем уровне разложения.
*/
void wtree::_uncut_branch(const p_t &branch)
{
	// проверка утверждений
	assert(sb().from_point(branch).lvl > subbands::LVL_0);
	assert(sb().from_point(branch).lvl < lvls());

	// перебор дочерних элементов
	for (coefs_iterator i = iterator_over_children(branch);
		 !i->end(); i->next())
	{
		at(i->get()).invalid = false;
	}
}


/*!	\param[in] prnt Координаты родительского элемента
	\return Указатель на итератор по дочерним элементам

	\note Данная функция не применима для родительских элементов из
	<i>LL</i> саббенда.

	\attention Необходимо использовать оператор <i>delete</i> для
	освобождения памяти, занимаемой возвращённым итератором.
*/
basic_iterator<p_t> *wtree::_iterator_over_children(const p_t &prnt) const
{
	// координаты верхнего левого дочернего элемента
	const p_t c = _children_top_left(prnt);

	// создание итератора
	return new snake_2d_iterator(p_t(c.x    , c.y    ),
								 p_t(c.x + 1, c.y + 1));
}


/*!	\param[in] prnt Координаты родительского элемента
	\return Указатель на итератор по дочерним элементам

	\note Данная функция не применима для родительских элементов не из
	<i>LL</i> саббенда.

	\attention Необходимо использовать оператор <i>delete</i> для
	освобождения памяти, занимаемой возвращённым итератором.
*/
basic_iterator<p_t> *wtree::_iterator_over_LL_children(const p_t &prnt) const
{
	const subbands::subband_t &sb_LL = sb().get_LL();

	assert(sb().test(prnt, sb_LL));

	return new LL_children_iterator(sb_LL.width, sb_LL.height, prnt);
}



}	// end of namespace wic
