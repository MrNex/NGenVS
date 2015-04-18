#include "AcceleratedVector.h"

#include <stdio.h>


///
//Allocates an accelerated vector on the GPU
//
//Parameters:
//	dim: the dimension of the vector to allocate
//
//Returns:
//	A pointer to an accelerated vector
AcceleratedVector* AcceleratedVector_Allocate()
{
	AcceleratedVector* aVec = (AcceleratedVector*)malloc(sizeof(AcceleratedVector));
	return aVec;

}

///
// Initializes an accelerated vector to match another vector of the same dimension
//
//Parameters:
//	aVec: 
void AcceleratedVector_Initialize(AcceleratedVector* aVec, const int dim)
{
	aVec->dimension = dim;
	cudaMalloc((void**)&aVec->d_components, sizeof(float)* dim);
}

///
//Frees an accelerated vector
//
//Parameters:
//	aVec: Pointer to the accelerated vector to free
void AcceleratedVector_Free(AcceleratedVector* aVec)
{
	cudaFree(aVec->d_components);
	free(aVec);
}

///
//Copies the contents of a vector to an accelerated vector of the same or larger dimension
//
//Parameters:
//	dest: A pointer to The accelerated vector to copy to
//	src: A pointer to the vector to copy from
void AcceleratedVector_CopyVector(AcceleratedVector* dest, const Vector* src)
{
	cudaMemcpy(dest->d_components, src->components, sizeof(float)* src->dimension, cudaMemcpyHostToDevice);
}


///
//Copies the contents from multiple vectors into a single concatenated accelerated vector
//The accelerated vector must have a dimension of at least the sum of the vectors dimension
//
//Parameters:
//	dest: A pointer to the accelerated vector to copy the concatenated contents to
//	srcs: An array of pointers to vectors to copy the contents from
//	dim: The dimension of all source vectors
//	numVectors: The number of source vectors
void AcceleratedVector_CopyVectors(AcceleratedVector* dest, const Vector** srcs, const unsigned int dim, const unsigned int numVectors)
{
	for (int i = 0; i < numVectors; i++)
	{
		cudaMemcpy(dest->d_components + (i * dim), srcs[i]->components, sizeof(float)* dim, cudaMemcpyHostToDevice);
	}
}

///
//Copies the contents of an accelerated vector to a vector
//Or pastes the contents of an acceleratedVector to a vector...
//Probably not the best name for this function but whatever.
//
//Parameters:
//	dest: The vector to paste the contents into
//	src: The vector to copy the contents of
void AcceleratedVector_PasteVector(Vector* dest, const AcceleratedVector* src)
{
	cudaMemcpy(dest->components, src->d_components, sizeof(float)* src->dimension, cudaMemcpyDeviceToHost);
}

///
//Pastes the contents of an accelerated vector where it's contents are the concatenated contents of various vectors
//Into an array of vectors
//
//Parameters:
//	dest: An array of vectors to paste the contents to
//	src: An accelerated vector to copy the contents from
//	dim: The dimension of each vector
//	numVectors: The number of vectors being pasted.
void AcceleratedVector_PasteVectors(Vector** dest, const AcceleratedVector* src, const unsigned int dim, const unsigned int numVectors)
{
	for (int i = 0; i < numVectors; i++)
	{
		cudaMemcpy(dest[i]->components, src->d_components + (i * dim), sizeof(float)* dim, cudaMemcpyDeviceToHost);
	}
}

///
//Increments a vector by another vector on the GPU
//Calculates possible optimal block/thread sizes then
//Calls Vector_AcceleratedIncrement
//
//Parameters:
//	d_dest: A device pointer to an array of floats representing the vector
//	d_src: A device pointer to an array of floats representing the incrementor
//	dim: The dimension of the vectors
void AcceleratedVector_LaunchIncrement(float* d_dest, const float* d_src, unsigned int dimension)
{
	int blockSize;
	int gridSize;
	int minBlockSize;
	//Get the max potential block size
	cudaOccupancyMaxPotentialBlockSize(&minBlockSize, &blockSize, AcceleratedVector_Increment, 0, dimension);
	//Calculate the grid size
	gridSize = (dimension + blockSize - 1) / blockSize;

	//Call kernel
	AcceleratedVector_Increment << <gridSize, blockSize >> >(d_dest, d_src, dimension);
}

