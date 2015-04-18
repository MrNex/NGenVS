//Throughout this file you will see variable names prefixed with 'd_'
//and the term "device pointer".What these indicate is that the memory being referenced
//through aforementioned variables live on the GPU
//
//Memory which lives on the GPU must not be dereferenced from the host (CPU)!

#ifndef ACCELERATEDVECTOR_H
#define ACCELERATEDVECTOR_H
#include <cuda.h>
#include <cuda_runtime.h>

#include "Vector.h"


///
// AcceleratedVector consists of an array of components stored on the device
// And a dimension, which is the number of components
//
typedef struct AcceleratedVector
{
	int dimension;
	float* d_components;
} AcceleratedVector;

///
//Allocates an accelerated vector on the GPU
//
//Parameters:
//	dim: the dimension of the vector to allocate
//
//Returns:
//	A pointer to an accelerated vector
AcceleratedVector* AcceleratedVector_Allocate();

///
// Initializes an accelerated vector to match another vector of the same dimension
//
//Parameters:
//	aVec: 
void AcceleratedVector_Initialize(AcceleratedVector* aVec,const int dim);

///
//Frees an accelerated vector
//
//Parameters:
//	aVec: Pointer to the accelerated vector to free
void AcceleratedVector_Free(AcceleratedVector* aVec);

///
//Copies the contents of a vector to an accelerated vector of the same or larger dimension
//
//Parameters:
//	dest: A pointer to The accelerated vector to copy to
//	src: A pointer to the vector to copy from
void AcceleratedVector_CopyVector(AcceleratedVector* dest, const Vector* src);

///
//Copies the contents from multiple vectors into a single concatenated accelerated vector
//The accelerated vector must have a dimension of at least the sum of the vectors dimension
//
//Parameters:
//	dest: A pointer to the accelerated vector to copy the concatenated contents to
//	srcs: An array of pointers to vectors to copy the contents from
void AcceleratedVector_CopyVectors(AcceleratedVector* dest, const Vector** srcs, const unsigned int dim, const unsigned int numVectors);

///
//Copies the contents of an accelerated vector to a vector
//Or pastes the contents of an acceleratedVector to a vector...
//Probably not the best name for this function but whatever.
//
//Parameters:
//	dest: The vector to paste the contents into
//	src: The vector to copy the contents of
void AcceleratedVector_PasteVector(Vector* dest, const AcceleratedVector* src);

///
//Pastes the contents of an accelerated vector where it's contents are the concatenated contents of various vectors
//Into an array of vectors
//
//Parameters:
//	dest: An array of vectors to paste the contents to
//	src: An accelerated vector to copy the contents from
//	dim: The dimension of each vector
//	numVectors: The number of vectors being pasted.
void AcceleratedVector_PasteVectors(Vector** dest, const AcceleratedVector* src, const unsigned int dim, const unsigned int numVectors);

///
//Increments a vector by another vector on the GPU
//Calculates possible optimal block/thread sizes then
//Calls Vector_AcceleratedIncrement
//
//Parameters:
//	d_dest: A device pointer to an array of floats representing the vector
//	d_src: A device pointer to an array of floats representing the incrementor
//	dim: The dimension of the vectors
void AcceleratedVector_LaunchIncrement(float* d_dest, const float* d_src, unsigned int dim);
///
//Increments a vector by another vector on the GPU
//
//Parameters:
//	<<< Grid Size , Block Size >>>
//	Grid Size: the number of blocks to execute in parallel
//	Block Size: The number of threads each block can have
//	d_dest: A device pointer to an array of floats representing the vector
//	d_src: A device pointer to an array of floats representing the incrementor
//	dim: The dimension of the vectors
__global__ void AcceleratedVector_Increment(float* d_dest, const float* d_src, unsigned int dim);

