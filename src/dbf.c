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
 * Revision 1.7  2004-08-30 08:02:19  steinm
 * - added comment for each function
 *
 * Revision 1.6  2004/08/27 07:02:02  steinm
 * - use gettext to output error messages
 *
 * Revision 1.5  2004/08/27 05:27:58  steinm
 * - various modification to api
 * - added functions to get record
 * - store header and fields of file in P_DBF for faster access
 *
 * Revision 1.4  2004/06/18 14:45:54  steinm
 * - complete switch to autotools
 *
 * Revision 1.2  2004/05/18 15:27:33  rollinhand
 * splitted header file to libdbf und dbf. libdbf is official
 *
 * Revision 1.1  2004/05/14 20:37:17  rollinhand
 * *** empty log message ***
 *
 * Revision 1.1.1.1  2004/05/14 20:30:36  rollinhand
 *****************************************************************************/


#include "../include/libdbf/libdbf.h"
#include "dbf.h"

/* get_db_version() {{{
 * Convert version field of header into human readable string.
 */
const char *get_db_version(int version) {
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
			sprintf(name, _("Unknown (code 0x%.2X)"), version);
			return name;
	}
}
/* }}} */

/* dbf_ReadHeaderInfo() {{{
 * Reads header from file into struct
 */
static int dbf_ReadHeaderInfo(P_DBF *p_dbf)
{
	DB_HEADER *header;
	if(NULL == (header = malloc(sizeof(DB_HEADER)))) {
		return -1;
	}
	if ((read( p_dbf->dbf_fh, header, sizeof(DB_HEADER))) == -1 ) {
		return -1;
	}

	/* Endian Swapping */
	header->header_length = rotate2b(header->header_length);
	header->record_length = rotate2b(header->record_length);
	header->records = rotate4b(header->records);
	p_dbf->header = header;

	return 0;
}
/* }}} */

/* dbf_Open() {{{
 * Open the a dbf file and returns file handler
 */
P_DBF *dbf_Open(const char *file)
{
	P_DBF *p_dbf;
	if(NULL == (p_dbf = malloc(sizeof(P_DBF)))) {
		return NULL;
	}

	if (file[0] == '-' && file[1] == '\0') {
		p_dbf->dbf_fh = fileno(stdin);
	} else if ((p_dbf->dbf_fh = open(file, O_RDONLY|O_BINARY)) == -1) {
		free(p_dbf);
		return NULL;
	}

	p_dbf->header = NULL;
	if(0 > dbf_ReadHeaderInfo(p_dbf)) {
		free(p_dbf);
		return NULL;
	}

	p_dbf->fields = NULL;
	if(0 > dbf_ReadFieldInfo(p_dbf)) {
		free(p_dbf->header);
		free(p_dbf);
		return NULL;
	}

	p_dbf->cur_record = -1;

	return p_dbf;
}
/* }}} */

/* dbf_Close() {{{
 * Close the current open dbf file and free all memory
 */
int dbf_Close(P_DBF *p_dbf)
{
	if(p_dbf->header)
		free(p_dbf->header);

	if(p_dbf->fields)
		free(p_dbf->fields);

	if ( p_dbf->dbf_fh == fileno(stdin) )
		return 0;

	if( (close(p_dbf->dbf_fh)) == -1 ) {
		return -1;
	}

	free(p_dbf);

	return 0;
}
/* }}} */

/******************************************************************************
	Block with functions to get information about the amount of
		- rows and
		- columns
 ******************************************************************************/

/* dbf_NumRows() {{{
 * Returns the number of records.
 */
int dbf_NumRows(P_DBF *p_dbf)
{
	if ( p_dbf->header->records > 0 ) {
		return p_dbf->header->records;
	} else {
		perror(_("In function dbf_NumRows(): "));
		return -1;
	}

	return 0;
}
/* }}} */

/* dbf_NumCols() {{{
 * Returns the number of fields.
 */
int dbf_NumCols(P_DBF *p_dbf)
{
	if ( p_dbf->header->header_length > 0) {
		// TODO: Backlink muss noch eingerechnet werden
		return ((p_dbf->header->header_length - sizeof(DB_HEADER) -1)
					 / sizeof(DB_FIELD));
	} else {
		perror(_("In function dbf_NumCols(): "));
		return -1;
	}

	return 0;
}
/* }}} */

/******************************************************************************
	Block with functions to get information about the columns
 ******************************************************************************/

/* dbf_ReadFieldInfo() {{{
 * Sets p_dbf->fields to an array of DB_FIELD containing the specification
 * for all columns.
 */
int dbf_ReadFieldInfo(P_DBF *p_dbf)
{
	int columns, i, offset;
	DB_FIELD *fields;

	columns = dbf_NumCols(p_dbf);

	if(NULL == (fields = malloc(columns * sizeof(DB_FIELD)))) {
		return -1;
	}

	lseek(p_dbf->dbf_fh, sizeof(DB_HEADER), SEEK_SET);

	if ((read( p_dbf->dbf_fh, fields, columns * sizeof(DB_FIELD))) == -1 ) {
		perror(_("In function dbf_ReadFieldInfo(): "));
		return -1;
	}
	p_dbf->fields = fields;
	p_dbf->columns = columns;
	/* The first byte of a record indicates whether it is deleted or not. */
	offset = 1;
	for(i = 0; i < columns; i++) {
		fields[i].field_offset = offset;
		offset += fields[i].field_length;
	}

	return 0;
}
/* }}} */

