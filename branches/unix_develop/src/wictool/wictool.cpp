/*!	\file     wictool.cpp
	\version  0.0.1
	\author   mice, ICQ: 332-292-380, mailto:wonder.mice@gmail.com
	\brief    Главный файл утилиты wictool

	\todo     Более подробно описать файл wictool.cpp
*/


///////////////////////////////////////////////////////////////////////////////
// include

// standard C++ library headers
#include <assert.h>
#include <string>
#include <fstream>
#include <iostream>

// external library header
// none

// libwic headers
#include <wic/libwic/encoder.h>



///////////////////////////////////////////////////////////////////////////////
// types declaration

struct compressed_hdr_t {
	short version;
};



///////////////////////////////////////////////////////////////////////////////
// constants definition

//!	\brief Версия утилиты
static const short VERSION	= 1;



///////////////////////////////////////////////////////////////////////////////
// functions declaration

//!	\brief Выводит информацию о использовании
int usage(int argc, char **args);

//!	\brief Осуществляет декодирование файла
int decode(const std::string &compressed_path,
		   const std::string &result_path);



///////////////////////////////////////////////////////////////////////////////
// functions definitions

/*!	\param[in] argc Number of arguments
	\param[in] args Arguments list
	\return <i>-1</i>, предполагая, что указанные аргументы не корректны
*/
int usage(int argc, char **args)
{
	std::cout << "Usage:" << std::endl << std::endl;

	std::cout << "wictool -e|--encode source.bmp compressed.wic q lambda";
	std::cout << std::endl;
	std::cout << "wictool -e|--encode source.bmp compressed.wic";
	std::cout << std::endl;
	std::cout << "wictool -d|--decode compressed.wic result.bmp";
	std::cout << std::endl;

	return (-1);
}


/*!	\param[in] compressed_path Путь к закодированному файлу
	\param[in] result_path Путь к результирующему файлу
	\return 0 если файл декодирован успешно
*/
int decode(const std::string &compressed_path,
		   const std::string &result_path)
{
	// open compressed file
	std::ifstream compressed(compressed_path.c_str(),
							 std::ios::in|std::ios::binary);
	if (!compressed.good()) {
		std::cout << "Error: can't open compressed file for reading";
		std::cout << std::endl;
		return -1;
	}

	// ### rak: 0 - just to make gcc not produce error. TODO.
	return 0;
}



///////////////////////////////////////////////////////////////////////////////
// main() function definition
int main(int argc, char **args)
{
	return usage(argc, args);

	wic::encoder e(10, 10, 10);

	return 0;
}