///
//Increments a vector by another vector on the GPU
//
//Parameters:
//	<<< Grid Size , Block Size >>>
//	d_dest: A device pointer to an array of floats representing the vector
//	d_src: A device pointer to an array of floats representing the incrementor
//	dim: The dimension of the vectors
__global__ void AcceleratedVector_Increment(float* dest, const float* src, const unsigned int dim)
{
	unsigned int index = threadIdx.x + blockIdx.x * blockDim.x;
	if (index < dim)
	{
		dest[index] += src[index];
	}
}

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
void AcceleratedVector_LaunchAdd(float* d_dest, const float* d_vec1, const float* d_vec2, unsigned int dim)
{
	int blockSize;
	int gridSize;
	int minBlockSize;
	//Calculate potential optimal block / grid size
	cudaOccupancyMaxPotentialBlockSize(&minBlockSize, &blockSize, AcceleratedVector_Add, 0, dim);
	gridSize = (dim + blockSize - 1) / blockSize;

	//Call accelerated add
	AcceleratedVector_Add << <gridSize, blockSize >> >(d_dest, d_vec1, d_vec2, dim);

}
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
__global__ void AcceleratedVector_Add(float* d_dest, const float* d_vec1, const float* d_vec2, unsigned int dim)
{
	unsigned int index = threadIdx.x + blockIdx.x * blockDim.x;
	if (index < dim)
	{
		d_dest[index] = d_vec1[index] + d_vec2[index];
	}
}

///
//Adds a concatenated array of vectors storing the sum in another vector on the GPU
//Calls AcceleratedVector_AddAll
//
//Parameters:
//	d_dest: A device pointer to an array of floats representing the vector to store the sum
//	d_srcs: A device pointer to an array of floats representing the concatenated contents of the vectors to sum
//	dim: The dimension of the vectors
//	numVectors: Thenumber of vectors to sum
void AcceleratedVector_LaunchAddAll(float* d_dest, const float* d_srcs, int dim, int numVectors)
{
	AcceleratedVector_AddAll << <dim, numVectors / 2, ceilf(numVectors / 2.0f) * sizeof(float) >> >(d_dest, d_srcs, dim, numVectors);
}
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
__global__ void AcceleratedVector_AddAll(float* d_dest, const float* d_srcs, int dim, int numVectors)
{
	int sumSize = ceilf(numVectors / 2.0f);
	extern __shared__ float sum[];	//Array to be used for reduction addition
	//Dimension of above array is passed in from AcceleratedVector_LaunchAddAll using special optional third parameter
	//In triple angle brackets.

	//let the threadID be the vector this thread is responsible for summing with the vector next to it.
	//Let blockIdx.x be the element this thread is resposible for summing. 
	if (threadIdx.x < (numVectors / 2))	//Do not perform any operations if the thread ID is outside the range of vectors/2.
	{
		int index1 = blockIdx.x + (threadIdx.x * 2) * dim;	//Each thread will take element blockIdx.x of vector threadIdx
		int index2 = blockIdx.x + (((threadIdx.x * 2) + 1) * dim);	//And element blockIdx.x of vector threadIdx + 1
		//Thread 0 of block x compute the sum of the x element of the first two vectors storing the result in sum[0]
		//Threads 1 of block x compute the sum of the x element of the second and third vectors storing the result in sum[1]
		//Thread y of block x computes the sum of the x element of the 2y and 2y+1 vectors storing the result in sum[y]
		sum[threadIdx.x] = d_srcs[index1] + d_srcs[index2];

		if (numVectors % 2 == 1)	//If there is an extra vector which was not included in the sum
		{
			//0th thread will Set sum[sumSize-1] = missing index
			if (threadIdx.x == 0)
			{
				sum[sumSize - 1] = d_srcs[blockIdx.x + ((numVectors - 1) * dim)];
			}

		}
	}

	//Fill sum before going any further
	__syncthreads();

	//Begin reduction
	AcceleratedVector_dReduceArray(sum, sumSize);

	//Wait for all threads here
	__syncthreads();



	if (threadIdx.x == 0)
	{
		//Wrte final sum to each index of d_dest
		d_dest[blockIdx.x] = sum[0];
	}


}

