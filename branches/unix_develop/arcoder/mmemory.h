// Массивы

/*
	Written by Michael Kochetkov, 1994.
*/

#ifndef _INC_MMEMORY_
#define _INC_MMEMORY_

#include "mexcept.h"
#include <stdio.h>
#include <memory.h>
#include <assert.h>

template <class T>
class MemPtr {
	T *ptr;
public:
	inline MemPtr( void ) : ptr(NULL) {}
	inline MemPtr( size_t size ) {
		if ( (ptr = new T[size]) == 0 ) {
			throw MException(EX_MEMORY);
		}
	}
	inline MemPtr( size_t size, T val ) {
		if ( (ptr = new T[size]) == 0 ) {
			throw MException(EX_MEMORY);
		}
		for ( size_t i = 0; i < size; i++) {
			ptr[i] = val;
		}
	}
	inline MemPtr( size_t size, const T* tPtr ) {
		assert(tPtr);
		if ( (ptr = new T[size]) == 0 ) {
			throw MException(EX_MEMORY);
		}
		memcpy(ptr,tPtr,sizeof(T)*size);
	}
	inline ~MemPtr() {
		if (ptr) {
			delete[] ptr;
			ptr = 0; //Можем указатель одного объекта MemPtr передать в другой и удалиться он только один раз.
		}
	}
	inline void init(size_t size) {
		if (ptr) {
			delete[] ptr;
		}
		if ( (ptr = new T[size]) == 0 ) {
			throw MException(EX_MEMORY);
		}
	}
	inline T* set( T* t ) { T* temp = ptr; ptr = t; return temp; }
	T* & operator ()() const { return (T*&)ptr; }
	T& operator [] ( int i ) const { return ptr[i]; }
};


template <class T>
class array_base {
protected:
	MemPtr<T> data;
	int amount;
public:
	array_base() : amount(0) {}
	array_base(int _amount) : amount(_amount), data(_amount) {}
	array_base(const array_base& ar) : amount(ar.amount), data(ar.amount,ar.data()) {}
	virtual ~array_base() {}
	void init( int _amount ) {
		amount = _amount;
		T* tmp = data.set(new T[_amount]);
		if ( tmp) {
			delete[] tmp;
		}
	}
	int Size( void ) const {
		return amount;
	}
	inline T& operator [] ( int i ) const  {
		assert( i < amount );
		return data[i];
	}
	array_base<T> & operator = ( const array_base& ar ) {
		assert( amount == ar.Size());
		memcpy(data(),&ar[0],sizeof(T)*amount);
		return *this;
	}
};

template <class T>
class MArray : public array_base<T> {
public:
	MArray() : array_base<T>() {}
	MArray(int _amount) : array_base<T>(_amount) {}
	MArray(const MArray& ar) : array_base<T>(ar) {}
	virtual ~MArray() {}
};

template <class T>
class M2DArray : public array_base<T> {
protected:
	int I;
	int J;
public:
	M2DArray() {}
	M2DArray( int i, int j ) : array_base<T>(i*j), I(i), J(j) {}
	inline T* operator [] ( int i ) const {
		assert( i < I );
		return this->data() + i * J;
	}
	int getI( void ) const { return I; }
	int getJ( void ) const { return J; }
};

#endif  /* _INC_MMEMORY_ */

