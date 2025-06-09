#include "array_list.h"
#include <string.h>

ArrayList *array_list_create(const size_t element_size) {
    ArrayList *list = malloc(sizeof(ArrayList));
    if (!list) return NULL;
    list->data = malloc(sizeof(void *) * 2);
    if (!list->data) {
        free(list);
        return NULL;
    }
    list->size = 0;
    list->capacity = 2;
    list->element_size = element_size;
    return list;
}

static int array_list_resize(ArrayList *list) {
    const size_t new_capacity = list->capacity * 2;
    void **new_data = realloc(list->data, sizeof(void *) * new_capacity);
    if (!new_data) return 0;

    list->data = new_data;
    list->capacity = new_capacity;
    return 1;
}

int array_list_add(ArrayList *list, void *element) {
    if (!list || !element) return 0;

    if (list->size >= list->capacity) {
        if (!array_list_resize(list)) return 0;
    }

    void *elem_copy = malloc(list->element_size);
    if (!elem_copy) return 0;

    memcpy(elem_copy, element, list->element_size);
    list->data[list->size++] = elem_copy;
    return 1;
}

void *array_list_get(ArrayList *list, const size_t index) {
    if (!list || index >= list->size) return NULL;
    return list->data[index];
}

int array_list_remove(ArrayList *list, const size_t index) {
    if (!list || index >= list->size) return 0;

    free(list->data[index]);

    for (size_t i = index; i < list->size - 1; i++) {
        list->data[i] = list->data[i + 1];
    }

    list->size--;
    return 1;
}

size_t array_list_size(const ArrayList *list) {
    return list ? list->size : 0;
}

void array_list_free(ArrayList *list) {
    if (!list) return;

    for (size_t i = 0; i < list->size; i++) {
        free(list->data[i]);
    }
    free(list->data);
    free(list);
}
