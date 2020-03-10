/*
 * CS 261 Data Structures
 * Assignment 5
 * Name: Andrew Helmsworth
 * Date: 2020/02/18
 */

#include "hashMap.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

int hashFunction1(const char* key)
{
    int r = 0;
    for (int i = 0; key[i] != '\0'; i++)
    {
        r += key[i];
    }
    return r;
}

int hashFunction2(const char* key)
{
    int r = 0;
    for (int i = 0; key[i] != '\0'; i++)
    {
        r += (i + 1) * key[i];
    }
    return r;
}

/**
 * Creates a new hash table link with a copy of the key string.
 * @param key Key string to copy in the link.
 * @param value Value to set in the link.
 * @param next Pointer to set as the link's next.
 * @return Hash table link allocated on the heap.
 */
HashLink* hashLinkNew(const char* key, int value, HashLink* next)
{
    HashLink* link = malloc(sizeof(HashLink));
    assert(link != 0);
    link->key = malloc(sizeof(char) * (strlen(key) + 1));
    assert(link->key != 0);
    strcpy(link->key, key);
    link->value = value;
    link->next = next;
    return link;
}

/**
 * Free the allocated memory for a hash table link created with hashLinkNew.
 * @param link
 */
static void hashLinkDelete(HashLink* link)
{
    free(link->key);
    free(link);
}

/**
 * Initializes a hash table map, allocating memory for a link-pointer table with
 * the given number of buckets.
 * @param map
 * @param capacity The number of table buckets.
 */
void hashMapInit(HashMap* map, int capacity)
{
    map->capacity = capacity;
    map->size = 0;
    map->table = malloc(sizeof(HashLink*) * capacity);
    assert(map->table != 0);
    for (int i = 0; i < capacity; i++)
    {
        map->table[i] = 0;
    }
}

/**
 * Removes all links in the map and frees all allocated memory. You can use
 * hashLinkDelete to free the links.
 * @param map
 */
void hashMapCleanUp(HashMap* map)
{
    //Variable declarations
    int i;
    HashLink* currentLink;
    HashLink* tempLink;
    // FIXED: implement

    //Check map not null
    assert(map != 0);

    //For each link in the bucket
    for (i = 0; i < map->capacity; i++) {
        //If there is data in the bucket
        if (map->table[i] != 0) {
            //Get the top link
            currentLink = map->table[i];
            //While the bucket is still not empty
            while (map->table[i] != 0) {
                //Set temp to current
                tempLink = currentLink;
                //Walk current forward
                currentLink = currentLink->next;
                //Free the old link
                hashLinkDelete(tempLink);
            }
        }
    }

}

/**
 * Creates a hash table map, allocating memory for a link pointer table with
 * the given number of buckets.
 * @param capacity The number of buckets.
 * @return The allocated map.
 */
HashMap* hashMapNew(int capacity)
{
    HashMap* map = malloc(sizeof(HashMap));
    hashMapInit(map, capacity);
    return map;
}

/**
 * Removes all links in the map and frees all allocated memory, including the
 * map itself.
 * @param map
 */
void hashMapDelete(HashMap* map)
{
    //Check map not null
    assert(map != 0);
    hashMapCleanUp(map);
    free(map);
}

/**
 * Returns a pointer to the value of the link with the given key  and skip traversing as well. Returns NULL
 * if no link with that key is in the table.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket. Also make sure to search the entire list.
 * 
 * @param map
 * @param key
 * @return Link value or NULL if no matching link.
 */
int* hashMapGet(HashMap* map, const char* key)
{
    //Variable declarations
    int hashedValue;
    int bucketIndex;
    HashLink* currentLink;

    // FIXED: implement start here
    //Check map not null
    assert(map != 0);

    //Get the bucket from the hashed value and the cap
    hashedValue = HASH_FUNCTION(key);
    bucketIndex = hashedValue % map->capacity;

    currentLink = map->table[bucketIndex];

    //If the top link isn't null
    if (currentLink != NULL) {

        //While current link is still not null
        while (currentLink != NULL) {

            //If current matches the passed in key  
            if (strcmp(currentLink->key, key) == 0) {
                return &(currentLink->value);
            }
            else {
                //Advance current link
                currentLink = currentLink->next;
            }
        }
    }

    //Not found
    return NULL;
}

