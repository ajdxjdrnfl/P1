#pragma once
#include "ResourceBase.h"
class Collider :
    public ResourceBase
{
public:
    Collider(EColliderType colliderType) : _colliderType(colliderType) { }
    virtual ~Collider() { }

    EColliderType GetColliderType() { return _colliderType; }


protected:
    EColliderType _colliderType;
};

class ColliderBox :
     public Collider
{
    using Super = Collider;
public:
    ColliderBox() : Super(EColliderType::COLLIDER_BOX) { }
    virtual ~ColliderBox() { }

    virtual void Load(const filesystem::path& path) override;

public:
    Vector extent;
};

class ColliderCircle :
    public Collider
{
    using Super = Collider;
public:
    ColliderCircle() : Super(EColliderType::COLLIDER_CIRCLE) { }
    virtual ~ColliderCircle() { }

    virtual void Load(const filesystem::path& path) override;

public:
    float _radius;
};

class ColliderRay :
    public Collider
{
    using Super = Collider;
public:
    ColliderRay() : Super(EColliderType::COLLIDER_RAY) { }
    virtual ~ColliderRay() { }

    virtual void Load(const string& path) { };

public:
    Vector _start = { 0,0 };
    Vector _end = { 0, 0 };
};