///
//Decrements one vector by another on the GPU
//First calculates possible optimal block and grid sizes
//Then calls Vector_AcceleratedDecrement
//
//Parameters:
//	d_dest: A device pointer to an array of floats representing the vector being decremented
//	d_src: A device pointer to an array of floats representing the vector to increment by
//	dim: The dimension of the two vectors
void AcceleratedVector_LaunchDecrement(float* d_dest, const float* d_src, unsigned int dim)
{
	int blockSize;
	int gridSize;
	int minBlockSize;

	cudaOccupancyMaxPotentialBlockSize(&minBlockSize, &blockSize, AcceleratedVector_Decrement, 0, dim);
	gridSize = (dim + blockSize - 1) / blockSize;

	AcceleratedVector_Decrement << <gridSize, blockSize >> >(d_dest, d_src, dim);
}
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
__global__ void AcceleratedVector_Decrement(float* d_dest, const float* d_src, unsigned int dim)
{
	int index = threadIdx.x + blockIdx.x * blockDim.x;
	if (index < dim)
	{
		d_dest[index] -= d_src[index];
	}
}


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
void AcceleratedVector_LaunchSubtract(float* d_dest, const float* d_vec1, const float* d_vec2, unsigned int dim)
{
	int blockSize;
	int gridSize;
	int minBlockSize;
	//Get the max potential block size
	cudaOccupancyMaxPotentialBlockSize(&minBlockSize, &blockSize, AcceleratedVector_Subtract, 0, dim);
	//Calculate the grid size
	gridSize = (dim + blockSize - 1) / blockSize;

	AcceleratedVector_Subtract << <gridSize, blockSize >> >(d_dest, d_vec1, d_vec2, dim);

}
///
//Let d_dest, d_vec1, and d_vec2 be dim dimension vectors
//Computes d_vec1 - d_vec2 and stores the result in d_dest
//
//Parameters:
//	<<< Grid Size , Block Size >>>
//	Grid Size: the number of blocks to execute in parallel
//	Block Size: The number of threads each block can have
//	d_dest: A device pointer to an array of floats where the result can be stored
//	d_vec1: A device pointer to an array of floats representing the Left hand side vector operand
//	d_vec2: A device pointer to an array of floats representing the Right had side vector operand
//	dim: The dimension of the vectors
__global__ void AcceleratedVector_Subtract(float* d_dest, const float* d_vec1, const float* d_vec2, unsigned int dim)
{
	unsigned int index = threadIdx.x + blockIdx.x * blockDim.x;
	if (index < dim)
	{
		d_dest[index] = d_vec1[index] - d_vec2[index];
	}
}

///
//Gets the scalar product of a vector with a scalar on the GPU
//Computes a possible optimal gridsize and blocksize then calls AcceleratedVector_GetScalarProduct
//
//Parameters:
//	d_dest: The destination of the scaled vector
//	d_vec1: The vector to scale
//	scalar: The scalar to scale the vector by
//	dim: The dimension of the vector getting scaled
void AcceleratedVector_LaunchGetScalarProduct(float* d_dest, const float* d_vec1, const float scalar, unsigned int dim)
{
	int blockSize;
	int gridSize;
	int minBlockSize;
	//Get the max potential block size
	cudaOccupancyMaxPotentialBlockSize(&minBlockSize, &blockSize, AcceleratedVector_Subtract, 0, dim);
	//Calculate the grid size
	gridSize = (dim + blockSize - 1) / blockSize;

	AcceleratedVector_GetScalarProduct << <gridSize, blockSize >> >(d_dest, d_vec1, scalar, dim);
}

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
__global__ void AcceleratedVector_GetScalarProduct(float* d_dest, const float* d_vec1, const float scalar, unsigned int dim)
{
	float* d_blockDest = d_dest + (blockIdx.x * blockDim.x);
	const float* d_blockVec1 = d_vec1 + (blockIdx.x * blockDim.x);
	//AcceleratedVector_dGetScalarProduct(d_dest, d_vec1, scalar, dim);
	AcceleratedVector_dGetScalarProduct(d_blockDest, d_blockVec1, scalar, dim);
}

///
//Gets the scalar product of a vector with a scalar on the GPU
//
//Parameters:
//	d_dest: The destination of the scaled vector
//	d_Vec1: The vector to scale
//	scalar: The scalar to scale the vector by
//	dim: The dimension of the vector getting scaled
__device__ void AcceleratedVector_dGetScalarProduct(float* d_dest, const float* d_vec1, const float scalar, unsigned int dim)
{
	//unsigned int index = threadIdx.x + blockIdx.x * blockDim.x;
	/*
	if (index < dim)
	{
		d_dest[index] = d_vec1[index] * scalar;
	}
	*/

	if (threadIdx.x < dim)
	{
		d_dest[threadIdx.x] = d_vec1[threadIdx.x] * scalar;
	}
}

