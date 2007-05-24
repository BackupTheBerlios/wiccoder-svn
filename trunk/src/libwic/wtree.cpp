/*******************************************************************************
* file:         wtree.cpp                                                      *
* version:      0.1.0                                                          *
* author:       mice (mailto:my_mice@mail.ru, ICQ:332-292-380)                 *
* description:  not available                                                  *
* tests:        none                                                           *
*******************************************************************************/


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

/*!	\param[in] width Ширина изображения
	\param[in] height Высота изображения
	\param[in] lvls Количество уровней разложения
*/
wtree::wtree(const sz_t width, const sz_t height, const sz_t lvls):
	_nodes(0), _subbands(0), _q(0)
{
	// сохранение параметров спектра
	_width	= width;
	_height	= height;
	_lvls	= lvls;

	// выделение памяти под карту вейвлет коэффициентов (спектр)
	_nodes = new wnode[coefs()];
	if (0 == _nodes) throw std::bad_alloc();

	// сброс всех значений в ноль
	_reset_trees_content();

	// создание информации о саббендах
	_subbands = new subbands(_width, _height, _lvls);
	if (0 == _nodes) throw std::bad_alloc();
}


/*!	Освобождает занимаемую память
*/
wtree::~wtree() {
	// Освобождение информации о саббендах
	if (0 != _subbands) delete _subbands;

	// Освобождение памяти под карту вейвлет коэффициентов (спектр)
	if (0 != _nodes) delete[] _nodes;
}


/*!	\return Количество байт, которое было выделенно для хранения всей
	информации о деревьях (не считая информацию о саббендах)
*/
sz_t wtree::nodes_sz() const {
	return (coefs() * sizeof(wnode));
}


/*!	\param[in] from Вейвлет спектр, значения коэффициентов которого,
	будут скопированы

	Функция также автоматически выполняет квантование с <i>q = 1</i>
	(вызывает wtree::quantize()).
*/
void wtree::load(const w_t *const from) {
	assert(0 != from);

	// _reset_trees_content();

	for (sz_t i = 0; coefs() > i; ++i) {
		_nodes[i].w = from[i];
	}

	quantize();
}


/*!	\param[in] q Квантователь

	Функция производит квантование коэффициентов. Исходные коэффициенты
	берутся из wnode::w, результат квантования помещается в wnode::wq.

	После квантования функция вызывает wtree::refresh() для сброса
	остальных полей элементов дерева.
*/
void wtree::quantize(const q_t q) {
	for (sz_t i = 0; coefs() > i; ++i) {
		wnode &node = _nodes[i];
		node.wq = wnode::quantize(node.w, q);
	}

	_q = q;

	refresh();
}


/*! Функция производит следующие действия:
	- Выполняет присваивание wnode::wc = wnode::wq и тем самым даёт
	  начальное значение для откорректированного коэффициента
	- Обнуляет значения функции Лагранжа (wnode::j0 и wnode::j1)
	- Обнуляет групповой признак подрезания wnode::n, делая все ветви дерева
	  не подрезанными
	- Обнуляет признак валидности коэффициента (wnode::invalid), делая
	  все элементы валидными
*/
void wtree::refresh()
{
	// для каждого элемента из дерева
	for (sz_t i = 0; coefs() > i; ++i) {
		// ссылка на очередной элемент
		wnode &node = _nodes[i];

		// восстанавливаем значение откорректированного коэффициента
		node.wc			= node.wq;
		// значение функций лагранжа не известно
		node.j0			= 0;
		node.j1			= 0;
		// ничего не подрезано
		node.n			= 0;
		// узел хороший
		node.invalid	= false;
	}
}


/*!	\return Константная ссылка на объект wiv::subbands
	\sa subbands
*/
subbands &wtree::sb() {
	assert(0 != _subbands);

	return (*_subbands);
}


