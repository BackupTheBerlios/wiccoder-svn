/*!	\file     wavelets.h
	\version  0.0.1
	\author   mice, ICQ: 332-292-380, mailto:wonder.mice@gmail.com
	\brief    �������� ������� � ������� ��� ������ � ���������������
			  ����������� ������� �������� (��������������)
	
	\ref pgCoding_sectUnifiedWaveletFilters.
*/

/*! \page pgCoding ����������

	\section pgCoding_sectUnifiedWaveletFilters ������� ������
*/

#pragma once

#ifndef WIC_WICFILTERS_WAVELETS_INCLUDED
#define WIC_WICFILTERS_WAVELETS_INCLUDED

////////////////////////////////////////////////////////////////////////////////
// headers

// standard C++ library headers
#include <assert.h>
#include <string>

// wavelets headers
#include <wavelets/cdf97/cdf97.h>

// libwic headers
#include <wic/libwic/types.h>


////////////////////////////////////////////////////////////////////////////////
// wic namespace
namespace wic {


////////////////////////////////////////////////////////////////////////////////
// smart_2d_matrix class declaration
//!	\brief ����� ��������� �������
/*! ��������� �������, �������������� �������� �������� ������.
*/
template <class value_t>
class smart_2d_matrix
{
public:
	// public types ------------------------------------------------------------

	//!	\brief ��� ��������� �������
	typedef value_t value_type;

	//!	\brief ��� ����� ������� (�����������������)
	typedef smart_2d_matrix<value_type> this_type;

	// public constants --------------------------------------------------------

	//! \brief ��������� �������� ��� �������� ������
	static const int INIT_REF_COUNT			= 1;

	// public methods ----------------------------------------------------------

	//!	\name ������������ � �����������
	//@{

	//! \brief �����������
	/*!	\param[in] width ������ �������
		\param[in] height ������ �������
	*/
	smart_2d_matrix(const int width, const int height)
	{
		_alloc_matrix(width, height);
	}

	//!	\brief ����������� ���������
	/*!	\param[in] elements ������ ���������, ������� ����� ��������������
		��������
		\param[in] width ������ �������
		\param[in] height ������ �������
	*/
	smart_2d_matrix(value_type *const elements,
					const int width, const int height):
		_width(width), _height(height)
		_matrix(elements), _ref_count_ptr(0),
	{
		// �������� ����������
		assert(0 != _matrix);

		// �������� �������������� ��������
		assert(0 < _width && 0 < _height);
	}

	//!	\brief ����������� � �������������� � ��������������� ���������
	/*!	\param[in] elements ������, ��������������� �������� ��������
		����� �������� � �������
		\param[in] width ������ �������
		\param[in] height ������ �������
	*/
	template <class value_from_t>
	smart_2d_matrix(value_from_t *const elements,
					const int width, const int height):
	{
		_alloc_matrix(width, height);
		_copy_elements<value_from_t>(elements);
	}

	//!	\brief ���������� �����������
	/*!	\param[in] src �������� �������

		\attention �������� � �������������� ������� ����� ������������ �����
		������ ����� �������� �������� ������.
	*/
	smart_2d_matrix(const this_type &src):
		_width(src._width), _height(src._height), 
		_matrix(src._matrix),
		_ref_count_ptr(src._ref_count_ptr)
	{
		// ��������� ������� ������, ���� �� ������������
		if (0 != _ref_count_ptr) ++(*_ref_count_ptr);
	}

	//!	\brief ���������� ����������� � ���������������
	/*!	\param[in] src �������� �������

		\attention �������� � �������������� ������� ����� ������������
		���������� ������.
	*/
	template <class value_from_t>
	smart_2d_matrix(const smart_2d_matrix<value_from_t> &src)
	{
		_alloc_matrix(src._width, src._height);
		_copy_elements<value_from_t>(src._matrix);
	}

	//!	\brief ����������
	/*! ��������� ���������� ������ �� �������, � ���� �� ����������
		������ <i>0</i> ����������� �������, ���������� ��������.
	*/
	~smart_2d_matrix()
	{
		// ���� ������� ������ �� ������������, ������ ����������� �� ����
		if (0 != _ref_count_ptr) return;

		// ���������� �������� ������
		--(*_ref_count_ptr);

		// �������� ���������� ��������� ������ �������� 0
		if (0 >= (*_ref_count_ptr))
		{
			// ������������ �������
			_free_matrix();
		}
	}

	//@}