/* dbf_ColumnName() {{{
 * Returns the name of a column. Column names cannot be longer than
 * 11 characters.
 */
const char *dbf_ColumnName(P_DBF *p_dbf, int column)
{
	if ( column >= p_dbf->columns ) {
		return "invalid";
	}

	return p_dbf->fields[column].field_name;
}
/* }}} */

/* dbf_ColumnSize() {{{
 */
int dbf_ColumnSize(P_DBF *p_dbf, int column)
{
	if ( column >= p_dbf->columns ) {
		return -1;
	}

	return (int) p_dbf->fields[column].field_length;
}
/* }}} */

/* dbf_ColumnType() {{{
 */
const char dbf_ColumnType(P_DBF *p_dbf, int column)
{
	if ( column >= p_dbf->columns ) {
		return -1;
	}

	return p_dbf->fields[column].field_type;
}
/* }}} */

/* dbf_ColumnDecimals() {{{
 */
int dbf_ColumnDecimals(P_DBF *p_dbf, int column)
{
	if ( column >= p_dbf->columns ) {
		return -1;
	}

	return p_dbf->fields[column].field_decimals;
}
/* }}} */

/* dbf_ColumnAddress() {{{
 */
u_int32_t dbf_ColumnAddress(P_DBF *p_dbf, int column)
{
	if ( column >= p_dbf->columns ) {
		return -1;
	}

	return p_dbf->fields[column].field_address;
}
/* }}} */

/******************************************************************************
	Block with functions to read out special dBASE information, like
		- date
		- usage of memo
 ******************************************************************************/

/* dbf_GetDate() {{{
 * Returns the date of last modification as a human readable string.
 */
const char *dbf_GetDate(P_DBF *p_dbf)
{
	static char date[10];

	if ( p_dbf->header->last_update[0] ) {
		sprintf(date, "%d-%02d-%02d",
		1900 + p_dbf->header->last_update[0], p_dbf->header->last_update[1], p_dbf->header->last_update[2]);

		return date;
	} else {
		perror("In function GetDate(): ");
		return (char *)-1;
	}

	return 0;
}
/* }}} */

/* dbf_HeaderSize() {{{
 */
int dbf_HeaderSize(P_DBF *p_dbf)
{
 	if ( p_dbf->header->header_length > 0 ) {
		return p_dbf->header->header_length;
	} else {
		perror(_("In function dbf_HeaderSize(): "));
		return -1;
	}

	return 0;
}
/* }}} */

/* dbf_RecordLength() {{{
 * Returns the length of a record.
 */
int dbf_RecordLength(P_DBF *p_dbf)
{
 	if (p_dbf->header->record_length > 0) {
		return p_dbf->header->record_length;
	} else {
		perror(_("In function dbf_RecordLength(): "));
		return -1;
	}

	return 0;
}
/* }}} */

/* dbf_GetStringVersion() {{{
 * Returns the verion of the dbase file as a human readable string.
 */
const char *dbf_GetStringVersion(P_DBF *p_dbf)
{
	if ( p_dbf->header->version == 0 ) {
		perror(_("In function dbf_GetStringVersion(): "));
		return (char *)-1;
	}

	return get_db_version(p_dbf->header->version);
}
/* }}} */

/* dbf_GetVersion() {{{
 * Returns the verion field as it is storedi in the header.
 */
int dbf_GetVersion(P_DBF *p_dbf)
{
	if ( p_dbf->header->version == 0 ) {
		perror(_("In function dbf_GetVersion(): "));
		return -1;
	}

	return p_dbf->header->version;
}
/* }}} */

/* dbf_IsMemo() {{{
 */
int dbf_IsMemo(P_DBF *p_dbf)
{
	int memo;

	if ( p_dbf->header->version == 0 ) {
		perror(_("In function dbf_IsMemo(): "));
		return -1;
	}

	memo = (p_dbf->header->version  & 128)==128 ? 1 : 0;

	return memo;
}
/* }}} */

/******************************************************************************
	Block with functions to read records
 ******************************************************************************/

/* dbf_ReadRecord() {{{
 */
int dbf_ReadRecord(P_DBF *p_dbf, char *record, int len) {
	off_t offset;

	if((p_dbf->cur_record+1) >= p_dbf->header->records)
		return -1;

	offset = lseek(p_dbf->dbf_fh, p_dbf->header->header_length + (p_dbf->cur_record+1) * (p_dbf->header->record_length), SEEK_SET);
//	fprintf(stdout, "Offset = %d, Record length = %d\n", offset, p_dbf->header->record_length);
	if (read( p_dbf->dbf_fh, record, p_dbf->header->record_length) == -1 ) {
		return -1;
	}
	p_dbf->cur_record++;
	return p_dbf->cur_record;
}
/* }}} */

/* dbf_GetRecordData() {{{
 */
char *dbf_GetRecordData(P_DBF *p_dbf, char *record, int column) {
	return(record + p_dbf->fields[column].field_offset);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
