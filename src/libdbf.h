/******************************************************************************
 * libdbf.h
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
 * Revision 1.1  2004-05-18 15:29:02  rollinhand
 * official include to libdbf
 *
 *
 ***********************************************************************************/

/*! \file libdbf.h
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