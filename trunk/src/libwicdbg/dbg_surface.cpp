/*!	\file     dbg_surface.cpp
	\version  0.0.1
	\author   mice, ICQ: 332-292-380, mailto:wonder.mice@gmail.com
	\brief    Оппределение класса wicdbg::dbg_surface - поверхности для отладки
			  процесса кодирования

	\todo     Более подробно описать файл dbg_surface.cpp
*/


///////////////////////////////////////////////////////////////////////////////
// include

// standard C++ library headers
// none

// libwicdbg headers
#include <wic/libwicdbg/dbg_surface.h>


////////////////////////////////////////////////////////////////////////////////
// wicdbg namespace
namespace wicdbg
{


////////////////////////////////////////////////////////////////////////////////
// dbg_surface class public function definitions

/*!	\param[in] w Ширина поверхности
	\param[in] h Высота поверхности
*/
dbg_surface::dbg_surface(const wic::sz_t w, const wic::sz_t h):
	_surface(0), _w(w), _h(h), _sz(w * h)
{
	// проверка утверждений
	assert(0 < _w && 0 < _h);
	assert(_sz == _w * _h);

	// выделения памяти под поверхность
	_surface = new dbg_pixel[_w * _h];

	// проверка успешного выделения памяти (чтоб была =)
	assert(0 != _surface);
}


/*!
*/
dbg_surface::~dbg_surface()
{
	// освобождение памяти, занимаемой поверхностью
	delete[] _surface;
}


/*!
*/
void dbg_surface::clear()
{
	// проверка утверждений
	assert(0 != _surface);

	// очистка поверхности
	for (wic::sz_t i = 0; _sz > i; ++i)
	{
		dbg_pixel &pixel	= _surface[i];
		pixel.wc			= 0;
		pixel.wc_model		= INVALID_MODEL;
		pixel.sign_model	= INVALID_MODEL;
		pixel.n				= 0;
		pixel.n_model		= INVALID_MODEL;
	}
}


/*!	\param[in] i Индекс (смещение) элемента
	\return Константная ссылка на элемент поверхности
*/
const dbg_pixel &dbg_surface::get(const wic::sz_t i) const
{
	// проверка утверждений
	assert(0 != _surface);
	assert(0 <= i && i < _sz);

	// возврат результата
	return _surface[i];
}


/*!	\param[in] i Индекс (смещение) элемента
	\return Ссылка на элемент поверхности
*/
dbg_pixel &dbg_surface::get(const wic::sz_t i)
{
	// проверка утверждений
	assert(0 != _surface);
	assert(0 <= i && i < _sz);

	// возврат результата
	return _surface[i];
}


/*!	\param[in] x <i>X</i> координата элемента
	\param[in] y <i>Y</i> координата элемента
	\return Константная ссылка на элемент поверхности
*/
const dbg_pixel &dbg_surface::get(const wic::sz_t x, const wic::sz_t y) const
{
	// возврат результата
	return get(_offset(x, y));
}


/*!	\param[in] x <i>X</i> координата элемента
	\param[in] y <i>Y</i> координата элемента
	\return Ссылка на элемент поверхности
*/
dbg_pixel &dbg_surface::get(const wic::sz_t x, const wic::sz_t y)
{
	// возврат результата
	return get(_offset(x, y));
}


/*!	\param[in] p Координаты элемента
	\return Константная ссылка на элемент поверхности
*/
const dbg_pixel &dbg_surface::get(const wic::p_t &p) const
{
	return get(p.x, p.y);
}


/*!	\param[in] p Координаты элемента
	\return Ссылка на элемент поверхности
*/
dbg_pixel &dbg_surface::get(const wic::p_t &p)
{
	return get(p.x, p.y);
}


////////////////////////////////////////////////////////////////////////////////
// dbg_surface class protected function definitions

/*!	\param[in] x <i>X</i> координата элемента
	\param[in] y <i>Y</i> координата элемента
	\return Смещение элемента в массиве
*/
wic::sz_t dbg_surface::_offset(const wic::sz_t x, const wic::sz_t y) const
{
	// проверка входных аргументов
	assert(0 <= x && x < _w);
	assert(0 <= y && x < _h);

	// смещение элемента в массиве
	const wic::sz_t offset = x + _w * y;

	// проверка смещения
	assert(0 <= offset && offset < _sz);

	// возврат результата
	return offset;
}



}	// end of namespace wicdbg
