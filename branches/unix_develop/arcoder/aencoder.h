/*******************************************************************************
* file:         aencoder.cpp                                                   *
* version:      0.0.1                                                          *
* author:       veld (mailto:yaxen@mail.ru, ICQ:250-8-250)					   *
* description:  not available                                                  *
* tests:        none                                                           *
*******************************************************************************/


#ifndef WTC_AENCODER_INCLUDED
#define WTC_AENCODER_INCLUDED

////////////////////////////////////////////////////////////////////////////////
// headers

// standard C++ library headers
#include <string.h>

// original arcoder headers
#include "arcodec.h"
#include "arbitstream.h"

// wrappers headers
#include "acoder.h"


////////////////////////////////////////////////////////////////////////////////
// wtc namespace
namespace wtc {


////////////////////////////////////////////////////////////////////////////////
// aencoder class declaration
template <
	class v_t = long,	// алфавит арифметического кодера
	class b_t = char	// тип для буферов арифметического кодера
>
class aencoder: public acoder<v_t, b_t> {
public:
	// public types ------------------------------------------------------------
	// public constants --------------------------------------------------------
    // public functions --------------------------------------------------------
	inline aencoder();
	inline ~aencoder();

	inline bool mk(const int *const models, const int n);
	inline bool mk(const int *const models, const int n, const int sz);
	inline void rm();

	inline void begin(const bool update_models = true);
	inline void end();

	inline void model(const int i);

	inline void put(const v_t &v, const bool fixed_models = false);

	inline float entropy(const v_t &v) const;
	inline float entropy(const v_t &v, const int i) const;
	inline int bit_counter();

	inline int n() const { return _n; }
	inline int packed() const { return _bos->PackedBytes(); }

	inline void save(const std::string &name);
	inline void load(const std::string &name);

protected:
	// protected functions -----------------------------------------------------
	inline bool _mk_coder(const int *const models, const int n);
	inline void _rm_coder(const bool init = false);

private:
	int _n;
	int *_models;

	BITOutMemStream *_bos;
    ArCoderWithBitCounter<BITOutMemStream> *_enc;
};


////////////////////////////////////////////////////////////////////////////////
// acoder class public definitions
template <class v_t, class b_t>
inline aencoder<v_t, b_t>::aencoder() {
	_rm_coder(true);
}


template <class v_t, class b_t>
inline aencoder<v_t, b_t>::~aencoder() {
	_rm_coder();
}


template <class v_t, class b_t>
inline bool aencoder<v_t, b_t>::mk(const int *const models, const int n)
{
	_rm_coder();

	return _mk_coder(models, n);
}


template <class v_t, class b_t>
inline bool aencoder<v_t, b_t>::mk(const int *const models,
								   const int n, const int sz)
{
	if (!mk_buf(sz)) return false;

	_rm_coder();

	return _mk_coder(models, n);
}


template <class v_t, class b_t>
inline void aencoder<v_t, b_t>::rm() {
	_rm_coder();
	rm_buf();
}


template <class v_t, class b_t>
inline void aencoder<v_t, b_t>::begin(const bool update_models) {
	_enc->StartPacking();

	// if (!update_models) _enc->load("wtc.stat");

	_init_coder(_enc, _models, _n, update_models);
}


template <class v_t, class b_t>
inline void aencoder<v_t, b_t>::end() {
	_enc->PutEOF();
	_enc->EndPacking();
}


template <class v_t, class b_t>
inline void aencoder<v_t, b_t>::model(const int i) {
	_enc->model(i);
}


template <class v_t, class b_t>
inline void aencoder<v_t, b_t>::put(const v_t &v, const bool fixed_models) {
	_enc->put(int(v));
	if (!fixed_models) _enc->update_model(int(v));
}


template <class v_t, class b_t>
inline float aencoder<v_t, b_t>::entropy(const v_t &v) const {
	return _enc->entropy_eval(v);
}


template <class v_t, class b_t>
inline float aencoder<v_t, b_t>::entropy(const v_t &v, const int i) const {
	_enc->model(i);
	return _enc->entropy_eval(long(v));
}


template <class v_t, class b_t>
inline int aencoder<v_t, b_t>::bit_counter() {
	return _enc->model()->bit_counter;
}


template <class v_t, class b_t>
inline void aencoder<v_t, b_t>::save(const std::string &name) {
	_enc->save(name.c_str());
}


template <class v_t, class b_t>
inline void aencoder<v_t, b_t>::load(const std::string &name) {
	_enc->load(name.c_str());
}


////////////////////////////////////////////////////////////////////////////////
// aencoder class protected definitions
template <class v_t, class b_t>
inline bool aencoder<v_t, b_t>::_mk_coder(
	const int *const models, const int n)
{
	if (NULL == buf()) return false;

	_models = new int[_n = n];
	if (NULL == _models) return false;

	memcpy(_models, models, n * sizeof(int));

	_bos = new BITOutMemStream((char *)buf(), buf_sz());
	if (NULL == _bos) return false;

	_enc = new ArCoderWithBitCounter<BITOutMemStream>(n, models, *_bos);
	
	if (NULL == _enc) return false;
	
	return true;
}


template <class v_t, class b_t>
inline void aencoder<v_t, b_t>::_rm_coder(const bool init) {
	if (!init) {
		if (NULL != _enc) delete _enc;
		if (NULL != _bos) delete _bos;
		if (NULL != _models) delete[] _models;
	}

	_n = 0;
	_models = NULL;
	_bos = NULL;
	_enc = NULL;
}



}	// wtc namespace



#endif	// WTC_AENCODER_INCLUDED

