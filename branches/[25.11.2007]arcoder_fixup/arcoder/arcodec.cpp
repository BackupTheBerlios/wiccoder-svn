#include "ArCodec.h"
#include "ArBitStream.h"	// for explicit instantiation of ArCoder and ArDecoder

#include <stdio.h>

/////////////////////////////////////////////////////////////////////
// Model

Model::Model(int _no_of_chars) : 
	NO_OF_CHARS(_no_of_chars),
	EOF_SYMBOL(NO_OF_CHARS),
	ESCAPE_SYMBOL(NO_OF_CHARS),
	NO_OF_SYMBOLS(_no_of_chars+1),
	context(_no_of_chars+2),
	freq(_no_of_chars+2),
	char_to_index(_no_of_chars+1),
	index_to_char(_no_of_chars+2),
	nsymbols(1),
	bit_counter(0)
{
	if ( _no_of_chars ) {
		_init();
	}
}

void Model::_init(void) {
	for (int i=0; i < NO_OF_SYMBOLS; i++) {
		context[i] = freq[i] = char_to_index[i]=index_to_char[i] = 0;
	}
	context[NO_OF_SYMBOLS] = freq[NO_OF_SYMBOLS] = 0;
	char_to_index[ESCAPE_SYMBOL] = 1;
	index_to_char[1] = ESCAPE_SYMBOL;
	freq[1] = 1;
	// ��������� ����� ��������� ����������� ��� ESCAPE_SYMBOL
	context[0] = 1;	// �. �. ��. ��������
}

void Model::init(int _no_of_chars) {
	assert(_no_of_chars);
	NO_OF_CHARS       = _no_of_chars;
	NO_OF_SYMBOLS     = _no_of_chars+1;
	context.init(NO_OF_SYMBOLS+1);
	freq.init(NO_OF_SYMBOLS+1);
	char_to_index.init(NO_OF_SYMBOLS);
	index_to_char.init(NO_OF_SYMBOLS+1);
	nsymbols = 1;
	bit_counter = 0;
	_init();
}

/////////////////////////////////////////////////////////////////////
// EscapeContext

EscapeContext::EscapeContext(int _no_of_symbols) :
	NO_OF_SYMBOLS(_no_of_symbols),
	context(_no_of_symbols+1)
{
	if ( _no_of_symbols ) {
		_init();
	}
}

// ��������� ������� ��������������� ���������
void EscapeContext::_init(void)
{
	// Escape-context contains equiprobable symbols
	for (int i=0; i < NO_OF_SYMBOLS; i++) {
		context[i] = NO_OF_SYMBOLS-i;
	}
	context[NO_OF_SYMBOLS] = 0;
}

// ��������� ��� ������������� ������, �������������
void EscapeContext::init(int _no_of_symbols)
{
	assert(_no_of_symbols);
	NO_OF_SYMBOLS     = _no_of_symbols;
	context.init(NO_OF_SYMBOLS+1);
	_init();
}

/////////////////////////////////////////////////////////////////////
// Log2Table

Log2Table::Log2Table()
{
	// ������������� ������� �������� ���������� ���������� ��� ����������� �� ������ �����
	double ln2 = log(2.0);
	int nResolution = 1 << LOG_RESOLUTION;
	for(int i=1; i<nResolution; i++) {
		m_Log2[i] = (float)( -1*log( (double)i/nResolution ) / ln2 );
	}
	m_Log2[nResolution] = 0;
	m_Log2[0] = SHIFT;
}

/////////////////////////////////////////////////////////////////////
// RateLog2Table

RateLog2Table::RateLog2Table()
{
	// ������������� ������� �������� ���������� ���������� ��� ����������� �� ������ �����
	double ln2 = log(2.0);
	int nResolution = 1 << LOG_RESOLUTION;
	for(int i=1; i<nResolution; i++) {
		m_Log2[i] = int( -(1<<RATE_RESOLUTION)*log( (double)i/nResolution ) / ln2 + 0.5 );
	}
	m_Log2[nResolution] = 0;
	m_Log2[0] = SHIFT;
}

