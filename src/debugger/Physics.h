#ifndef _PHYSICS_H_
#define _PHYSICS_H_

#include "mi/MainSystem/MainSystem.h"
#include "mi/Mesh/mesh.h"

enum class miCollisionShapeType
{
	Sphere
};

// Все Collision Shape должны существовать поку идёт симуляция.
class miCollisionShape
{
public:
	miCollisionShape() {}
	virtual ~miCollisionShape() {}

	Aabb m_aabb;
	virtual void UpdateAABB() = 0;

	miCollisionShapeType m_type = miCollisionShapeType::Sphere;
};

class miCollisionShapeSphere : public miCollisionShape
{
public:
	miCollisionShapeSphere(f32 radius) : m_radius(radius) { UpdateAABB(); }
	virtual ~miCollisionShapeSphere() {}

	f32 m_radius = 0.f;
	virtual void UpdateAABB() override 
	{ 
		m_aabb.m_min.set(-m_radius);
		m_aabb.m_max.set(m_radius);
	}
};

// miDestroy. Удалять самому.
class miRigidBody
{
public:
	miRigidBody() {}
	~miRigidBody() {}

	miCollisionShape* m_shape = 0;
	
	//miArray<miRigidBody*> m_;
};

class miPhysicsWorld
{
public:
	miPhysicsWorld();
	~miPhysicsWorld();

	v3f m_gravity;

	// miDestroy
	miCollisionShapeSphere* CreateCollisionShapeShpere(f32 radius);

	miArray<miRigidBody*> m_bodies;

	miRigidBody* CreateRigidBody(miCollisionShape*);
	// перед ручным удалением нужно убрать тело из мира 
	void RemoveRigidBody(miRigidBody*);

	void Update(f32 dt);

	// only m_bodies.clear();
	// это не уничтожит miRigidBody
	void ClearWorld();
};

#endif