#include "dbf.h"
#include "endian.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>

void drawline (int length, int *cross, int numberofcrosses) {
  int count, i;
  char sign = '-';

  count = 1;
  while(count <= length) {
	for(i=0; i < numberofcrosses; i++) {
	    if(cross[i] == count) sign = '+';
	}
	printf("%c",sign);
	sign = '-';
	count++;
  }
  printf("\n");
}


int dbf_file_info (P_DBF *p_dbf)
{
	printf("\n-- File statistics\n");
	printf("dBase version.........: \t %s (%s)\n",
			dbf_GetVersion(p_dbf,"text"),
			dbf_IsMemo(p_dbf)?"with memo":"without memo");
	printf( "Date of last update...: \t %s\n", dbf_GetDate(p_dbf) );
	printf("Number of records.....: \t %d\n", dbf_Numrows(p_dbf) );
	printf("Length of header......: \t %d \n", dbf_HeaderSize(p_dbf) );
	printf("Record length.........: \t %d \n", dbf_RecordLength(p_dbf) );
	printf("Columns in file.......: \t %d \n", dbf_Numcols(p_dbf) );
	printf("Rows in file..........: \t %d\n\n", dbf_Numrows(p_dbf) );

	return 0;
}

#define linelength	73

void dbf_field_stat(P_DBF *p_dbf)
{
	int cross[] = {1,17,25,41,57,73};
	int i;

	drawline(linelength, cross, (sizeof(cross)/sizeof(int)));
	printf("| field name\t| type\t| field adress\t| length\t| field dec.\t|\n");
	drawline(linelength, cross, sizeof(cross)/sizeof(int));
	for (i = 1; i <= dbf_Numcols(p_dbf); i++)
		printf("|%13.11s\t| %3c\t| %8x\t| %3d\t\t| %3d\t\t|\n",
			   dbf_ColumnName(p_dbf,i), dbf_ColumnType(p_dbf,i),
			   dbf_ColumnAddress(p_dbf,i),dbf_ColumnSize(p_dbf,i),
			   dbf_ColumnDecimals(p_dbf,i) );
	drawline(linelength, cross, sizeof(cross)/sizeof(int));
}

int main(int argc, char *argv[])
{
	P_DBF *p_dbf;

	/*if (argc < 2) {
		fprintf(stderr, "Usage: %s [option][argument] dbf-file, -h for help\n", *argv);
		exit(1);
	}*/


	if ( dbf_Connect(argv[1],p_dbf) == 0 ) {
		fprintf(stdout, "Datei geöffnet: %s\n", argv[1]);
	}

	dbf_file_info(p_dbf);
	dbf_field_stat(p_dbf);

	fprintf(stdout, "Date: %s\n", dbf_GetDate(p_dbf));


	if ( dbf_Close(p_dbf) == 0 ) {
		fprintf(stdout, "Datei geschlossen.\n");
	}

	return 0;
}