/////////////////////////////////////////////////////////////////////
// arcoder_base

#ifdef FAST_ENTROPY_EVAL
Log2Table arcoder_base::Log2;			// ������� �������� ����������
#endif

RateLog2Table arcoder_base::RateLog2;	// ������� ������������� �������� ����������

arcoder_base::arcoder_base( int model_amount, const int _no_of_chars[] ) :
	models(model_amount), escapes(model_amount),
	low(0), high(TOP_VALUE)
{
	int i;

	for ( i = 0; i < model_amount; i++ ) {
		models[i].init(_no_of_chars[i]);
	}
	curmodel = &models[0];

	for ( i = 0; i < model_amount; i++ ) {
		escapes[i].init(_no_of_chars[i]+1);		// + end-of-file symbol
	}
	cur_escape = &escapes[0];
}

arcoder_base::arcoder_base(char* modelfile) :
	models(1), escapes(1), low(0), high(TOP_VALUE)
{
	models[0].init(1);
	curmodel = &models[0];
	escapes[0].init(1+1);		// + end-of-file symbol
	cur_escape = &escapes[0];
	if ( !load(modelfile) )
		throw MException(EX_FILE_ERROR,"Fail to open configuration file.");
}

// ��������� ������������ ������ � ���������� �� ���������� �����
bool arcoder_base::load(const char *const modelfile)
{
	std::ifstream imod(modelfile, std::ios::in | std::ios::binary);
	if ( imod.fail() )
		return false;
	load(imod);	// ��������� ������ �� �����
	return true;
}

bool arcoder_base::save(const char *const modelfile) const
{
	std::ofstream imod(modelfile, std::ios::out | std::ios::binary);
	if ( imod.fail() )
		return false;
	save(imod);
	imod.close();
	return true;
}

// ��������� ������������ ������ � ���������� �� ������
void arcoder_base::load(std::istream& ins)
{
	// ������� �������� �� ������ � ���������� ������, � ����� �����������
	// � ������. ������ �� ������� ��������� �������������.

	int nModels = ins.get();	// ����� �������
	if ( ins.eof() ) throw MException(0,"Invalid configuration file.");
	if ( nModels != models.Size() )
	{
		models.init(nModels);
		escapes.init(nModels);
		curmodel = &models[0];
		cur_escape = &escapes[0];
	}
	for ( int i = 0; i < models.Size(); i++ )
	{
		int nMaxChars;	// ������������ ����� �������� � ������
		int nSymbols;	// �������� ����� �������� � ������
		int t;
		nMaxChars = ( t=ins.get() ) + 256*ins.get(); // ����� ������� ��������
		if ( ins.eof() || nMaxChars<=0) throw MException(0,"Invalid configuration file.");
		if ( nMaxChars != models[i].NO_OF_CHARS )
		{
			models[i].init(nMaxChars);
			escapes[i].init(nMaxChars+1);		// + end-of-file symbol
		}
		nSymbols = ( t=ins.get() ) + 256*ins.get(); // ����� ������� ��������
		if ( ins.eof() || nSymbols<=0) throw MException(0,"Invalid configuration file.");
		models[i].nsymbols = nSymbols;
		int* p;
		DWORD* pc;
		// ������ char_to_index �� ������
		p = new int[nMaxChars+1];
		if ( p == 0 ) throw MException(EX_MEMORY);
		ins.read( (char*)p, (nMaxChars+1)*sizeof(int) );
		p = models[i].char_to_index.set(p);
		if (p) delete [] p;	// ������� ������ ������
		// ������ index_to_char �� ������
		p = new int[nMaxChars+2];
		if ( p == 0 ) throw MException(EX_MEMORY);
		ins.read( (char*)p, (nMaxChars+2)*sizeof(int) );
		p = models[i].index_to_char.set(p);
		if (p) delete [] p;	// ������� ������ ������
		// ������ freq �� ������
		p = new int[nMaxChars+2];
		if ( p == 0 ) throw MException(EX_MEMORY);
		ins.read( (char*)p, (nMaxChars+2)*sizeof(int) );
		p = models[i].freq.set(p);
		if (p) delete [] p;	// ������� ������ ������
		// ������ context �� ������
		pc = new DWORD[nMaxChars+2];
		if ( pc == 0 ) throw MException(EX_MEMORY);
		ins.read( (char*)pc, (nMaxChars+2)*sizeof(DWORD) );
		if ( ins.gcount()!=(nMaxChars+2)*sizeof(DWORD) )
			throw MException(0,"Invalid configuration file.");
		pc = models[i].context.set(pc);
		if (pc) delete [] pc;// ������� ������ ������
	}
	if ( ins.peek()!=EOF )
		throw MException(0,"Invalid configuration file.");
}