///
//Scales an accelerated vector by a scalar value on the GPU
//Calculates possible optimal block & grid dimensions
//Then calls AcceleratedVector_Scale
//
//Parametrs:
//	d_dest: A device pointer to the vector to scale
//	scalar: The scalar value toscale the vector by
//	dim: The dimension of the vector ebing scale
void AcceleratedVector_LaunchScale(float* d_dest, const float scalar, const unsigned int dim)
{
	int blockSize;
	int gridSize;
	int minBlockSize;
	//Get the max potential block size
	cudaOccupancyMaxPotentialBlockSize(&minBlockSize, &blockSize, AcceleratedVector_Subtract, 0, dim);
	//Calculate the grid size
	gridSize = (dim + blockSize - 1) / blockSize;

	AcceleratedVector_Scale << <gridSize, blockSize >> >(d_dest, scalar, dim);

}

///
//Scales an Acceeereated vector by a scalar value on the GPU
//Calls AcceleratedVector_dScale
//
//PArameters:
//	<<< Grid Size, Block Size>>>
//	Grid Size: The number of blocks to execute in parallel
//	Block size: The number of threads each block will execute in parallel
//	d_Dest: A device pointer to the vector to scale
//	scalar: The scalar to scale the vector by
//	dim: The dimension of the vector being scaled
__global__ void AcceleratedVector_Scale(float* d_dest, const float scalar, const unsigned int dim)
{
	AcceleratedVector_dScale(d_dest, scalar, dim);
}

///
//Scales an accelerated vector by a scalar value on the GPU
//
//Parameters:
//	d_dest: A device pointer to the vector to scale
//	scalar: The scalar to scae the vector by
//	dim: THe dimension of the vector being scaled
__device__ void AcceleratedVector_dScale(float* d_dest, const float scalar, const unsigned int dim)
{
	int index = threadIdx.x + blockIdx.x * blockDim.x;
	if (index < dim)
	{
		d_dest[index] *= scalar;
	}
}

///
//Scales multiple accelerated vectors at once by different scale values on the GPU
//Determines Block & Grid sizes then calls AcceleratedVector_ScaleAll
//
//PArameters:
//	d_dests: A device pointer to an array of floats representing the concatenated components of the vectors being scaled
//	scalars: A device pointer to an array of scalars respective to the array of vectors that they scale
//	dim: The dimension of each vector
//	numVectors: The number of vectors to scale
void AcceleratedVector_LaunchScaleAll(float* d_dests, const float* d_scalars, const unsigned int dim, const unsigned int numVectors)
{
	AcceleratedVector_ScaleAll <<<numVectors, dim >>>(d_dests, d_scalars, dim, numVectors);

}

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
__global__ void AcceleratedVector_ScaleAll(float* d_dests, const float* d_scalars, const unsigned int dim, const unsigned int numVectors)
{
	int index = threadIdx.x + blockIdx.x * blockDim.x;
	if (index < dim * numVectors)
	{
		d_dests[index] *= d_scalars[blockIdx.x];
	}
}

///
//Calculates the magnitude of a vector on the GPU
//Determines block & grid sizes then calles AcceleratedVector_Magnitude
//
//Parameters:
//	d_mag: A device pointer to store the magnitude in
//	d_vector: A device pointer to an array of floats representing the components of the vector to calculate the magnitude of
//	dim: The dimension of the vector
void AcceleratedVector_LaunchMagnitude(float* d_mag, const float* d_vector, const unsigned int dim)
{
	AcceleratedVector_Magnitude << <1, dim, dim * sizeof(float) >> >(d_mag, d_vector, dim);

}

///
//Calculates the magnitude of a vector on the GPU
//
//Parameters:
//	<<<GridSize, Block Size, Dynamic Allocation>>>
//	Grid Size: The number of blocks to run in parallel (Should be 1 for this algorithm)
//	Block Size: The number of threads each block will run in parallel. Should be equal to the dimension of the vector * the size of each component.
//	Dynamic Allocation: This algorithm needs to allocate memory equal to the 
//	d_mag: A device pointer to store the magnitude in
//	d_vector: A device pointer to an array of floats representing the components of the vector to calculate the magnitude of
//	dim: The dimension of the vector
__global__ void AcceleratedVector_Magnitude(float* d_mag, const float* d_vector, const unsigned int dim)
{
	//AcceleratedVector_dMagnitude(d_mag, d_vector, dim);

	extern __shared__ float squares[];	//Array to be used for reduction addition
	
	AcceleratedVector_dMagnitude(d_mag, d_vector, dim, squares);

	/*
	if (threadIdx.x < dim)
	{
		//Fill squares with the square of each component
		squares[threadIdx.x] = powf(d_vector[threadIdx.x], 2.0f);
	}

	//Wait for all threads to complete
	__syncthreads();

	//Begin reduction
	AcceleratedVector_dReduceArray(squares, dim);

	//Wait for all threads to complete
	__syncthreads();

	if (threadIdx.x == 0)
	{
		d_mag[0] = sqrtf(squares[0]);
	}
	*/
}


