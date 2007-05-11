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
	w_t w = 0;
	wk_t wk = 0;
	j_t j = 0;
	n_t n = 0;
	bool invalid = false;
	// _wtree.get_safe<wnode::member_wk>(0, 0, w);
	wnode node;
	node.w = 32;
	node.wc = 64;
	node.wk = 128;
	node.j0 = 11;
	node.j1 = 22;
	node.n = 4;
	node.invalid = true;

	// w = wnode::field<wnode::member_wk>::get(node);

	w = node.get<wnode::member_w>();
	w = node.get<wnode::member_wc>();
	wk = node.get<wnode::member_wk>();
	j = node.get<wnode::member_j0>();
	j = node.get<wnode::member_j1>();
	n = node.get<wnode::member_n>();
	invalid = node.get<wnode::member_invalid>();
	//const sz_t lvl = tree.sb().lvls() + subbands::LVL_PREV;
}



////////////////////////////////////////////////////////////////////////////////
// wtree class protected definitions



}	// end of namespace wic
