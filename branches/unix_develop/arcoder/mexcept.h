/*
	Written by Michael Kochetkov, 1994.
*/
#ifndef MEXCEPT_H_INCLUDED
#define MEXCEPT_H_INCLUDED

class MExceptionBase {
	public:
	 MExceptionBase() {}
	virtual  ~MExceptionBase() {}
};

typedef unsigned short EXCEPTYPE;
const EXCEPTYPE EX_UNDEFINED		= 0;
const EXCEPTYPE EX_FILE_ERROR		= 1;
const EXCEPTYPE EX_MEMORY			= 2;
const EXCEPTYPE EX_INTERNAL_ERROR	= 3;
const EXCEPTYPE EX_BUFFER_OVERFLOW	= 4;
const EXCEPTYPE EX_LAST_MEXCEPTION	= 5;

extern const char *sExceptMsg[];

class MException : public MExceptionBase {
public:
	int		iExceptCode;
	const char *msg;
	 MException(EXCEPTYPE _ex = EX_UNDEFINED, const char *_msg = 0) : iExceptCode(_ex), msg(_msg) {}
	virtual void print( void );
};

#endif	// MEXCEPT_H_INCLUDED

