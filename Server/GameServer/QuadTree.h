#pragma once

struct QuadNode
{
public:
	~QuadNode();

	bool IsBoundOverlapped(class Collision* collision);
	bool IsBoundOverlapped(Bound bound);

	bool IsInBound(class Collision* collision);
	bool IsInBound(Bound cb);

	bool IsInBound(Vector pos);

	void GetCollideObjects(class Collision* collision, vector<GameObjectRef>& results);

	void Clear();
	// child

public:
	Bound bound;
	vector<weak_ptr<GameObject>> objects;

	QuadNode* parent = nullptr;
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

	// Ʈ�� ���� object���� ������Ʈ
	void Insert(GameObjectRef object);

	// Ʈ�� ���� object��� collision�� üũ
	vector<GameObjectRef> Check(class Collision* collision);

	bool Check(GameObjectRef object, vector<GameObjectRef> output);

	void Clear();

public:
	QuadNode* GetNode(GameObjectRef object);
	void GetAroundPlayers(GameObjectRef object, Vector maxDistance, vector<PlayerRef>& outputPlayers);

private:
	QuadNode* _head = nullptr;

	Vector _minSize = { 100.f, 100.f };
};