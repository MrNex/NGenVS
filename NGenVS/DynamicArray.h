#ifndef DYNAMIC_ARRAY
#define DYNAMIC_ARRAY

typedef struct DynamicArray {
	unsigned int capacity;	//Total available slots
	unsigned int size;		//Current size
	float growthRate;		//Rate array will grow
	unsigned int dataSize;	//Size of data
	void* data;				//Array of data

} DynamicArray;

///
//Allocates a Dynamic Array
//
//Returns:
//	Pointer to a newly allocated dynamic array
DynamicArray* DynamicArray_Allocate();

///
//Initializes a Dynamic Array
//
//Parameters:
//	arr: Dynamic array to initialize
void DynamicArray_Initialize(DynamicArray* arr, unsigned int dataSize);

///
//Frees resources being used by a dynamic array
//
//Parameters:
//	arr: The dynamic array to free
void DynamicArray_Free(DynamicArray* arr);

//Internal members
static int defaultCapacity = 8;
static float defaultGrowth = 2.0f;
//Internal functions
static void DynamicArray_Grow(DynamicArray* arr);

//Functions

///
//Appends a dynamic array with data
//
//Parameters:
//	arr: Dynamic array to append
//	data: Data to append to array
void DynamicArray_Append(DynamicArray* arr, void* data);


///
//Indexes a dynamic array
//
//Parameters:
//	arr: The dynamic array to index
//	index: The index to get
//
//Returns:
//	Pointer to data at the index of arr
void* DynamicArray_Index(DynamicArray* arr, unsigned int index); 

///
//Clears a dynamic array setting all indices & size to 0
//Does not lessen capacity
//
//Parameters:
//	arr: Pointer to dynamic array being cleared
void DynamicArray_Clear(DynamicArray* arr);



#endif