#pragma once
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

typedef struct _dyn_arr_header dynamic_array_buffer_t;

#define dynamic_array(type)                                                    \
  struct {                                                                     \
    size_t size;                                                               \
    size_t allocated_length;                                                   \
    size_t length;                                                             \
    size_t item_size;                                                          \
    pthread_mutex_t mutex;                                                     \
    type *buffer;                                                              \
  }

#define dynamic_array_init(array_ptr)                                          \
  ({                                                                           \
    dynamic_array(typeof((*array_ptr)->buffer[0])) *__dynamic_array =          \
        malloc(sizeof(*__dynamic_array));                                      \
    __dynamic_array->item_size = sizeof(typeof((*array_ptr)->buffer[0]));      \
    pthread_mutex_init(&__dynamic_array->mutex, NULL);                         \
    __dynamic_array->allocated_length = 30;                                    \
    __dynamic_array->length = 0;                                               \
    __dynamic_array->size =                                                    \
        __dynamic_array->item_size * __dynamic_array->allocated_length;        \
    __dynamic_array->buffer = malloc(__dynamic_array->size);                   \
    memset(__dynamic_array->buffer, 0, __dynamic_array->size);                 \
    *array_ptr = (void *)__dynamic_array;                                      \
  })

#define new_dynamic_array(type)                                                \
  ({                                                                           \
    dynamic_array(type) *dynamic_array = malloc(sizeof(*dynamic_array));       \
    dynamic_array->item_size = sizeof(type);                                   \
    pthread_mutex_init(&dynamic_array->mutex, NULL);                           \
    dynamic_array->allocated_length = 30;                                      \
    dynamic_array->length = 0;                                                 \
    dynamic_array->size =                                                      \
        dynamic_array->item_size * dynamic_array->allocated_length;            \
    dynamic_array->buffer = malloc(dynamic_array->size);                       \
    memset(dynamic_array->buffer, 0, dynamic_array->size);                     \
    (void *)dynamic_array;                                                     \
  })

#define free_dynamic_array(array)                                              \
  ({                                                                           \
    free(array->buffer);                                                       \
    free(array);                                                               \
  })

#define __dynamic_array_resize_internal(array, new_allocated_length)           \
  ({                                                                           \
    array->buffer =                                                            \
        realloc(array->buffer, new_allocated_length * array->item_size);       \
    array->allocated_length = new_allocated_length;                            \
  })

#define dynamic_array_resize(array, new_length)                                \
  ({                                                                           \
    pthread_mutex_lock(&array->mutex);                                         \
    array->buffer = realloc(array->buffer, new_length * array->item_size);     \
    array->allocated_length = new_length;                                      \
    memset(&array->buffer[array->length], 0,                                   \
           (new_length - array->length) * array->item_size);                   \
    array->length = new_length;                                                \
    pthread_mutex_unlock(&array->mutex);                                       \
  })

#define dynamic_array_push(array, value)                                       \
  ({                                                                           \
    pthread_mutex_lock(&array->mutex);                                         \
    if (array->length == array->allocated_length)                              \
      __dynamic_array_resize_internal(array, array->length * 2);               \
    array->buffer[array->length] = value;                                      \
    array->length++;                                                           \
    pthread_mutex_unlock(&array->mutex);                                       \
  })

#define dynamic_array_pop(array, index)                                        \
  ({                                                                           \
    size_t _index = ((index) == (size_t) - 1) ? (array)->length - 1 : (index); \
    pthread_mutex_lock(&(array)->mutex);                                       \
    typeof((array)->buffer[0]) _result = (array)->buffer[_index];              \
    for (size_t i = _index; i < (array)->length - 1; ++i) {                    \
      (array)->buffer[i] = (array)->buffer[i + 1];                             \
    }                                                                          \
    (array)->length--;                                                         \
    pthread_mutex_unlock(&(array)->mutex);                                     \
    _result;                                                                   \
  })

#define dynamic_array_get(array, index)                                        \
  ({                                                                           \
    pthread_mutex_lock(&array->mutex);                                         \
    typeof((array)->buffer[0]) _result = array->buffer[index];                 \
    pthread_mutex_unlock(&array->mutex);                                       \
    _result;                                                                   \
  })

#define dynamic_array_for_each(array, value)                                   \
  typeof(array->buffer[0]) value;                                              \
  for (size_t __i = 0, __ph = pthread_mutex_lock(&(array)->mutex);             \
       __i < (array)->length && (value = (array)->buffer[__i], 1);             \
       __i++, (__i == (array)->length ? pthread_mutex_unlock(&(array)->mutex)  \
                                      : (void)0))

#define dynamic_array_for_each_index(array, value, idx)                        \
  typeof(array->buffer[0]) value;                                              \
  for (size_t __i = 0, __ph = pthread_mutex_lock(&(array)->mutex), idx = __i;  \
       __i < (array)->length && (idx = __i, 1) &&                              \
       (value = (array)->buffer[__i], 1);                                      \
       __i++, (__i == (array)->length ? pthread_mutex_unlock(&(array)->mutex)  \
                                      : (void)0))
