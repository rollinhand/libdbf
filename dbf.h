/******************************************************************************
 * dbf.h
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
 * Revision 1.1  2004-05-14 20:30:36  rollinhand
 * Initial revision
 *
 *
 ***********************************************************************************/

/*!	\file dbf.h
	\brief provides access to libdbf.

	If you want to use functions of libdbf in your own programme, you have to add
	dbf.h to your source files. dbf.h describes all necessary accessible functions
	that libdbf provides.
*/
/*!	\mainpage libdbf Documentation
	\section intro Introduction
	<p>libdbf is the backend library for dbf to convert dBASE/FoxPro files to any
	format you want to. libdbf provides generic interfaces to access information
	in a dBASE file very easily without knowing much about the structure of the
	file.</p>
	<p>The commands and functions of libdbf orintate much on PHP's functions to access
	MySQL or Oracle databases. So, first steps should be very easy.</p>

	\section data Data Structures
	<p>The Link "Data Structures" gives a brief overview of the used classes and
	structures in libdbf.</p>

	\section file File List
	<p>This section is less important</p>

	\section fields Data Fields
	<p>This section describes the used variables used in libdbf.</p>

	\section globals Globals
	<p>This is the most important section. Behind this section you find all accessible
	functions and defines you can use in your own programme.</p>

	\section license License
	<p>Permission to use, copy, modify and distribute this software and its
 	 documentation for any purpose is hereby granted without fee, provided that
 	 the above copyright notice appear in all copies and that both that copyright
 	 notice and this permission notice appear in supporting documentation. The
 	 author makes no representations about the suitability of this software for
 	 any purpose. It is provided "as is" without express or implied warranty.</p>
*/

#ifndef __DBF_CORE__
#define __DBF_CORE__

#ifdef __unix__
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#ifndef __ANUBISNET_TYPES__
#define __ANUBISNET_TYPES__
	typedef u_int16_t uint16_t;
	typedef u_int32_t uint32_t;
#endif
#elif _WIN32
#include <io.h>
#include <sys\stat.h>
#include <windows.h>
#ifndef __ANUBISNET_TYPES__
#define __ANUBISNET_TYPES__
	typedef UINT32 u_int32_t;
	// Windows does not know UINT16 types
	typedef unsigned short u_int16_t;
#endif
#elif __MSDOS__
#include <io.h>
#include <sys\stat.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#endif

#ifndef O_BINARY
#define O_BINARY 0
#endif

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>

/*
 * special anubisnet and dbf includes
 */
#include "endian.h"

/*! \def FoxBase Code for FoxBase */
#define FoxBase 0x02
/*! \def FoxBasePlus Code for FoxBasePlus, same as for dBase III */
#define FoxBasePlus 0x03
/*! \def dBase3 Code for dBase III */
#define dBase3 0x03
/*! \def dBase3WM Code for dBase III with memo fields */
#define dBase3WM 0x83
/*! \def dBase4 Code for dBase IV */
#define dBase4 0x04
/*! \def dBase4WM Code for dBase IV with memo fields */
#define dBase4WM 0x8B
/*! \def dBase4SQL Code for dBase IV with SQL table */
#define dBase4SQL 0x8E
/*! \def dBase5 Code for dBase 5.0 */
#define dBase5 0x05
/*! \def FoxPro2WM Code for FoxPro 2.0 (or earlier) with memo fields */
#define FoxPro2WM 0xF5
/*! \def VisualFoxPro Code for Visual FoxPro without memo fields */
#define VisualFoxPro 0x30

//@{
/** These defines are used to distinguish between types in the dbf fields. */
#define IS_STRING 1
#define IS_NUMERIC 2
//@}

/*
 *	STRUCTS
 */

/*! \struct DB_HEADER
	\brief table file header
 	 Standard dBASE Header
	 Offsets of this header are the same in all versions of
	 dBASE except dBASE 7.0
	\warning It is recommend not to access DB_HEADER directly.
 */
typedef struct {
	/*! Byte: 0; dBase version */
	unsigned char version;
	/*! Byte: 1-3; date of last update */
	unsigned char last_update[3];
	/*! Byte: 4-7; number of records in table */
	u_int32_t records;
	/*! Byte: 8-9; number of bytes in the header */
	u_int16_t header_length;
	/*! Byte: 10-11; number of bytes in the record */
	u_int16_t record_length;
	/*! Byte: 12-13; reserved, see specification of dBase databases */
	unsigned char reserved01[2];
	/*! Byte: 14; Flag indicating incomplete transaction */
	unsigned char transaction;
	/*! Byte: 15; Encryption Flag */
	unsigned char encryption;
	/*! Byte: 16-27; reserved for dBASE in a multiuser environment*/
	unsigned char reserved02[12];
	/*! Byte: 28; Production MDX file flag */
	unsigned char mdx;
	/*! Byte: 29; Language driver ID, for Visual FoxPro */
	unsigned char language;
	/*! Byte: 30-31; reserved, filled with zero */
	unsigned char reserved03[2];
} DB_HEADER;

