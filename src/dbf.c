/******************************************************************************
 * dbf.c
 ******************************************************************************
 * Library to read information from dBASE files
 * Author: Bjoern Berg, clergyman@gmx.de
 * (C) Copyright 2004, Björn Berg
 *
 ******************************************************************************
 * Permission to use, copy, modify and distribute this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation. The
 * author makes no representations about the suitability of this software for
 * any purpose. It is provided "as is" without express or implied warranty.
 *
 * History:
 * $Log$
 * Revision 1.3  2004-05-27 21:17:26  rollinhand
 * introduced errmsg in p_dbf
 *
 * Revision 1.2  2004/05/18 15:27:33  rollinhand
 * splitted header file to libdbf und dbf. libdbf is official
 *
 * Revision 1.1  2004/05/14 20:37:17  rollinhand
 * *** empty log message ***
 *
 * Revision 1.1.1.1  2004/05/14 20:30:36  rollinhand
 ***********************************************************************************/


#include "dbf.h"
#include "libdbf.h"

DB_HEADER h_buf, *header = &h_buf;
DB_FIELD db_buf, *db = &db_buf;

/*static void dbf_GetLibVersion()
{
	fprintf(stderr, "libdbf (c) 2002 - 2004 by Bjoern Berg\n");
}*/

const char *get_db_version (int version) {
	static char name[31];

	switch (version) {
		case 0x02:
			// without memo fields
			return "FoxBase";
		case 0x03:
			// without memo fields
			return "FoxBase+/dBASE III+";
		case 0x04:
			// without memo fields
			return "dBASE IV";
		case 0x05:
			// without memo fields
			return "dBASE 5.0";
		case 0x83:
			return "FoxBase+/dBASE III+";
		case 0x8B:
			return "dBASE IV";
		case 0x30:
			// without memo fields
			return "Visual FoxPro";
		case 0xF5:
			// with memo fields
			return "FoxPro 2.0";
		default:
			sprintf(name, "Unknown (code 0x%.2X)", version);
			return name;
	}
}

/*
 * reads header from file into struct
 */
static int dbf_ReadHeaderInfo(P_DBF *p_dbf)
{
	if ((read( p_dbf->dbf_fh, (char *)header, sizeof(DB_HEADER))) == -1 ) {
		return -1;
	}

	/* Endian Swapping */
	header->header_length = rotate2b(header->header_length);
	header->record_length = rotate2b(header->record_length);
	header->records = rotate4b(header->records);

	return 0;
}

/*
 * open the the current dbf file and returns file handler
 */
int dbf_Open(const char *file, P_DBF *p_dbf)
{
	if (file[0] == '-' && file[1] == '\0') {
		p_dbf->dbf_fh = fileno(stdin);
		return 0;
	}

	if ((p_dbf->dbf_fh = open(file, O_RDONLY|O_BINARY)) == -1) {
		return -1;
	}
	
	if ( dbf_ReadHeaderInfo(p_dbf) == -1 ) {
		p_dbf->errmsg = perror("In function dbf_Open(): ");
		return -1;
	}

	return 0;
}

/*
 * close the current open dbf file incl. error handling routines
 */
int dbf_Close(P_DBF *p_dbf)
{
	if ( p_dbf->dbf_fh == fileno(stdin) )
		return 0;

	if( (close(p_dbf->dbf_fh)) == -1 ) {
		p_dbf->errmsg = perror("In function dbf_Close(): ");
		return -1;
	}

	return 0;
}

/******************************************************************************
	Block with functions to get information about the amount of
		- rows and
		- columns
 ******************************************************************************/

int dbf_Numrows(P_DBF *p_dbf)
{
	if ( header->records > 0 ) {
		return header->records;
	} else {
		p_dbf->errmsg = perror("In function dbf_Numrows(): ");
		return -1;
	}

	return 0;
}

