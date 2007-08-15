/*!	\file     defines.h
	\version  0.0.1
	\author   mice, ICQ: 332-292-380, mailto:wonder.mice@gmail.com
	\brief    Макросы и определения

	\todo     Более подробно описать файл defines.h
*/

#pragma once

#ifndef WIC_LIBWIC_DEFINES
#define WIC_LIBWIC_DEFINES

////////////////////////////////////////////////////////////////////////////////
// defines

// debug mode ------------------------------------------------------------------
#if defined _DEBUG || defined DEBUG

//!	\brief Автоматически определяется в отладочном режиме
/*!	Для переносимости, удобства и гибгости лучше использовать этот макрос
	для определения отладочного режима.
*/
#define LIBWIC_DEBUG

#endif



#endif
