// Классы битовых потоков, работающие по одному биту.
// Нужны для статистического кодека.

#ifndef _INC_ARBITSTREAM_
#define _INC_ARBITSTREAM_

#include "MEXCEPT.H"
#include <fstream>
#include <assert.h>

typedef char* LPSTR;

///////////////////////////////////////////////////////////////////////
// Bitstreams for arithmetic codec

class bit_io_base 
{
public:

	bit_io_base() : buffer(0), bits_to_go(0) {}

protected:

	int bits_to_go;
	int buffer;
};

// Abstract class for bit input
class BITInStream
{
public:
	virtual void StartUnpacking() = 0;
	virtual int input_bit() = 0;
	virtual void EndUnpacking() = 0;
};

// Abstract class for bit output
class BITOutStream
{
public:
	virtual void StartPacking() = 0;
	virtual void output_bit(int bit) = 0;
	virtual void EndPacking() = 0;
};

/////////////////////////////////////////////////////////////////////
// Bitstreams working with memory buffer

class bit_mem_io_base : public bit_io_base
{
public:

	bit_mem_io_base() : localbuffer(0) {}
	bit_mem_io_base(LPSTR _buffer, unsigned long _bufsize)
		: localbuffer(_buffer), bufsize(_bufsize) {}
	virtual void SetBuffer(LPSTR _buffer, unsigned long _bufsize) {
		localbuffer = _buffer; bufsize = _bufsize;
	}

protected:
	LPSTR localbuffer;
	unsigned long bufsize;
};

class BITInMemStream 
: public bit_mem_io_base
, public BITInStream
{
public:

	BITInMemStream() {}
	BITInMemStream(LPSTR _buffer, unsigned long _bufsize)
		: bit_mem_io_base(_buffer, _bufsize) {}

	void StartUnpacking() {
		assert(localbuffer);	// must be valid memory buffer
		unpacked_bytes = 0;
		buffer = 0;
		bits_to_go = 0;
	}

	int input_bit(void ) {
		// ввод 1 бита из сжатого файла
		if (bits_to_go--==0) {
			if ((unsigned long)unpacked_bytes >= bufsize)
				throw MException(EX_BUFFER_OVERFLOW);
			// заполняем буфер битового ввода
			buffer = localbuffer[unpacked_bytes++];
			bits_to_go = 7;	// снова полный буфер
		}
		int t=((buffer&0x80)!=0);
		buffer<<=1;
		return t;
	}

	void EndUnpacking() {}

	long UnpackedBytes() const {return unpacked_bytes;}

private:
	long unpacked_bytes;
};

class BITOutMemStream 
: public bit_mem_io_base
, public BITOutStream
{
public:

	BITOutMemStream() {
		bits_to_go = 8;
	}
	BITOutMemStream(LPSTR _buffer, unsigned long _bufsize)
		: bit_mem_io_base(_buffer, _bufsize) {
		bits_to_go = 8;
	}

	void StartPacking() {
		assert(localbuffer);	// must be valid memory buffer
		packed_bytes = 0;
		buffer = 0;
		bits_to_go = 8;
	}

	void output_bit(int bit) {
		// вывод одного бита в сжатый поток
		buffer=(buffer<<1) | bit;	// в битовый буфер (один байт)
		if (--bits_to_go==0) {		// битовый буфер заполнен, сброс буфера
			if ((unsigned long)packed_bytes >= bufsize)
				throw MException(EX_BUFFER_OVERFLOW);
			localbuffer[packed_bytes++] = (unsigned char)(buffer & 0xFF);
			bits_to_go=8;
		}
	}

	void EndPacking() {
		if (bits_to_go < 8)
		{
			if ((unsigned long)packed_bytes >= bufsize)
				throw MException(EX_BUFFER_OVERFLOW);
			localbuffer[packed_bytes++] = (unsigned char)(0xFF & (buffer << bits_to_go));
	        bits_to_go = 8; // Second function call will be safe.
		}
	}

	long PackedBytes() const {return packed_bytes;}

private:
	long packed_bytes;
};