	//!	\name ���������� � �������
	//@{

	//!	\brief ������ �������
	int width() const { return _width; }

	//!	\brief ������ �������
	int height() const { return _height; }

	//@}

	//!	\name ������ � ��������� �������
	//@{

	//!	\brief ���������� ����������� �������� �� �������� �������
	const value_type *data() const { return _matrix; }

	//!	\brief ���������� �������� �� �������� �������
	value_type *data() { return _matrix; }

	//!	\brief ���������� ����������� ������ �� ������� ������� ��
	//!	����������� <i>(x, y)</i>
	/*!	\param[in] x ���������� <i>x</i>
		\param[in] y ���������� <i>y</i>
	*/
	const value_type &at(const int x, const int y) const
	{
		return _matrix[_width * y + x];
	}

	//!	\brief ���������� ������ �� ������� ������� �� �����������
	//!	<i>(x, y)</i>
	/*!	\param[in] x ���������� <i>x</i>
		\param[in] y ���������� <i>y</i>
	*/
	value_type &at(const int x, const int y)
	{
		return _matrix[_width * y + x];
	}

	//!	\brief �������� �������� ������� �� ������� ��������� ��������������
	//!	�����
	/*!	\param[in] elements ������ � ����������
	*/
	template <class value_from_t>
	void load(const value_from_t *const elements)
	{
		_copy_elements(elements);
	}

	//@}

protected:
	// protected methods -------------------------------------------------------

	//!	\name ������ � �������
	//@{

	//!	\brief �������� ������ ��� �������
	/*!	\param[in] width ������ �������
		\param[in] height ������ �������
	*/
	void _alloc_matrix(const int width, const int height)
	{
		// �������������� ������� �������
		_width = width;
		_height = height;

		// �������� �������������� ��������
		assert(0 < _width && 0 < _height);

		// ��������� ������ ��� �������� �������
		_matrix = new value_type[_width * _height];

		// ��������� ������ ��� ������� ������
		_ref_count_ptr = new int(INIT_REF_COUNT);

		// �������� ����������
		assert(0 != _matrix);
		assert(0 != _ref_count_ptr);
	}

	//!	\brief ����������� �������, ���������� ��������
	/*!
	*/
	void _free_matrix()
	{
		// �������� ����������
		assert(0 != _matrix);
		assert(0 != _ref_count_ptr);

		// ������������ ������� ���������
		if (0 != _matrix)
		{
			delete[] _matrix;
			_matrix = 0;
		}

		// ������������ �������� ������
		if (0 != _ref_count_ptr)
		{
			delete _ref_count_ptr;
			_ref_count_ptr = 0;
		}
	}

	//!	\brief �������� �������� ������� �� ������� ��������� ��������������
	//!	�����
	/*!	\param[in] elements ������ � ����������
	*/
	template <class value_from_t>
	void _copy_elements(const value_from_t *const elements)
	{
		// �������� ���������� � �������������� ��������
		assert(0 != elements);
		assert(0 != _matrix);
		assert(0 < _width && 0 < _height);

		// ������������ ����������� � ���������������
		for (int i = (_width * _height - 1); 0 <= i; --i)
		{
			_matrix[i] = (value_type)(elements[i]);
		}
	}

	//@}

private:
	// private data ------------------------------------------------------------

	//!	\brief ������ ��������� �������
	value_t *_matrix;

	//!	\brief ������ �������
	int _width;

	//!	\brief ������ �������
	int _height;

	//!	\brief ��������� �� ������� ������
	int *_ref_count_ptr;
};



////////////////////////////////////////////////////////////////////////////////
// wavelets class declaration

//!	\brief 
/*!	
*/	
template <class src_t, class dest_t>
static smart_2d_matrix<dest_t>
forward_2d_transform(const src_t *const src, const std::string &filter,
					 const int width, const int height, const int levels)
{
	smart_2d_matrix<float> result(width, height);
	result.load<src_t>(src);
	wt2d_cdf97_fwd(result.data(), height, width, levels);

	return result;
}

template <class src_t, class dest_t>
static smart_2d_matrix<dest_t>
inverse_2d_transform(const src_t *const src, const std::string &filter,
					 const int width, const int height, const int levels)
{
	smart_2d_matrix<float> result(width, height);
	result.load<src_t>(src);
	wt2d_cdf97_inv(result.data(), height, width, levels);

	return result;
}



}	/// end of namespace wic



#endif	// WIC_WICFILTERS_WAVELETS_INCLUDED