/*!	\param[in,out] outs Output stream
	\param[in] count Number of models to save
	\param[in] offsets Array of size <i>count</i> with offsets of
	zero-symbol in models.
*/
void arcoder_base::load_dummy(std::istream& ins,
							  const int count,
							  const int *const offsets)
{
	// ����� ���������� �������
	const int N = ins.get();

	// �������� ���������� �������
	if (count > models.Size() || count > N)
		throw MException(0, "count argument too large");
	if (count > N)
		throw MException(0, "count argument too large");

	// �������� ��������
	int *const saved_offsets = new int[N];

	ins.read((char *)saved_offsets,	N * sizeof(saved_offsets[0]));

	// ��������� ������ ������ �� �����������
	for (int i = 0; count > i; ++i)
	{
		// ���������� �������� � ����������� ������
		int M = 0;

		ins.read((char *)(&M), sizeof(M));

		// ���������� �������������� �������� � ������
		int U = 0;

		ins.read((char *)(&U), sizeof(U));

		// ���������� �������� � ������� ������
		int L = models[i].NO_OF_CHARS;

		// ������� ��������� �������� � ����������� ������
		int *const saved_freq = new int[M + 1];
		ins.read((char *)saved_freq,
				 (M + 1)*sizeof(saved_freq[0]));

		// ������������� ��������
		M += 1;
		L += 1;

		// ������������� ����������� ������
		if (M >= U && L >= U  && 0 < U) --(saved_freq[U - 1]);

		// ����� ������ � �������������� ������
		model(i);

		const int d = saved_offsets[i] - offsets[i];

		// ��������� ������� �������
		for (int k1 = 0; M > k1 && L > k1; ++k1)
		{
			const int k2 = k1 + d;

			for(int j = 0; saved_freq[k1] > j; ++j)
				update_model(k2);
		}

		delete[] saved_freq;
	}

	// �������� ������ ���������� ������������ ����������
	delete[] saved_offsets;
}


/*!	\param[in] modelfile ���� � ����� � ����������� ��������
	\param[in] count ���������� ����������� �������
	\param[in] offsets ������ �������� ������� ���������
*/
bool arcoder_base::load_dummy(const char *const modelfile,
							  const int count,
							  const int *const offsets)
{
	std::ifstream ins(modelfile, std::ios::in | std::ios::binary);

	if (ins.fail()) return false;

	load_dummy(ins, count, offsets);

	return true;
}


/*!	\param[in,out] outs Output stream
	\param[in] count Number of models to save
	\param[in] offsets Array of size <i>count</i> with offsets of
	zero-symbol in models.
*/
void arcoder_base::save_dummy(std::ostream& outs,
							  const int count,
							  const int *const offsets)
{
	// �������� ���������� �� ���� ������������� ����������
	// ����������� �������
	const int N = (count > models.Size())? models.Size(): count;

	// ����� ����������� �������
	outs.put(N);

	// ���������� ��������
	outs.write((char *)offsets,	N * sizeof(offsets[0]));

	// ���������� ������ ������ �� �����������
	for (int i = 0; N > i; ++i)
	{
		// ���������� �������� � ������
		const int M = models[i].NO_OF_CHARS;

		outs.write((char *)(&M), sizeof(M));
		const int kkk = models[i].nsymbols;

		// ���������� �������������� �������� � ������
		const int U = models[i].nsymbols;
		outs.write((char *)(&U), sizeof(U));

		// ������� ��������� ��������
		const int *const freq = models[i].freq();
		outs.write((char *)freq, (M + 1)*sizeof(freq[0]));
	}
}


