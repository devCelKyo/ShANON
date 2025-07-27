#include <dynamic_array.h>
#include <stdlib.h>
#include <string.h>

DynamicArray dynarray()
{
   DynamicArray a = { 0, 30, NULL };
   a.elements = malloc(a.capacity * sizeof(void*));
   memset(a.elements, 0, a.capacity);
   return a;
}

void* dynarray_get(DynamicArray* array, size_t index)
{
   if (index < array->length)
   {
      return array->elements[index];
   }
   return NULL;
}

// internal
void resize(DynamicArray* array, size_t size)
{
   void** resized = malloc(size * sizeof(void*));
   memset(resized, 0, size);
   for (size_t index = 0; index < array->length; ++index)
   {
      resized[index] = array->elements[index];
   }
   free(array->elements);
   array->elements = resized;
   array->capacity = size;
}

void dynarray_push(DynamicArray* array, void* element)
{
   if (array->length == array->capacity)
   {
      resize(array, array->capacity * 2);
   }
   array->elements[array->length] = element;
   ++array->length;
}