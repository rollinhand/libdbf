/******************************************************************************
 * csv.h
 ******************************************************************************
 * dbf Reader and Converter for dBASE files
 * Author: Bjoern Berg <clergyman@gmx.de>
 *
 ******************************************************************************
 * This includes enable dbf to write CSV files
 ******************************************************************************
 * History:
 * $Log$
 * Revision 1.1  2004-05-14 20:30:37  rollinhand
 * Initial revision
 *
 * Revision 1.5  2004/04/25 15:21:10  rollinhand
 * replaced tables.h with dbf.h
 *
 * Revision 1.4  2003/11/11 15:07:08  rollin_hand
 * added cvs keyword log:
 *
 *
 ******************************************************************************/

#ifndef _ANUBISNET_CSV_
#define _ANUBISNET_CSV_

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

#ifdef __unix__
	#include <sys/stat.h>
    #include <unistd.h>
    #define NEWLINE_LENGTH 1
#elif __MSDOS__ || _WIN32
    #include <io.h>
    #include <sys\stat.h>
    #define NEWLINE_LENGTH 2
#else
    #define NEWLINE_LENGTH 1
#endif

/*
 * Special anubisnet includes
 */
#include "an_string.h"
#include "dbf.h"

int setCSVSep (FILE *fp, const struct DB_FIELD * header,
    int header_length, const char *input, const char *separator);
int writeCSVHeader (FILE *fp, const struct DB_FIELD * header,
    int header_length, const char *input, const char *output);
int writeCSVLine (FILE *fp, const struct DB_FIELD * header, const unsigned char *value,
    int header_length, const char *input, const char *output);
#endif
