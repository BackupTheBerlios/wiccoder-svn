/*******************************************************************************
* file:         subbands.cpp                                                   *
* version:      0.1.0                                                          *
* author:       mice (mailto:my_mice@mail.ru, ICQ:332-292-380)                 *
* description:  not available                                                  *
* tests:        none                                                           *
*******************************************************************************/


////////////////////////////////////////////////////////////////////////////////
// headers

// standard C++ library headers
#include <new>

// libwic headers
#include <wic/libwic/types.h>
#include <wic/libwic/subbands.h>


////////////////////////////////////////////////////////////////////////////////
// wic namespace
namespace wic {


////////////////////////////////////////////////////////////////////////////////
// subbands class public definitions

/*!	\param[in] width Ширина изображения
	\param[in] height Высота изображения
	\param[in] lvls Количество уровней вейвлет преобразования
*/
subbands::subbands(const sz_t width, const sz_t height, const sz_t lvls) {
	// сохранение параметров изображения
	_width	= width;
	_height	= height;
	_lvls	= lvls;

	// вычисление количества саббендов
	_mcount	= SUBBANDS_PER_LEVEL * _lvls;
	_count	= _mcount + SUBBANDS_ON_0_LEVEL;

	// выделение памяти
	_sb = new subband_t[_count];
	if (0 == _sb) throw std::bad_alloc();

	// координаты HH саббенда
	sz_t x_min = _width / 2;
	sz_t y_min = _height / 2;
	sz_t x_max = _width - 1;
	sz_t y_max = _height - 1;

	// количество коэффициентов в HH саббенде
	sz_t n = _width * _height / SUBBANDS_DENOMINATOR;

	// заполнения информации о саббендах (кроме LL)
	for (sz_t lvl = _lvls; 0 < lvl; --lvl) {

		_mk_lvl(lvl, x_min, y_min, x_max, y_max);

		x_max = x_min - 1;
		y_max = y_min - 1;
		x_min /= 2;
		y_min /= 2;
	}

	// количество деревьев в разложении
	const sz_t trees_count = (x_max * y_max);

	// заполнения информации о LL саббенде
	subband_t &sb_ll = subbands::sb_ll();
	sb_ll.x_min = 0;
	sb_ll.y_min = 0;
	sb_ll.x_max = x_max;
	sb_ll.y_max = y_max;
	sb_ll.count = trees_count;

	// подсчёт количества узлов приходяшегося на одно дерево у саббенде
	for (sz_t i = 0; _count > i; ++i) {
		subband_t &sb = subbands::sb(i);
		sb.npt = sb.count / n;
	}
}


/*!
*/
subbands::~subbands() {
	if (0 != _sb) delete[] _sb;
}


/*!	\param[in] i Номер (индекс) саббенда
	\return Саббенд с указанным индексом

	Это одна из основных открытая функция для доступа к саббендам,
	через которую работают все остальные.
*/
subbands::subband_t &subbands::sb(const sz_t i) {
	assert(0 <= i && i < _count);

	if (0 == i) return _sb[_mcount];

	return _sb[i - SUBBANDS_ON_0_LEVEL];
}


/*!	\param[in] lvl Уровень разложения
	\param[in] i Номер саббеда с уровня \c lvl

	Уровень разложения - число от 0 до количества уровней разложения
	(включительно). На уровне 0 находится только один LL саббенд и
	параметр \c i не учитывается.

	Это одна из основных открытая функция для доступа к саббендам,
	через которую работают все остальные.
*/
subbands::subband_t &subbands::sb(const sz_t lvl, const sz_t i) {

	assert(0 <= i && i < SUBBANDS_PER_LEVEL);
	assert(0 <= lvl && lvl <= _lvls);

	if (0 == lvl) return _sb[_mcount];

	const sz_t k = (lvl - 1) * SUBBANDS_PER_LEVEL + i;

	assert(0 <= k && k <= _count);

	return _sb[k];
}


////////////////////////////////////////////////////////////////////////////////
// subbands class public definitions

/*!	\param[in] lvl Уровень вейвлет преобразования
	\param[in] x_min Координата HH саббенда
	\param[in] y_min Координата HH саббенда
	\param[in] x_max Координата HH саббенда
	\param[in] y_max Координата HH саббенда
*/
void subbands::_mk_lvl(const sz_t lvl,
					   const sz_t x_min, const sz_t y_min,
					   const sz_t x_max, const sz_t y_max)
{
	// количество коэффициентов в саббендах на этом уровне
	const sz_t count = x_min * y_min;

	// ссылки на HL, LH и HH саббенды этого уровня соответственно
	subband_t &sb_hl = sb(lvl, 0);
	subband_t &sb_lh = sb(lvl, 1);
	subband_t &sb_hh = sb(lvl, 2);

	// саббенд HL
	sb_hl.x_min = x_min;
	sb_hl.y_min = 0;
	sb_hl.x_max = x_max;
	sb_hl.y_max = y_min - 1;
	sb_hl.count = count;

	// саббенд LH
	sb_lh.x_min = 0;
	sb_lh.y_min = y_min;
	sb_lh.x_max = x_min - 1;
	sb_lh.y_max = y_max;
	sb_lh.count = count;

	// саббенд HH
	sb_hh.x_min = x_min;
	sb_hh.y_min = y_min;
	sb_hh.x_max = x_max;
	sb_hh.y_max = y_max;
	sb_hh.count = count;
}



}	// namespace wic
