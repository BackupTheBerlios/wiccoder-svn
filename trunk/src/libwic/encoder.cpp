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
	_wtree.load(image);
}


/*!
*/
encoder::~encoder() {
}


/*!
*/
void encoder::encode() {
	_encode_step_1();
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


/*!
*/
void encoder::_encode_step_1() {
	// просматриваются все узлы предпоследнего уровня
	const sz_t lvl = _wtree.sb().lvls() + subbands::LVL_PREV;

	// корректировка проквантованных коэффициентов
	int i = 1;
	int ar[4] = {i, i+1, i+2, i+3};
	// for ()

	/*
	snake_square_iterator iter(p_t(0, 0), p_t(15, 15));
	for (; !iter.end(); iter.next()) {
		iter.get()
	}
	*/
	
}



}	// end of namespace wic
