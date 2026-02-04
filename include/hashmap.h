#ifndef POMOINTER_HASHMAP_H
#define POMOINTER_HASHMAP_H

// Key-value structure
typedef struct Entry {
  char* key;
  void* value;
  struct Entry* next;
} Entry;

// Main HashMap structure
typedef struct HashMap {
  Entry** buckets;        // Buckets array(linked list)
  int capacity;           // Buckets array size 
  int size;               // Actual number of elements
  float load_factor;      // Load factor for resizing
} HashMap;

// Iterator structure
typedef struct {
  HashMap* map;
  int bucket_index;
  Entry* current_entry;
} HashMapIterator;


HashMap* hashmap_create(int initial_capacity, float load_factor);
void hashmap_resize(HashMap* map);
void hashmap_put(HashMap* map, const char* key, void* value);
void* hashmap_get(HashMap* map, const char* key);
int hashmap_remove(HashMap* map, const char* key, void (*free_value)(void*));
int hashmap_contains(HashMap* map, const char* key);
int hashmap_size(HashMap* map);
void hashmap_destroy(HashMap* map, void (*free_value)(void*));
int hashmap_merge_inplace(HashMap* dest, HashMap* src);


void hashmap_foreach(HashMap* map, void (*callback)(const char*, void*, void*), void* user_data);
int hashmap_iterator_next(HashMapIterator* iter);
HashMapIterator* hashmap_iterator_create(HashMap* map);
const char* hashmap_iterator_key(HashMapIterator* iter);
void* hashmap_iterator_value(HashMapIterator* iter);
void hashmap_iterator_destroy(HashMapIterator* iter);

#endif
