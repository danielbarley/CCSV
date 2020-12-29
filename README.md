# CCSV
A basic C header library for reporting data in csv format.

## Usage
CCSV is a header only library that allows arrays to be printed as columns
separated by commas. Note that printing individual rows is not supported
(mostly because something like `printf("%f, %f, %f", val1, val2, val3)` 
will get you there a lot faster).
Just add `ccsv.h` to your projects include path and you're ready to go.
For a small example programm see `test/test.c` compile with `gcc -o test test.c`.

CCSV defines three main functions:
* create a column object from an array
```
ccsv_column *ccsv_column_create(void *data, size_t len, ccsv_type type) 
```
This creates a column object that can be printed from an array.
Just provide a pointer to your data, the number of elements and the type.
Supported types are:
```
CCSV_INT
CCSV_UINT
CCSV_LONG
CCSV_ULONG
CCSV_LONGLONG
CCSV_ULONGLONG
CCSV_FLOAT
CCSV_DOUBLE
CCSV_LONGDOUBLE
```
alternatively you can of course just use a statically allocated `ccsv_column`

* delete a column object
    ```
void ccsv_column_delete(ccsv_column *col)
```
simply frees the column object after use

* print columns
```
int ccsv_print_columns(FILE *fd, int num_cols, ...)
```
This prints an arbitrary number of columns.
Note: this takes *pointers* to columns as its variadic argument