/**
 * Resizes the hash table to have a number of buckets equal to the given 
 * capacity (double of the old capacity). After allocating the new table, 
 * all of the links need to rehashed into it because the capacity has changed.
 * 
 * Remember to free the old table and any old links if you use hashMapPut to
 * rehash them.
 * 
 * @param map
 * @param capacity The new number of buckets.
 */
void resizeTable(HashMap* map, int capacity)
{
    //Variable declarations
    HashLink* currentLink;
    HashLink* nextLink;
    HashLink* newLink;
    HashMap* newMap;
    HashMap* tempMapPointer;
    int i;
    int bucketIndex;
    int newHashValue;

    // FIXED: implement
    //Check map not null
    assert(map != 0);

    //Allocate memory for new table, check that worked
    newMap = hashMapNew(capacity);
    assert(newMap != 0);

    //For each bucket in the old array
    for (i = 0; i < map->capacity; i++) {

        //Set current to the top link in bucket
        currentLink = map->table[i];

        //If the bucket is not empty
        if (currentLink != 0) {

            //While current is still not set to an empty link
            while (currentLink != 0) {

                //Hash the new key, set bucket index to mod of cap
                newHashValue = HASH_FUNCTION(currentLink->key);
                bucketIndex = newHashValue % capacity;

                //Initialize nextLink to 0
                nextLink = 0;

                //If there is a next link, set it
                if (currentLink->next != 0) {
                    nextLink = currentLink->next;
                }

                //Build the new link, check that worked
                 newLink = hashLinkNew(currentLink->key, currentLink->value, nextLink);
                 assert(newLink != 0);

                 //Assign link value to new bucket
                 newMap->table[bucketIndex] = newLink;

                 //Advance the current pointer
                 currentLink = currentLink->next;
            }
        }
    }

    //Set the old map to a temp pointer, update map to newMap
    tempMapPointer = map;
    map = newMap;

    //Free the old map
    free(tempMapPointer);
}

/**
 * Updates the given key-value pair in the hash table. If a link with the given
 * key already exists, this will just update the value and skip traversing. Otherwise, it will
 * create a new link with the given key and value and add it to the table
 * bucket's linked list. You can use hashLinkNew to create the link.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket.
 * 
 * @param map
 * @param key
 * @param value
 */
void hashMapPut(HashMap* map, const char* key, int value)
{
    //Variable declarations
    struct HashLink* newLink;
    struct HashLink* currentLink;
    struct HashLink* nextLink;
    int i;
    int newHashValue;
    int bucketIndex;
    int keepLooping;

    // FIXED: implement
    //Check map, key not null
    assert(map != 0);
    assert(key != NULL);

    //Init keepLooping to 1, this allows early traversal exit
    keepLooping = 1;

    //For each bucket in the old array
    for (i = 0; i < map->capacity; i++) {

        //Set current to the top link in bucket
        currentLink = map->table[i];

        //If the bucket is not empty
        if (currentLink != 0) {

            //While current is still not set to an empty link
            while ((currentLink != 0) && (keepLooping == 1)) {

                //If key found, update value
                if (strcmp(currentLink->key, key) == 0) {
                    currentLink->value = value;
                    keepLooping = 0;
                    //Skip further traversal
                    i = map->capacity + 5;
                }
                else {
                    //Advance the current pointer
                    currentLink = currentLink->next;
                }
            }
        }
        else if ((currentLink == 0) && (keepLooping == 1)) {
            //The list has been traversed and no matching link was found
            //Hash the new key, set bucket index to mod of cap
            newHashValue = HASH_FUNCTION(key);
            bucketIndex = newHashValue % map->capacity;

            //Init nextLink
            nextLink = 0;

            //Get head node of bucket list if there is one
            if (map->table[bucketIndex] != 0) {
                nextLink = map->table[bucketIndex];
            }

            //Make the new link
            newLink = hashLinkNew(key, value, nextLink);

            map->table[bucketIndex] = newLink;

            //Update size
            map->size++;

            //Skip further traversal
            i = map->capacity + 5;
        }
    }


}

/**
 * Removes and frees the link with the given key from the table. If no such link
 * exists, this does nothing. Remember to search the entire linked list at the
 * bucket. You can use hashLinkDelete to free the link.
 * @param map
 * @param key
 */