/*! \struct DB_FIELD
	\brief The field descriptor array
	Offsets of this header are the same in all versions of dBASE
	\warning It is recommend not to access DB_FIELD directly.
 */
typedef struct {
	/*! Byte: 0-10; fieldname in ASCII */
	unsigned char field_name[11];
	/*! Byte: 11; field type in ASCII (C, D, L, M or N) */
	unsigned char field_type;
	/*! Byte: 12-15; field data adress */
	u_int32_t field_address;
	/*! Byte: 16; field length in binary */
	unsigned char field_length;
	/*! Byte: 17; field decimal count in binary */
	unsigned char field_decimals;
	/*! Byte: 18-30; reserved */
	unsigned char reserved[13];
	/*! Byte: 31; Production MDX field flag */
	unsigned char mdx;
} DB_FIELD;

/*! \struct P_DBF
	\brief P_DBF is a global file handler

	P_DBF store the file handlers for the dbf-file and if exists
	the appropriate memo file.
*/
typedef struct {
	/*! filehandler of *.dbf */
	int dbf_fh;
	/*! filehandler of memo */
	int dbt_fh;
	/*! the pysical size of the file, as stated from filesystem */
	u_int32_t real_filesize;
	/*! the calculated filesize */
	u_int32_t calc_filesize;
	/*! integrity could be: valid, invalid */
	unsigned char integrity[7];
} P_DBF;

/* Memo File Structure (.FPT)
 * Memo files contain one header record and any number of block structures.
 * The header record contains a pointer to the next free block and the size
 * of the block in bytes. The size is determined by the SET BLOCKSIZE command
 * when the file is created. The header record starts at file position zero and
 * occupies 512 bytes. The SET BLOCKSIZE TO 0 command sets the block size width to 1.
 */
/* Following the header record are the blocks that contain a block header and
 * the text of the memo. The table file contains block numbers that are used to
 * reference the memo blocks. The position of the block in the memo file is
 * determined by multiplying the block number by the block size (found in the
 * memo file header record). All memo blocks start at even block boundary
 * addresses. A memo block can occupy more than one consecutive block.
 */


/* Memo Header Record */

struct DB_MEMO_HEADER {
	/* 00  03	Location of next free block [1] */
	unsigned int block_adress;
	/* 04  05	Unused */
	unsigned char reserved[2];
	/* 06  07	Block size (bytes per block) [1] */
	unsigned short block_size;
	/* 08  511	Unused */
	unsigned char reserved2[504];
 };

 /* [1] Integers stored with the most
 significant byte first. See: endian.h   */

/* Memo Block Header and Memo Text */
struct DB_MEMO_BLOCK_TOP {
	/* 00  03			Block signature [1]  */
	/*	(indicates the type of data in the block)
	 *	0  picture (picture field type)
	 *	1  text (memo field type) */
	unsigned int signature;
	/* 04  07	Length [1] of memo (in bytes) */
	unsigned int block_length;
	/* 08 -  n	Memo text (n = length) */
};

/* [1] Integers stored with the most
significant byte first.    */




/*
 *	FUNCTIONS
 */

/*! \fn dbf_GetLibVersion()
	\brief outputs the version of libdbf
*/
static void dbf_GetLibVersion();

/*! \fn int dbf_Connect (const char *file, P_DBF *p_dbf)
	\brief dbf_Connect opens a dBASE \a file and stores the filehandler in \a P_DBF
	\param file the filename of the dBASE file
	\param *p_dbf the filehandler of the opened file is stored in it
	\warning function does not return the filehandler

	dbf_Connect opens a dBASE file and stores the filehandler in the global P_DBF.
	Additionally information about the dBASE header are read in.
	\return 0 if connection was successful and -1 if not.
*/
int dbf_Connect (const char *file, P_DBF *p_dbf);

/*! \fn int dbf_Open (const char *file, P_DBF *p_dbf)
	\brief dbf_Open opens a dBASE \a file and stores the filehandler in \a P_DBF
	\param file the filename of the dBASE file
	\param *p_dbf the filehandler of the opened file is stored in it
	\warning dbf_Open is a meta function. If you want to work with the opened dBASE
	file, use dbf_Connect()

	dbf_Connect opens a dBASE file and stores the filehandler in the global P_DBF.
	It returns 0 if connection was successful and -1 if not.
*/
int dbf_Open (const char *file, P_DBF *p_dbf);

/*! \fn int dbf_Close (P_DBF *p_dbf)
	\brief dbf_Close closes a dBASE file.
	\param *p_dbf the filehandler of the opened file

	dbf_Close closes a dBASE file.
	It returns 0 if connection was successful and -1 if not.
*/
int dbf_Close (P_DBF *p_dbf);

