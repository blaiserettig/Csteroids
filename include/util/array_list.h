#ifndef ARRAY_LIST_H
#define ARRAY_LIST_H

#include <stdlib.h>

typedef struct {
    void **data;
    size_t size;
    size_t capacity;
    size_t element_size;
} ArrayList;

ArrayList *array_list_create(size_t element_size);

int array_list_add(ArrayList *list, void *element);

void array_list_free(ArrayList *list);

void *array_list_get(ArrayList *list, size_t index);

size_t array_list_size(const ArrayList *list);

int array_list_remove(ArrayList *list, size_t index);

#endif //ARRAY_LIST_H