///
//Adds two vectors storing the result in a third
//Calculates possible optimal grid/block sizes then calls
//Vector_AcceleratedAdd
//
//Parameters:
//	d_dest: a device pointer to an array of floats to store the sum vector
//	d_vec1: a device pointer to an array of floats representing the first addend vector
//	d_vec2: a device pointer to an array of floats representing the second addend vector
//	dim: the dimension of the vectors
void AcceleratedVector_LaunchAdd(float* d_dest, const float* d_vec1, const float* d_vec2, unsigned int dim);
///
//Adds two vectors storing the result in a third on the GPU
//
//Parameters:
//	<<< Grid Size , Block Size >>>
//	Grid Size: the number of blocks to execute in parallel
//	Block Size: The number of threads each block can have
//	d_dest: a device pointer to an array of floats to store the sum vector
//	d_vec1: a device pointer to an array of floats representing the first addend vector
//	d_vec2: a device pointer to an array of floats representing the second addend vector
//	dim: the dimension of the vectors
__global__ void AcceleratedVector_Add(float* d_dest, const float* d_vec1, const float* d_vec2, unsigned int dim);

///
//Adds a concatenated array of vectors storing the sum in another vector on the GPU
//Calls AcceleratedVector_AddAll
//
//Parameters:
//	<<< Grid Size, Block Size >>>
//	grid size:	The dimension of the vectors (One block will compute each index)
//	block size: The number of vectors to add (One thread per vector)
//	d_dest: A device pointer to an array of floats representing the vector to store the sum
//	d_srcs: A device pointer to an array of floats representing the concatenated contents of the vectors to sum
//	dim: The dimension of the vectors
//	numVectors: Thenumber of vectors to sum
void AcceleratedVector_LaunchAddAll(float* d_dest, const float* d_srcs, int dim, int numVectors);
///
//Adds a concatenated array of vectors storing the sum in another vector on the GPU
//
//Parameters:
//	<<< Grid Size, Block Size >>>
//	grid size:	The dimension of the vectors (One block will compute each index)
//	block size: The number of vectors to add (One thread per vector)
//	d_dest: A device pointer to an array of floats representing the vector to store the sum
//	d_srcs: A device pointer to an array of floats representing the concatenated contents of the vectors to sum
//	dim: The dimension of the vectors
//	numVectors: Thenumber of vectors to sum
__global__ void AcceleratedVector_AddAll(float* d_dest, const float* d_srcs, int dim, int numVectors);

///
//Decrements one vector by another on the GPU
//First calculates possible optimal block and grid sizes
//Then calls Vector_AcceleratedDecrement
//
//Parameters:
//	d_dest: A device pointer to an array of floats representing the vector being decremented
//	d_src: A device pointer to an array of floats representing the vector to increment by
//	dim: The dimension of the two vectors
void AcceleratedVector_LaunchDecrement(float* d_dest, const float* d_src, unsigned int dim);
///
//Decrements one vector by another on the GPU
//
//Parameters:
//	<<< Grid Size , Block Size >>>
//	Grid Size: The number of blocks to execute in parallel
//	Block Size: The nuber of threads each block can have
//	d_dest: A device pointer to an array of floats representing the vector being decremented
//	d_src: A device pointer to an array of floats representing the vector to increment by
//	dim: The dimension of the two vectors
__global__ void AcceleratedVector_Decrement(float* d_dest, const float* d_src, unsigned int dim);

