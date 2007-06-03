/*!	\file     subbands.h
	\version  0.0.1
	\author   mice, ICQ: 332-292-380, mailto:wonder.mice@gmail.com
	\brief    Определение класса wic::subbands для работы с саббендами

	\todo     Более подробно описать файл subbands.h
*/

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

//! \brief Содержит информацию о расположении, размерах саббендов и связях
//!	между ними.
/*!	Объекты этого класса хранят информацию о расположении саббендов. Для
	каждого саббенда имеется запись, которая включает:
	- граничные координаты саббенда
	- общее количество коэффициентов в саббенде
	- количество коэффициентов, приходящихся на одно дерево в саббенде
	- указатель на родительский саббенд
	- размеры саббенда
	- размеры каждого дерева в саббенде
	- уровень на котором находится саббенд
	- номер саббенда на уровне

	\sa subbands::subband_t
*/
class subbands {
public:
	// public types ------------------------------------------------------------

	//! \brief Структура описывает отдельно взятый саббенд
	/*!	Координаты указываются включительно со всех сторон.
	*/
	struct subband_t {
		sz_t	x_min;		//!<\brief x координата левого верхнего
							//!	коэффициента
		sz_t	y_min;		//!<\brief y координата левого верхнего
							//!	коэффициента
		sz_t	x_max;		//!<\brief x координата правого нижнего
							//!	коэффициента
		sz_t	y_max;		//!<\brief y координата правого нижнего
							//!	коэффициента
		sz_t	count;		//!<\brief Элементов в саббенде
		sz_t	npt;		//!<\brief Элементов в одной ветке дерева
							//!	(nodes per tree)
		subband_t *prnt;	//!<\brief Указатель на родительский саббенд.
							//!	0 если это LL саббенд.
		sz_t	lvl;		//!<\brief Номер уровня на котором находится
							//!	саббенд
		sz_t	i;			//!<\brief Индекс (номер) саббенда в уровне
		sz_t	tree_w;		//!<\brief Ширина деревьев в этом саббенде (в
							//!	элементах)
		sz_t	tree_h;		//!<\brief Высота деревьев в этом саббенде (в
							//!	элементах)
		sz_t	width;		//!<\brief Ширина саббенда (в элементах)
		sz_t	height;		//!<\brief Высота саббенда (в элементах)
	};

	// public constants --------------------------------------------------------

	//! \brief Количество саббендов в уровне (кроме нулевого уровня)
	static const sz_t SUBBANDS_ON_LEVEL		= 3;

	//! \brief Количество саббендов в нулевом уровне
	static const sz_t SUBBANDS_ON_0_LEVEL	= 1;

	//! \brief Индекс саббенда LL в уровне разложения
	static const sz_t SUBBAND_LL			= 0;

	//! \brief Индекс саббенда HL в уровне разложения
	static const sz_t SUBBAND_HL			= 0;

	//! \brief Индекс саббенда LH в уровне разложения
	static const sz_t SUBBAND_LH			= 1;

	//! \brief Индекс саббенда HH в уровне разложения
	static const sz_t SUBBAND_HH			= 2;

	//! \brief Номер нулевого уровня, на котором располагается <i>LL</i>
	//!	саббенд.
	static const sz_t LVL_0					= 0;

	//! \brief Номер первого уровня, на котором располагаются три саббенда
	//!	дочерних от <i>LL</i>.
	static const sz_t LVL_1					= 1;

	//! \brief Смещение для получения предидущего уровня (ближнего к вершине)
	static const dsz_t LVL_PREV				= -1;

	//! \brief Смещение для получения следуюшего уровня (дальнего от вершины)
	static const dsz_t LVL_NEXT				= 1;

    // public functions --------------------------------------------------------

	//!	\name Конструкторы и деструкторы
	//@{

	//! \brief Конструктор
	subbands(const sz_t width, const sz_t height, const sz_t lvls);

	//! \brief Деструктор
	~subbands();

	//@}

	//!	\name Общая информация о спектре
	//@{

	//! \brief Ширина изображения
	sz_t width() const { return _width; }

	//! \brief Высота изображения
	sz_t height() const { return _height; }

