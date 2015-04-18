#include  "AssetManager.h"

#include "Generator.h"

//Functions

///
//Initializes the Asset MAnager
void AssetManager_Initialize(void)
{
	assetBuffer = AssetManager_AllocateBuffer();
	AssetManager_InitializeBuffer(assetBuffer);

}

///
//Frees all resources allocated by the asset manager
//And all data with pointers being held by the asset manager
//You should probably only do this when you're closing the engine.
void AssetManager_Free(void)
{
	AssetManager_FreeBuffer(assetBuffer);
}

///
//Gets the internal asset buffer being managed by the asset manager
//
//Returns:
//	Internal asset buffer of asset manager
AssetBuffer AssetManager_GetAssetBuffer(void)
{
	return *assetBuffer;
}

///
//Loads all of the engines assets into the internal asset buffer
void AssetManager_LoadAssets(void)
{
	//Load meshes
	HashMap_Add(assetBuffer->meshMap, "Cube", Loader_LoadOBJFile("./Assets/Models/cube.obj"));
	//HashMap_Add(assetBuffer->meshMap, "Cube", Generator_GenerateCubeMesh(2.0f));
	HashMap_Add(assetBuffer->meshMap, "Sphere", Loader_LoadOBJFile("./Assets/Models/sphere.obj"));
	//HashMap_Add(assetBuffer->meshMap, "Sphere", Generator_GenerateSphereMesh(2.0f, 25));
	HashMap_Add(assetBuffer->meshMap, "Cylinder", Loader_LoadOBJFile("./Assets/Models/cylinder.obj"));
	//HashMap_Add(assetBuffer->meshMap, "Cylinder", Generator_GenerateCylinderMesh(1.0f, 2.0f, 25));
	HashMap_Add(assetBuffer->meshMap, "Cone", Loader_LoadOBJFile("./Assets/Models/cone.obj"));
	//HashMap_Add(assetBuffer->meshMap, "Cone", Generator_GenerateConeMesh(1.0f, 2.0f, 25));
	HashMap_Add(assetBuffer->meshMap, "Pipe", Loader_LoadOBJFile("./Assets/Models/pipe.obj"));
	//HashMap_Add(assetBuffer->meshMap, "Pipe", Generator_GenerateTubeMesh(1.0f, 0.5f, 2.0f, 25));
	HashMap_Add(assetBuffer->meshMap, "Torus", Loader_LoadOBJFile("./Assets/Models/torus.obj"));
	//HashMap_Add(assetBuffer->meshMap, "Torus", Generator_GenerateTorusMesh(2.0f, 1.0f, 10));

	HashMap_Add(assetBuffer->meshMap, "Suzanne", Loader_LoadOBJFile("./Assets/Models/suzanne.obj"));
	HashMap_Add(assetBuffer->meshMap, "Triangle", Loader_LoadOBJFile("./Assets/Models/triangle.obj"));
	HashMap_Add(assetBuffer->meshMap, "Square", Loader_LoadOBJFile("./Assets/Models/square.obj"));
	HashMap_Add(assetBuffer->meshMap, "Circle", Loader_LoadOBJFile("./Assets/Models/circle.obj"));
	HashMap_Add(assetBuffer->meshMap, "Tetrahedron", Loader_LoadOBJFile("./Assets/Models/tetrahedron.obj"));
	
	

	
	


	//Load textures
	struct Image* i = Loader_Load24BitBMPFile("./Assets/Textures/test.bmp");
	Texture* t = Texture_Allocate();
	Texture_Initialize(t, i);
	HashMap_Add(assetBuffer->textureMap, "Test", t);
	
	i = Loader_Load24BitBMPFile("./Assets/Textures/earth.bmp");
	t = Texture_Allocate();
	Texture_Initialize(t, i);
	HashMap_Add(assetBuffer->textureMap, "Earth", t);

	i = Loader_Load24BitBMPFile("./Assets/Textures/white.bmp");
	t = Texture_Allocate();
	Texture_Initialize(t, i);
	HashMap_Add(assetBuffer->textureMap, "White", t);

}

///
//Looks up a mesh from the asset manager's internal buffer
//
//Parameters:
//	key: Name of the mesh to lookup
//
//Returns:
//	Pointer to the requested mesh, or NULL if mesh was not found
Mesh* AssetManager_LookupMesh(char* key)
{
	return (Mesh*)HashMap_LookUp(assetBuffer->meshMap, key)->data;
}

///
//Looks up a texture from he asset manager's internal buffer
//
//Parameters
//	key: The name of the texture to lookup
//
//Returns:
//	Pointer to the requested texture, or NULL if the texture was not found
Texture* AssetManager_LookupTexture(char* key)
{
	return (Texture*)HashMap_LookUp(assetBuffer->textureMap, key)->data;
}

///
//Allocates a new AssetBuffer
//
//Returns:
//	Pointer to a newly allocated uninitialized asset buffer
static AssetBuffer* AssetManager_AllocateBuffer(void)
{
	AssetBuffer* buffer = (AssetBuffer*)malloc(sizeof(AssetBuffer));
	return buffer;
}

///
//Initializes an asset buffer
//
//Parameters:
//	buffer: pointer to the buffer to initialize
static void AssetManager_InitializeBuffer(AssetBuffer* buffer)
{
	buffer->meshMap = HashMap_Allocate();
	buffer->textureMap = HashMap_Allocate();
	HashMap_Initialize(buffer->meshMap, 11);
	HashMap_Initialize(buffer->textureMap, 10);
}

///
//Frees resuorces allocated by an Asset Buffer
//Deletes data being held in buffer!!
//
//PArameters:
//	buffer: pointer to The buffer to free
static void AssetManager_FreeBuffer(AssetBuffer* buffer)
{
	for (int i = 0; i < buffer->meshMap->size; i++)
	{
		Mesh_Free((Mesh*)buffer->meshMap->data[i]->data);
	}
	HashMap_Free(buffer->meshMap);

	for (int i = 0; i < buffer->textureMap->size; i++)
	{
		Texture_Free((Texture*)buffer->meshMap->data[i]->data);
	}

	HashMap_Free(buffer->textureMap);
}