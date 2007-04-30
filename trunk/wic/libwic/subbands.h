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
#include <assert.h>

// libwic headers
#include <wic/libwic/types.h>


////////////////////////////////////////////////////////////////////////////////
// wic namespace
namespace wic {


////////////////////////////////////////////////////////////////////////////////
// subbands class declaration

//! \brief Тип который содержит информацию о расположении саббендов
/*!	
*/
class subbands {
public:
	// public types ------------------------------------------------------------

	//! \brief Структура описывает отдельно взятый саббенд
	/*!	Координаты указываются включительно со всех сторон.
	*/
	struct subband_t {
		sz_t	x_min;
		sz_t	y_min;
		sz_t	x_max;
		sz_t	y_max;
		sz_t	count;	//!< \brief Вершин в саббенде
		sz_t	npt;	//!< \brief Вершин в одной ветке дерева (nodes per tree)
	};

	// public constants --------------------------------------------------------

	//! \brief Количество саббендов в уровне (кроме нулевого уровня)
	static const sz_t SUBBANDS_PER_LEVEL	= 3;

	//! \brief Количество саббендов в нулевом уровне
	static const sz_t SUBBANDS_ON_0_LEVEL	= 1;

	//! \brief Какую часть уровня разложения занимает один саббенд
	static const sz_t SUBBANDS_DENOMINATOR	= 4;

	//! \brief Индекс саббенда LL
	static const sz_t SUBBAND_LL			= 0;

	//! \brief Индекс саббенда HL
	static const sz_t SUBBAND_HL			= 0;

	//! \brief Индекс саббенда LH
	static const sz_t SUBBAND_LH			= 1;

	//! \brief Индекс саббенда HH
	static const sz_t SUBBAND_HH			= 2;

    // public functions --------------------------------------------------------

	//! \brief Конструктор
	subbands(const sz_t width, const sz_t height, const sz_t lvls);

	//! \brief Деструктор
	~subbands();

	//! \brief Ширина изображения
	sz_t width() const { return _width; }

	//! \brief Высота изображения
	sz_t height() const { return _height; }

	//! \brief Количество уровней разложения
	sz_t lvls() const { return _lvls; }

	//! \brief Возвращает саббенд по его индексу (номеру)
	subband_t &sb(const sz_t i = 0);

	//! \brief Возвращает нужный саббенд с нужного уровня
	subband_t &sb(const sz_t lvl, const sz_t i);

	//! \brief Возвращает LL саббенд
	subband_t &sb_ll() { return sb(); }

	//! \\brief Количество саббендов без LL
	sz_t mcount() const { return _mcount; }

	//! \brief Общее количество саббендов
	sz_t count() const { return _count; }

protected:
	// protected functions -----------------------------------------------------

	//! \brief Заполняет данные о уровне вейвлет преобразования
	inline void _mk_lvl(const sz_t lvl,
						const sz_t x_min, const sz_t y_min,
						const sz_t x_max, const sz_t y_max);

private:
	// private data ------------------------------------------------------------

	//!	\brief Ширина изображения
	sz_t _width;

	//! \brief Высота изображения
	sz_t _height;

	//! \brief Количество уровней вейвлет преобразования
	sz_t _lvls;

	//! \brief Массив описаний саббендов
	subband_t *_sb;

	//! \\brief Количество саббендов без LL
	sz_t _mcount;

	//! \brief Общее количество саббендов
	sz_t _count;
};



}	// wtc namespace



#endif	// WIC_LIBWIC_SUBBANDS_INCLUDED