///
//Calculates the magnitude of a Vector on the GPU
//This function is only callable from the device
//Each thread is in charge of squaring it's respective element. Then reduction
// is used to find the sum and that is returned.
//
//Parameters:
//	d_mag: A device pointer to a float to store the magnitude
//	d_vector: A device pointer to an array of floats representing the vector to get he magnitude of
//	dim: The dimension of the vector tg et the magnitude from
//	d_sharedStorage: A device pointer to shared memory with dim * sizeof(vector element) space. This is to perform reduction.
__device__ void AcceleratedVector_dMagnitude(float* d_mag, const float* d_vector, const unsigned int dim, float* d_sharedStorage)
{
	/*
	if (threadIdx.x < dim)
	{
		//Fill squares with the square of each component
		d_sharedStorage[threadIdx.x] = powf(d_vector[threadIdx.x], 2.0f);
	}



	//Wait for all threads to complete
	__syncthreads();

	//Begin array reduction
	AcceleratedVector_dReduceArray(d_sharedStorage, dim);

	//Wait for all threads to complete
	__syncthreads();

	if (threadIdx.x == 0)
	{
		d_mag[0] = sqrtf(d_sharedStorage[0]);
	}*/
	AcceleratedVector_dMagnitudeSq(d_mag, d_vector, dim, d_sharedStorage);
	
	if (threadIdx.x == 0)
	{
		d_mag[0] = sqrtf(d_mag[0]);
	}

}

///
//Calculates the magnitude squared of a vector on the GPU
//This function is only callable from the device
//
//Parameters:
//	d_mag: A device pointer to a float to store the magnitude
//	d_vector: A device pointer to an array of floats representing the vector to get he magnitude of
//	dim: The dimension of the vector tg et the magnitude from
//	d_sharedStorage: A device pointer to shared memory with dim * sizeof(vector element) space
__device__ void AcceleratedVector_dMagnitudeSq(float* d_mag, const float* d_vector, const unsigned int dim, float* d_sharedStorage)
{
	//int index = threadIdx.x + blockIdx.x + blockDim.x;
	if (threadIdx.x < dim)
	{
		//Fill squares with the square of each component
		d_sharedStorage[threadIdx.x] = powf(d_vector[threadIdx.x], 2.0f);
	}



	//Wait for all threads to complete
	__syncthreads();

	//Begin array reduction
	AcceleratedVector_dReduceArray(d_sharedStorage, dim);

	//Wait for all threads to complete
	__syncthreads();

	if (threadIdx.x == 0)
	{
		d_mag[0] = d_sharedStorage[0];
	}
}

///
//Gets a Normalized a vector the GPU
//First determines gridsize and blocksize then calls AcceleratedVector_Normalize
//
//Parameters:
//	d_dest: Device pointer to an array to store the components of the normalized src
//	d_src: Device pointer to an array representing the components of the vector to normalize
//	dim: The dimension of th vector
void AcceleratedVector_LaunchGetNormalize(float* d_dest, const float* d_src, const unsigned int dim)
{
	AcceleratedVector_GetNormalize << <1, dim, dim * sizeof(float) >> >(d_dest, d_src, dim);
}

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
__global__ void AcceleratedVector_GetNormalize(float* d_dest, const float* d_src, const unsigned int dim)
{
	extern __shared__ float reductionStorage[];	//Used for performing reduction in AcceleratedVector_dMagnitude
	__shared__ float magnitude;

	AcceleratedVector_dMagnitude(&magnitude, d_src, dim, reductionStorage);

	//Wait for magnitude
	__syncthreads();

	if (magnitude != 0)
	{
		AcceleratedVector_dGetScalarProduct(d_dest, d_src, 1.0f / magnitude, dim);
	}
}

///
//Normalizes a vector on the GPU
//First determines gridsize and blocksize then calls AcceleratedVector_Normalize
//
//PArameters:
//	d_vec: A device pointer to an array of floats representing the components to normalize
//	dim: The dimension of the vector
void AcceleratedVector_LaunchNormalize(float* d_vec, const unsigned int dim)
{
	AcceleratedVector_Normalize << <1, dim, dim * sizeof(float) >> >(d_vec, dim);


}

