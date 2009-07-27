/*!	\file     dbg_surface.cpp
	\version  0.0.1
	\author   mice, ICQ: 332-292-380, mailto:wonder.mice@gmail.com
	\brief    ������������ ������ wicdbg::dbg_surface - ����������� ��� �������
			  �������� �����������

	\todo     ����� �������� ������� ���� dbg_surface.cpp
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

/*!	\param[in] w ������ �����������
	\param[in] h ������ �����������
*/
dbg_surface::dbg_surface(const wic::sz_t w, const wic::sz_t h):
	_surface(0), _w(w), _h(h), _sz(w * h)
{
	// �������� �����������
	assert(0 < _w && 0 < _h);
	assert(_sz == _w * _h);

	// ��������� ������ ��� �����������
	_surface = new dbg_pixel[_w * _h];

	// �������� ��������� ��������� ������ (���� ���� =)
	assert(0 != _surface);
}


/*!
*/
dbg_surface::~dbg_surface()
{
	// ������������ ������, ���������� ������������
	delete[] _surface;
}


/*!
*/
void dbg_surface::clear()
{
	// �������� �����������
	assert(0 != _surface);

	// ������� �����������
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


/*!	\param[in] i ������ (��������) ��������
	\return ����������� ������ �� ������� �����������
*/
const dbg_pixel &dbg_surface::get(const wic::sz_t i) const
{
	// �������� �����������
	assert(0 != _surface);
	assert(0 <= i && i < _sz);

	// ������� ����������
	return _surface[i];
}


/*!	\param[in] i ������ (��������) ��������
	\return ������ �� ������� �����������
*/
dbg_pixel &dbg_surface::get(const wic::sz_t i)
{
	// �������� �����������
	assert(0 != _surface);
	assert(0 <= i && i < _sz);

	// ������� ����������
	return _surface[i];
}


/*!	\param[in] x <i>X</i> ���������� ��������
	\param[in] y <i>Y</i> ���������� ��������
	\return ����������� ������ �� ������� �����������
*/
const dbg_pixel &dbg_surface::get(const wic::sz_t x, const wic::sz_t y) const
{
	// ������� ����������
	return get(_offset(x, y));
}


/*!	\param[in] x <i>X</i> ���������� ��������
	\param[in] y <i>Y</i> ���������� ��������
	\return ������ �� ������� �����������
*/
dbg_pixel &dbg_surface::get(const wic::sz_t x, const wic::sz_t y)
{
	// ������� ����������
	return get(_offset(x, y));
}


/*!	\param[in] p ���������� ��������
	\return ����������� ������ �� ������� �����������
*/
const dbg_pixel &dbg_surface::get(const wic::p_t &p) const
{
	return get(p.x, p.y);
}


/*!	\param[in] p ���������� ��������
	\return ������ �� ������� �����������
*/
dbg_pixel &dbg_surface::get(const wic::p_t &p)
{
	return get(p.x, p.y);
}


////////////////////////////////////////////////////////////////////////////////
// dbg_surface class protected function definitions

/*!	\param[in] x <i>X</i> ���������� ��������
	\param[in] y <i>Y</i> ���������� ��������
	\return �������� �������� � �������
*/
wic::sz_t dbg_surface::_offset(const wic::sz_t x, const wic::sz_t y) const
{
	// �������� ������� ����������
	assert(0 <= x && x < _w);
	assert(0 <= y && x < _h);

	// �������� �������� � �������
	const wic::sz_t offset = x + _w * y;

	// �������� ��������
	assert(0 <= offset && offset < _sz);

	// ������� ����������
	return offset;
}



}	// end of namespace wicdbg
