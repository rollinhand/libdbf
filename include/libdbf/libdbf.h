/******************************************************************************
 * liblibdbf.h
 ******************************************************************************
 * Library to read information from dBASE files
 * Author: Bjoern Berg, clergyman@gmx.de
 * (C) Copyright 2004, Björn Berg
 *
 ****************************************************************************
 * Permission to use, copy, modify and distribute this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation. The
 * author makes no representations about the suitability of this software for
 * any purpose. It is provided "as is" without express or implied warranty.
 *
 * History:
 * $Log$
 * Revision 1.2  2004-08-27 05:29:03  steinm
 * - definition of internal structs removed
 * - added defines for different versions of a dbf file
 *
 * Revision 1.1  2004/06/18 14:45:54  steinm
 * - complete switch to autotools
 *
 * Revision 1.2  2004/05/27 21:16:56  rollinhand
 * deleted dbf_Connect, makes no sense
 *
 * Revision 1.1  2004/05/18 15:29:02  rollinhand
 * official include to libdbf 
 ****************************************************************************/

#include <sys/types.h>

/*! \file liblibdbf.h
	\brief provides access to libdbf.

	If you want to use functions of libdbf in your own programme, you have to add
	libdbf.h to your source files. libdbf.h describes all necessary accessible functions
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

typedef struct _P_DBF P_DBF;

/*
 *	FUNCTIONS
 */

/*! \fn dbf_GetVersion(P_DBF *p_dbf)
	\brief return the version of dbf file as string
*/
const char *dbf_GetStringVersion(P_DBF *p_dbf);

/*! \fn dbf_GetVersion(P_DBF *p_dbf)
	\brief return the version of dbf file
	\param *p_dbf the filehandler of the opened file

	\return version or -1 on error
*/
int dbf_GetVersion(P_DBF *p_dbf);

/*! \fn P_DBF *dbf_Open (const char *file)
	\brief dbf_Open opens a dBASE \a file and returns the object handle
	\param file the filename of the dBASE file

	dbf_Connect opens a dBASE file and returns the object handle.
	Additionally information about the dBASE header are read in.
	\return NULL in case of an error.
*/
P_DBF *dbf_Open (const char *file);

/*! \fn int dbf_Close (P_DBF *p_dbf)
	\brief dbf_Close closes a dBASE file.
	\param *p_dbf the filehandler of the opened file

	dbf_Close closes a dBASE file.
	It returns 0 if connection was successful and -1 if not.
*/
int dbf_Close (P_DBF *p_dbf);

// Functions to info about rows and columns
/*! \fn int dbf_NumRows (P_DBF *p_dbf)
	\brief dbf_NumRows returns the number of datasets/rows
	\param *p_dbf the filehandler of the opened file

	dbf_NumRows returns the number of datasets/rows. Returnvalues are
	number of rows or -1 if determination of rows fails.
*/
int dbf_NumRows (P_DBF *p_dbf);

/*! \fn int dbf_NumCols (P_DBF *p_dbf)
	\brief dbf_NumCols returns the number of attributes/columns
	\param *p_dbf the filehandler of the opened file

	dbf_NumCols returns the number of attributes/columns. Returnvalues are
	number of columns or -1 if determination of rows fails.
*/
int dbf_NumCols (P_DBF *p_dbf);

/*! \fn const char *dbf_ColumnName(P_DBF *p_dbf, int column)
	\brief dbf_ColumnName returns the name of a selected \a column
	\param *p_dbf the filehandler of the opened file
	\param column the number of the column

	dbf_ColumnName returns the name of a selected column. Columnumber
	starts with 1, maximum number of columns can be determined with
	dbf_NumCols.
	\return Name of column or -1 on error
*/
const char *dbf_ColumnName(P_DBF *p_dbf, int column);

/*! \fn int dbf_ColumnSize(P_DBF *p_dbf, int column);
	\brief dbf_ColumnSize returns the field length of a column
	\param *p_dbf the filehandler of the opened file
	\param column the number of the column

	dbf_ColumnSize returns the field length of a column. Columnumber
	starts with 1, maximum number of columns can be determined with
	dbf_NumCols.
	\return field length of column or -1 on error
*/
int dbf_ColumnSize(P_DBF *p_dbf, int column);

/*! \fn const char dbf_ColumnType(P_DBF *p_dbf, int column)
	\brief dbf_ColumnType returns the type of a field resp. column
	\param *p_dbf the filehandler of the opened file
	\param column the number of the column

	dbf_ColumnType returns the type of a field resp. column. Columnumber
	starts with 1, maximum number of columns can be determined with
	dbf_NumCols.
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

/*! \fn int dbf_RecordLength(P_DBF *p_dbf)
	\brief dbf_RecordLength returns length of a dataset
	\param *p_dbf the filehandler of the opened file

	dbf_RecordLength returns length of a dataset. Do not confuse this
	with dbf_HeaderSize().

	\return record length or -1 on error
*/
int dbf_RecordLength(P_DBF *p_dbf);

/*! \fn int dbf_IsMemo(P_DBF *p_dbf)
	\brief dbf_IsMemo tells if dbf provides also a memo file
	\param *p_dbf the filehandler of the opened file

	dbf_IsMemo tells if dbf provides also a memo file

	\return 0 no memo, 1 memo, -1 on error
*/
int dbf_IsMemo(P_DBF *p_dbf);
