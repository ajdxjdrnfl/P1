#pragma once

struct QuadNode
{
public:
	~QuadNode();

	bool IsBoundOverlapped(class Collision* collision);

	bool IsInBound(class Collision* collision);

	bool IsInBound(Vector pos);

	void GetCollideObjects(class Collision* collision, vector<GameObjectRef>& results);

	void Clear();
	// child

public:
	Bound bound;
	vector<weak_ptr<GameObject>> objects;

	QuadNode* nw = nullptr;
	QuadNode* ne = nullptr;
	QuadNode* sw = nullptr;
	QuadNode* se = nullptr;
};

class QuadTree
{
public:
	QuadTree();
	~QuadTree();

public:
	void Init(Bound bound);

	// 트리 내에 object들을 업데이트
	void Insert(GameObjectRef object);

	// 트리 내에 object들과 collision을 체크
	vector<GameObjectRef> Check(class Collision* collision);

	bool Check(GameObjectRef object, vector<GameObjectRef> output);

	void Clear();

private:
	QuadNode* _head = nullptr;

	Vector _minSize = { 100.f, 100.f };
};