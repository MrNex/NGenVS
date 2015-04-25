#include "ObjectManager.h"
#include <stdio.h>

///
//Initializes the Object manager
void ObjectManager_Initialize(void)
{
	objectBuffer = ObjectManager_AllocateBuffer();
	ObjectManager_InitializeBuffer(objectBuffer);
}

///
//Frees all internal data managed by the Object Manager
void ObjectManager_Free(void)
{
	ObjectManager_FreeBuffer(objectBuffer);
}

///
//Gets the internal Object Buffer being managed by the Object Manager
//
//Returns:
//	Internal Object Buffer of Object Manager
ObjectBuffer ObjectManager_GetObjectBuffer(void)
{
	return *objectBuffer;
}

///
//Updates the internal state of all contained objects.
void ObjectManager_Update(void)
{
	struct LinkedList_Node* current = objectBuffer->gameObjects->head;

	while (current != NULL)
	{
		GObject* gameObj = (GObject*)(current->data);

		GObject_Update(gameObj);

		current = current->next;
	}
}

///
//Updates the internal state of the OctTree
void ObjectManager_UpdateOctTree(void)
{
	struct LinkedList_Node* current = objectBuffer->gameObjects->head;

	while(current != NULL)
	{
		GObject* gameObj = (GObject*)current->data;
		//Find all gameObjects which have entries in the octtree (& treemap)
		if(gameObj->collider != NULL)
		{
			//Get the treemap entry
			DynamicArray* log = (DynamicArray*)HashMap_LookUp(objectBuffer->treeMap, gameObj, sizeof(GObject*))->data;
			//For each OctTree_Node the gobject was in
			
			if(log->size == 0)
			{
				OctTree_AddAndLog(objectBuffer->octTree, log, gameObj);
			}
			for(int i = 0; i < log->size; i++)
			{
				struct OctTree_NodeStatus* nodeStatus = (struct OctTree_NodeStatus*)DynamicArray_Index(log, i);
				//get it's current status for this node
				unsigned char currentStatus = OctTree_Node_DoesObjectCollide(nodeStatus->node, gameObj);

				//If the status has remained the same, continue to the next one
				if(nodeStatus->collisionStatus == currentStatus)
				{
					//Did the node subdivide?
					if(nodeStatus->node->children != NULL)
					{
						struct OctTree_Node* node = nodeStatus->node;
						//Remove this index from log
						DynamicArray_Remove(log, i);
						//Re-add to this node
						OctTree_Node_AddAndLog(objectBuffer->octTree, log, node, gameObj);
					}
					continue;
				}
				//If the current status says that the object is no longer in the node
				if(currentStatus == 0)
				{
					printf("Left tree\n");

					//Remove the object from this node
					OctTree_Node_Remove(nodeStatus->node, gameObj);
					//Find where it moved
					struct OctTree_Node* containingNode = OctTree_SearchUp(nodeStatus->node, gameObj);
					//If it is still in a node
					if(containingNode != NULL)
					{
						//Add it to that node!
						OctTree_Node_AddAndLog(objectBuffer->octTree, log, containingNode, gameObj);
					}
					//Remove the ith nodeStatus from the log
					DynamicArray_Remove(log, i);
				}
				//Object was fully contained, and now it is not
				else if(currentStatus == 1)
				{
					printf("Leaving tree\n");

					//Update the status
					nodeStatus->collisionStatus = currentStatus;
					//Find where it moved
					struct OctTree_Node* containingNode = OctTree_SearchUp(nodeStatus->node, gameObj);
					//If it is even in a node anymore
					if(containingNode != NULL)
					{
						//Add it to that node!
						OctTree_Node_AddAndLog(objectBuffer->octTree, log, containingNode, gameObj);
					}
				}
				//Object was partially contained and now it is fully contained!
				else
				{
					printf("Entered tree\n");

					//Update the status
					nodeStatus->collisionStatus = currentStatus;
				}
			}
		}
		current = current->next;
	}
}

///
//Adds an object to collection of objects managed by the Object Manager
//
//Parameters:
//	obj: The object to add
void ObjectManager_AddObject(GObject* obj)
{
	LinkedList_Append(objectBuffer->gameObjects, obj);
	if(obj->collider != NULL)
	{
		//Create a hashmap entry for the object in the treemap
		DynamicArray* log = DynamicArray_Allocate();	//will log the octents this object is apart of
		DynamicArray_Initialize(log, sizeof(struct OctTree_NodeStatus));

		//OctTree_Add(objectBuffer->octTree, obj);
		//Add the object
		OctTree_AddAndLog(objectBuffer->octTree, log, obj);

		//Add the log to the treemap
		HashMap_Add(objectBuffer->treeMap, obj, log, sizeof(GObject*));
	}
}

///
//Removes an object from the collection of objects managed by the Object Manager
//Upon removal the Object Manager will NOT deallocate memory being used by the object!!!
//
//Parameters:
//	obj: The object to remove
void ObjectManager_RemoveObject(GObject* obj)
{
	LinkedList_RemoveValue(objectBuffer->gameObjects, obj);
	if(obj->collider != NULL)
	{
		OctTree_Remove(objectBuffer->octTree, obj);

		//Remove the object from the treemap
		DynamicArray* log = (DynamicArray*)HashMap_Remove(objectBuffer->treeMap, obj, sizeof(GObject*));

		//Delete the log
		DynamicArray_Free(log);
	}
}

///
//Removes an object from the collection of objects being managed by the object manager.
//Upon removal the object manager will deallocate memory being used by the object.
//
//Parameters:
//	obj: the object to remove and free
void ObjectManager_DeleteObject(GObject* obj)
{
	ObjectManager_RemoveObject(obj);
	GObject_Free(obj);
}


///
//Allocates a new Object buffer
//
//Returns:
//	Returns a pointer to a newly allocated Object Buffer
static ObjectBuffer* ObjectManager_AllocateBuffer(void)
{
	ObjectBuffer* buffer = (ObjectBuffer*)malloc(sizeof(ObjectBuffer));
	return buffer;
}

///
//Initializes an Object Buffer
//
//Parameters:
//	buffer: The object buffer to initialize
static void ObjectManager_InitializeBuffer(ObjectBuffer* buffer)
{
	buffer->gameObjects = LinkedList_Allocate();
	LinkedList_Initialize(buffer->gameObjects);

	buffer->octTree = OctTree_Allocate();
	OctTree_Initialize(buffer->octTree, -50.0f, 50.0f, -50.0f, 50.0f, -50.0f, 50.0f);

	buffer->treeMap = HashMap_Allocate();
	HashMap_Initialize(buffer->treeMap, 16);
}

///
//Frees resources being used by an Object Buffer
//
//Parameters:
//	buffer: The object buffer to free
static void ObjectManager_FreeBuffer(ObjectBuffer* buffer)
{
	//Free the hash map
	HashMap_Free(buffer->treeMap);

	//Free the oct tree
	OctTree_Free(buffer->octTree);

	//Delete all Objects being held in the object buffer
	struct LinkedList_Node* current = buffer->gameObjects->head;

	while (current != NULL)
	{
		GObject* data = (GObject*)current->data;
		GObject_Free(data);
		current = current->next;
	}

	//Now remove the list
	LinkedList_Free(buffer->gameObjects);

}