void hashMapRemove(HashMap* map, const char* key)
{
    //Variable declarations
    //HashLink* linkToRemove;
    HashLink* currentLink;
    HashLink* previousLink;
    int hashedValue;
    int bucketIndex;

    // FIXME: implement
    assert(map != NULL);

    //Get the bucket from the hashed value and the cap
    hashedValue = HASH_FUNCTION(key);
    bucketIndex = hashedValue % map->capacity;

    currentLink = map->table[bucketIndex];
    previousLink = currentLink;

    //If the top link isn't null
    if (currentLink != NULL) {

        //While current link is still not null
        while (currentLink != NULL) {

            //If current matches the passed in key  
            if (strcmp(currentLink->key, key) == 0) {
                
                if (previousLink != currentLink) {
                    //Splice out
                    previousLink->next = currentLink->next;

                    //Delete the link
                    hashLinkDelete(currentLink);
                    currentLink = NULL;
                    //Decrement size
                    map->size--;
                }
                else {
                    //Last link in list, set bucket to null
                    map->table[bucketIndex] = 0;
                    //Delete the link
                    hashLinkDelete(currentLink);
                    currentLink = NULL;
                    //Decrement size
                    map->size--;
                }

            }
            else {
                //Advance current link
                previousLink = currentLink;
                currentLink = currentLink->next;
            }
            
        }
    }
}

/**
 * Returns 1 if a link with the given key is in the table and 0 otherwise.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket. Also make sure to search the entire list.
 * 
 * @param map
 * @param key
 * @return 1 if the key is found, 0 otherwise.
 */
int hashMapContainsKey(HashMap* map, const char* key)
{
    //Check map not null
    assert(map != 0);

    //If get returns something
    if (hashMapGet(map, key) != NULL) {
        //Found the key
        return 1;
    }
    // FIXED: implement
    //Didn't find the key
    return 0;
}

/**
 * Returns the number of links in the table.
 * @param map
 * @return Number of links in the table.
 */
int hashMapSize(HashMap* map)
{
    //Variable declarations
    int mapSize;

    //Check map not null
    assert(map != 0);

    //Get size
    mapSize = map->size;

    // FIXED: implement
    //Return the size
    return mapSize;
}

/**
 * Returns the number of buckets in the table.
 * @param map
 * @return Number of buckets in the table.
 */
int hashMapCapacity(HashMap* map)
{
    //Variable declarations
    int currentCap;

    //Check map not null
    assert(map != 0);

    //Get capacity
    currentCap = map->capacity;

    // FIXED: implement
    //Return capacity
    return currentCap;
}

/**
 * Returns the number of table buckets without any links.
 * @param map
 * @return Number of empty buckets.
 */
int hashMapEmptyBuckets(HashMap* map)
{
    //Variable declarations
    int emptyCounter;
    int i;

    //Check map not null
    assert(map != 0);

    //Init counter
    emptyCounter = 0;

    //Loop over each bucket
    for (i = 0; i < map->capacity; i++) {

        //If the bucket is empty
        if (map->table[i] == NULL) {
            //Increment emptyCounter
            emptyCounter++;
        }
    }

    // FIXED: implement
    return emptyCounter;
}

/**
 * Returns the ratio of (number of links) / (number of buckets) in the table.
 * Remember that the buckets are linked lists, so this ratio tells you nothing
 * about the number of empty buckets. Remember also that the load is a floating
 * point number, so don't do integer division.
 * @param map
 * @return Table load.
 */
float hashMapTableLoad(HashMap* map)
{
    //Variable declarations
    float numLinksFloat;
    float numBucketsFloat;
    float ratio;

    //Get float of links and buckets
    numLinksFloat = (float)map->size;
    numBucketsFloat = (float)map->capacity;

    //Get the ratio
    ratio = numLinksFloat / numBucketsFloat;

    // FIXED: implement
    return ratio;
}

/**
 * Prints all the links in each of the buckets in the table.
 * @param map
 */
void hashMapPrint(HashMap* map)
{
    //Variable declarations
    HashLink* currentLink;
    int i;

    //Check map not null
    assert(map != 0);

    //For each bucket
    for (i = 0; i < map->capacity; i++) {
        
        //Print bucket index
        printf("%d: ", i);
        
        //Get head link
        currentLink = map->table[i];

        //if head link is not null
        if (currentLink != 0) {
            //While it is still set to a value that is not null
            while (currentLink != 0) {
                //Print a tuple of the key-value pair
                printf("(key: %s, val: %d) ", currentLink->key, currentLink->value);
                //Advance current counter
                currentLink = currentLink->next;
            }
        }
        //Newline to prep for next bucket
        printf("\n\n");
    }

  // FIXED: implement

}