///
//calculates vec1 - vec2 on GPU
//Calculates possible optimal block/thread sizes then
//Calls Vector_AcceleratedIncrement
//
//Parameters:
//	<<< Grid Size , Block Size >>>
//	Grid Size: the number of blocks to execute in parallel
//	Block Size: The number of threads each block can have
//	d_dest: A device pointer to an array of floats where the result can be stored
//	d_vec1: A device pointer to an array of floats representing the Left hand side vector operand
//	d_vec2: A device pointer to an array of floats representing the Right had side vector operand
//	dim: The dimension of the vectors
void AcceleratedVector_LaunchSubtract(float* d_dest, const float* d_vec1, const float* d_vec2, unsigned int dim);
///
//Let d_dest, d_vec1, and d_vec2 be dim dimension vectors
//Computes d_vec1 - d_vec2 and stores the result in d_dest on the GPU
//
//Parameters:
//	<<< Grid Size , Block Size >>>
//	Grid Size: the number of blocks to execute in parallel
//	Block Size: The number of threads each block can have
//	d_dest: A device pointer to an array of floats where the result can be stored
//	d_vec1: A device pointer to an array of floats representing the Left hand side vector operand
//	d_vec2: A device pointer to an array of floats representing the Right had side vector operand
//	dim: The dimension of the vectors
__global__ void AcceleratedVector_Subtract(float* d_dest, const float* d_vec1, const float* d_vec2, unsigned int dim);

///
//Gets the scalar product of a vector with a scalar on the GPU
//Computes a possible optimal gridsize and blocksize then calls AcceleratedVector_GetScalarProduct
//
//Parameters:
//	d_dest: The destination of the scaled vector
//	d_vec1: The vector to scale
//	scalar: The scalar to scale the vector by
//	dim: The dimension of the vector getting scaled
void AcceleratedVector_LaunchGetScalarProduct(float* d_dest, const float* d_vec1, const float scalar, unsigned int dim);

///
//Gets the scalar product of a vector with a scalar on the GPU
//
//Parameters:
//	<<<Grid Size, Block Size>>>
//	Grid Size: The number of blocks to execute in parallel
//	BlockSize: The number of threads to execute in parallel
//	d_dest: The destination of the scaled vector
//	d_vec1: The vector to scale
//	scalar: The scalar to scale the vector by
//	dim: The dimension of the vector getting scaled
__global__ void AcceleratedVector_GetScalarProduct(float* d_dest, const float* d_vec1, const float scalar, unsigned int dim);

///
//Gets the scalar product of a vector with a scalar on the GPU
//
//Parameters:
//	d_dest: The destination of the scaled vector
//	d_Vec1: The vector to scale
//	scalar: The scalar to scale the vector by
//	dim: The dimension of the vector getting scaled
__device__ void AcceleratedVector_dGetScalarProduct(float* d_dest, const float* d_vec1, const float scalar, unsigned int dim);


///
//Scales an accelerated vector by a scalar value on the GPU
//Calculates possible optimal block & grid dimensions
//Then calls AcceleratedVector_Scale
//
//Parametrs:
//	d_dest: A device pointer to the vector to scale
//	scalar: The scalar value toscale the vector by
//	dim: The dimension of the vector ebing scale
void AcceleratedVector_LaunchScale(float* d_dest, const float scalar, const unsigned int dim);

///
//Scales an Acceeereated vector by a scalar value on the GPU
//
//PArameters:
//	<<< Grid Size, Block Size>>>
//	Grid Size: The number of blocks to execute in parallel
//	Block size: The number of threads each block will execute in parallel
//	d_Dest: A device pointer to the vector to scale
//	scalar: The scalar to scale the vector by
//	dim: The dimension of the vector being scaled
__global__ void AcceleratedVector_Scale(float* d_dest, const float scalar, const unsigned int dim);

///
//Scales an accelerated vector by a scalar value on the GPU
//
//Parameters:
//	d_dest: A device pointer to the vector to scale
//	scalar: The scalar to scae the vector by
//	dim: THe dimension of the vector being scaled
__device__ void AcceleratedVector_dScale(float* d_dest, const float scalar, const unsigned int dim);

///
//Scales multiple accelerated vectors at once by different scale values on the GPU
//Determines Block & Grid sizes then calls AcceleratedVector_ScaleAll
//
//PArameters:
//	d_dests: A device pointer to an array of floats representing the concatenated components of the vectors being scaled
//	scalars: a vecide pointer to an array of scalars respective to the array of vectors that they scale
//	dim: The dimension of each vector
//	numVectors: The number of vectors to scale
void AcceleratedVector_LaunchScaleAll(float* d_dests, const float* d_scalars, const unsigned int dim, const unsigned int numVectors);