///
//Normalizes a vector on the GPU
//
//PArameters:
//	<<<Grid Size, Block Size, Dynamic Allocation>>>
//	Grid Size: The number of blocks to execute in parallel. Should be 1 for this algorithm
//	Block Size: The number of threads to run in parallel. Should be equal to the vector dimension for this algorithm
//	Dynamic Allocation: Algorithm needs additional memory on GPU to run equal to dim * sizeof(vector element)
//	d_vec: A device pointer to an array of floats representing the components to normalize
//	dim: The dimension of the vector
__global__ void AcceleratedVector_Normalize(float* d_vec, const unsigned int dim)
{
	extern __shared__ float reductionStorage[];	//Used for performing reduction in AcceleratedVector_dMagnitude
	__shared__ float magnitude;

	AcceleratedVector_dMagnitude(&magnitude, d_vec, dim, reductionStorage);


	//Wait for magnitude from thread0
	__syncthreads();

	if (magnitude != 0)
	{
		//Scale vector by inverse magnitude
		AcceleratedVector_dScale(d_vec, 1.0f / magnitude, dim);
	}
	
}


///
//Computes the dot product of two vectors on the GPU
//
//Parameters:
//	d_dest: Pointer to a float on the GPU to store the result of the dot product
//	d_vec1: a device pointer to an array of floats representing the first vector being dotted
//	d_vec2: a device pointer to an array of floats representing the second vector to be dotted
//	dim: The dimension of the vectors being dotted
void AcceleratedVector_LaunchDotProduct(float* d_dest, const float* d_vec1, const float* d_vec2, unsigned int dim)
{
	AcceleratedVector_DotProduct << <1, dim, dim * sizeof(float) >> >(d_dest, d_vec1, d_vec2, dim);
}


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
__global__ void AcceleratedVector_DotProduct(float* d_dest, const float* d_vec1, const float* d_vec2, unsigned int dim)
{
	extern __shared__ float products[];	//Array to be used for reduction addition

	AcceleratedVector_dDotProduct(d_dest, d_vec1, d_vec2, dim, products);

	/*
	if (threadIdx.x < dim)
	{
		products[threadIdx.x] = d_vec1[threadIdx.x] * d_vec2[threadIdx.x];

		__syncthreads();

		//Reduce array of products
		AcceleratedVector_dReduceArray(products, dim);

		if (threadIdx.x == 0)
		{
			d_dest[0] = products[0];
		}
	}
	*/

}

///
//Computes the dot product of two vectors on the GPU
//
//Parameters:
//	d_dest: Pointer to a float on the GPU to store the result of the dot product
//	d_vec1: a device pointer to an array of floats representing the first vector being dotted
//	d_vec2: a device pointer to an array of floats representing the second vector to be dotted
//	dim: The dimension of the vectors being dotted
//	reductionStorage: A device pointer to an array of floats with dim components to be used for reduction
__device__ void AcceleratedVector_dDotProduct(float* d_dest, const float* d_vec1, const float* d_vec2, unsigned int dim, float* reductionStorage)
{
	//int index = threadIdx.x + blockIdx.x * blockDim.x;
	if (threadIdx.x < dim)
	{
		//reductionStorage[threadIdx.x] = d_vec1[index] * d_vec2[index];
		reductionStorage[threadIdx.x] = d_vec1[threadIdx.x] * d_vec2[threadIdx.x];
	}
	//Wait for all threads to complete
	__syncthreads();

	//Reduce array to get sum
	AcceleratedVector_dReduceArray(reductionStorage, dim);

	if (threadIdx.x == 0)
	{
		d_dest[0] = reductionStorage[0];
	}


}

///
//Computes the dot product of multiple vectors on the GPU
//
//Parameters:
//	d_dest: A device pointer to an array of floats to store the respective dot product results
//	d_vecs1: A device pointer to an array of floats representing numVectors vectors of dimension dim as the LHS vectors
//	d_vecs2: A device pointer to an array of floats represnting numVectors vectors of dimension dim as the RHS vectors
//	dim: The dimension of the vectors being dotted
//	numVectors: number of vectors in each vector component array
void AcceleratedVector_LaunchDotProductAll(float* d_dest, const float* d_vecs1, const float* d_vecs2, unsigned int dim, unsigned int numVectors)
{
	AcceleratedVector_DotProductAll << <numVectors, dim, dim * sizeof(float) >> >(d_dest, d_vecs1, d_vecs2, dim, numVectors);
}

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
__global__ void AcceleratedVector_DotProductAll(float* d_dest, const float* d_vecs1, const float* d_vecs2, unsigned int dim, unsigned int numVectors)
{
	extern __shared__  float products[];

	float* d_blockDest = d_dest + (blockIdx.x);
	const float* d_blockVecs1 = d_vecs1 + (blockIdx.x * blockDim.x);
	const float* d_blockVecs2 = d_vecs2 + (blockIdx.x * blockDim.x);

	//AcceleratedVector_dDotProductAll(d_dest, d_vecs1, d_vecs2, dim, numVectors, products);
	AcceleratedVector_dDotProductAll(d_blockDest, d_blockVecs1, d_blockVecs2, dim, numVectors, products);
}

