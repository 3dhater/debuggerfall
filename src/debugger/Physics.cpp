#include "Physics.h"

miPhysicsWorld::miPhysicsWorld() 
{

}

miPhysicsWorld::~miPhysicsWorld()
{

}

miCollisionShapeSphere* miPhysicsWorld::CreateCollisionShapeShpere(f32 radius)
{
	return miCreate<miCollisionShapeSphere>(radius);
}

void miPhysicsWorld::Update(f32 dt)
{
	for (u32 i = 0; i < m_bodies.m_size; ++i)
	{

	}
}

miRigidBody* miPhysicsWorld::CreateRigidBody(miCollisionShape* shape)
{
	miRigidBody* body = miCreate<miRigidBody>();
	body->m_shape = shape;
	m_bodies.push_back(body);
	return body;
}

void miPhysicsWorld::ClearWorld()
{
	m_bodies.clear();
}

void miPhysicsWorld::RemoveRigidBody(miRigidBody* rb)
{
	m_bodies.erase_first(rb);
}



