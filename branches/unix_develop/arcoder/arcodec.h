#ifndef _INC_ARCODEC_
#define _INC_ARCODEC_

/*
 * Библиотека арифметического кодирования общего назначения
 * 
 * Версия 1.5
 */
#include "mmemory.h"
#include <math.h>
#include <assert.h>
#include <istream>
#include <ostream>
//llama:
#include "arbitstream.h"

typedef unsigned long DWORD;

/* для избежания переполнения:	MAX_FREQUENCY * (TOP_VALUE+1) < ULONG_MAX
*  число MAX_FREQUENCY должно быть не менее, чем в 8 раза меньше TOP_VALUE
*  число символов NO_OF_CHARS должно быть много меньше MAX_FREQUENCY
*/
// MAX_FREQUENCY=2^SHIFT определяет макс. размер пополняемых таблиц вероятностей
#define SHIFT				13	// log2(NO_OF_CHARS)+1<=SHIFT<=15
// Частоты каждого символа уменьшаются в 2^SCALE_SHIFT раз после наполнения таблицы
// 1(минимальное. обновление таблицы)<=SCALE_SHIFT<=SHIFT(полное обновление)
#define SCALE_SHIFT			1
#define BITS_IN_REGISTER	(32-SHIFT)	// размер битового буфера кодового потока-value
#define TOP_VALUE			(((unsigned long)1<<BITS_IN_REGISTER)-1)	// 1111...1
// Константы для анализа значений high и low
#define FIRST_QTR			((TOP_VALUE>>2) +1)							// 0100...0
#define HALF				(FIRST_QTR<<1)								// 1000...0
#define THIRD_QTR			(HALF+FIRST_QTR)							// 1100...0
// Максимальный размер пополняемых таблиц вероятностей
#define MAX_FREQUENCY		((unsigned)1<<SHIFT)	// 2^SHIFT

// Задает табличное вычисление энтропии для оценки длины кода
// #define FAST_ENTROPY_EVAL

// #ifdef FAST_ENTROPY_EVAL
// Точность представления двоичного логарифма для вычисления энтропии.
// Таблица логарифмов для отрезока [0;1] содержит 2^LOG_RESOLUTION+1 значений.
// Это также число бит для целочисленного представления вероятности символов.
// Целочисленная вероятность изменяется от 0 до 2^LOG_RESOLUTION,
//	таким образом количество логарифмов в таблице соответствует числу
//	возможных значений вероятности символа.
#define LOG_RESOLUTION		13
//#else
// В entropy_eval(long ch) используем вычисления с плавающей точкой, а не таблицу логарифмов
#define LOG2				0.69314718055994530941723212145818

// Точность целочисленного представления двоичной энтропии.
// Оценка длины кода представляется в единицах (1 / 2^RATE_RESOLUTION) бита.
// Целочисленная оценка вычисляется функцией rate_eval().
#define RATE_RESOLUTION		8

/////////////////////////////////////////////////////////////////////
// Model

class Model
{
	friend class arcoder_base;
	int NO_OF_CHARS;
	int &EOF_SYMBOL;
	int &ESCAPE_SYMBOL;
	int NO_OF_SYMBOLS;
	MemPtr<DWORD> context;			// основные таблицы
	MemPtr<int> freq;
	MemPtr<int>	char_to_index;	// индексы символов в context
	MemPtr<int>	index_to_char;	// символы по индексам
	int nsymbols; // Кол-во символов в таблице
	void _init(void);

public:

	unsigned long bit_counter; // Количество бит на выходе модели
	Model(int _no_of_chars = 0);
	void init(int _no_of_chars);
	void ResetStatistics(){
		nsymbols = 1;
		bit_counter = 0;	// - не надо
		_init();
	}
#ifndef NDEBUG
	int getEOF_SYMBOL( void ) const {
		return EOF_SYMBOL;
	}
#endif
};

// Неизменяемая таблица с равновероятными символами
class EscapeContext
{
	friend class arcoder_base;
	int NO_OF_SYMBOLS;
	MemPtr<DWORD> context;			// основная таблица
	void _init(void);

public:

	EscapeContext(int _no_of_symbols = 0);
	void init(int _no_of_symbols);
};

/////////////////////////////////////////////////////////////////////
// Log2Table