// Functions to info about rows and columns
/*! \fn int dbf_Numrows (P_DBF *p_dbf)
	\brief dbf_Numrows returns the number of datasets/rows
	\param *p_dbf the filehandler of the opened file

	dbf_Numrows returns the number of datasets/rows. Returnvalues are
	number of rows or -1 if determination of rows fails.
*/
int dbf_Numrows (P_DBF *p_dbf);

/*! \fn int dbf_Numcols (P_DBF *p_dbf)
	\brief dbf_Numcols returns the number of attributes/columns
	\param *p_dbf the filehandler of the opened file

	dbf_Numcols returns the number of attributes/columns. Returnvalues are
	number of columns or -1 if determination of rows fails.
*/
int dbf_Numcols (P_DBF *p_dbf);

/*! \fn const char *dbf_ColumnName(P_DBF *p_dbf, int column)
	\brief dbf_ColumnName returns the name of a selected \a column
	\param *p_dbf the filehandler of the opened file
	\param column the number of the column

	dbf_ColumnName returns the name of a selected column. Columnumber
	starts with 1, maximum number of columns can be determined with
	dbf_Numcols.
	\return Name of column or -1 on error
*/
const char *dbf_ColumnName(P_DBF *p_dbf, int column);

/*! \fn int dbf_ColumnSize(P_DBF *p_dbf, int column);
	\brief dbf_ColumnSize returns the field length of a column
	\param *p_dbf the filehandler of the opened file
	\param column the number of the column

	dbf_ColumnSize returns the field length of a column. Columnumber
	starts with 1, maximum number of columns can be determined with
	dbf_Numcols.
	\return field length of column or -1 on error
*/
int dbf_ColumnSize(P_DBF *p_dbf, int column);

/*! \fn const char dbf_ColumnType(P_DBF *p_dbf, int column)
	\brief dbf_ColumnType returns the type of a field resp. column
	\param *p_dbf the filehandler of the opened file
	\param column the number of the column

	dbf_ColumnType returns the type of a field resp. column. Columnumber
	starts with 1, maximum number of columns can be determined with
	dbf_Numcols.
	\return field type of column or -1 on error
*/
const char dbf_ColumnType(P_DBF *p_dbf, int column);

/*! \fn int dbf_ColumnDecimals(P_DBF *p_dbf, int column)
	\brief
	\param *p_dbf the filehandler of the opened file
	\param column the number of the column


	\return decimals of column or -1 on error
*/
int dbf_ColumnDecimals(P_DBF *p_dbf, int column);

/*! \fn u_int32_t dbf_ColumnAddress(P_DBF *p_dbf, int column)
	\brief dbf_ColumnAddress returns the address of a column
	\param *p_dbf the filehandler of the opened file
	\param column the number of the column

	\return address of column or -1 on error
*/
u_int32_t dbf_ColumnAddress(P_DBF *p_dbf, int column);

/*! \fn const char *dbf_GetDate(P_DBF *p_dbf)
	\brief dbf_GetDate returns a formatted date string
	\param *p_dbf the filehandler of the opened file

	dbf_GetDate returns a formatted date string of the type
	yyyy-mm-dd. The date indicates the time the file was last
	modified.

	\return formatted date or -1 on error
*/
const char *dbf_GetDate(P_DBF *p_dbf);

/*! \fn int dbf_HeaderSize(P_DBF *p_dbf)
	\brief dbf_HeaderSize returns length of dBASE header
	\param *p_dbf the filehandler of the opened file

	dbf_HeaderSize returns length of dBASE header

	\return header length or -1 on error
*/
int dbf_HeaderSize(P_DBF *p_dbf);

/*! \fn int dbf_RecordLength(P_DBF *p_dbf)
	\brief dbf_RecordLength returns length of a dataset
	\param *p_dbf the filehandler of the opened file

	dbf_RecordLength returns length of a dataset. Do not confuse this
	with dbf_HeaderSize().

	\return record length or -1 on error
*/
int dbf_RecordLength(P_DBF *p_dbf);

/*! \fn const char *dbf_GetVersion(P_DBF *p_dbf, char *param)
	\brief dbf_GetVersion returns the version as text or number
	\param *p_dbf the filehandler of the opened file
	\param param expects "text" or "numeric"

	dbf_GetVersion returns the version of the dbf file as text or
	number depending on the parameter "text" or "numeric"

	\return version or -1 on error
	\warning parameter numeric does not work yet.
*/
const char *dbf_GetVersion(P_DBF *p_dbf, char *param);

/*! \fn int dbf_IsMemo(P_DBF *p_dbf)
	\brief dbf_IsMemo tells if dbf provides also a memo file
	\param *p_dbf the filehandler of the opened file

	dbf_IsMemo tells if dbf provides also a memo file

	\return 0 no memo, 1 memo, -1 on error
*/
int dbf_IsMemo(P_DBF *p_dbf);

#endif

