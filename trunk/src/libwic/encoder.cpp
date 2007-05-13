/*******************************************************************************
* file:         encoder.cpp                                                    *
* version:      0.1.0                                                          *
* author:       mice (mailto:my_mice@mail.ru, ICQ:332-292-380)                 *
* description:  not available                                                  *
* tests:        none                                                           *
*******************************************************************************/


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
	_wtree(width, height, lvls)
{
	// загрузка коэффициентов разложения в дерево
	_wtree.load(image);

	// выделение памяти для арифметического енкодера
	_aenc.mk_buf(width * height * sizeof(aencoder::tv_t));
}


/*!
*/
encoder::~encoder() {
}


/*!
*/
void encoder::encode() {
	_aenc.begin();

	_encode_step_1();

	_aenc.end();
}



////////////////////////////////////////////////////////////////////////////////
// wtree class protected definitions

/*!	\param[in] s Значение прогнозной величины <i>S<sub>j</sub></i>
	\param[in] lvl Номер уровня разложения, из которого был взят коэффициент
	\return Номер выбираемой модели
*/
sz_t encoder::_ind_spec(const pi_t &s, const sz_t lvl) {
	if (0 == lvl) return 0;
	if (1 == lvl) return 1;

	if (26.0  <= s) return 1;
	if ( 9.8  <= s) return 2;
	if ( 4.1  <= s) return 3;
	if ( 1.72 <= s) return 4;

	return 5;
}


/*!	\param[in] p Значение прогнозной величины <i>P<sub>i</sub></i>
	\param[in] is_LL берётся ли коэффициент из саббенда LL
	\return Номер выбираемой модели
*/
sz_t encoder::_ind_map(const pi_t &p, const bool is_LL) {
	if (is_LL) return 0;

	if (4.0 <= p) return 4;
	if (1.1 <= p) return 3;
	if (0.3 <= p) return 2;

	return 1;
}


/*!	\param[in] m Номер модели для кодирования
	\param[in] wk Значение коэффициента для кодирования
	\return Битовые затраты, необходимые для кодирования коэффициента с
	использованием этой модели

	\todo Необходимо учитывать смешение при выборе номера модели, т.к.
	модели нумеруются с 0 как для коэффициентов, так и для признаков.
*/
h_t encoder::_h_spec(const sz_t m, const wk_t &wk) {
	return _aenc.entropy(wk, m);
}


/*!	\param[in] m Номер модели для кодирования
	\param[in] n Значение группового признака подрезания ветвей
	\return Битовые затраты, необходимые для кодирования группового
	признака подрезания

	\todo Необходимо учитывать смешение при выборе номера модели, т.к.
	модели нумеруются с 0 как для коэффициентов, так и для признаков.
*/
h_t encoder::_h_map(const sz_t m, const n_t &n) {
	return _aenc.entropy(n, m);
}


/*!	\param[in] p Координаты коэффициента для корректировки
	\param[in] sb Саббенд, в котором находится коэффициент
	\return Откорректированное значение коэффициента

	\todo Реализовать эту функцию
*/
wk_t &encoder::_coef_fix(const p_t &p, const subbands::subband_t &sb)
{
	wnode &node = _wtree.at(p);

	wk_t fixed_wk = 0;

	for (;;) {
	}

	return node.wk;
}


/*!
*/
void encoder::_encode_step_1() {
	// просматриваются все узлы предпоследнего уровня
	const sz_t lvl = _wtree.sb().lvls() + subbands::LVL_PREV;

	// корректировка проквантованных коэффициентов
	for (sz_t k = 0; subbands::SUBBANDS_ON_LEVEL > k; ++k) {
		const subbands::subband_t &sb = _wtree.sb().get(lvl, k);

		// для всех коэффициентов в саббенде
		for (wtree::coefs_iterator i = _wtree.iterator_over_subband(sb);
			!i->end(); i->next())
		{
			_coef_fix(i->get(), sb);
		}
	}	
}



}	// end of namespace wic
