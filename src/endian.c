/*****************************************************************************
 * endian.c
 *****************************************************************************
 * Routines for Little Endian and Big Endian Systems
 * Library version
 *
 * Version 0.4, 2003-09-08
 * Author: Björn Berg, clergyman@gmx.de
 *
 * History:
 * 2003-09-08	berg	changes in u_int types, adapted to different system
 *						standards
 * 2003-05-09   jones	changes for AIX in IsBigEndian()
 * 2003-04-18	berg	endian.h splitted to endian.h and endian.c
 *						implemented fix by Uwe Steinmann
 * 2003-02-16	jones	the #ifndef inclusion guard only protected the #defines
 *						where it really needs to protect the whole file. Just
 *						moved the #endif to the end of the file.
 * 2003-02-09	jones	improved IsBigEndian function
 *						changes in rotate4b
 * 2003-02-01	berg	rotate2b / rotate4b added
 * 2002-12-12	berg	first implementation
 ****************************************************************************/

#include "endian.h"

/*******************************************************************
 * Routine to test if System uses Big Endian or Little Endian
 *******************************************************************/
_bool IsBigEndian() {
#if defined(__aix__)
 return _true;
#else
 int i = 1;
 int c = *((char*)&i);
 return c == 0;
#endif
}

/*******************************************************************
 * Changes byte construction if dbf is used on another platform
 * than Little Endian. dBASE databases are written in Little Endian
 * format.
 *******************************************************************/
short rotate2b(short var) {
	short tmp;
	char *ptmp;
	tmp = var;
	ptmp = (char *) &tmp;
	return(((short) ptmp[1] << 8) + (short) ptmp[0]);
}

unsigned int rotate4b(u_int32_t var) {
	unsigned int tmp;
	char *ptmp;
	tmp = var;
	ptmp = (char *) &tmp;
	return(((unsigned int) ptmp[3] << 24) + ((unsigned int) ptmp[2] << 16) + ((unsigned int) ptmp[1] << 8) + (unsigned int) ptmp[0]);
}