///
//Scales multiple accelerated vectors at once by different scale values on the GPU
//
//PArameters:
//	<<< Grid Size, Block Size>>>
//	Grid Size: The number of blocks to execute in parallel
//	Block size: The number of threads each block will execute in parallel
//	d_dests: A device pointer to an array of floats representing the concatenated components of the vectors being scaled
//	scalars: A device pointer to an array of scalars respective to the array of vectors that they scale
//	dim: The dimension of each vector
//	numVectors: The number of vectors to scale
__global__ void AcceleratedVector_ScaleAll(float* d_dests, const float* d_scalars, const unsigned int dim, const unsigned int numVectors);

///
//Calculates the magnitude of a vector on the GPU
//Determines block & grid sizes then calles AcceleratedVector_Magnitude
//
//Parameters:
//	d_mag: A device pointer to store the magnitude in
//	d_vector: A device pointer to an array of floats representing the components of the vector to calculate the magnitude of
//	dim: The dimension of the vector
void AcceleratedVector_LaunchMagnitude(float* d_mag, const float* d_vector, const unsigned int dim);

///
//Calculates the magnitude of a vector on the GPU
//
//Parameters:
//	d_mag: A device pointer to store the magnitude in
//	d_vector: A device pointer to an array of floats representing the components of the vector to calculate the magnitude of
//	dim: The dimension of the vector
__global__ void AcceleratedVector_Magnitude(float* d_mag, const float* d_vector, const unsigned int dim);

///
//Calculates the magnitude of a Vector on the GPU
//This function is only callable from the device
//
//Parameters:
//	d_mag: A device pointer to a float to store the magnitude
//	d_vector: A device pointer to an array of floats representing the vector to get he magnitude of
//	dim: The dimension of the vector tg et the magnitude from
//	d_sharedStorage: A device pointer to shared memory with dim * sizeof(vector element) space
__device__ void AcceleratedVector_dMagnitude(float* d_mag, const float* d_vector, const unsigned int dim, float* d_sharedStorage);

///
//Calculates the magnitude squared of a vector on the GPU
//This function is only callable from the device
//
//Parameters:
//	d_mag: A device pointer to a float to store the magnitude
//	d_vector: A device pointer to an array of floats representing the vector to get he magnitude of
//	dim: The dimension of the vector tg et the magnitude from
//	d_sharedStorage: A device pointer to shared memory with dim * sizeof(vector element) space
__device__ void AcceleratedVector_dMagnitudeSq(float* d_mag, const float* d_vector, const unsigned int dim, float* d_sharedStorage);

///
//Gets a Normalized a vector the GPU
//First determines gridsize and blocksize then calls AcceleratedVector_Normalize
//
//Parameters:
//	d_dest: Device pointer to an array to store the components of the normalized src
//	d_src: Device pointer to an array representing the components of the vector to normalize
//	dim: The dimension of th vector
void AcceleratedVector_LaunchGetNormalize(float* d_dest, const float* d_src, const unsigned int dim);

///
//Gets a Normalized a vector the GPU
//
//Parameters:
//	<<<Grid Size, Block Size>>>
//	Grid Size: The number of blocks to run in parallel. Should be 1 for this algorithm.
//	Block Size: The number of threads each block should run in parallel. Should be equal to the dimension of the vector being normalized
//	d_dest: A device pointer to an array of floats to store the components of the normalized vector
//	d_src: Adevice pointer to an array of floats representing the components of the vector to normalize
//	dim: The dimension of the vector to normalize
__global__ void AcceleratedVector_GetNormalize(float* d_dest, const float* d_src, const unsigned int dim);