/*!	\return Константная ссылка на объект wiv::subbands
	\sa subbands
*/
const subbands &wtree::sb() const {
	assert(0 != _subbands);

	return (*_subbands);
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


/*!	\param[in] node Ссылка на узел
*/
p_t wtree::get_pos(const wnode &node) const {
	assert(_nodes <= &node);

	const sz_t offset = sz_t(&node - _nodes);

	assert(offset < coefs());

	return p_t(offset % _width, offset / _height);
}


/*!	\param[in] c Координаты элемента
*/
p_t wtree::prnt(const p_t &c) {
	assert(0 <= c.x && c.x < _width);
	assert(0 <= c.y && c.y < _height);

	return p_t(c.x / 2, c.y / 2);
}


/*!	\param[in] sb Саббенд по которому будет происходить обход с помощью
	возвращённого итератора
	\return Итератор по саббенду
*/
wtree::coefs_iterator wtree::iterator_over_subband(
	const subbands::subband_t &sb)
{
	return new snake_square_iterator(p_t(sb.x_min, sb.y_min),
									 p_t(sb.x_max, sb.y_max));
}


/*!	\param[in] prnt Координаты родительского коэффициента
	\return Итератор по дочерним коэффициентам
*/
wtree::coefs_iterator wtree::iterator_over_children(const p_t &prnt)
{
	// координаты верхнего левого дочернего элемента
	const p_t c = _children_top_left(prnt);

	// создание итератора
	return new snake_square_iterator(p_t(c.x    , c.y    ),
									 p_t(c.x + 1, c.y + 1));
}


/*!	\param[in] root Координаты корневого элемента
	\param[in] sb Саббенд, листья из которого будут рассматриваться во время
	процесса итерации.
	\return Итератор по листьям

	\todo Написать функцию получения саббенда в котором находится коэффициент
	\todo Эту функцию необходимо протестировать
*/
wtree::coefs_iterator wtree::iterator_over_leafs(const p_t &root,
												 const subbands::subband_t &sb)
{
	const p_t c = _leafs_top_left(root, sb.lvl, sb.i);

	return new snake_square_iterator(p_t(c.x, c.y),
									 p_t(c.x + sb.tree_w, c.y + sb.tree_h));
}


/*!	\param[in] root Координаты корневого элемента
	\param[in] lvl Уровень, листья с которого будут рассматриваться во время
	процесса итерации
	\param[in] i Индекс саббенда внутри уровня, листья из которого буду
	рассматриваться
	\return Итератор по листьям
*/
wtree::coefs_iterator wtree::iterator_over_leafs(const p_t &root,
												 const sz_t lvl, const sz_t i)
{
	return iterator_over_leafs(root, sb().get(lvl, i));
}


////////////////////////////////////////////////////////////////////////////////
// wtree class protected definitions

/*!	Очищает память, занимаемую деревом коэффициентов. Все значения
	сбрасываются и устанавливаются в 0.
*/
void wtree::_reset_trees_content() {
	memset(_nodes, 0, nodes_sz());
}


/*!	\param[in] prnt Координаты родительского элемента
	\return Координаты верхнего левого дочернего элемента из группы
	дочерних элементов

	\warning Функция не пригодна для использования с координатами
	родительских элементов из саббенда LL, так как в этом случае дочерние
	элементы оказываются в разных саббендах.
*/
p_t wtree::_children_top_left(const p_t &prnt) {
	return p_t(2 * prnt.x, 2 * prnt.y);
}


/*!	\param[in] root Координаты корневого элемента дерева
	\param[in] lvl Уровень на котором находится требуемый блок листьев
	\param[in] i Индекс саббенда на уровне (аналогично subbands::_get())
	\return Координаты верхнего левого элемента блока листьев данного
	дерева.

	\todo Написать более подробное описание
*/
p_t wtree::_leafs_top_left(const p_t &root, const sz_t lvl, const sz_t i)
{
	// будем часто обращаться к LL саббенду
	const subbands::subband_t &sb_LL = sb().get_LL();

	// удостоверимся, что корневой элемент из LL саббенда
	assert(sb_LL.x_min <= root.x && root.x <= sb_LL.x_max);
	assert(sb_LL.y_min <= root.y && root.y <= sb_LL.y_max);

	// если нулевой уровень, то независимо от индекса саббенда возвращаем
	// корневой элемент
	if (0 == lvl) return root;

	// находим координаты элемента из саббенда нулевого уровня с нужным
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



}	// end of namespace wic
