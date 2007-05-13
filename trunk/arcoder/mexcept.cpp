#include <iostream>
#include "MExcept.h"
using namespace std;

static const char *sExceptMsg[] = {
	"\nUndefined exception occurred.",
	"\nFile error.",
	"\nNot enough memory for this operation.",
	"\nInternal Error"
};

void MException::print( void ) {
	if ( msg ) {
		cerr << msg;
	}
	else if ( iExceptCode >= EX_LAST_MEXCEPTION ) {
		cerr << "\nException number " << iExceptCode << "occurred.";
	}
	else if ( iExceptCode == EX_FILE_ERROR ) {
		perror("Error");
	}
	else {
		cerr << sExceptMsg[iExceptCode];
	}
}
