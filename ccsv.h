#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum ccsv_type {
	CCSV_INT,
	CCSV_UINT,
	CCSV_LONG,
	CCSV_ULONG,
	CCSV_LONGLONG,
	CCSV_ULONGLONG,
	CCSV_FLOAT,
	CCSV_DOUBLE,
	CCSV_LONGDOUBLE
} ccsv_type;

typedef enum ccsv_ret {
	CCSV_SUCCESS = 0,
	CCSV_MALLOC_FAILED = -1,
	CCSV_COLS_SIZE_NOT_EQUAL = -2,
	CCSV_IO_ERROR = -3,
	CCSV_UNKNOWN_TYPE = -4
} ccsv_ret;

typedef struct ccsv_column {
	void *data;
	size_t len;
	ccsv_type type;
} ccsv_column;

typedef void (*ccsv_print_fn)(FILE *, void *, size_t);

ccsv_column *ccsv_column_create(void *data, size_t len, ccsv_type type) {
	/**
	 * @brief creates and initializes a column struct, returns NULL on
	 * failure
	 * @param[in] data pointer to numeric data
	 * @param[in] len number of elements in data
	 * @param[in] type type of data (see ccsv_types enum)
	 * @param[out] initialized ccsv_column struct (NULL on failure)
	 */

	ccsv_column *col = malloc(sizeof(ccsv_column));
	if (col == NULL) return NULL;
	*col = (struct ccsv_column){.data = data, .len = len, .type = type};
	return col;
}

void ccsv_column_delete(ccsv_column *col) {
	/**
	 * @brief frees column structure
	 * @param[in] col column to be freed
	 */
	free(col);
}

static inline void ccsv_print_int(FILE *fd, void *val, size_t idx) {
	int *d = (int *)val;
	fprintf(fd, "%d", d[idx]);
}

static inline void ccsv_print_uint(FILE *fd, void *val, size_t idx) {
	unsigned *d = (unsigned *)val;
	fprintf(fd, "%u", d[idx]);
}

static inline void ccsv_print_long(FILE *fd, void *val, size_t idx) {
	long *d = (long *)val;
	fprintf(fd, "%ld", d[idx]);
}

static inline void ccsv_print_ulong(FILE *fd, void *val, size_t idx) {
	unsigned long *d = (unsigned long *)val;
	fprintf(fd, "%lu", d[idx]);
}

static inline void ccsv_print_longlong(FILE *fd, void *val, size_t idx) {
	long long *d = (long long *)val;
	fprintf(fd, "%lld", d[idx]);
}

static inline void ccsv_print_ulonglong(FILE *fd, void *val, size_t idx) {
	unsigned long long *d = (unsigned long long *)val;
	fprintf(fd, "%llu", d[idx]);
}

static inline void ccsv_print_float(FILE *fd, void *val, size_t idx) {
	float *d = (float *)val;
	fprintf(fd, "%lf", d[idx]);
}

static inline void ccsv_print_double(FILE *fd, void *val, size_t idx) {
	double *d = (double *)val;
	fprintf(fd, "%lf", d[idx]);
}

static inline void ccsv_print_longdouble(FILE *fd, void *val, size_t idx) {
	long double *d = (long double *)val;
	fprintf(fd, "%Lf", d[idx]);
}

int ccsv_print_columns(FILE *fd, int num_cols, ...) {
	/**
	 * @brief prints an arbitrary amount of columns sparated with commas.
	 * length of the columns has to be equal
	 * @param[in] fd file descriptor four output
	 * @param[in] num_cols number of columns passed
	 * @param[in] ... pointers to column data to be printed
	 */
	// pointers to columns
	ccsv_column **cols = malloc(sizeof(ccsv_column *) * num_cols);
	// function pointer table stores the print functions for the different
	// columns
	ccsv_print_fn *print_fns = malloc(sizeof(ccsv_print_fn) * num_cols);

	if (cols == NULL || print_fns == NULL) {
		// at least one malloc failed -> free both to be sure
		free(cols);
		free(print_fns);
		return CCSV_MALLOC_FAILED;
	}

	va_list args;
	va_start(args, num_cols);
	for (int i = 0; i < num_cols; ++i) {
		cols[i] = va_arg(args, ccsv_column *);
		// get corresponding print function depending on the type of the
		// column, fallthrough case returns with unknown type error
		switch (cols[i]->type) {
			case CCSV_INT:
				print_fns[i] = &ccsv_print_int;
				break;
			case CCSV_UINT:
				print_fns[i] = &ccsv_print_uint;
				break;
			case CCSV_LONG:
				print_fns[i] = &ccsv_print_long;
				break;
			case CCSV_ULONG:
				print_fns[i] = &ccsv_print_ulong;
			case CCSV_LONGLONG:
				print_fns[i] = &ccsv_print_longlong;
				break;
			case CCSV_ULONGLONG:
				print_fns[i] = &ccsv_print_ulonglong;
				break;
			case CCSV_FLOAT:
				print_fns[i] = &ccsv_print_float;
				break;
			case CCSV_DOUBLE:
				print_fns[i] = &ccsv_print_double;
				break;
			case CCSV_LONGDOUBLE:
				print_fns[i] = &ccsv_print_longdouble;
				break;
			default:
				return CCSV_UNKNOWN_TYPE;
		}
	}

	if (num_cols > 1) {
		for (int i = 1; i < num_cols; ++i) {
			if (cols[i]->len != cols[i-1]->len) {
				return CCSV_COLS_SIZE_NOT_EQUAL;
			}
		}
	}

	bool first = true;  // don't print a "," for the first value in row
	for (size_t i = 0; i < cols[0]->len; ++i) {
		for (int j = 0; j < num_cols; ++j) {
			if (!first) fputs(", ", fd);
			print_fns[j](fd, cols[j]->data, i);
			first = false;
		}
		fputs("\n", fd);
		first = true;
	}
	free(cols);
	free(print_fns);
	return CCSV_SUCCESS;
}
