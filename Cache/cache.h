/*
 * cache.h
 *
 * Definition of the structure used to represent a cache.
 */
#ifndef CACHE_H
#define CACHE_H

#include <stdlib.h>
#include <inttypes.h>
#include <time.h>
#include <stdio.h>
#include <stdbool.h>

/*
 * Replacement policies. The MASK defines which bits are used to
 * represent policies. As we have three policies, we assign
 * them the values 0, 1 and 2.
 *
 * Therefore, you can check for a specific policy using:
 * if (policy & CACHE_REPLACEMENTPOLICY_MASK == CACHE_REPLACEMENTPOLICY_LRU) { ... }
 */
#define CACHE_REPLACEMENTPOLICY_MASK               0b00011100

#define CACHE_REPLACEMENTPOLICY_RANDOM             0b00000000
#define CACHE_REPLACEMENTPOLICY_LRU                0b00000100
#define CACHE_REPLACEMENTPOLICY_MRU                0b00001000
#define CACHE_REPLACEMENTPOLICY_RANDOMIZED_MARKING 0b00010000

/*
 * Write policies: We use two bits to indicate the write policy.
 * one bit represents either writethrough/writeback; the other
 * represents writeallocate/writenoallocate.
 */
#define CACHE_WRITEPOLICY_MASK   0b00000011

#define CACHE_WRITEPOLICY_WRITETHROUGH       0b00000000
#define CACHE_WRITEPOLICY_WRITEBACK          0b00000001

#define CACHE_WRITEPOLICY_WRITEALLOCATE      0b00000000
#define CACHE_WRITEPOLICY_WRITENOALLOCATE    0b00000010

/*
 * Other policies: Do we want to use cache tracing.
 */
#define CACHE_TRACE_MASK  0b00100000
#define CACHE_TRACEPOLICY 0b00100000

/*
 * Structure used to store a single cache line.
 */
typedef struct cache_line_s {

    /* The valid bit. */
    bool is_valid;

    /* The dirty bit. */
    bool is_dirty;

    /* The marked bit (for randomized marking) */
    bool is_marked;
    
    /* The tag. */
    uintptr_t tag;
  
    /* The cache block as bytes */
    uint8_t *block;
  
} cache_line_t;

/*
 * Structure used to store a cache set: a cache set contains a size
 * and a reference to the lines of the set.
 */
typedef struct cache_set_s {
    cache_line_t *lines;
    size_t first_index;
    size_t *lru_list;
    size_t num_marked;
} cache_set_t;

/*
 * Structure used to store a cache.
 */
typedef struct cache_s { 
    /* Number of sets in the cache. */
    size_t num_sets;
  
    /* Number of lines in cache. */
    size_t num_lines;
  
    /* Number of bytes in a line. */
    size_t line_size;
  
    /* Associativity of the cache */
    size_t associativity;

    /* Mask for block offset. */
    uintptr_t block_offset_mask;
  
    /* Mask for cache index. */
    uintptr_t cache_index_mask;
  
    /* Shift for cache index. */
    unsigned int cache_index_shift;
  
    /* Mask for tag. */
    uintptr_t tag_mask;
  
    /* Shift for tag. */
    uint8_t tag_shift;
  
    /* Replacement and write policies. */
    uint8_t policies;
  
    /* All the memory in the cache */
    uint8_t *memory;

    /* Array of lines, each of which is an array of bytes. */
    cache_line_t *lines;
  
    /* Array of sets, each of which refers to its lines */
    cache_set_t *sets;
  
    /* Statistics about cache usage. */
    uint32_t access_count, miss_count;
} cache_t;

typedef int (*func_t)(void);

/* Public functions */

/*
 * Create a new cache that contains a total of num_bytes line, each of which is block_size
 * bytes long, with the given associativity and policies.
 */
cache_t *cache_new(size_t num_bytes, size_t block_size, size_t associativity, uint8_t policies);

/*
 * Frees all memory allocated for the given cache.
 */
void cache_free(cache_t *cache);

/*
 * Read a single long integer from the cache.
 */
uint64_t cache_read(cache_t *cache, uintptr_t address, func_t generate_random_number);

/*
 * Write a single long integer to memory and/or the cache.
 */
void cache_write(cache_t *cache, uintptr_t address, uint64_t value, func_t generate_random_number);

/*
 * Return the number of cache misses since the cache was created.
 */
uint32_t cache_miss_count(cache_t *cache);

/*
 * Return the number of cache accesses since the cache was created.
 */
uint32_t cache_access_count(cache_t *cache);

/*
 *  Helpers
 */
bool cache_line_check_validity_and_tag(cache_line_t *cache_line, uintptr_t tag);
uint64_t cache_line_retrieve_data(cache_line_t *cache_line, size_t offset);
cache_line_t *cache_set_find_matching_line(cache_t *cache, cache_set_t *cache_set, uintptr_t tag);
size_t choose_unmarked_cache_line(cache_t *cache, cache_set_t *cache_set, func_t generate_random_number);
cache_line_t *find_available_cache_line(cache_t *cache, cache_set_t *cache_set, func_t generate_random_number);

#endif
