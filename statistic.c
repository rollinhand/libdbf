/***********************************************************************************
 * statistic.c
 * inherits the statistic functions for dBASE files
 * Author: Bjoern Berg, September 2002
 * Email: clergyman@gmx.de
 * dbf Reader and Converter for dBase III, IV, 5.0
 *
 *
 * History:
 * $Log$
 * Revision 1.1  2004-05-14 20:30:37  rollinhand
 * Initial revision
 *
 * Revision 1.6  2004/04/19 18:32:47  rollinhand
 * get_db_version can static, should handle unexpected values
 *
 * Revision 1.5  2004/03/16 20:56:30  rollinhand
 * Endian Swapping centralized in dbf_read_header
 *
 * 2003-11-05	berg	get_db_version() and verbosity checks for header values in
 *						dbf_file_info
 ************************************************************************************/

#include "statistic.h"
#include "dbf.h"


static const char *get_db_version (int version) {
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


/* output for header statistic */
void
dbf_file_info (const struct DB_HEADER *db)
{
	int version, memo;

	version	= db->version;
	memo = (version  & 128)==128 ? 1 : 0;
	printf("\n-- File statistics\n");
	printf("dBase version.........: \t %s (%s)\n",
			get_db_version(version), memo?"with memo":"without memo");
	printf("Date of last update...: \t %d-%02d-%02d\n",
			1900 + db->last_update[0], db->last_update[1], db->last_update[2]);
	printf("Number of records.....: \t %d (%08xd)\n",
			db->records, db->records);
	printf("Length of header......: \t %d (%04xd)\n",
			db->header_length, db->header_length);
	printf("Record length.........: \t %d (%04xd)\n",
			db->record_length, db->record_length);
	printf("Columns in file.......: \t %d \n",
			dbc?((db->header_length - 263)/32)-1:
			(db->header_length/32)-1);



	printf("Rows in file..........: \t %d\n\n",
			rotate4b(db->records));
}

/* output for field statistic */
#define linelength	73

void
dbf_field_stat (const struct DB_FIELD *header, int header_length)
{
	const struct DB_FIELD *dbf;
	int cross[] = {1,17,25,41,57,73};

	drawline(linelength, cross, (sizeof(cross)/sizeof(int)));
	printf("| field name\t| type\t| field adress\t| length\t| field dec.\t|\n");
	drawline(linelength, cross, sizeof(cross)/sizeof(int));
	for (dbf = header + 1; --header_length; dbf++)
		printf("|%13.11s\t| %3c\t| %8x\t| %3d\t\t| %3d\t\t|\n",
			   dbf->field_name, dbf->field_type, rotate4b(dbf->field_adress), dbf->field_length, dbf->field_decimals);
	drawline(linelength, cross, sizeof(cross)/sizeof(int));
}