///
//Normalizes a vector on the GPU
//First determines gridsize and blocksize then calls AcceleratedVector_Normalize
//
//PArameters:
//	d_vec: A device pointer to an array of floats representing the components to normalize
//	dim: The dimension of the vector
void AcceleratedVector_LaunchNormalize(float* d_vec, const unsigned int dim);

///
//Normalizes a vector on the GPU
//
//PArameters:
//	<<<Grid Size, Block Size>>>
//	Grid Size: The number of blocks to execute in parallel. Should be 1 for this algorithm
//	Block Size: The number of threads to run in parallel. Should be equal to the vector dimension for this algorithm
//	d_vec: A device pointer to an array of floats representing the components to normalize
//	dim: The dimension of the vector
__global__ void AcceleratedVector_Normalize(float* d_vec, const unsigned int dim);

///
//Computes the dot product of two vectors on the GPU
//
//Parameters:
//	d_dest: Pointer to a float on the GPU to store the result of the dot product
//	d_vec1: a device pointer to an array of floats representing the first vector being dotted
//	d_vec2: a device pointer to an array of floats representing the second vector to be dotted
//	dim: The dimension of the vectors being dotted
void AcceleratedVector_LaunchDotProduct(float* d_dest, const float* d_vec1, const float* d_vec2, unsigned int dim);

///
//Computes the dot product of two vectors on the GPU
//
//Parameters:
//	<<< Grid Size, Block Size >>>
//	Grid Size: Single dot product must be launched in one block.
//	Block Size: Number of threads to give the active block. Equal to the dimension of vectors being dotted.
//	d_dest: Pointer to a float on the GPU to store the result of the dot product
//	d_vec1: a device pointer to an array of floats representing the first vector being dotted
//	d_vec2: a device pointer to an array of floats representing the second vector to be dotted
//	dim: The dimension of the vectors being dotted
__global__ void AcceleratedVector_DotProduct(float* d_dest, const float* d_vec1, const float* d_vec2, unsigned int dim);

///
//Computes the dot product of two vectors on the GPU
//
//Parameters:
//	d_dest: Pointer to a float on the GPU to store the result of the dot product
//	d_vec1: a device pointer to an array of floats representing the first vector being dotted
//	d_vec2: a device pointer to an array of floats representing the second vector to be dotted
//	dim: The dimension of the vectors being dotted
//	reductionStorage: A device pointer to an array of floats with dim components to be used for reduction
__device__ void AcceleratedVector_dDotProduct(float* d_dest, const float* d_vec1, const float* d_vec2, unsigned int dim, float* reductionStorage);

///
//Computes the dot product of multiple vectors on the GPU
//
//Parameters:
//	d_dest: A device pointer to an array of floats to store the respective dot product results
//	d_vecs1: A device pointer to an array of floats representing numVectors vectors of dimension dim as the LHS vectors
//	d_vecs2: A device pointer to an array of floats represnting numVectors vectors of dimension dim as the RHS vectors
//	dim: The dimension of the vectors being dotted
//	numVectors: number of vectors in each vector component array
void AcceleratedVector_LaunchDotProductAll(float* d_dest, const float* d_vecs1, const float* d_vecs2, unsigned int dim, unsigned int numVectors);

///
//Computes the dot product of multiple vectors on the GPU
//
//Parameters:
//	<<< Grid Size , Block size >>>
//	Grid Size : The amount of dot products being performed, numVectors
//	Block Size: The amount of threads to run per block, in this case, equal to the dimension of the vectors
//	d_dest: A device pointer to an array of floats to store the respective dot product results
//	d_vecs1: A device pointer to an array of floats representing a contiguous array of multiple sets of vector components representing the LHS vectors
//	d_vecs2: A device pointer to an array of floats representing a contiguous array of multiple sets of vector components representing the RHS vectors
//	dim: The dimension of the vectors being dotted
//	numVectors: The number of vectors in each vector component array
__global__ void AcceleratedVector_DotProductAll(float* d_dest, const float* d_vecs1, const float* d_vecs2, unsigned int dim, unsigned int numVectors);

