#ifndef ARRAY_H
#define ARRAY_H

#include <stddef.h>

typedef enum et {
	et_int,
	et_uint,
	et_short,
	et_ushort,
	et_char,
	et_uchar,
	et_float,
	et_double
} entry_type;

typedef struct {
	size_t cols;
	size_t rows;
	void * ptr;
	size_t sizeof_entry;
	entry_type entry_type;
} array;

#endif /* ARRAY_H */