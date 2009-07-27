/*!	\file     dbg_surface.h
	\version  0.0.1
	\author   mice, ICQ: 332-292-380, mailto:wonder.mice@gmail.com
	\brief    �������� ������ wicdbg::dbg_surface - ����������� ��� �������
			  �������� �����������

	\todo     ����� �������� ������� ���� dbg_surface.h
*/

#ifndef WIC_LIBWICDBG_DBG_SURFACE
#define WIC_LIBWICDBG_DBG_SURFACE

///////////////////////////////////////////////////////////////////////////////
// include

// standard C++ library headers
#include <assert.h>
#include <math.h>
#include <string>
#include <fstream>
#include <iomanip>

// imgs headers
#include <imgs/bmp_file.h>
#include <imgs/bmp_dump.h>

// libwic headers
#include <wic/libwic/types.h>

// libwicdbg headers
#include <wic/libwicdbg/dbg_pixel.h>


///////////////////////////////////////////////////////////////////////////////
// wicdbg namespace
namespace wicdbg
{


////////////////////////////////////////////////////////////////////////////////
// dbg_surface class declaration
//!	\brief ���������� �����������, ������� ��������� ��������� ��������������
//!	���������� � ���������� ������������� � ������������ � � ������� �����.
/*!
*/
class dbg_surface
{
public:
	// public types ------------------------------------------------------------

	//!	\brief ��������� ��� ���� imgs::img_rgb::rgb24_t, ����� ��������� ���
	//!	�������������
	typedef imgs::img_rgb::rgb24_t rgb24_t;

	// public constants --------------------------------------------------------

	//!	\brief ����� �� ������������ ������ ��������������� ������
	static const wic::sz_t INVALID_MODEL	= wic::MAX_ACODER_MODELS;

	// public methods ----------------------------------------------------------

	//!	\name ������������ � �����������
	//@{

	//!	\brief �����������
	dbg_surface(const wic::sz_t w, const wic::sz_t h);

	//!	\brief ����������
	~dbg_surface();

	//@}

	//!	\brief ���������� � �����������
	//@{

	//!	\bref ���������� ������ �����������
	wic::sz_t width() const { return _w; }

	//!	\bref ���������� ������ �����������
	wic::sz_t height() const { return _h; }

	//!	\bref ���������� ����� ���������� ��������� � �����������
	wic::sz_t size() const { return _sz; }

	//@}

	//!	\brief ������ � ������������
	//@{

	//!	\brief ���������� ������� �����������, �������� ��� ���� ���������
	//! ������������ ����������
	void clear();

	//!	\brief ���������� ����������� ������ �� ������� � �����������
	//!	���������
	const dbg_pixel &get(const wic::sz_t i) const;

	//!	\brief ���������� ������ �� ������� � ������������ ���������
	dbg_pixel &get(const wic::sz_t i);

	//!	\brief ���������� ����������� ������ �� ������� � ������������
	//!	������������
	const dbg_pixel &get(const wic::sz_t x, const wic::sz_t y) const;

	//!	\brief ���������� ������ �� ������� � ������������ ������������
	dbg_pixel &get(const wic::sz_t x, const wic::sz_t y);

	//!	\brief ���������� ����������� ������ �� ������� � ������������
	//!	������������
	const dbg_pixel &get(const wic::p_t &p) const;

	//!	\brief ���������� ������ �� ������� � ������������ ������������
	dbg_pixel &get(const wic::p_t &p);

	//@}

	//!	\brief ������� ����������� � ����
	//@{

	//!	\brief
	/*!
	*/
	template <const dbg_pixel::members member>
	void save(const std::string &path, const bool as_bmp = false) const
	{
		// �������� �����������
		assert(0 < _w && 0 < _h && 0 < _sz);
		assert(0 != _surface);

		// ��������� ��� ���� ������������� �����
		typedef dbg_pixel::type_selector<member>::result member_type;

		// �������� ���������� �������
		member_type *const data = new member_type[_sz];

		// ����������� �������� ���������� ����� �� ��������� �����
		for (wic::sz_t i = 0; _sz > i; ++i)
		{
			data[i] = _surface[i].get<member>();
		}

		// ���������� � ��������� �������
		if (as_bmp)
		{
			imgs::bmp_dump<member_type, wic::sz_t>::dump(data, _w, _h,
														 path);
		}
		else
		{
			imgs::bmp_dump<member_type, wic::sz_t>::txt_dump(data, _w, _h,
															 path);
		}

		// ������������ ���������� �������
		delete[] data;
	}

	//!	\ brief
	/*!
	*/
	template <const dbg_pixel::members member>
	void diff(const dbg_surface &another, const std::string &diff_path)
	{
		// �������� �����������
		assert(size()	== another.size());
		assert(width()	== another.width());
		assert(height()	== another.height());

		// ����� ��� ������ ���������� � ���������
		std::ofstream diff_out(diff_path.c_str());

		// ��� ����� � ������� ������������ ������
		typedef dbg_pixel::type_selector<member>::result member_type;

		// ���������� ��������� ��������
		wic::sz_t diffs_count = 0;

		// ���� �� ���� �����������
		for (wic::sz_t y = 0; height() > y; ++y)
		{
			for (wic::sz_t x = 0; width() > x; ++x)
			{
				// ��������� �������� ������ ��������� ������������
				const member_type a = get(x, y).get<member>();
				const member_type b = another.get(x, y).get<member>();

				// ��������� ��������
				if (a == b) continue;

				// ���������� ���������� ��������� ��������
				++diffs_count;

				// ������ ���������� � �������� � ����
				diff_out << std::setw(5) << diffs_count << " - ["
						 << std::setw(4) << x << ", "
						 << std::setw(4) << y << "]: a = "
						 << a << ", b = " << b << std::endl;
			}
		}
	}

	//@}

	//!	\name ����������� ������
	//@{

	template <class T>
	static bool diff_dump(const T *const src1, const T *const src2,
						  const std::string &path,
						  const wic::sz_t w, const wic::sz_t h,
						  const bool as_bmp = false)
	{
		// ��������� �� ��������� ������ ���� �� ��������
		assert(0 != src1 && 0 != src2);

		// ���������� ��������� � ������ ���������
		const wic::sz_t sz = w * h;

		// ��������� ������ ��� ��������
		T *const diff = (T *)malloc(sz * sizeof(T));

		if (0 == diff) return false;

		// ������� �������
		for (int i = 0; sz > i; ++i)
		{
			diff[i] = (src1[i] - src2[i]);
		}

		// ���������� � ��������� �������
		if (as_bmp)
		{
			imgs::bmp_dump<T, wic::sz_t>::dump(diff, w, h, path);
		}
		else
		{
			imgs::bmp_dump<T, wic::sz_t>::txt_dump(diff, w, h, path, 10);
		}

		free(diff);

		return true;
	}

	//@}

protected:
	// protected members -------------------------------------------------------

	//!	\brief ���������� �������� �������� �� ��� �����������
	wic::sz_t _offset(const wic::sz_t x, const wic::sz_t y) const;

private:
	// private data ------------------------------------------------------------

	//!	\brief ������ �����������
	const wic::sz_t _w;

	//!	\brief ������ �����������
	const wic::sz_t _h;

	//!	\brief ���������� ��������� � �����������
	const wic::sz_t _sz;

	//!	\brief ���������� �����������
	dbg_pixel *_surface;

};



}	// end of namespace wicdbg



#endif	// WIC_LIBWICDBG_DBG_SURFACE
