#include "PointQuadTree.hpp"

/*

Point QuadTree
-------------------
	Data structure adapted from binary trees used to represent 2D information. Quadtrees have the following feature

		1. They decompose space into adaptable cells
		2. Each cell (or bucket) has a maximum capacity. When maximum capacity is reached, the bucket splits
		3. The tree directory follows the spatial decomposition of the quadtree.

	New points are added such that the cell that contains it is divided into quadrants by the vertical and horizontal lines that run through the point.
	Consequently, cells are rectangular but not necessarily square. 

	Since the division of the plane is decided by the order of point-insertion, 
	the tree's height is sensitive to and dependent on insertion order. 
	Inserting in a "bad" order can lead to a tree of height linear in the number of input points (at which point it becomes a linked-list). 
	If the point-set is static, pre-processing can be done to create a tree of balanced height.
*/

/*
	Contructor
*/
template <typename T>
PointQuadTree<T>::PointQuadTree(Rect_AABB _boundaries) : northWest(nullptr), northEast(nullptr), southEast(nullptr), southWest(nullptr)
{
	boundaries = _boundaries;
	points = {};
}

/*
	Destructor
*/
template <typename T>
PointQuadTree<T>::~PointQuadTree()
{
	delete northWest;
	delete northEast;
	delete southEast;
	delete southWest;

}

/*
	Insert
	----
	Returns true or false depending on whether successful insertion, subdividing each node when necessary.

	Time complexity
	---------------

	Space complexity
	---------------

*/
template <typename T>
bool PointQuadTree<T>::Insert(Data_Node<T> data) {
	//If data does not belong in our point
	if (!boundaries.contains(data.point)) {
		return false;
	}

	//If data belongs here and we are not full
	if (points.size() < QT_NODE_MAX_CAPACITY) {
		points.push_back(data);
		return true;
	}

	//We have too much data and should split it into Quads
	//If any of the quadrants are created, none of them are null so
	//one check is enough
	if (northWest == nullptr)
		Subdivide();

	if (northWest->Insert(data)) return true;
	if (northEast->Insert(data)) return true;
	if (southEast->Insert(data)) return true;
	if (southWest->Insert(data)) return true;

	//otherwise node cannot be inserted. This should never happen
	return false;
}

/*
Subdivide
----
Creates four equally spaced rectangle nodes as children.

Time complexity
---------------

Space complexity
---------------

*/
template <typename T>
void PointQuadTree<T>::Subdivide() {
	Vector2 children_size = { boundaries.halfDimension.x, boundaries.halfDimension.y };

	Vector2 child_center = {boundaries.centerPoint.x - children_size.x, boundaries.centerPoint.y - children_size.y};
	southWest = new PointQuadTree(Rect_AABB(child_center, children_size));

	child_center = { boundaries.centerPoint.x + children_size.x, boundaries.centerPoint.y - children_size };
	southEast = new PointQuadTree(Rect_AABB(child_center, children_size));

	child_center = { boundaries.centerPoint.x - children_size, boundaries.centerPoint.y + children_size };
	northWest = new PointQuadTree(Rect_AABB(child_center, children_size));

	child_center = { boundaries.centerPoint.x + children_size, boundaries.centerPoint.y + children_size };
	northEast = new PointQuadTree(Rect_AABB(child_center_children_size));
}

/*
QueryRange
----
Public function that hide the recursive function QueryRange_private_recursive.

Time complexity
---------------

Space complexity
---------------
	O(1)
*/
template <typename T>
std::vector<Data_Node<T>> PointQuadTree<T>::QueryRange(Rect_AABB& range) {
	std::vector<Data_Node<T>> results = {};
	QueryRange_private_recursive(range, results);

	return results;
}

/*
QueryRange
----
Public function that hide the recursive function QueryRange_private_recursive.

Time complexity
---------------

Space complexity
---------------
O(1)
*/
template <typename T>
void PointQuadTree<T>::QueryRange_private_recursive(Rect_AABB& range, std::vector<Data_Node<T>>& result) {
	//abort if not within boundary
	if (!boundaries.Intersects_AABB(range)) {
		return;	//empty
	}

	//check objects at this level
	for each (auto point in points) {
		if (range.ContainsPoint(point)) {
			results.push_back(point);
		}
	}

	//terminite if no children
	if (northEast == nullptr)
		return results;

	//otherwise query all children for results
	northEast->QueryRange_private_recursive(range, result);
	northWest->QueryRange_private_recursive(range, result);
	southEast->QueryRange_private_recursive(range, result);
	southWest->QueryRange_private_recursive(range, result);
}