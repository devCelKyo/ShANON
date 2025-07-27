#pragma once

#include <stddef.h>

typedef struct DynamicArray
{
   size_t length;
   size_t capacity;
   void** elements;
} DynamicArray;

DynamicArray dynarray();
void* dynarray_get(DynamicArray*, size_t);
void dynarray_push(DynamicArray*, void*);