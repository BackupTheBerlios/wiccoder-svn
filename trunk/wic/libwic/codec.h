/*!	\file     codec.h
	\version  0.0.1
	\author   mice, ICQ: 332-292-380, mailto:wonder.mice@gmail.com
	\brief    Главный заголовочный файл

	Данный заголовочный файл включает почти всё необходимое для
	использования библиотеки wiccoder.

	\todo     Более подробно описать файл coder.h
*/

#pragma once

#ifndef WIC_LIBWIC_INCLUDED
#define WIC_LIBWIC_INCLUDED

////////////////////////////////////////////////////////////////////////////////
// include

// libwic headers
#include <wic/libwic/types.h>


////////////////////////////////////////////////////////////////////////////////
// wic namespace
//!	\brief Namespace for wiccoder library
/*!	\todo Add detailed description for <i>%wic</i> namespace.
*/
namespace wic {


////////////////////////////////////////////////////////////////////////////////
// doxygen documentation
/*!	\mainpage

	\section mainpage_sectAbout О проекте

	<a href="http://developer.berlios.de/projects/wiccoder/">
	<b>wiccoder</b></a> - кросс-платформенный кодек с открытым исходным
	кодом для сжатия изображений с потерями. Разрабатывается в
	<a href="http://www.miee.ru">Московском Институте Электронной Техники
	(МИЭТ)</a>.
	
	<a href="http://developer.berlios.de/projects/wiccoder/">
	<b>wiccoder</b></a> представляет собой библиотеку и набор инструментов для
	отладки программного кода и исследования методов сжатия изображений.
	Основная часть библиотеки написана на языке <i>C++</i> с высокой
	степенью абстракции, что делает её очень гибкой и пригодной для
	исследования алгоритмов, отличных от реализованного. Библиотека
	прекрасно документированна, что делает её изучение предельно простым.

	\section mainpage_sectDocIndex Страницы документации

	Здесь приведено содержание страниц документации не относящихся на
	прямую к какмоу либо элементу исходного кода. Они описывают общие
	идеи и принципы которые были заложены в <i>wiccoder</i>. Для более
	детальной информации по исходному коду обращайтесь к соответсвующему
	разделу данной документации.

	<b>Содержание:</b>
	- \ref pgTerminology
		- \ref pgTerminology_sectIntroduction
		- \ref pgTerminology_sectSubbands
		- \ref pgTerminology_sectTrees_leafs_and_branches
		- \ref pgTerminology_sectElements_and_Coefs
		- \ref pgTerminology_sectGroupedCutSigns
	- \ref pgCoding
		- \ref pgCoding_sectAbout
		- \ref pgCoding_sectIterators
	- \ref pgLicense
		- \ref pgLicense_sectAbout

	\section mainpage_sectContacts Контакты

	Вопросы, пожелания и предложения принимаются на электронный почтовый
	ящик <a href="mailto:wonder.mice@gmail.com">wonder.mice@gmail.com</i>.
*/

/*!	\page pgTerminology Терминология

	\section pgTerminology_sectIntroduction Вступление

	В данной реализации алгоритма иногда применяется специфическая
	терминология. Некоторые термины и определения характерные для методов
	сжатия информации и цифровой обработки сигналов иногда используются не по
	назначению или в переносном смысле. Так же, иногда можно заметить,
	использование одиннаковых или похожих терминов в разном контексте в
	разных значениях. В этой секции будет описано большинство таких случаев,
	поэтому, при возникновении трудностей с пониманием той или иной части
	описания исходного кода или документации, следует первым делом заглянуть
	в этот раздел. Хорошей идеей будет также дополнение и исправление
	существующей документации, где был найдены ошибки или где описание были
	не достаточно полными и ясными.

	\section pgTerminology_sectSubbands Саббенды

	Термин <i>саббенд</i> (<i>subband</i>) везде используется в своём
	оригинальном значении и представляет собой прямоугольный диапазон
	спектра двухмерного вейвлет преобразования. На каждом уровне разложения
	содержится по три саббенда одинакового размера. Ещё один саббенд
	(<i>LL</i>, от <i>LowLow</i>, самый низкочастотный) всегда располагается
	в верхнем левом углу спектра. Таким образом, количество саббендов
	зависит от числа уровней разложения как <i>Sb<sub>count</sub> = 
	3*Levels<sub>count</sub> + 1</i>. Смотри <a href="../35.pdf">35.pdf</a>
	для более подробной информации о саббендах.

	\section pgTerminology_sectTrees_leafs_and_branches Деревья, листья и ветви

	Термин <i>дерево</i> (<i>tree</i>) часто используется в коде не по
	назначению. Деревом может называться как и весь вейвлет спектр (результат
	двумерного вейвлет преобразования), так и отдельно взятое дерево (в
	обычном понимании этого определения, корнем которого является элемент,
	лежащий в самом низкочастотном (<i>LL</i>) диапазоне спектра
	(<i>саббэнде</i>). Второе определение является более верным и приоритетным.
	В дальнейшем следует пользоваться термином <i>дерево</i> только для группы
	(<i>грозди</i>) коэффициентов и их потомков, с общим родителем, находящимся
	в <i>LL</i> саббенде.

	<i>Корневым элементом дерева</i> (<i>root</i>) будем называть любой элемент
	из <i>LL</i> саббенда.

	Термин <i>листья</i> (<i>leafs</i>) будет применяться для обозначения
	группы коэффициентов принадлежащих одному дереву и лежащих в пределе одного
	саббенда или нескольких саббендах одного уровня разложения. Это не совсем
	традиционно, но удобно в контексте алгоритма.

	Под <i>ветвью</i> (<i>branch</i>) будем понимать набор коэффициентов и их
	потомков с общим родителем (не обязательно из <i>LL</i> саббенда).

	\section pgTerminology_sectElements_and_Coefs Элементы и коэффициенты

	Следует понимать, что в данной реализации алгоритма, термины
	<i>элемент</i> (<i>element</i>) и <i>коэффициент</i> (<i>coefficient</i>,
	<i>factor</i>) различны. <i>Коэффициент</i> - это отдельное значение
	вейвлет коэффициента. Он может быть проквантованным, откорректированным
	или любым другим, но главное, что это всегда единтсвенное числовое
	значение. <i>Элемент</i> представляет собой группу значений (это структура
	в терминах языка <i>C++</i>), объединённых общим положением в вейвлет
	спектре. Так проквантованное, откорректированное и оригинальное значения
	одного коэффициента объединяются в <i>элемент</i>.

	\section pgTerminology_sectGroupedCutSigns Групповые признаки подрезания

	В описаниях и в коде групповые признаки подрезания обозначаются буквой
	<i>N</i> (или <i>n</i>). <i>N<sub>i</sub></i> представляет собой
	групповой признак подрезания ветвей, относящийся к элементу с индексом
	<i>i</i>, в то время как <i>n<sub>i</sub></i> относится к одиночному
	признаку подрезания <i>i-го</i> элемента из группы. Используемая модель
	признаков подрезания довольно сложна для понимания (хоть и проста в
	реализации), поэтому для более глубокого понимания рекомендуется
	ознакомится с <a href="../35.pdf">35.pdf</a>.
*/

/*! \page pgCoding Реализация

	\section pgCoding_sectAbout О реализации

	Библиотека <i>wiccoder</i> написана на <i>C++</i> с высокой степенью
	абстракции, что позволяет с лёгкостью использовать её составляющие.
*/

/*!	\page pgLicense Лицензионное соглашение

	\section pgLicense_sectAbout О соглашении

	Библиотека <i>wiccoder</i> выпускается под 
	лицензионным соглашением <i>GNU Lesser General Public License (LGPL)</i>.

	Ниже приведён текст, который следует мысленно прикреплять к каждому
	исходному файлу <i>wiccoder</i>:

	\verbatim
	MIEE (C) 2007, MIEE DSP Team

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
	USA
	\endverbatim
*/


////////////////////////////////////////////////////////////////////////////////
// types

//! \brief Входные данные для енкодера
struct encode_in_t {
	//! \brief Коэффициенты вейвлет преобразования изображения
	w_t *image;
	//! \brief Количество уровней вейвлет преобразования
	sz_t lvls;
	//! \brief Ширина изображения
	sz_t width;
	//! \brief Высота изображения
	sz_t height;
};

//! \brief Выходные данные енкодера
struct encode_out_t {
	//! \brief Буфер для приёма сжатого изображения
	byte_t *data;
	//! \brief Размер буфера
	sz_t data_sz;
	//! \brief Размер записанных в буфер данных
	sz_t enc_sz;
};

//! \brief Входные данные для декодера
struct decode_in_t {
};

//! \brief Выходные данные декодера
struct decode_out_t {
};


////////////////////////////////////////////////////////////////////////////////
// encode and decode functions declaration

//! \brief Функция кодирования (сжатия) изображения
bool encode(const encode_in_t &in, encode_out_t &out);

//! \brief Функция декодирования (восстановления) изображения
bool decode(const decode_in_t &in, decode_out_t &out);



}	// end of namespace wic



#endif	// WIC_LIBWIC_INCLUDED
