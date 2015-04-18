#include <GL/glew.h>
#include <GL/freeglut.h>

#include "ShaderProgram.h"
#include "Camera.h"

#include "GObject.h"

#include "LinkedList.h"

typedef struct RenderingBuffer
{
	ShaderProgram** shaderPrograms;
	Camera* camera;
	Vector* directionalLightVector;
} RenderingBuffer;

//Internals
static RenderingBuffer* renderingBuffer;

///
//Allocates memory for a rendering buffer
//
//Returns:
//	Pointer to a newly allocated rendering buffer
static RenderingBuffer* RenderingManager_AllocateBuffer(void);

///
//Initializes a rendering buffer
//
//Parameters:
//	buffer: Rendering buffer to initialize
static void RenderingManager_InitializeBuffer(RenderingBuffer* buffer);

///
//Frees the memory consumed by a Rendering Buffer
//
//Parameters:
//	buffer: The buffer to free
static void RenderingManager_FreeBuffer(RenderingBuffer* buffer);


//Functions

///
//Initialize the Rendering Manager
void RenderingManager_Initialize(void);

///
//Frees resources taken up by the RenderingManager
void RenderingManager_Free(void);

///
//Renders a gameobject as it's mesh.
//
//Parameters:
//	GO: Game object to render
void RenderingManager_Render(LinkedList* GameObjects);


///
//Gets the Rendering Manager's internal Rendering Buffer
//
//Returns:
//	RenderingManager's internal Rendering Buffer
RenderingBuffer RenderingManager_GetRenderingBuffer();
