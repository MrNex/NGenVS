#include "HashMap.h"

#include <stdlib.h>
#include <string.h>

#include "Hash.h"

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
	map->Hash = Hash_SDBM;
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
//	keyLength: The size of the key in bytes
void HashMap_Add(HashMap* map, void* key, void* data, unsigned int keyLength)
{
	struct HashMap_KeyValuePair* pair = HashMap_KeyValuePair_Allocate();
	HashMap_KeyValuePair_Initialize(pair, key, data, keyLength);

	unsigned int index = map->Hash(key, keyLength) % map->capacity;
	while (map->data[index] != 0)
	{
		index = (index + 1) % map->capacity;
	}

	map->data[index] = pair;
	map->size++;
}

///
//Removes an entry from the hashmap
//Does not delete data!
//
//Parameters:
//	map: Map to remove entry from
//	key: Key relating to data to be removed
//	keyLength: The size of the key in bytes
void* HashMap_Remove(HashMap* map, void* key, unsigned int keyLength)
{
	unsigned short found = 0;

	unsigned int index = (map->Hash(key, keyLength) % map->capacity);
	struct HashMap_KeyValuePair* pairToRemove = 0;
	for (unsigned int i = 0; i < map->capacity; i++)
	{
		pairToRemove = map->data[(index + i) % map->capacity];
		if(keyLength == pairToRemove->keyLength)
		{
			if (memcmp(key, pairToRemove->key, pairToRemove->keyLength) == 0)
			{
				index = (index + i) % map->capacity;
				found = 1;
				break;
			}
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
//	keyLength: The size of the key in bytes
//
//Returns:
//	Pointer to data
struct HashMap_KeyValuePair* HashMap_LookUp(HashMap* map, void* key, unsigned int keyLength)
{
	unsigned int index = (map->Hash(key, keyLength) % map->capacity);
	struct HashMap_KeyValuePair* pair = 0;
	for (unsigned int i = 0; i < map->capacity; i++)
	{
		pair = map->data[(index + i) % map->capacity];
		if (pair != NULL)
			if(keyLength == pair->keyLength)
				if (memcmp(key, pair->key, pair->keyLength) == 0) 	return pair;

	}

	return NULL;

}

///
//Checks if a key is contained within the hashmap
//
//Parameters:
//	map: The map to search
//	key: The key to search for
//	keyLength: The length of the key in bytes
unsigned char HashMap_Contains(HashMap* map, void* key, unsigned int keyLength)
{
	unsigned int index = (map->Hash(key, keyLength) % map->capacity);
	struct HashMap_KeyValuePair* pair = map->data[index % map->capacity];
	for(unsigned int i = 1; i < map->capacity; i++)
	{
		if(pair == NULL)
			break;
		else
		{
			if(keyLength == pair->keyLength)
			{
				if(memcmp(key, pair->key, pair->keyLength) == 0) return 1;
			}
		}
		pair = map->data[(index + i) % map->capacity];
	}
	return 0;
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
//	keyLength: The size of the key in bytes
void HashMap_KeyValuePair_Initialize(struct HashMap_KeyValuePair* pair, void* key, void* data, unsigned int keyLength)
{
	pair->key = (char*)malloc(sizeof(char) * keyLength);
	memcpy(pair->key, key, keyLength);
	pair->keyLength = keyLength;
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