/*!	\param[in] modelfile ���� � ����� ��� ���������� �������
	\param[in] count ���������� ����������� �������
	\param[in] offsets ������ �������� ������� ���������
*/
bool arcoder_base::save_dummy(const char *const modelfile, const int count,
							  const int *const offsets)
{
	std::ofstream outs(modelfile, std::ios::out | std::ios::binary);

	if (outs.fail()) return false;

	save_dummy(outs, count, offsets);

	outs.close();

	return true;
}


// ��������� ������������ ������ � ���������� � �����
void arcoder_base::save(std::ostream& outs) const
{
	outs.put( models.Size() );	// ����� �������
	for ( int i = 0; i < models.Size(); i++ )
	{
		outs.write( (char*)( &models[i].NO_OF_CHARS ),	2 );
		outs.write( (char*)( &models[i].nsymbols ),		2 );
		outs.write( (char*)( models[i].char_to_index() ),	sizeof(int)*(models[i].NO_OF_CHARS+1) );
		outs.write( (char*)( models[i].index_to_char() ),	sizeof(int)*(models[i].NO_OF_CHARS+2) );
		outs.write( (char*)( models[i].freq() ),			sizeof(int)*(models[i].NO_OF_CHARS+2) );
		outs.write( (char*)( models[i].context() ),			sizeof(DWORD)*(models[i].NO_OF_CHARS+2) );
	}
}

float arcoder_base::entropy_eval(long ch) const
{
	// �������� ch ������ ���� ������, ��� ����� �������� ������� ������.
	// ��������, ���������� ���������� ������ �h=10 � ������,
	// ������������ �� 4 ������� (������ ch=0,1,2,3).
	// ����� ���-�� ����� ���� ������ ������.
	assert( 0<=ch && ch<model()->NO_OF_SYMBOLS );	// ���������� ��� ������� ��� ������� ������

    long symbol = char_to_index()[ch];       // ������ � ������� �� �������
	int Norm = context()[0];
#ifdef FAST_ENTROPY_EVAL
	if (symbol==0) // ������ ������� � ������� �������� ������� ���?
	{
		// -log2(P) - log2(P in escape)
		int n_in_escape = escape()->NO_OF_SYMBOLS;	// ����� �������� � ������� escape-���������
		return Log2[ ((freq()[char_to_index()[ESCAPE_SYMBOL()]]<<LOG_RESOLUTION)+(Norm>>1)) / Norm ]
				+ Log2[ ( (1<<LOG_RESOLUTION) + (n_in_escape>>1) ) / n_in_escape ];
	}
	else
		// -log2(P)
		return Log2[ ((freq()[symbol]<<LOG_RESOLUTION)+(Norm>>1)) / Norm ];
#else
	if (0 == symbol) // ������ ������� � ������� �������� ������� ���?
	{
		// -log2(P) - log2(P in escape)
		return (float)(( log( Norm / (double)(freq()[char_to_index()[ESCAPE_SYMBOL()]]) ) / LOG2 )
				+ log( (double)(escape()->NO_OF_SYMBOLS) ) / LOG2);
	}
	else
	{
		// -log2(P)
		return (float)( log( Norm / (double)(freq()[symbol]) ) / LOG2 );
	}
#endif //FAST_ENTROPY_EVAL
}

