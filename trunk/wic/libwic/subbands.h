/*******************************************************************************
* file:         subbands.h                                                     *
* version:      0.1.0                                                          *
* author:       mice (mailto:my_mice@mail.ru, ICQ:332-292-380)                 *
* description:  not available                                                  *
* tests:        none                                                           *
*******************************************************************************/

#pragma once

#ifndef WIC_LIBWIC_SUBBANDS_INCLUDED
#define WIC_LIBWIC_SUBBANDS_INCLUDED

////////////////////////////////////////////////////////////////////////////////
// headers

// standard C++ library headers
#include <new>							// for std::bad_alloc exception class
#include <assert.h>

// libwic headers
#include <wic/libwic/types.h>


////////////////////////////////////////////////////////////////////////////////
// wic namespace
namespace wic {


////////////////////////////////////////////////////////////////////////////////
// subbands class declaration

//! \brief �������� ���������� � ������������ ���������
/*!	������� ����� ������ ������ ���������� � ������������ ���������. ���
	������� �������� ������� ������, ������� ��������:
	- ��������� ���������� ��������
	- ����� ���������� ������������� � ��������
	- ���������� �������������, ������������ �� ���� ������ � ��������
	- ��������� �� ������������ �������
*/
class subbands {
public:
	// public types ------------------------------------------------------------

	//! \brief ��������� ��������� �������� ������ �������
	/*!	���������� ����������� ������������ �� ���� ������.
	*/
	struct subband_t {
		sz_t	x_min;		//!< \brief x ���������� ������ ��������
							//!< ������������
		sz_t	y_min;		//!< \brief y ���������� ������ ��������
							//!< ������������
		sz_t	x_max;		//!< \brief x ���������� ������� �������
							//!< ������������
		sz_t	y_max;		//!< \brief y ���������� ������� �������
							//!< ������������
		sz_t	count;		//!< \brief ������ � ��������
		sz_t	npt;		//!< \brief ������ � ����� ����� ������
							//!< (nodes per tree)
		subband_t *prnt;	//!< \brief ��������� �� ������������ �������.
							//!< 0 ���� ��� LL �������.
		sz_t	lvl;		//!< \brief ����� ������ �� ������� ���������
							//!< �������
	};

	// public constants --------------------------------------------------------

	//! \brief ���������� ��������� � ������ (����� �������� ������)
	static const sz_t SUBBANDS_PER_LEVEL	= 3;

	//! \brief ���������� ��������� � ������� ������
	static const sz_t SUBBANDS_ON_0_LEVEL	= 1;

	//! \brief ������ �������� LL � ������ ����������
	static const sz_t SUBBAND_LL			= 0;

	//! \brief ������ �������� HL � ������ ����������
	static const sz_t SUBBAND_HL			= 0;

	//! \brief ������ �������� LH � ������ ����������
	static const sz_t SUBBAND_LH			= 1;

	//! \brief ������ �������� HH � ������ ����������
	static const sz_t SUBBAND_HH			= 2;

	//!	\brief ���������� ��������� �� ������ (����� ��������, �� �������
	//!	����� ���� �������)
	static const sz_t SUBBANDS_ON_LEVEL		= 3;

	//! \brief �������� ��� ��������� ����������� ������ (�������� � �������)
	static const dsz_t LVL_PREV				= -1;

	//! \brief �������� ��� ��������� ���������� ������ (�������� �� �������)
	static const dsz_t LVL_NEXT				= 1;

    // public functions --------------------------------------------------------

	//! \brief �����������
	subbands(const sz_t width, const sz_t height, const sz_t lvls);

	//! \brief ����������
	~subbands();

	//! \brief ������ �����������
	sz_t width() const { return _width; }

	//! \brief ������ �����������
	sz_t height() const { return _height; }

	//! \brief ���������� ������� ����������
	/*!	����� ��������, ��� �������� ������������� �� <i>(%lvls() + 1)</i>
		�������. ��� ���������� ��-�� ����, ��� �� ������� 0 ������ ���������
		<i>LL</i> �������, � ��� ������� �� ������� �� ���������� �������
		����������. �������, ���� ���������� �������� ������ � ��������� �
		���������� ������ (� ���������� ��������), ���������� ��������� �
		�������� get() ����� ������ ������ <i>%lvls()</i>.
	*/
	sz_t lvls() const { return _lvls; }

	//! \brief ���������� ������� �� ��� ������� (������)
	subband_t &get(const sz_t i = 0) {
		return *(_get(i));
	}

	//! \brief ���������� ������� �� ��� ������� (������)
	const subband_t &get(const sz_t i = 0) const {
		return *(_get(i));
	}

	//! \brief ���������� ������ ������� � ������� ������
	subband_t &get(const sz_t lvl, const sz_t i) {
		return *(_get(lvl, i));
	}

	//! \brief ���������� ������ ������� � ������� ������
	const subband_t &get(const sz_t lvl, const sz_t i) const {
		return *(_get(lvl, i));
	}

	//! \brief ���������� LL �������
	subband_t &get_LL() { return get(); }

	//! \brief ���������� ��������� ��� LL ��������
	sz_t mcount() const { return _mcount; }

	//! \brief ����� ���������� ���������
	sz_t count() const { return _count; }

protected:
	// protected functions -----------------------------------------------------

	//! \brief ���������� ��������� �� ������� �� ��� ������� (������)
	subband_t *_get(const sz_t i = 0) const;

	//! \brief ���������� ��������� �� ������ ������� � ������� ������
	subband_t *_get(const sz_t lvl, const sz_t i) const;

	//! \brief ��������� ������ � ������ ������� ��������������
	inline void _mk_lvl(const sz_t lvl,
						const sz_t x_min, const sz_t y_min,
						const sz_t x_max, const sz_t y_max);

private:
	// private data ------------------------------------------------------------

	//!	\brief ������ �����������
	sz_t _width;

	//! \brief ������ �����������
	sz_t _height;

	//! \brief ���������� ������� ������� ��������������
	sz_t _lvls;

	//! \brief ������ �������� ���������
	subband_t *_sb;

	//! \brief ���������� ��������� ��� LL ��������
	sz_t _mcount;

	//! \brief ����� ���������� ���������
	sz_t _count;
};



}	// wtc namespace



#endif	// WIC_LIBWIC_SUBBANDS_INCLUDED
