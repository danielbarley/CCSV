#include <stdio.h>

#include "../ccsv.h"

int main(int argc, char *argv[])
{
	int a[] = {1, 2, 3, 4, 5};
	double b[] = {6.24, 7.44, 8.92, 9.19, 10.45};

	ccsv_column *col_a = ccsv_column_create(a, 5, CCSV_INT);
	ccsv_column *col_b = ccsv_column_create(b, 5, CCSV_DOUBLE);

	ccsv_print_columns(stdout, 2, col_a, col_b);

	ccsv_column_delete(col_a);
	ccsv_column_delete(col_b);
	return 0;
}