int arcoder_base::rate_eval(int ch) const
{
	assert( 0<=ch && ch<model()->NO_OF_SYMBOLS );	// ���������� ��� ������� ��� ������� ������
	int Norm = context()[0];
    long symbol = char_to_index()[ch];       // ������ � ������� �� �������
	if (symbol==0) // ������ ������� � ������� �������� ������� ���?
	{
		// -log2(P) - log2(P in escape)
		int n_in_escape = escape()->NO_OF_SYMBOLS;	// ����� �������� � ������� escape-���������
		return RateLog2[ ((freq()[char_to_index()[ESCAPE_SYMBOL()]]<<LOG_RESOLUTION)+(Norm>>1)) / Norm ]
				+ RateLog2[ ( (1<<LOG_RESOLUTION) + (n_in_escape>>1) ) / n_in_escape ];
	}
	else
		// -log2(P)
		return RateLog2[ ((freq()[symbol]<<LOG_RESOLUTION)+(Norm>>1)) / Norm ];
}

void arcoder_base::update_model(int ch)
{
	assert( 0<=ch && ch<model()->NO_OF_SYMBOLS );	// ���������� ��� ������� ��� ������� ������

   // ����������, � ��� ������������� ��������������� ������
	int i;
	if (context()[0]==MAX_FREQUENCY) {
		//	������������ ��������� ���������, �������� �� � 2 ����
		int cum=0; 
		for ( i=nsymbols(); i>0; i--) {
			if ( (freq()[i]>>=SCALE_SHIFT)==0 ) {
				char_to_index()[index_to_char()[i]]=0;
				nsymbols()--;
			}
			context()[i]=cum;
			cum+=freq()[i];
		}
		context()[0]=cum;
		// �������������� �������������� ESCAPE_SYMBOL
		nsymbols()++;
		char_to_index()[ESCAPE_SYMBOL()]=nsymbols();
		index_to_char()[nsymbols()]=ESCAPE_SYMBOL();
		freq()[nsymbols()]=1;
		i=nsymbols();
		while (i>0) context()[--i]++; // ���������� ����������
	}
	// ���������� (���������� ���������� ������)
	assert( 0<=ch && ch<model()->NO_OF_SYMBOLS );	// ���������� ��� ������� ��� ������� ������
	int symbol=char_to_index()[ch];	// ������ ������� � �������
	if (symbol==0)	{ // ������ ������� � ������� ���?
		// ��������� ����� ������ � ������� ������� context
		nsymbols()++;
		char_to_index()[ch]=nsymbols();
		index_to_char()[nsymbols()]=ch;
		i=nsymbols();
	}
	else	// ������ ��� ����
	{	// ������, ����� ��������� � context ���� ����������� �� ��������
		// (��� ��������� �������� ����������� � �������������)
		for (i=symbol; freq()[i] == freq()[i-1]; i--);
		if (i<symbol) {
			int ch_i=index_to_char()[i];
			int ch_symbol=index_to_char()[symbol];
			index_to_char()[i]=ch_symbol;
			index_to_char()[symbol]=ch_i;
			char_to_index()[ch_i]=symbol;
			char_to_index()[ch_symbol]=i;
		}
	}
	// ��������������� ���������� ����������� �� ������ ������� ch
	freq()[i]++;						// ���������� �������
	while (i>0) context()[--i]++;	// ���������� ��������� ��� ch
}

/////////////////////////////////////////////////////////////////////
// ArCoder<BITOutStreamClass>

template <class BITOutStreamClass>
void ArCoder<BITOutStreamClass>::encode_symbol(int symbol, DWORD* contextPtr)
{// ����������� ������ ������� �� ��������� ������� ������������
// (symbol - ������ ������� � �������)
// �������� ������ ��������� ��������������� �����������
	int range = high-low+1;
	// symbol==1 - ����� ������ ������
	if (symbol != 1) {
		high=low+range*contextPtr[symbol-1]/contextPtr[0] -1;
	}
	low += range*contextPtr[symbol]/contextPtr[0];
	// ����� ��� ������������� - ����� ���� ��� ���� �� ������������
	for (;;) {
		// ���������: ������ low<high
		assert(low < high);
		if ( high < HALF ) { // ������� ���� low � high - ������� (���)
			output_bit_plus_follow(0);	//����� ������������ �������� ����
		}
		else if ( low >= HALF ) { // ������� ���� low � high - ���������
			output_bit_plus_follow(1);	//����� �������� ����
			low-=HALF;					//����� �������� ���� � 0
			high-=HALF;					//����� �������� ���� � 0
		}
		else if ( low >= FIRST_QTR && high < THIRD_QTR ) {		
			/* �������� ������������, �.�. 
				HALF<=high<THIRD_QTR, i.e. high=10...
				FIRST_QTR<=low<HALF,	i.e. low =01...
				����������� ������ �� ����������� ���	*/
			high-=FIRST_QTR;		// high	=01...
			low-=FIRST_QTR;			// low	=00...
			bits_to_follow++;		//����������� ����� (���) ������ ����
						// ������� ��� ����� ������ �����
		}
		else {
			return;	// ��������� ����� ��� ���� 
		}
		//	������� ��� � low � high �������, ��������� ����� ��� � ������� ������ 
		low <<= 1;		// ��������� 0
		(high <<= 1)++;	// ��������� 1
	}
}

