#include "pch.h"
#include "QuadTree.h"
#include "Collision.h"
#include "GameObject.h"

QuadNode::~QuadNode()
{
	if (nw != nullptr)
	{
		delete nw;
		nw = nullptr;
	}

	if (ne != nullptr)
	{
		delete ne;
		ne = nullptr;
	}

	if (sw != nullptr)
	{
		delete sw;
		sw = nullptr;
	}

	if (se != nullptr)
	{
		delete se;
		se = nullptr;
	}

}

bool QuadNode::IsBoundOverlapped(Collision* collision)
{
	Bound cb = collision->GetBound();

	float cmaxX = cb.BottomRight.x;
	float cminX = cb.topLeft.x;
	float cmaxY = cb.BottomRight.y;
	float cminY = cb.topLeft.y;

	float maxX = bound.BottomRight.x;
	float minX = bound.topLeft.x;
	float maxY = bound.BottomRight.y;
	float minY = bound.topLeft.y;

	return !(cmaxX < cminX || maxX < cminX || cmaxY < minY || maxY < cminY);

}

bool QuadNode::IsInBound(Collision* collision)
{
	Bound cb = collision->GetBound();

	Vector bottomRight = cb.BottomRight;
	Vector topLeft = cb.topLeft;

	return bound.IsInBound(bottomRight.x, bottomRight.y) && bound.IsInBound(topLeft.x, topLeft.y);
}

bool QuadNode::IsInBound(Vector pos)
{
	return bound.IsInBound(pos.x, pos.y);
}

void QuadNode::GetCollideObjects(Collision* collision, vector<GameObjectRef>& results)
{
	for (int32 i = 0; i < objects.size(); ++i)
	{
		GameObjectRef object = objects[i].lock();
		if (object)
		{
			if (ComponentBase* collisionComponent = object->GetComponent(EComponentType::ECT_COLLISION))
			{
				if (static_cast<Collision*>(collisionComponent)->CheckCollision(collision))
				{
					results.push_back(object);
				}
			}
			
		}
	}
}

void QuadNode::Clear()
{

	objects.clear();
	if (nw)
	{
		nw->Clear();
	}
	if (ne)
	{
		ne->Clear();
	}
	if (sw)
	{
		sw->Clear();
	}
	if (se)
	{
		se->Clear();
	}

}

QuadTree::QuadTree()
{

}

QuadTree::~QuadTree()
{
	delete _head;
}

void QuadTree::Init(Bound bound)
{
	_head = new QuadNode();
	_head->bound = bound;
}

void QuadTree::Insert(GameObjectRef object)
{
	QuadNode* node = _head;

	Collision* collision = static_cast<Collision*>(object->GetComponent(EComponentType::ECT_COLLISION));

	if (collision == nullptr)
		return;

	while (node)
	{

		Vector topLeft = node->bound.topLeft;
		Vector bottomRight = node->bound.BottomRight;

		Vector boundSize = node->bound.GetSize();
		if (boundSize < _minSize)
		{
			break;
		}

		if (node->nw == nullptr)
		{
			node->nw = new QuadNode();
			node->nw->bound = Bound(topLeft.x, topLeft.y, (bottomRight.x + topLeft.x) / 2, (bottomRight.y + topLeft.y) / 2);
		}

		if (node->ne == nullptr)
		{
			node->ne = new QuadNode();
			node->ne->bound = Bound((bottomRight.x + topLeft.x) / 2, topLeft.y, bottomRight.x, (bottomRight.y + topLeft.y) / 2);
		}

		if (node->se == nullptr)
		{
			node->se = new QuadNode();
			node->se->bound = Bound((bottomRight.x + topLeft.x) / 2, (bottomRight.y + topLeft.y) / 2, bottomRight.x, bottomRight.y);
		}

		if (node->sw == nullptr)
		{
			node->sw = new QuadNode();
			node->sw->bound = Bound(topLeft.x, (bottomRight.y + topLeft.y) / 2, (bottomRight.x + topLeft.x) / 2, bottomRight.y);
		}

		if (node->nw->IsInBound(collision))
		{
			node = node->nw;

		}

		else if (node->ne->IsInBound(collision))
		{
			node = node->ne;

		}

		else if (node->sw->IsInBound(collision))
		{
			node = node->sw;

		}

		else if (node->se->IsInBound(collision))
		{
			node = node->se;

		}
		else
		{
			break;
		}
	}

	node->objects.push_back(object);
}

vector<GameObjectRef> QuadTree::Check(Collision* collision)
{

	Bound cBound = collision->GetBound();

	vector<GameObjectRef> result;

	queue<QuadNode*> q;
	q.push(_head);

	while (!q.empty())
	{
		QuadNode* node = q.front();
		q.pop();

		// check collision
		node->GetCollideObjects(collision, result);

		if (node->ne)
		{
			if (node->ne->IsBoundOverlapped(collision))
			{
				q.push(node->ne);
			}
		}

		if (node->nw)
		{
			if (node->nw->IsBoundOverlapped(collision))
			{
				q.push(node->nw);
			}
		}

		if (node->se)
		{
			if (node->se->IsBoundOverlapped(collision))
			{
				q.push(node->se);
			}
		}

		if (node->sw)
		{
			if (node->sw->IsBoundOverlapped(collision))
			{
				q.push(node->sw);
			}
		}
	}
	return result;
}

bool QuadTree::Check(GameObjectRef object, vector<GameObjectRef> output)
{
	if (object == nullptr)
		return false;

	if (Collision* collision = static_cast<Collision*>(object->GetComponent(EComponentType::ECT_COLLISION)))
	{
		output = Check(collision);
		return true;
	}

	return false;
}

void QuadTree::Clear()
{
	if (_head)
		_head->Clear();
}