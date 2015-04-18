#include "HashMap.h"

#include <stdlib.h>
#include <string.h>

///
//Allocates memory for a new HashMap
//
//Returns:
//	Pointer to newly allocated hashmap
HashMap* HashMap_Allocate(void)
{
	HashMap* map = (HashMap*)malloc(sizeof(HashMap));
	return map;
}

///
//Initializes a HashMap
//
//Parameters:
//	map: Hashmap to initialize
void HashMap_Initialize(HashMap* map, unsigned int capacity)
{
	map->capacity = capacity;
	map->size = 0;
	map->data = (HashMap_KeyValuePair**)calloc(capacity, sizeof(struct HashMap_KeyValuePair*));
	map->Hash = HashMap_SDBM;
}

///
//Frees a hashmap
//Does not free data!! Delete prior to this! OR have other references!!!
//
//Parameters:
//	map: The Hashmap to free
void HashMap_Free(HashMap* map)
{
	for (unsigned int i = 0; i < map->size; i++)
	{
		if (map->data[i] != NULL)
		{
			HashMap_KeyValuePair_Free(map->data[i]);
		}

	}

	free(map->data);
	free(map);
}

///
//Adds data to a hashmap
//
//Parameters:
//	map: Map to add to
//	key: Key to retrieve data later
//	data: pointer to The data being added
void HashMap_Add(HashMap* map, char* key, void* data)
{
	struct HashMap_KeyValuePair* pair = HashMap_KeyValuePair_Allocate();
	HashMap_KeyValuePair_Initialize(pair, key, data);

	unsigned int index = map->Hash(key) % map->capacity;
	while (map->data[index] != 0)
	{
		index = (index + 1) % map->capacity;
	}

	map->data[index] = pair;
	map->size++;
}

///
//Removes an entry from the hashmap
//Does not delete data!!
//
//Parameters:
//	map: Map to remove entry from
//	key: Null terminated Key relating to data to be removed
//
//Returns:
//	Poiner to data which was held in hashmap, you should probably free this, whatever it is..
void* HashMap_Remove(HashMap* map, char* key)
{
	unsigned short found = 0;

	unsigned int index = (map->Hash(key) % map->capacity);
	struct HashMap_KeyValuePair* pairToRemove = 0;
	for (unsigned int i = 0; i < map->capacity; i++)
	{
		pairToRemove = map->data[(index + i) % map->capacity];
		if (strcmp(key, pairToRemove->key) == 0)
		{
			index = (index + i) % map->capacity;
			found = 1;
			break;
		}

	}
	void* data = NULL;
	if (found == 1)
	{
		data = pairToRemove->data;
		HashMap_KeyValuePair_Free(pairToRemove);
		map->data[index] = 0;
		map->size--;

	}
	return data;
}

///
//Looks up a key and returns the related data
//
//Parameters:
//	map: The HashMap to lookup data in
//	key: The key related to the data
//
//Returns:
//	Pointer to key value pair containing data
//	NULL if data wasn't found
struct HashMap_KeyValuePair* HashMap_LookUp(HashMap* map, char* key)
{
	unsigned int index = (map->Hash(key) % map->capacity);
	struct HashMap_KeyValuePair* pair = 0;
	for (unsigned int i = 0; i < map->capacity; i++)
	{
		pair = map->data[(index + i) % map->capacity];
		if (pair != NULL)
			if (strcmp(key, pair->key) == 0) 	return pair;
		
	}

	return NULL;

}

///
//A quick 'n dirty implementation of the sdbm public domain hash. 
//
//Parameters:
//	key: The key to hash
//
//Returns:
//	The hashvalue of the key
unsigned long HashMap_SDBM(char* key)
{
	unsigned long hash = 0;
	int characterVal;

	while (characterVal = *key++)
	{
		hash = characterVal + (hash << 6) + (hash << 16) - hash;
	}
	return hash;
}

//Internals

///
//Allocates memory for a Key Value Pair for the HashMap data structure
//
//Returns:
//	A pointer to a newly allocated Key Value Pair
struct HashMap_KeyValuePair* HashMap_KeyValuePair_Allocate()
{
	struct HashMap_KeyValuePair* pair = (HashMap_KeyValuePair*)malloc(sizeof(struct HashMap_KeyValuePair));
	return pair;
}

///
//Initializes a key value pair
//
//Parameters:
//	pair: The key value pair being initialized
//	key: A null terminated character array containing the key to map the data to
//	data: A pointer to the data to be contained in this key value pair
void HashMap_KeyValuePair_Initialize(struct HashMap_KeyValuePair* pair, char* key, void* data)
{
	unsigned int keySize = strlen(key);
	pair->key = (char*)malloc(sizeof(char) * keySize+1);
	strcpy(pair->key, key);
	pair->data = data;
}

///
//Frees memory being used by a Key Value Pair
//Does not delete data!!
//
//PArameters:
//	pair: Key value pair to free
void HashMap_KeyValuePair_Free(struct HashMap_KeyValuePair* pair)
{
	free(pair->key);
	free(pair);
}