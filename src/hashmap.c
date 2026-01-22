#define _POSIX_C_SOURCE 200809L // For strdup
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "hashmap.h"

// ------------------- AUXILIARY FUNCTIONS -------------------------------

// Hash function(djb2 algorithm)
static unsigned long hash(const char* str, int capacity) {
  unsigned long hash = 5381;
  int c;

  while ((c = *str++)) {
    hash = ((hash << 5) + hash) + c; // hash * 33 + c
  }

  return hash % capacity;
}

// Function to create entry
static Entry* create_entry(const char* key, void* value) {
  Entry* entry = (Entry*)malloc(sizeof(Entry));
  if (!entry) return NULL;

  entry->key = strdup(key);
  entry->value = value;
  entry->next = NULL;

  return entry;
}

// Function to free an entry
static void free_entry(Entry* entry, void (*free_value)(void*)) {
  if (!entry) return;

  free(entry->key);
  if (free_value && entry->value) {
    free_value(entry->value);
  }

  free(entry);
}

// --------------------- BASIC OPERATIONS ---------------------------

// Create a new HashMap
HashMap* hashmap_create(int initial_capacity, float load_factor) {
  HashMap* map = (HashMap*)malloc(sizeof(HashMap));
  if (!map) return NULL;

  map->capacity = initial_capacity > 0 ? initial_capacity : 16;
  map->size = 0;
  map->load_factor = (load_factor > 0.1 && load_factor < 1.0) ? load_factor : 0.75;

  map->buckets = (Entry**)calloc(map->capacity, sizeof(Entry*));
  if (!map->buckets) {
    free(map);
    return NULL;
  }

  return map;
}

// Resize hashmap
void hashmap_resize(HashMap* map) {
  int old_capacity = map->capacity;
  Entry** old_buckets = map->buckets;

  // Double its capacity
  map->capacity *= 2;
  map->buckets = (Entry**)calloc(map->capacity, sizeof(Entry*));
  map->size = 0; // Will be recalculated during rehashing

  // Reinsert old elements
  for (int i = 0; i < old_capacity; i++) {
    Entry* entry = old_buckets[i];
    while (entry) {
      Entry* next = entry->next;

      // Reinsert in the new bucket
      unsigned long index = hash(entry->key, map->capacity);
      entry->next = map->buckets[index];
      map->buckets[index] = entry;
      map->size++;

      entry = next;
    }
  }

  free(old_buckets);
}


// Insert or update an element
void hashmap_put(HashMap* map, const char* key, void* value) {
  if (!map || !key ) return;

  // Verify if it needs to resize
  if ((float)map->size / map->capacity >= map->load_factor) {
    hashmap_resize(map);
  }

  unsigned long index = hash(key, map->capacity);
  Entry* entry = map->buckets[index];

  while (entry) {
    if (strcmp(entry->key, key) == 0) {
      // Update its value
      entry->value = value;
      return ;
    }
    entry = entry->next;
  }

  // Create new entry(at the start of the list)
  Entry* new_entry = create_entry(key, value); 
  if (!new_entry) return;

  new_entry->next = map->buckets[index];
  map->buckets[index] = new_entry;
  map->size++;
}

// Obtains an element by the key
void* hashmap_get(HashMap* map, const char* key) {
  if (!map || !key) return NULL;

  unsigned long index = hash(key, map->capacity);
  Entry* entry = map->buckets[index];

  while (entry) {
    if (strcmp(entry->key, key) == 0) {
      return entry->value;
    }
    entry = entry->next;
  }

  return NULL; // Not found
}

// Removes an element
int hashmap_remove(HashMap* map, const char* key, void (*free_value)(void*)) {
  if (!map || !key) return 0;

  unsigned long index = hash(key, map->capacity);
  Entry* entry = map->buckets[index];
  Entry* prev = NULL;

  while (entry) {
    if (strcmp(entry->key, key) == 0) {
      if (prev) {
        prev->next = entry->next;
      } else {
        map->buckets[index] = entry->next;
      }

      free_entry(entry, free_value);
      map->size--;
      return 1; // Removed
    }
    prev = entry;
    entry = entry->next;
  }

  return 0; // Key not found
}

// Checks if the key exists
int hashmap_contains(HashMap* map, const char* key) {
  return hashmap_get(map, key) != NULL;
}

// Returns number of elements
int hashmap_size(HashMap* map) {
  return map ? map->size : 0;
}

// Destroy HashMap
void hashmap_destroy(HashMap* map, void (*free_value)(void*)) {
  if (!map) return;

  for (int i = 0; i < map->capacity; i++) {
    Entry* entry = map->buckets[i];
    while (entry) {
      Entry* next = entry->next;
      free_entry(entry, free_value);
      entry = next;
    }
  }

  free(map->buckets);
  free(map);
}

// -------------------------- ITERATION FUNCTIONS -----------------------------

void hashmap_foreach(HashMap* map, void (*callback)(const char*, void*, void*), void* user_data) {
  if (!map || !callback) return ;

  for (int i = 0; i < map->capacity; i++) {
    Entry* entry = map->buckets[i];
    while (entry) {
      callback(entry->key, entry->value, user_data);
      entry = entry->next;
    }
  }
}


// Advances the iterator 
int hashmap_iterator_next(HashMapIterator* iter) {
  if (!iter || !iter->map) return 0;

  if (iter->current_entry) {
    iter->current_entry = iter->current_entry->next;
  }

  // If the current list is empty, look for the next non-empty bucket.
  while ( (!iter->current_entry) && (iter->bucket_index < iter->map->capacity) ) {
    iter->current_entry = iter->map->buckets[iter->bucket_index];
    iter->bucket_index++;
  } 

  return iter->current_entry != NULL;

}

// Creates an iterator
HashMapIterator* hashmap_iterator_create(HashMap* map) {
  if (!map) return NULL; 

  HashMapIterator* iter = (HashMapIterator*)malloc(sizeof(HashMapIterator));
  if (!iter) return NULL;

  iter->map = map;
  iter->bucket_index = 0;
  iter->current_entry = NULL;

  // Finds first element
  hashmap_iterator_next(iter);

  return iter;
}

// Get current key
const char* hashmap_iterator_key(HashMapIterator* iter) {
  if (!iter || !iter->current_entry) return NULL;
  return iter->current_entry->key;
}

// Get current value
void* hashmap_iterator_value(HashMapIterator* iter) {
  if (!iter || !iter->current_entry) return NULL;
  return iter->current_entry->value;
}

// Free iterator
void hashmap_iterator_destroy(HashMapIterator* iter) {
  if (iter) free(iter);
}
