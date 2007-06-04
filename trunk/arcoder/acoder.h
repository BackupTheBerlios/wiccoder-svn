/*******************************************************************************
* file:         acoder.h                                                       *
* version:      0.0.1                                                          *
* author:       veld (mailto:yaxen@mail.ru, ICQ:250-8-250)					   *
* description:  not available                                                  *
* tests:        none                                                           *
*******************************************************************************/

#ifndef WTC_ACODER_INCLUDED
#define WTC_ACODER_INCLUDED

////////////////////////////////////////////////////////////////////////////////
// defines
#define INIT_CODER_OLD_STAT

////////////////////////////////////////////////////////////////////////////////
// headers

// standard C++ library headers
#include <iostream>
#include <fstream>

// original arcoder headers
#include "arcodec.h"
#include "arbitstream.h"

// wrappers headers
// none


////////////////////////////////////////////////////////////////////////////////
// wtc namespace
namespace wtc {


////////////////////////////////////////////////////////////////////////////////
// acoder class declaration
template <
	class v_t = long,	// алфавит арифметического кодера
	class b_t = char	// тип для буферов арифметического кодера
>
class acoder {
public:
	// public types ------------------------------------------------------------
	typedef typename v_t tv_t;
	typedef typename b_t tb_t;

	// public constants --------------------------------------------------------
    // public functions --------------------------------------------------------
	inline acoder();
	inline ~acoder();

	inline bool mk_buf(const int sz, b_t *const b = NULL);
	inline void rm_buf();

	inline b_t *buf() const { return _buf; }
	inline int buf_sz() const { return _buf_sz; }

protected:
	// protected functions -----------------------------------------------------
	inline void _init_coder(arcoder_base *const coder_base,
							const int *const models, const int n,
							const bool update_models = true);
	
	inline bool _mk_buf(const int sz, b_t *const b = NULL);
	inline void _rm_buf(const bool init = false); 
	
private:
	b_t *_buf;
	int _buf_sz;
	bool _internal_buf;

};


////////////////////////////////////////////////////////////////////////////////
// acoder class public definitions
template <class v_t, class b_t>
inline acoder<v_t, b_t>::acoder() {
	_rm_buf(true);
}


template <class v_t, class b_t>
inline acoder<v_t, b_t>::~acoder() {
	_rm_buf();
}


template <class v_t, class b_t>
inline bool acoder<v_t, b_t>::mk_buf(const int sz, b_t *const b) {
	_rm_buf();

	return _mk_buf(sz, b);
}


template <class v_t, class b_t>
inline void acoder<v_t, b_t>::rm_buf() {
	_rm_buf();
}



////////////////////////////////////////////////////////////////////////////////
// acoder class protected definitions
template <class v_t, class b_t>
inline void acoder<v_t, b_t>::_init_coder(
	arcoder_base *const coder_base,
	const int *const models, const int n,
	const bool update_models)
{
	if (update_models) {
		coder_base->ResetStatistics();

		if (NULL != models) {
			for (int i = 0; i < n; i++) {
				coder_base->model(i);
				for (int j = 0; j < models[i]; j++) {
					coder_base->update_model(j);
				}
			}
		}
	}
}


template <class v_t, class b_t>
inline bool acoder<v_t, b_t>::_mk_buf(const int sz, b_t *const b) {
	_buf = (_internal_buf = (NULL == b))? new b_t[sz]: b;

	if (NULL == _buf) return false;

	_buf_sz = sz;

	return true;
}


template <class v_t, class b_t>
inline void acoder<v_t, b_t>::_rm_buf(const bool init) {
	if (init) {
	} else {
		if (NULL != _buf && _internal_buf) delete[] _buf;
	}

	_buf = NULL;
	_buf_sz = 0;
}


	
}	// wtc namespace



#endif	// WTC_ACODER_INCLUDED
