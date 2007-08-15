#ifndef _INC_ARCODEC_
#define _INC_ARCODEC_

/*
 * ���������� ��������������� ����������� ������ ����������
 * 
 * ������ 1.5
 */
#include "mmemory.h"
#include <math.h>
#include <assert.h>
#include <istream>
#include <ostream>
//llama:
#include "arbitstream.h"

typedef unsigned long DWORD;

/* ��� ��������� ������������:	MAX_FREQUENCY * (TOP_VALUE+1) < ULONG_MAX
*  ����� MAX_FREQUENCY ������ ���� �� �����, ��� � 8 ���� ������ TOP_VALUE
*  ����� �������� NO_OF_CHARS ������ ���� ����� ������ MAX_FREQUENCY
*/
// MAX_FREQUENCY=2^SHIFT ���������� ����. ������ ����������� ������ ������������
#define SHIFT				13	// log2(NO_OF_CHARS)+1<=SHIFT<=15
// ������� ������� ������� ����������� � 2^SCALE_SHIFT ��� ����� ���������� �������
// 1(�����������. ���������� �������)<=SCALE_SHIFT<=SHIFT(������ ����������)
#define SCALE_SHIFT			1
#define BITS_IN_REGISTER	(32-SHIFT)	// ������ �������� ������ �������� ������-value
#define TOP_VALUE			(((unsigned long)1<<BITS_IN_REGISTER)-1)	// 1111...1
// ��������� ��� ������� �������� high � low
#define FIRST_QTR			((TOP_VALUE>>2) +1)							// 0100...0
#define HALF				(FIRST_QTR<<1)								// 1000...0
#define THIRD_QTR			(HALF+FIRST_QTR)							// 1100...0
// ������������ ������ ����������� ������ ������������
#define MAX_FREQUENCY		((unsigned)1<<SHIFT)	// 2^SHIFT

// ������ ��������� ���������� �������� ��� ������ ����� ����
// #define FAST_ENTROPY_EVAL

// #ifdef FAST_ENTROPY_EVAL
// �������� ������������� ��������� ��������� ��� ���������� ��������.
// ������� ���������� ��� �������� [0;1] �������� 2^LOG_RESOLUTION+1 ��������.
// ��� ����� ����� ��� ��� �������������� ������������� ����������� ��������.
// ������������� ����������� ���������� �� 0 �� 2^LOG_RESOLUTION,
//	����� ������� ���������� ���������� � ������� ������������� �����
//	��������� �������� ����������� �������.
#define LOG_RESOLUTION		13
//#else
// � entropy_eval(long ch) ���������� ���������� � ��������� ������, � �� ������� ����������
#define LOG2				0.69314718055994530941723212145818

// �������� �������������� ������������� �������� ��������.
// ������ ����� ���� �������������� � �������� (1 / 2^RATE_RESOLUTION) ����.
// ������������� ������ ����������� �������� rate_eval().
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
	MemPtr<DWORD> context;			// �������� �������
	MemPtr<int> freq;
	MemPtr<int>	char_to_index;	// ������� �������� � context
	MemPtr<int>	index_to_char;	// ������� �� ��������
	int nsymbols; // ���-�� �������� � �������
	void _init(void);

public:

	unsigned long bit_counter; // ���������� ��� �� ������ ������
	Model(int _no_of_chars = 0);
	void init(int _no_of_chars);
	void ResetStatistics(){
		nsymbols = 1;
		bit_counter = 0;	// - �� ����
		_init();
	}
#ifndef NDEBUG
	int getEOF_SYMBOL( void ) const {
		return EOF_SYMBOL;
	}
#endif
};

// ������������ ������� � ��������������� ���������
class EscapeContext
{
	friend class arcoder_base;
	int NO_OF_SYMBOLS;
	MemPtr<DWORD> context;			// �������� �������
	void _init(void);

public:

	EscapeContext(int _no_of_symbols = 0);
	void init(int _no_of_symbols);
};

/////////////////////////////////////////////////////////////////////
// Log2Table

// ������� ��� ���������� �������� ���������� ��� ����������� �� ������ �����
class Log2Table
{
	float m_Log2[(1<<LOG_RESOLUTION)+1];	// ������� �������� ����������

public:
	Log2Table();
	~Log2Table() {}
	float operator [] (int i) const {return m_Log2[i];}
};

/////////////////////////////////////////////////////////////////////
// RateLog2Table

// ������� ��� ���������� �������� ���������� ��� ����������� �� ������ �����
class RateLog2Table
{
	int m_Log2[(1<<LOG_RESOLUTION)+1];	// ������� �������� ����������
public:
	RateLog2Table();
	int operator [] (int i) const {assert(0<=i&&i<=(1<<LOG_RESOLUTION)); return m_Log2[i];}
};

/////////////////////////////////////////////////////////////////////
// arcoder_base

class arcoder_base
{
	MArray<Model> models;			// ����� ����������� ������
	Model* curmodel;
	MArray<EscapeContext> escapes;	// ����� ��������������� Escape-����������
	EscapeContext* cur_escape;

protected:

	DWORD low;
	DWORD high;
#ifdef FAST_ENTROPY_EVAL
	static Log2Table Log2;			// ������� �������� ����������
#endif
	static RateLog2Table RateLog2;	// ������� ������������� �������� ����������

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

	// ����������� ������� ��� ��������� ������
	void put( int ch );

	// ����������� ������� � ���������� ���������� ������
	void operator << ( int ch ) {
		put(ch);
		update_model(ch);
	}

	//	(!)��� ����������� ����������� �� ����� ������� ����� ������,
	//	������ ���� ��� �������� �� ����� ������ update_model(EOF_SYMBOL),
	//	�.�. ������� ������ (����� �� ��������� ������).
	void PutEOF() {
		// EOF_SYMBOL ���� ������ � escape_context => ��������� ����
		encode_symbol(char_to_index()[ESCAPE_SYMBOL()],context());	
		// � �������� ������� ���������� ������� �� �������
		encode_symbol(EOF_SYMBOL()+1,escape_context());
	}

	void EndPacking();

protected:
	void encode_symbol(int symbol, DWORD* contextPtr);

	// ����� ������ ���������� ���� � ���, ������� ���� ��������
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

// ������ �� �� ��� � ������, �� ������������ ����� ��� � ������ ������
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

	// ��� � ������� ������, �� ���������� ���������������� �������� encode_symbol()
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

	// ������������� ������� ��� ��������� ������
	int get () {
		int ch;
		ch = index_to_char()[decode_symbol(context())];	// ��� ������� �� �������
		if (ch==ESCAPE_SYMBOL()) {
			// ������ ����������� � ������� ������� - ��������� escape_context
			// ������������ ���������� �������� �� �������
			ch=decode_symbol(escape_context())-1;
		}
		return ch;
	}

	// ������������� ������� � ���������� ���������� ������
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

// ������ �� �� ��� � ������, �� ������������ ����� ��� � ������ ������
template <class BITInStreamClass>
class ArDecoderWithBitCounter : public ArDecoder<BITInStreamClass>
{
public:
	ArDecoderWithBitCounter(int model_amount, const int _no_of_chars[], BITInStreamClass& _is) :
	  ArDecoder<BITInStreamClass>(model_amount, _no_of_chars, _is) {}
	ArDecoderWithBitCounter( char* modelfile, BITInStreamClass& _is ) :
	  ArDecoder<BITInStreamClass>(modelfile, _is) {}

	// ��� � ������� ������, �� ���������� ���������������� �������� decode_symbol()
	int get () {
		int ch;
		ch = this->index_to_char()[decode_symbol(this->context())];	// ��� ������� �� �������
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