///
//Computes the dotprduct of multiple vectors on the GPU
//
//Parameters:
//	d_dest: A device pointer to the proper index in an array of floats to store respective dot product result
//	d_vecs1: A device pointer to an array of floats representing the concatenation of all LHS vecto components involved in dot products
//	d_vecs2: A device pointer to an array of floats representing the concatenated components of all RHS vectors involved in dot products
//	dim: The dimension of the vectors being dotted
//	numVectors: The number of vectors being
//	d_reductionStorage: A device pointer to an array of floats of length sizeof(VectorElement) * dim
__device__ void AcceleratedVector_dDotProductAll(float* d_dest, const float* d_vecs1, const float* d_vecs2, unsigned int dim, unsigned int numVectors, float* d_reductionStorage)
{
	//Calculate the index this thread will get the product from the LHS and RHS
	//int index = threadIdx.x + blockIdx.x * blockDim.x;

	//If the thread ID is within the dimension of the vector
	if (threadIdx.x < dim)
	{

		//Store the product of LHS[index] * RHS[index] in product at index corresponding to threadID
		//d_reductionStorage[threadIdx.x] = d_vecs1[index] * d_vecs2[index];
		d_reductionStorage[threadIdx.x] = d_vecs1[threadIdx.x] * d_vecs2[threadIdx.x];
	}
	//Wait for threads to finish computing all of products
	__syncthreads();

	//Use regression to find the sum of the products
	AcceleratedVector_dReduceArray(d_reductionStorage, dim);

	if (threadIdx.x == 0)
	{
		d_dest[0] = d_reductionStorage[0];
	}
}

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
void AcceleratedVector_LaunchDotProductAllWith(float* d_dest, const float* d_vec1, const float* d_vecs2, unsigned int dim, unsigned int numVectors)
{
	AcceleratedVector_DotProductAllWith << <numVectors, dim, sizeof(float)* dim >> >(d_dest, d_vec1, d_vecs2, dim, numVectors);
}

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
__global__ void AcceleratedVector_DotProductAllWith(float* d_dest, const float* d_vec1, const float* d_vecs2, unsigned int dim, unsigned int numVectors)
{
	extern __shared__ float products[];

	//Get products of each element
	if (threadIdx.x < dim)
	{
		products[threadIdx.x] = d_vec1[threadIdx.x] * d_vecs2[threadIdx.x + blockIdx.x * blockDim.x];
	}

	__syncthreads();

	//Use reduction to get sum of products
	AcceleratedVector_dReduceArray(products, dim);

	__syncthreads();

	if (threadIdx.x == 0)
	{
		d_dest[blockIdx.x] = products[0];
	}
}

///
//Projects d_vec1 onto d_vec2 storing the projection vector in d_dest on the GPU
//Detemrines the grid dimension and block dimension then calls AcceleratedVector_GetProjection
//
//Parameters:
//	d_dest: A device pointer to an array of floats to store the result of the projection
//	d_vec1: A device pointer to an array of floats representing the components of The vector being projected
//	d_vec2: A device pointer to an array of floats containing the components of the vector being projected onto
//	dim: Te dimension of the vectors (The vectors dimension should match)
void AcceleratedVector_LaunchGetProjection(float* d_dest, const float* d_vec1, const float* d_vec2, const unsigned int dim)
{
	AcceleratedVector_GetProjection << <1, dim, sizeof(float)* dim >> >(d_dest, d_vec1, d_vec2, dim);

}

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
__global__ void AcceleratedVector_GetProjection(float* d_dest, const float* d_vec1, const float* d_vec2, const unsigned int dim)
{
	extern __shared__ float reductionStorage[];
	__shared__ float numerator;		//U dot V
	__shared__ float denominator;	//Mag(V) Squared


	AcceleratedVector_dDotProduct(&numerator, d_vec1, d_vec2, dim, reductionStorage);
	//Wait for all threads to complete
	__syncthreads();

	AcceleratedVector_dMagnitudeSq(&denominator, d_vec2, dim, reductionStorage);

	__syncthreads();
	if (threadIdx.x == 0)
	{
		numerator = numerator / denominator;
	}

	__syncthreads();

	AcceleratedVector_dGetScalarProduct(d_dest, d_vec2, numerator, dim);
}