// ����������� ������� ��� ��������� ������
template <class BITOutStreamClass>
void ArCoder<BITOutStreamClass>::put( int ch )
{
	// �������� ch ������ ���� ������, ��� ����� �������� ������� ������.
	// ��������, ���������� ���������� ������ �h=10 � ������,
	// ������������ �� 4 ������� (������ ch=0,1,2,3).
	// ����� ���-�� ����� ���� ������ ������.
	assert( ch < ESCAPE_SYMBOL() );
	assert( ch >= 0 );

	int symbol = char_to_index()[ch];	// ������ � ������� �� �������
	if (symbol==0)	{ // ������ ������� � ������� �������� ������� ���?
		// ������ ����������� � ������� ������� - ��������� escape_context
		encode_symbol(char_to_index()[ESCAPE_SYMBOL()], context());
		// � �������� ������� ���������� ������� �� �������
		encode_symbol(ch+1,escape_context());
	}
	else {
		encode_symbol(symbol,context());
	}
}

template <class BITOutStreamClass>
void ArCoder<BITOutStreamClass>::EndPacking()
{
	// ������ ��� ��� �������������� �����, ����� ����������.
	//	��� ����� ��� �������� ����������� �������������
	//	���������� �������.
	bits_to_follow++;
	if ( low < FIRST_QTR ) {
		output_bit_plus_follow(0);
	}
	else {
		output_bit_plus_follow(1);
	}
	os.EndPacking();
}

/////////////////////////////////////////////////////////////////////
// ArCoderWithBitCounter<BITOutStreamClass>

// �������� �������������� ��� �������� ����� ��� � ������ ������ (������ ������)
template <class BITOutStreamClass>
void ArCoderWithBitCounter<BITOutStreamClass>::encode_symbol(int symbol, DWORD* contextPtr)
{
	int range = high-low+1;
	if (symbol != 1) {
		high=low+range*contextPtr[symbol-1]/contextPtr[0] -1;
	}
	low += range*contextPtr[symbol]/contextPtr[0];
	for (;;) {
		assert(low < high);
		if ( high < HALF ) {
			output_bit_plus_follow(0);
			model()->bit_counter++;	// ����������
		}
		else if ( low >= HALF ) {
			output_bit_plus_follow(1);
			low-=HALF;
			high-=HALF;
			model()->bit_counter++;	// ����������
		}
		else if ( low >= FIRST_QTR && high < THIRD_QTR ) {		
			high-=FIRST_QTR;
			low-=FIRST_QTR;
			bits_to_follow++;
			model()->bit_counter++;	// ����������
		}
		else {
			return;
		}
		low <<= 1;
		(high <<= 1)++;
	}
}

// ��� � ������� ������, �� ���������� ���������������� �������� encode_symbol()
template <class BITOutStreamClass>
void ArCoderWithBitCounter<BITOutStreamClass>::put ( int ch )
{	
	assert( ch < ESCAPE_SYMBOL() );
	int symbol = char_to_index()[ch];
	if (symbol==0)	{
		encode_symbol(char_to_index()[ESCAPE_SYMBOL()], context());
		encode_symbol(ch+1,escape_context());
	}
	else {
		encode_symbol(symbol,context());
	}
}

