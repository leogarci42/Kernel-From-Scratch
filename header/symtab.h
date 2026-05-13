#ifndef SYMTAB_H
# define SYMTAB_H

#include <stdint.h>
#include <stddef.h>
#include "kernel.h"

typedef struct {
    uint_32t addr;
    const char *name;
} symbol_t;

extern symbol_t symtab[];
extern size_t symtab_size;

const char *find_symbol(uint_32t addr, uint_32t *offset);

#endif