/////////////////////////////////////////////////////////////////////
// Bitstreams working directly with file stream

// MAX_GARBAGE_BITS must be >= BITS_IN_REGISTER from ArCodec module,
//	т.е. размеру битового буфера кодового потока-value.
// Это нужно лишь для точной реакции на неверный входной файл при декодировании
#define MAX_GARBAGE_BITS	19

class bit_file_io_base 
: public bit_io_base
{
};

// Битовый ввод из файлового потока.
// Не отвечает за закрытие файла!
class BITInFileStream 
: public BITInStream
, public bit_file_io_base
{
public:

	BITInFileStream(std::istream &_is) : is(_is), garbage_bits(0) {
		if ( is.bad() ) {
			throw MException(0,"Bad input stream.");
		}
	}

	void StartUnpacking() {
		buffer = 0;
		bits_to_go = 0;
	}

	// ввод 1 бита из сжатого файла
	int input_bit(void) {
		if (bits_to_go--==0) {
			buffer=is.get();	// заполняем буфер битового ввода
			if (buffer==EOF) { // входной поток сжатых данных исчерпан !!!
				// Причина попытки дальнейшего чтения: следующим декодируемым символом
				// должен быть EOF_SYMBOL, но декодер об этом пока не знает и может
				// готовиться к дальнейшему декодированию, втягивая новые биты
				// (см. цикл for(;;) в процедуре decode_symbol).
				// Эти биты - "мусор", реально не несут никакой информации и их можно
				// выдать любыми.
				// Мусорные биты начинаем считать только после физического конца файла
				garbage_bits++;
				// мусорные биты из последнего байта файла, к сожалению, не учитываются
				if (garbage_bits>MAX_GARBAGE_BITS-2) {	
					// garbage_bits больше максимально возможного числа мусорных битов
					// (минус два дополнительных бита, которые выдает done_encoding() )
					throw MException(0,"Error in compressed file.");
				}
				bits_to_go=0;
				buffer = -1;
			}
			else {
				bits_to_go=7;	// снова полный буфер
			}
		}
		int t=((buffer&0x80)!=0);
		buffer<<=1;
		return t;
	}

	void EndUnpacking() {}

private:
	int garbage_bits;
	std::istream &is;
};

// Битовый вывод в файловый поток.
// Не отвечает за закрытие файла!
class BITOutFileStream 
: public BITOutStream
, public bit_file_io_base
{
public:

	BITOutFileStream(std::ostream &_os) : os(_os) {
		if ( os.bad() ) {
			throw MException(0,"Bad output stream.");
		}
		bits_to_go = 8;
	}

	void StartPacking() {
		buffer = 0;
		bits_to_go = 8;
	}

	// вывод одного бита в сжатый файл
	void output_bit(int bit) {
		buffer=(buffer<<1) | bit;	// в битовый буфер (один байт)
		if (--bits_to_go==0) {				// битовый буфер заполнен, сброс буфера
			os.write((const char*)&buffer,1);
			bits_to_go=8;
		}
	}

	void EndPacking() {
		if (bits_to_go < 8) {
			os.write((const char*)&(buffer<<=bits_to_go), 1);
            bits_to_go = 8; // Second function call will be safe.
		}
	}

private:
	std::ostream &os;
};

// Битовый вывод-заглушка.
// Просто считает полные байты.
class BITOutStubStream 
: public BITOutStream
, public bit_io_base
{
public:

    BITOutStubStream() {
		bits_to_go = 8;
	}

	unsigned long PackedBytes() const {
        return packed_bytes;
    }

	void StartPacking() {
		bits_to_go = 8;
        packed_bytes = 0;
	}

	// вывод одного бита в поток
	void output_bit(int bit) {
        if (--bits_to_go==0) { // битовый буфер заполнен, сброс буфера
			packed_bytes++;
			bits_to_go=8;
		}
	}

	void EndPacking() {
		if (bits_to_go < 8) {
            packed_bytes++;
            bits_to_go = 8; // Second function call will be safe.
		}
	}

private:

    unsigned long packed_bytes;
};

#endif /* _INC_ARBITSTREAM_ */