// Таблица для вычисления двоичных логарифмов для вероятности со знаком минус
class Log2Table
{
	float m_Log2[(1<<LOG_RESOLUTION)+1];	// таблица двоичных логарифмов

public:
	Log2Table();
	~Log2Table() {}
	float operator [] (int i) const {return m_Log2[i];}
};

/////////////////////////////////////////////////////////////////////
// RateLog2Table

// Таблица для вычисления двоичных логарифмов для вероятности со знаком минус
class RateLog2Table
{
	int m_Log2[(1<<LOG_RESOLUTION)+1];	// таблица двоичных логарифмов
public:
	RateLog2Table();
	int operator [] (int i) const {assert(0<=i&&i<=(1<<LOG_RESOLUTION)); return m_Log2[i];}
};

/////////////////////////////////////////////////////////////////////
// arcoder_base

class arcoder_base
{
	MArray<Model> models;			// Набор пополняемых таблиц
	Model* curmodel;
	MArray<EscapeContext> escapes;	// Набор соответствующих Escape-контекстов
	EscapeContext* cur_escape;

protected:

	DWORD low;
	DWORD high;
#ifdef FAST_ENTROPY_EVAL
	static Log2Table Log2;			// таблица двоичных логарифмов
#endif
	static RateLog2Table RateLog2;	// таблица целочисленных двоичных логарифмов

public:

   arcoder_base(int model_amount, const int _no_of_chars[]);
   arcoder_base(char* modelfile);
   virtual ~arcoder_base() {}

   void ResetStatistics(){
		for ( int i = 0; i < models.Size(); i++ ) {
			models[i].ResetStatistics();
		}
   }
   bool load(const char *const modelfile);
   void load(std::istream& ins);
   bool save(const char *const modelfile) const;
   void save(std::ostream& outs) const;
   float entropy_eval(long ch) const;
   float entropy_update(long ch) {
		float res = entropy_eval(ch);
		update_model(ch);
		return res;
	}
   int rate_eval(int ch) const;
   int* char_to_index(void) const {
		return model()->char_to_index();
	}
   DWORD* context( void ) const {
		return model()->context();
	}
   DWORD* escape_context( void ) const {
		return escape()->context();
	}
   int EOF_SYMBOL( void ) const {
		return model()->EOF_SYMBOL;
	}
   int ESCAPE_SYMBOL( void ) const {
		return model()->ESCAPE_SYMBOL;
	}
   int* freq( void ) const {
		return model()->freq();
	}
   int* index_to_char(void) const {
		return model()->index_to_char();
	}
	Model* model( void ) const {
		assert(curmodel);
		return curmodel;
	}
	EscapeContext* escape( void ) const {
		assert(cur_escape);
		return cur_escape;
	}
	void model( int modnum ) {
		curmodel = &models[modnum];
		assert(curmodel);
		cur_escape = &escapes[modnum];
		assert(cur_escape);
	}
	const MArray<Model>& Models() const {
		return models;
	}
   int& nsymbols( void ) const {
		return * (&model()->nsymbols);
	}
   void update_model(int ch);
};

/////////////////////////////////////////////////////////////////////
// ArCoder<BITOutStreamClass>

template <class BITOutStreamClass>
class ArCoder : public arcoder_base
{
public:

	ArCoder( int model_amount, const int _no_of_chars[], BITOutStreamClass& _os ) :
	  arcoder_base(model_amount,_no_of_chars), os(_os), bits_to_follow(0) {}
	ArCoder( char* modelfile, BITOutStreamClass& _os ) :
	  arcoder_base(modelfile), os(_os), bits_to_follow(0) {}
	virtual ~ArCoder() {}

	void StartPacking() {
		os.StartPacking();
		bits_to_follow = 0;
		low = 0;
		high = TOP_VALUE;
	}

	// Кодирование символа без изменения модели
	void put( int ch );

	// Кодирование символа с изменением адаптивной модели
	void operator << ( int ch ) {
		put(ch);
		update_model(ch);
	}

	//	(!)Для кодирования изображений не нужен признак конца потока,
	//	причем если его оставить то нужно делать update_model(EOF_SYMBOL),
	//	т.к. декодер делает (чтобы не разошлись модели).
	void PutEOF() {
		// EOF_SYMBOL есть только в escape_context => переходим туда
		encode_symbol(char_to_index()[ESCAPE_SYMBOL()],context());	
		// в запасной таблице индексация смещена на единицу
		encode_symbol(EOF_SYMBOL()+1,escape_context());
	}

