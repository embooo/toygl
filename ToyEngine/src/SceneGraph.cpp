#include "PCH.h"

/// <summary>
/// WIP
/// Implementation of a scene graph using directed graph
/// </summary>

using mat4 = glm::mat4;

struct Node
{
	int mesh;			
	int material;
	int parent;
	int left_child;		// Left neighbor  in the graph = id of first child,  -1 if none
	int right_sibling;	// Right neighbor in the graph = id of next sibling, -1 if none
};

struct Hierarchy
{
	int parent;
	int first_child;
	int next_sibling;
	int depth;			// Depth of the node from the tree's root
};

struct Scene
{
	std::vector<Hierarchy>			     hierarchy;
	std::vector<mat4>				     local_transforms;
	std::vector<mat4>				     global_transforms;
	std::vector<std::string>             node_names;				// Debug names
	std::vector<std::string>             material_names;

	std::unordered_map<int32_t, int32_t> mesh_of_node;				// Node Id -> Mesh
	std::unordered_map<int32_t, int32_t> material_of_node;			// Node Id -> Materials
	std::unordered_map<int32_t, int32_t> name_of_node;				// Node Id -> Node name
};