///
//Computes the dotprduct of multiple vectors on the GPU
//
//Parameters:
//	d_dest: A device pointer to an array of floats to store respective dot product results
//	d_vecs1: A device pointer to an array of floats representing the concatenation of all LHS vecto components involved in dot products
//	d_vecs2: A device pointer to an array of floats representing the concatenated components of all RHS vectors involved in dot products
//	dim: The dimension of the vectors being dotted
//	numVectors: The number of vectors being
//	d_reductionStorage: A device pointer to an array of floats of length sizeof(VectorElement) * dim
__device__ void AcceleratedVector_dDotProductAll(float* d_dest, const float* d_vecs1, const float* d_vecs2, unsigned int dim, unsigned int numVectors, float* d_reductionStorage);


///
//Computes the dot product of a set of vectors with another vector on the GPU.
//Stores the result in an array containing the result of each respective dot product
//Determines the grid size and block size then calls AcceleratedVector_DotProductAllWith
//
//Parameters:
//	d_dest: A device pointer to an array of floats containing the respective dot product results
//	d_vec1: A device pointer to an array of floats contaning the components of the constant vector involved in the dot products
//	d_vecs2: A device pointer to an array of floats containing the contiguous components of the set of vectors each being dotted with d_vec1
//	dim: The dimension of the vectors
//	numVectors: The number of vectors involved in the dot product
void AcceleratedVector_LaunchDotProductAllWith(float* d_dest, const float* d_vec1, const float* d_vecs2, unsigned int dim, unsigned int numVectors);

///
//Computes the dot product of a set of vectors with another vector on the GPU.
//Stores the result in an array containing the result of each respective dot product
//Determines the grid size and block size then calls AcceleratedVector_DotProductAllWith
//
//Parameters:
//	<<<Grid Size, Block Size, Dynamic Allocation>>>
//	Grid Size: The number of blocks to execute in parallel. Should be equal to the number of vectors in this case.
//	Block Size: The number of threads each block should execute in parallel. Should be equal to the dimension of the vector.
//	Dynamic Allocation: Array of memory to dynamically allocate for this function. This function requires sizeof(vectorElement) * dim extra space per block.
//	d_dest: A device pointer to an array of floats containing the respective dot product results
//	d_vec1: A device pointer to an array of floats contaning the components of the constant vector involved in the dot products
//	d_vecs2: A device pointer to an array of floats containing the contiguous components of the set of vectors each being dotted with d_vec1
//	dim: The dimension of the vectors
//	numVectors: The number of vectors involved in the dot product
__global__ void AcceleratedVector_DotProductAllWith(float* d_dest, const float* d_vec1, const float* d_vecs2, unsigned int dim, unsigned int numVectors);

///
//Projects d_vec1 onto d_vec2 storing the projection vector in d_dest on the GPU
//Detemrines the grid dimension and block dimension then calls AcceleratedVector_GetProjection
//
//Parameters:
//	d_dest: A device pointer to an array of floats to store the result of the projection
//	d_vec1: A device pointer to an array of floats representing the components of The vector being projected
//	d_vec2: A device pointer to an array of floats containing the components of the vector being projected onto
//	dim: Te dimension of the vectors (The vectors dimension should match)
void AcceleratedVector_LaunchGetProjection(float* d_dest, const float* d_vec1, const float* d_vec2, const unsigned int dim);