	void EndPacking();

protected:
	void encode_symbol(int symbol, DWORD* contextPtr);

	// вывод одного очередного бита и тех, которые были отложены
	void output_bit_plus_follow(int bit) {
		os.output_bit(bit);
		while (bits_to_follow > 0) {
				 os.output_bit(!bit);
				 bits_to_follow--;
		}
	}

	BITOutStreamClass& os;
	int bits_to_follow;
};

/////////////////////////////////////////////////////////////////////
// ArCoderWithBitCounter<BITOutStreamClass>

// Делает то же что и предок, но подсчитывает число бит в каждом потоке
template <class BITOutStreamClass>
class ArCoderWithBitCounter : public ArCoder<BITOutStreamClass>
{
public:
	ArCoderWithBitCounter(	int model_amount, const int _no_of_chars[], BITOutStreamClass& _os ) : 
	  ArCoder<BITOutStreamClass>(model_amount, _no_of_chars, _os) {}
	ArCoderWithBitCounter( char* modelfile, BITOutStreamClass& _os ) :
	  ArCoder<BITOutStreamClass>(modelfile, _os) {}

	void put( int ch );

	void operator << ( int ch ) {
		put(ch);
		this->update_model(ch);
	}

	// Как в базовом классе, но вызывается переопределенная операция encode_symbol()
	void PutEOF() {
		encode_symbol(this->char_to_index()[this->ESCAPE_SYMBOL()], this->context());	
		encode_symbol(this->EOF_SYMBOL() + 1, this->escape_context());
	}

private:
	void encode_symbol(int symbol, DWORD* contextPtr);
};

/////////////////////////////////////////////////////////////////////
// ArDecoder<BITInStreamClass>

template <class BITInStreamClass>
class ArDecoder : public arcoder_base
{
public:

	ArDecoder( int model_amount, const int _no_of_chars[], BITInStreamClass& _is ) :
	  arcoder_base(model_amount,_no_of_chars), is(_is), value(0) {}
	ArDecoder( char* modelfile, BITInStreamClass& _is ) :
	  arcoder_base(modelfile), is(_is), value(0) {}
	virtual ~ArDecoder() {}

	void StartUnpacking() {
		is.StartUnpacking();
		low = 0;
		high = TOP_VALUE;
		value = 0;
		for (int i=0; i < BITS_IN_REGISTER; i++) {
			value=(value<<1)+is.input_bit();
		}
	}

	// Декодирование символа без изменения модели
	int get () {
		int ch;
		ch = index_to_char()[decode_symbol(context())];	// код символа по индексу
		if (ch==ESCAPE_SYMBOL()) {
			// символ отсутствует в текущей таблице - применяем escape_context
			// одновременно поправляем значение на единицу
			ch=decode_symbol(escape_context())-1;
		}
		return ch;
	}

	// Декодирование символа с изменением адаптивной модели
	void operator >> ( int &ch ) {
		ch = get();
		update_model(ch);
	}

	void EndUnpacking() {
		is.EndUnpacking();
	}

protected:
	int decode_symbol(DWORD* contextPtr);

	int value;
	BITInStreamClass& is;
};

/////////////////////////////////////////////////////////////////////
// ArDecoderWithBitCounter<BITInStreamClass>

// Делает то же что и предок, но подсчитывает число бит в каждом потоке
template <class BITInStreamClass>
class ArDecoderWithBitCounter : public ArDecoder<BITInStreamClass>
{
public:
	ArDecoderWithBitCounter(int model_amount, const int _no_of_chars[], BITInStreamClass& _is) :
	  ArDecoder<BITInStreamClass>(model_amount, _no_of_chars, _is) {}
	ArDecoderWithBitCounter( char* modelfile, BITInStreamClass& _is ) :
	  ArDecoder<BITInStreamClass>(modelfile, _is) {}

	// Как в базовом классе, но вызывается переопределенная операция decode_symbol()
	int get () {
		int ch;
		ch = this->index_to_char()[decode_symbol(this->context())];	// код символа по индексу
		if (ch == this->ESCAPE_SYMBOL()) {
			ch = decode_symbol(this->escape_context()) - 1;
		}
		return ch;
	}

	void operator >> ( int &ch ) {
		ch = get();
		this->update_model(ch);
	}

private:
	int decode_symbol(DWORD* contextPtr);
};

typedef ArCoderWithBitCounter<BITOutStubStream> ArEncoder;

#endif /* _INC_ARCODEC_ */

