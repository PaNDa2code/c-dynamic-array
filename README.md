## Title: Generic C Dynamic Array with Mutex for Thread Safety**

### Description:

This repository provides a C implementation of a generic dynamic array with built-in thread safety using mutexes. The dynamic array is designed to handle various data types, allowing for flexible and concurrent use in multithreaded applications.

### Features:

- **Generic Type Support:** Implemented using macros to handle different data types in a type-safe manner.
- **Dynamic Resizing:** The array automatically grows as needed, accommodating more elements efficiently.
- **Thread Safety:** Utilizes mutexes to ensure safe concurrent access and modifications.
- **Easy Integration:** Provides simple functions for creating, modifying, and managing dynamic arrays.

**Usage Example:**

```c 
#include "dynamic_array.h"
#include <stdio.h>
#include <stdlib.h>


#define NUM_THREADS 4

void *thread_function(void *arg) {
  dynamic_array(int) *array = arg;
  for (int i = 0; i < 10; ++i) {
    dynamic_array_push(array, i);
  }
  return NULL;
}

int main() {
  dynamic_array(int) * array;
  dynamic_array_init(&array);

  pthread_t thread_bool[NUM_THREADS];
  for (int i = 0; i < NUM_THREADS; ++i) {
    pthread_create(&thread_bool[i], NULL, thread_function, array);
  }

  for (int i = 0; i < NUM_THREADS; ++i) {
    pthread_join(thread_bool[i], NULL);
  }

  dynamic_array_for_each_index(array, value, index) {
    printf("array[%zu] = %d\n", index, value);
  }

  // remove and return the element by index in the array
  int first_element = dynamic_array_pop(array, 0);

  // use -1 as index to pop the last element
  int last_element = dynamic_array_pop(array, -1);

  printf("Last element of the array is: %d\n", last_element);

  free_dynamic_array(array);
  return 0;
}
```

