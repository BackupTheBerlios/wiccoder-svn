/*!	\file     defines.h
	\version  0.0.1
	\author   mice, ICQ: 332-292-380, mailto:wonder.mice@gmail.com
	\brief    ������� � �����������

	\todo     ����� �������� ������� ���� defines.h
*/

#pragma once

#ifndef WIC_LIBWIC_DEFINES
#define WIC_LIBWIC_DEFINES

////////////////////////////////////////////////////////////////////////////////
// defines

// debug mode ------------------------------------------------------------------
#if defined _DEBUG || defined DEBUG

//!	\brief ������������� ������������ � ���������� ������
/*!	��� �������������, �������� � �������� ����� ������������ ���� ������
	��� ����������� ����������� ������.
*/
#define LIBWIC_DEBUG

//!	\brief ������ ��� ��������� ������� � ������� wicdbg::dbg_surface
/*! ��������� ������ ������� � �������������� wicdbg::dbg_surface ��������
	����������� ���������� ������������ � ������ � ������� ��� ��������. ��� ��
	�������� ����������� ������� ����� �������� ������������ � ������������ ��
	����� �����������. ������, ��������� ������ ������ �������, ����� ������
	��������� ������ ������.
*/
#define LIBWIC_USE_DBG_SURFACE

//!	\brief ������ ��� ��������� ����������� ��������� ���������� �
//!	������������ ��������
#define LIBWIC_ACODER_LOG_VALUES

//!	\brief ������ ��� ��������� ������ ���������� ������� ���������������
//!	������ � ����������� ����� ������
#define LIBWIC_ACODER_PRINT_STATISTIC



#endif	// defined _DEBUG || defined DEBUG


////////////////////////////////////////////////////////////////////////////////
/// dependencies
#ifdef LIBWIC_ACODER_PRINT_STATISTIC
#define LIBWIC_DEBUG
#endif



#endif