///
//Projects d_vec1 onto d_vec2 storing the projection vector in d_dest on the GPU
//
//Parameters:
//	<<<Grid Size, Block Size, Dynamic Allocation>>>
//	Grid Size: The amount of blocks to execute in parallel. Should be one for thisalgorithm
//	Block size: The aount of threads each block should execute in parallel. Should be equal to the dimension of the vectors being projected
//	Dynamic Allocation: The amount of extra data this algorithm will need to perform (Needs sizeof(VectorElement) * dim bytes)
//	d_dest: A device pointer to an array of floats to store the result of the projection
//	d_vec1: A device pointer to an array of floats representing the components of The vector being projected
//	d_vec2: A device pointer to an array of floats containing the components of the vector being projected onto
//	dim: Te dimension of the vectors (The vectors dimension should match)
__global__ void AcceleratedVector_GetProjection(float* d_dest, const float* d_vec1, const float* d_vec2, const unsigned int dim);

///
//Projects d_vec1 onto d_vec2 changing d_vec1 to represent the projection vector on the GPU
//Detemrines the grid dimension and block dimension then calls AcceleratedVector_Project
//
//Parameters:
//	d_vec1: A device pointer to an array of floats representing the components of The vector being projected
//	d_vec2: A device pointer to an array of floats containing the components of the vector being projected onto
//	dim: Te dimension of the vectors (The vectors dimension should match
void AcceleratedVector_LaunchProject(float* d_vec1, const float* d_vec2, const unsigned int dim);

///
//Projects d_vec1 onto d_vec2 on the GPU. Result is stored in d_vec1.
//
//Parameters:
//	<<<Grid Size, Block Size, Dynamic Allocation>>>
//	GridSize: The amount of blocks to execute in parallel (should be one for this algorithm)
//	blockSize: The amount of threads each block should execute in parallel (Should be equal to the dimension of the vectors)
//	Dynamic Allocation: The amount of extra data this algorithm will need to perform (Needs sizeof(VectorElement) * dim bytes)
//	d_vec1: A device pointer to an array of floats representing the components of The vector being projected
//	d_vec2: A device pointer to an array of floats containing the components of the vector being projected onto
//	dim: Te dimension of the vectors (The vectors dimension should match
__global__ void AcceleratedVector_Project(float* d_vec1, const float* d_vec2, const unsigned int dim);

///
//Projects each vector to be projected onto the respective vector being projected onto, altering the LHS vector to hold the solution
//Determines the grid size and block size then calls AcceleratedVector_ProjectAll
//
//Parameters:
//	d_vecs1: A device pointer to an array of floats representing the concatenated components of each vector being projected.
//	d_vecs2: A device pointer to an array of floats representing the concatenated components of each vector being projected onto.
//	dim: The dimension of the vectors being projected
//	numVectors: The number of vectors being projected
void AcceleratedVector_LaunchProjectAll(float* d_vecs1, const float* d_vecs2, const unsigned int dim, const unsigned int numVectors);

///
//Projects each vector to be projected onto the respective vector being projected onto, altering the LHS vector to hold the solution
//Determines the grid size and block size then calls AcceleratedVector_ProjectAll
//
//Parameters:
//	<<<Grid Size, Block Size, Dynamic Allocation>>>
//	Grid Size: THe amount of blocks to execute in parallel. The number of vectors being projected
//	Block size: THe amount of threads each block can execute in parallel. The dimension of the vectors being projected
//	Dynamic Allocation: The aount of extra data this algorithm will need to perform. Needs sizeof(VectorElement) * dim bytes.
//	d_vecs1: A device pointer to an array of floats representing the concatenated components of each vector being projected.
//	d_vecs2: A device pointer to an array of floats representing the concatenated components of each vector being projected onto.
//	dim: The dimension of the vectors being projected
//	numVectors: The number of vectors being projected
__global__ void AcceleratedVector_ProjectAll(float* d_vecs1, const float* d_vecs2, const unsigned int dim, const unsigned int numVectors);

///
//Uses reduction techniques to compute the sum of components in the array storing te sum in the first element
//There must be at least arrSize threads.
//
//Parameters:
//	arrToReduce: The array to reduce
//	arrSize: The size of the array
__device__ void AcceleratedVector_dReduceArray(float* arrToReduce, const unsigned int arrSize);



#endif