/////////////////////////////////////////////////////////////////////
// ArDecoder<BITInStreamClass>

template <class BITInStreamClass>
int ArDecoder<BITInStreamClass>::decode_symbol(DWORD* contextPtr)
{
	// ������������� ������ ������� �� ��������� ������� ������������
	// (���������� ������ ������� � �������)
	DWORD range, cum;
	int symbol;
	range=high-low+1;
	assert( low<=(unsigned long)value && (unsigned long)value<=high );
	// ����� cum - ��� ����� value, ������������� �� ���������
	// low..high � �������� 0..context[0]
	cum=( (value-low+1)*contextPtr[0] -1 )/range;
	// ����� ���������, ���������������� ����� cum (������� � ������ ������� �������)
	for (symbol=1; contextPtr[symbol]>cum; symbol++);
	// �������� ������ ��������� ��������������� �����������
	// symbol==1 - ����� ������ ������
	if (symbol != 1) {
		high=low+range*contextPtr[symbol-1]/contextPtr[0] -1;
	}
	low =low+range*contextPtr[symbol]/contextPtr[0];
	// ���������� � ������������� ��������� ��������
	for (;;) {
		if (high<HALF);		 // ������� ���� low � high - �������
		else if (low>=HALF) { // ������� ���� low � high - ���������, ����������
			value-=HALF;
			low-=HALF;
			high-=HALF;
		}
		else if (low>=FIRST_QTR && high<THIRD_QTR) {
			// ��������� ��� ��, ��� ��� �����������
			value-=FIRST_QTR;	// ������� ���������� ��� �� value !
			low-=FIRST_QTR;
			high-=FIRST_QTR;
		}
		else return(symbol);	// ��������� ����� ��� ���� 
		low<<=1;							// ��������� ����� ��� 0
		(high<<=1)++;						// ��������� ����� ��� 1
		value=(value<<1)+is.input_bit();		// ��������� ����� ��� ����������
	}
}

/////////////////////////////////////////////////////////////////////
// ArDecoderWithBitCounter<BITInStreamClass>

// �������� �������������� ��� �������� ����� ��� � ������ ������ (������ ������)
template <class BITInStreamClass>
int ArDecoderWithBitCounter<BITInStreamClass>::decode_symbol(DWORD* contextPtr)
{
	DWORD range, cum;
	int symbol;
	range=high-low+1;
	assert( low<=(unsigned long)value && (unsigned long)value<=high );
	cum=( (value-low+1)*contextPtr[0] -1 )/range;
	for (symbol=1; contextPtr[symbol]>cum; symbol++);
	if (symbol != 1) {
		high=low+range*contextPtr[symbol-1]/contextPtr[0] -1;
	}
	low =low+range*contextPtr[symbol]/contextPtr[0];
	for (;;) {
		if (high<HALF)
			model()->bit_counter++;	// ����������
		else if (low>=HALF) {
			value-=HALF;
			low-=HALF;
			high-=HALF;
			model()->bit_counter++;	// ����������
		}
		else if (low>=FIRST_QTR && high<THIRD_QTR) {
			value-=FIRST_QTR;
			low-=FIRST_QTR;
			high-=FIRST_QTR;
			model()->bit_counter++;	// ����������
		}
		else return(symbol);
		low<<=1;
		(high<<=1)++;
		value=(value<<1)+is.input_bit();
	}
}

// Explicit Instantiation of templates
// Without it this cpp-file is like a header
template class ArCoder<BITOutMemStream>;
template class ArDecoder<BITInMemStream>;
template class ArCoder<BITOutFileStream>;
template class ArDecoder<BITInFileStream>;
template class ArCoderWithBitCounter<BITOutMemStream>;
template class ArDecoderWithBitCounter<BITInMemStream>;
template class ArCoderWithBitCounter<BITOutFileStream>;
template class ArCoder<BITOutStubStream>;
template class ArCoderWithBitCounter<BITOutStubStream>;
