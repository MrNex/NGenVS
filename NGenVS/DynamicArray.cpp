#include "DynamicArray.h"
#include <stdlib.h>
#include <string.h>

///
//Allocates a Dynamic Array
//
//Returns:
//	Pointer to a newly allocated dynamic array
DynamicArray* DynamicArray_Allocate()
{
	DynamicArray* arr = (DynamicArray*)malloc(sizeof(DynamicArray));
	arr->capacity = defaultCapacity;
	arr->growthRate = defaultGrowth;
	return arr;
}

///
//Initializes a Dynamic Array
//
//Parameters:
//	arr: Dynamic array to initialize
void DynamicArray_Initialize(DynamicArray* arr, unsigned int dataSize)
{
	arr->size = 0;
	arr->dataSize = dataSize;
	arr->data = calloc(dataSize, arr->capacity);
}

///
//Frees resources being used by a dynamic array
//
//Parameters:
//	arr: The dynamic array to free
void DynamicArray_Free(DynamicArray* arr)
{
	free(arr->data);
	free(arr);
}


///
//Appends a dynamic array with data
//
//Parameters:
//	arr: Dynamic array to append
//	data: Data to append to array
void DynamicArray_Append(DynamicArray* arr, void* data)
{
	if (arr->size == arr->capacity) DynamicArray_Grow(arr);
	memcpy(((char*)arr->data) + (arr->dataSize * arr->size), data, arr->dataSize);
	arr->size++;
}


///
//Indexes a dynamic array
//
//Parameters:
//	arr: The dynamic array to index
//	index: The index to get
//
//Returns:
//	Pointer to data at the index of arr
void* DynamicArray_Index(DynamicArray* arr, unsigned int index)
{
	return ((char*)arr->data) + (arr->dataSize * index);
}


///
//Clears a dynamic array setting all indices & size to 0
//Does not lessen capacity
//
//Parameters:
//	arr: Pointer to dynamic array being cleared
void DynamicArray_Clear(DynamicArray* arr)
{
	memset(arr->data, arr->capacity, 0);
	arr->size = 0;
}


//Internal functions
///
//Increases the capacity of a dynamic array by
//It's current capacity times it's growth rate
//
//Parameters:
//	arr: the array to increase in capacity
static void DynamicArray_Grow(DynamicArray* arr)
{
	//This information gets lost, we need it in case things go wrong.
	unsigned int oldSize = arr->capacity;

	//Change capacity
	arr->capacity = (unsigned int)(arr->growthRate * arr->capacity);

	//Allocate new larger memory
	void* newPtr = malloc(arr->dataSize * arr->capacity);
	memcpy(newPtr, arr->data, oldSize * arr->dataSize);
	free(arr->data);
	arr->data = newPtr;
}