///
//Projects d_vec1 onto d_vec2 changing d_vec1 to represent the projection vector on the GPU
//Detemrines the grid dimension and block dimension then calls AcceleratedVector_LaunchProject
//
//Parameters:
//	d_vec1: A device pointer to an array of floats representing the components of The vector being projected
//	d_vec2: A device pointer to an array of floats containing the components of the vector being projected onto
//	dim: Te dimension of the vectors (The vectors dimension should match
void AcceleratedVector_LaunchProject(float* d_vec1, const float* d_vec2, const unsigned int dim)
{
	AcceleratedVector_Project << <1, dim, sizeof(float)* dim >> >(d_vec1, d_vec2, dim);
}

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
__global__ void AcceleratedVector_Project(float* d_vec1, const float* d_vec2, const unsigned int dim)
{
	extern __shared__ float reductionStorage[];
	__shared__ float numerator;		//U dot V
	__shared__ float denominator;	//Mag(V) Squared


	AcceleratedVector_dDotProduct(&numerator, d_vec1, d_vec2, dim, reductionStorage);
	//Wait for all threads to complete
	__syncthreads();

	AcceleratedVector_dMagnitudeSq(&denominator, d_vec2, dim, reductionStorage);

	__syncthreads();
	if (threadIdx.x == 0)
	{
		numerator = numerator / denominator;
	}


	__syncthreads();

	AcceleratedVector_dGetScalarProduct(d_vec1, d_vec2, numerator, dim);
}

///
//Projects each vector to be projected onto the respective vector being projected onto, altering the LHS vector to hold the solution
//Determines the grid size and block size then calls AcceleratedVector_ProjectAll
//
//Parameters:
//	d_vecs1: A device pointer to an array of floats representing the concatenated components of each vector being projected.
//	d_vecs2: A device pointer to an array of floats representing the concatenated components of each vector being projected onto.
//	dim: The dimension of the vectors being projected
//	numVectors: The number of vectors being projected
void AcceleratedVector_LaunchProjectAll(float* d_vecs1, const float* d_vecs2, const unsigned int dim, const unsigned int numVectors)
{
	AcceleratedVector_ProjectAll<<<numVectors, dim, sizeof(float)* dim>>>(d_vecs1, d_vecs2, dim, numVectors);
}

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
__global__ void AcceleratedVector_ProjectAll(float* d_vecs1, const float* d_vecs2, const unsigned int dim, const unsigned int numVectors)
{
	extern __shared__ float d_reductionStorage[];
	__shared__ float numerator;
	__shared__ float denominator;

	__shared__ float* d_blockVecs1;
	__shared__ const float* d_blockVecs2;

	if (threadIdx.x == 0)
	{
		d_blockVecs1 = d_vecs1 + (blockIdx.x * blockDim.x);
		d_blockVecs2 = d_vecs2 + (blockIdx.x * blockDim.x);
	}
	
	__syncthreads();

	//AcceleratedVector_dDotProduct(&numerator, d_vecs1, d_vecs2, dim * numVectors, d_reductionStorage);
	AcceleratedVector_dDotProduct(&numerator, d_blockVecs1, d_blockVecs2, dim, d_reductionStorage);

	__syncthreads();

	//AcceleratedVector_dMagnitudeSq(&denominator, d_vecs2, dim * numVectors, d_reductionStorage);
	AcceleratedVector_dMagnitudeSq(&denominator, d_blockVecs2, dim, d_reductionStorage);

	__syncthreads();

	if (threadIdx.x == 0)
	{
		numerator = numerator / denominator;
	}


	__syncthreads();

	//AcceleratedVector_dGetScalarProduct(d_vecs1, d_vecs2, numerator, dim * numVectors);
	AcceleratedVector_dGetScalarProduct(d_blockVecs1, d_blockVecs2, numerator, dim);

}

///
//Uses reduction techniques to compute the sum of components in the array storing te sum in the first element
//There must be at least arrSize threads.
//
//Parameters:
//	arrToReduce: The array to reduce
//	arrSize: The size of the array
__device__ void AcceleratedVector_dReduceArray(float* arrToReduce, const unsigned int arrSize)
{
	for (int i = 1; i < arrSize; i *= 2)
	{
		//Add every other, then every 4, then every 8 ... indices together!
		if (threadIdx.x % (i * 2) == 0 && threadIdx.x + i < arrSize)
		{
			arrToReduce[threadIdx.x] += arrToReduce[threadIdx.x + i];
		}
		__syncthreads();
	}
}