	//! \brief Количество уровней разложения
	/*!	Стоит заметить, что саббенды располагаются на <i>(%lvls() + 1)</i>
		уровнях. Это происходит из-за того, что на уровене 0 всегда находится
		<i>LL</i> саббенд, и его наличие не зависит от количества уровней
		разложения. Поэтому, если необходимо получить доступ к саббендам с
		последнего уровня (с наибольшей площадью), необходимо указывать в
		функциях get() номер уровня равный <i>%lvls()</i>.
	*/
	sz_t lvls() const { return _lvls; }

	//@}

	//!	\name Информация о саббендах
	//@{

	//! \brief Возвращает саббенд по его индексу (номеру)
	subband_t &get(const sz_t i = 0) {
		return *(_get(i));
	}

	//! \brief Возвращает саббенд по его индексу (номеру)
	const subband_t &get(const sz_t i = 0) const {
		return *(_get(i));
	}

	//! \brief Возвращает нужный саббенд с нужного уровня
	subband_t &get(const sz_t lvl, const sz_t i) {
		return *(_get(lvl, i));
	}

	//! \brief Возвращает нужный саббенд с нужного уровня
	const subband_t &get(const sz_t lvl, const sz_t i) const {
		return *(_get(lvl, i));
	}

	//! \brief Возвращает LL саббенд
	subband_t &get_LL() { return get(); }

	//! \brief Возвращает LL саббенд
	const subband_t &get_LL() const { return get(); }

	//! \brief Количество саббендов без LL саббенда
	sz_t mcount() const { return _mcount; }

	//! \brief Общее количество саббендов
	sz_t count() const { return _count; }

	//!	\brief Возвращает количество саббендов на уровне
	sz_t subbands_on_lvl(const sz_t lvl) const;

	//@}

	//!	\name Вспомогательные функции
	//@{

	//!	\brief Проверяет вхождение элемента в саббенд
	/*!	\param[in] p Координаты элемента
		\param[in] sb Саббенд
		\return <i>true</i> если элемент с координатами <i>%p</i> входит
		в саббенд <i>%sb</i>, иначе <i>false</i>

		\todo Необходимо протестировать эту функцию
	*/
	inline bool test(const p_t &p, const subband_t &sb) {
		return (sb.x_min <= p.x && p.x <= sb.x_max &&
				sb.y_min <= p.y && p.y <= sb.y_max);
	}

	//!	\brief Проверяет вхождение элемента в <i>LL</i> саббенд
	/*!	\param[in] p Координаты элемента
		\return <i>true</i> если элемент с координатами <i>%p</i> входит
		в <i>LL</i> саббенд, иначе <i>false</i>

		Функция аналогична subbands::test(), но требует выполнения в два раза
		меньшего числа сравнений (в отладочной версии также проверяется выход
		координат за пределы спектра, что делает количество сравнений таким же
		как и в функции subbands::test()).
	*/
	inline bool test_LL(const p_t &p) {
		const subband_t &sb = get_LL();

		return (p.x <= sb.x_max && p.y <= sb.y_max);
	}

	//@}

protected:
	// protected functions -----------------------------------------------------

	//! \brief Возвращает указатель на саббенд по его индексу (номеру)
	subband_t *_get(const sz_t i = 0) const;

	//! \brief Возвращает указатель на нужный саббенд с нужного уровня
	subband_t *_get(const sz_t lvl, const sz_t i) const;

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

	//! \brief Количество саббендов без LL саббенда
	sz_t _mcount;

	//! \brief Общее количество саббендов
	sz_t _count;

	//!	\brief Ширина <i>LL</i> саббенда (в элементах). Равна количеству
	//!	вершин деревьев в любой горизонтальной линии элементов из <i>LL</i>
	//!	саббенда.
	sz_t _LL_width;

	//!	\brief Высота <i>LL</i> саббенда (в элементах). Равна количеству
	//!	вершин деревьев в любой вертикальной линии элементов из <i>LL</i>
	//!	саббенда.
	sz_t _LL_height;
};



}	// wtc namespace



#endif	// WIC_LIBWIC_SUBBANDS_INCLUDED