int dbf_Numcols(P_DBF *p_dbf)
{
	if ( header->header_length > 0) {
		// TODO: Backlink muss noch eingerechnet werden
		return ((header->header_length - sizeof(DB_HEADER) -1)
					 / sizeof(DB_FIELD));
	} else {
		p_dbf->errmsg = perror("In function dbf_Numcols(): ");
		return -1;
	}

	return 0;
}

/******************************************************************************
	Block with functions to get information about the columns
 ******************************************************************************/

int dbf_ReadFieldInfo(P_DBF *p_dbf, int column)
{
	// seek to specified column, internal we start with 0
	column = column-1;
	if (dbf_Numcols(p_dbf) < column) {
		return -1;
	}

	lseek(p_dbf->dbf_fh,
			sizeof(DB_HEADER)+(sizeof(DB_FIELD)*column),
			SEEK_SET);

	if ((read( p_dbf->dbf_fh, (char*)db, sizeof(DB_FIELD))) == -1 ) {
		p_dbf->errmsg = perror("In function dbf_ReadFieldInfo()");
		return -1;
	}

	return 0;
}

// maximum 11 characters to return
const char *dbf_ColumnName(P_DBF *p_dbf, int column)
{
	if ( dbf_ReadFieldInfo(p_dbf,column) != 0 ) {
		return "invalid";
	}

	return db->field_name;
}

int dbf_ColumnSize(P_DBF *p_dbf, int column)
{
	if ( dbf_ReadFieldInfo(p_dbf,column) != 0 ) {
		return -1;
	}

	return (int)db->field_length;
}

const char dbf_ColumnType(P_DBF *p_dbf, int column)
{
	if ( dbf_ReadFieldInfo(p_dbf,column) != 0 ) {
		return 'X';
	}

	return db->field_type;
}

int dbf_ColumnDecimals(P_DBF *p_dbf, int column)
{
	if ( dbf_ReadFieldInfo(p_dbf,column) != 0 ) {
		return -1;
	}

	return db->field_decimals;
}

u_int32_t dbf_ColumnAddress(P_DBF *p_dbf, int column)
{
	if ( dbf_ReadFieldInfo(p_dbf,column) != 0 ) {
		return -1;
	}

	return db->field_address;
}

/******************************************************************************
	Block with functions to read out special dBASE information, like
		- date
		- usage of memo
 ******************************************************************************/

const char *dbf_GetDate(P_DBF *p_dbf)
{
	static char date[10];

	if ( header->last_update[0] ) {
		sprintf(date, "%d-%02d-%02d",
		1900 + header->last_update[0], header->last_update[1], header->last_update[2]);

		return date;
	} else {
		p_dbf->errmsg = perror("In function dbf_GetDate(): ");
		return (char *)-1;
	}

	return 0;
}

int dbf_HeaderSize(P_DBF *p_dbf)
{
 	if ( header->header_length > 0 ) {
		return header->header_length;
	} else {
		p_dbf->errmsg = perror("In function dbf_HeaderSize(): ");
		return -1;
	}

	return 0;
}

int dbf_RecordLength(P_DBF *p_dbf)
{
 	if (header->record_length > 0) {
		return header->record_length;
	} else {
		p_dbf->errmsg = perror("In function dbf_RecordLength(): ");
		return -1;
	}

	return 0;
}

const char *dbf_GetVersion(P_DBF *p_dbf, char *param)
{
	if ( header->version == 0 ) {
		p_dbf->errmsg = perror("In function dbf_GetVersion(): ");
		return (char *)-1;
	}

	if ( strcmp(param,"text") == 0 ) {
		return get_db_version(header->version);
	}

	/*if ( strcmp(param,"numeric") == 0 ) {
		return (char)header->version;
	}*/

	return 0;
}

int dbf_IsMemo(P_DBF *p_dbf)
{
	int memo;

	if ( header->version == 0 ) {
		p_dbf->errmsg = perror("In function dbf_IsMemo(): ");
		return -1;
	}

	memo = (header->version  & 128)==128 ? 1 : 0;

	return memo;
}


