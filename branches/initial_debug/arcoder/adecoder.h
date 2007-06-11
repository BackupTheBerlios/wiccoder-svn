/*******************************************************************************
* file:         adecoder.cpp                                                   *
* version:      0.0.1                                                          *
* author:       veld (mailto:yaxen@mail.ru, ICQ:250-8-250)					   *
* description:  not available                                                  *
* tests:        none                                                           *
*******************************************************************************/


#ifndef WTC_ADECODER_INCLUDED
#define WTC_ADECODER_INCLUDED

////////////////////////////////////////////////////////////////////////////////
// headers

// standard C++ library headers
// none

// original arcoder headers
#include "arcodec.h"
#include "arbitstream.h"

// wrappers headers
#include "acoder.h"


////////////////////////////////////////////////////////////////////////////////
// wtc namespace
namespace wtc {


////////////////////////////////////////////////////////////////////////////////
// adecoder class declaration
template <
	class v_t = long,	// алфавит арифметического кодера
	class b_t = char	// тип для буферов арифметического кодера
>
class adecoder: public acoder<v_t, b_t> {
public:
	// public types ------------------------------------------------------------
	// public constants --------------------------------------------------------
    // public functions --------------------------------------------------------
	inline adecoder();
	inline ~adecoder();

	inline bool mk(const int *const models, const int n);
	inline bool mk(const int *const models, const int n, const int sz);
	inline void rm();

	inline void begin(const bool update_models = true);
	inline void end();

	inline void model(const int i);

	inline void get(v_t &v);
	inline v_t get();
	inline v_t eof() const { return _dec->EOF_SYMBOL(); }

	inline void save(const std::string &name);
	inline void load(const std::string &name);

protected:
	// protected functions -----------------------------------------------------
	inline bool _mk_coder(const int *const models, const int n);
	inline void _rm_coder(const bool init = false);

private:
	int _n;
	int *_models;

	BITInMemStream *_bis;
	ArDecoderWithBitCounter<BITInMemStream> *_dec;
};


////////////////////////////////////////////////////////////////////////////////
// acoder class public definitions
template <class v_t, class b_t>
inline adecoder<v_t, b_t>::adecoder() {
	_rm_coder(true);
}


template <class v_t, class b_t>
inline adecoder<v_t, b_t>::~adecoder() {
	_rm_coder();
}


template <class v_t, class b_t>
inline bool adecoder<v_t, b_t>::mk(const int *const models, const int n)
{
	_rm_coder();

	return _mk_coder(models, n);
}


template <class v_t, class b_t>
inline bool adecoder<v_t, b_t>::mk(const int *const models,
								   const int n, const int sz)
{
	if (!mk_buf(sz)) return false;

	_rm_coder();

	return _mk_coder(models, n);
}


template <class v_t, class b_t>
inline void adecoder<v_t, b_t>::rm() {
	_rm_coder();
	rm_buf();
}


template <class v_t, class b_t>
inline void adecoder<v_t, b_t>::begin(const bool update_models) {
	_dec->StartUnpacking();

	// if (!update_models) _dec->load("wtc.stat");

	_init_coder(_dec, _models, _n, update_models);
}


template <class v_t, class b_t>
inline void adecoder<v_t, b_t>::end() {
	_dec->EndUnpacking();
}


template <class v_t, class b_t>
inline void adecoder<v_t, b_t>::model(const int i) {
	_dec->model(i);
}


template <class v_t, class b_t>
inline void adecoder<v_t, b_t>::get(v_t &v) {
	/*DEBUG*/
	int k;
	(*_dec) >> k;
	v = k;
}


template <class v_t, class b_t>
inline v_t adecoder<v_t, b_t>::get() {
	int k;
	(*_dec) >> k;

	return v_t(k);
}


template <class v_t, class b_t>
inline void adecoder<v_t, b_t>::save(const std::string &name) {
	_dec->save(name.c_str());
}

template <class v_t, class b_t>
inline void adecoder<v_t, b_t>::load(const std::string &name) {
	_dec->load(name.c_str());
}


////////////////////////////////////////////////////////////////////////////////
// adecoder class protected definitions
template <class v_t, class b_t>
inline bool adecoder<v_t, b_t>::_mk_coder(
	const int *const models, const int n)
{
	if (NULL == buf()) return false;

	_models = new int[_n = n];
	if (NULL == _models) return false;

	memcpy(_models, models, n * sizeof(int));

	_bis = new BITInMemStream((char *)buf(), buf_sz());
	if (NULL == _bis) return false;

	_dec = new ArDecoderWithBitCounter<BITInMemStream>(_n, _models, *_bis);
	if (NULL == _dec) return false;

	return true;
}


template <class v_t, class b_t>
inline void adecoder<v_t, b_t>::_rm_coder(const bool init) {
	if (!init) {
		if (NULL != _dec) delete _dec;
		if (NULL != _bis) delete _bis;
		if (NULL != _models) delete[] _models;
	}

	_n = 0;
	_models = NULL;
	_bis = NULL;
	_dec = NULL;
}



}	// wtc namespace



#endif	// WTC_adecoder_INCLUDED
