#include "mi/MainSystem/MainSystem.h"
#include "mi/Mesh/mesh.h"
#include "mi/GraphicsSystem/GPUMesh.h"
#include "mi/Scene/common.h"
#include "mi/Scene/cameraFly.h"

#include "application.h"
#include "Player.h"

#include "btBulletDynamicsCommon.h"


extern Application* g_app;

Player::Player() 
{
	m_cameraFly = new miCameraFly;
	m_cameraFly->m_localPosition.set(0.f, 0.0002f, 0.f, 0.f);
	m_cameraFly->m_near = 0.0001000f;
	m_cameraFly->m_far = 1000.f;
	m_cameraFly->m_moveSpeedDefault = 0.001f;
	m_cameraFly->m_moveSpeedDefault = 0.001f;
	{
		Mat4 lookAt;
		math::makeLookAtRHMatrix(lookAt, m_cameraFly->m_localPosition, v4f(0.f, 0.f, 1.f, 0.f), v4f(0.f, 1.f, 0.f, 0.f));
		m_cameraFly->m_rotationMatrix.setBasis(lookAt);
	}

	SetPosition(0.f, 0.f, 0.f);
}

Player::~Player()
{
	delete m_cameraFly;
}

void Player::MoveLeft(f32 dt)
{
	m_cameraFly->MoveLeft(dt);
	m_position = m_cameraFly->m_localPosition;
	g_app->m_needUpdateMapCell = true;
}

void Player::MoveRight(f32 dt)
{
	m_cameraFly->MoveRight(dt);
	m_position = m_cameraFly->m_localPosition;
	g_app->m_needUpdateMapCell = true;
}

void Player::MoveUp(f32 dt)
{
	m_cameraFly->MoveUp(dt);
	m_position = m_cameraFly->m_localPosition;
	g_app->m_needUpdateMapCell = true;
}

void Player::MoveDown(f32 dt)
{
	m_cameraFly->MoveDown(dt);
	m_position = m_cameraFly->m_localPosition;
	g_app->m_needUpdateMapCell = true;
}

void Player::MoveBackward(f32 dt)
{
	m_cameraFly->MoveBackward(dt);
	m_position = m_cameraFly->m_localPosition;
	g_app->m_needUpdateMapCell = true;
}

void Player::MoveForward(f32 dt)
{
	m_cameraFly->MoveForward(dt);
	m_position = m_cameraFly->m_localPosition;
	g_app->m_needUpdateMapCell = true;

}

void Player::MoveNWRB(f32 dt)
{
	MoveRB(v4f(-m_cameraFly->m_moveSpeed * dt, 0.f, -m_cameraFly->m_moveSpeed * dt, 1.f));
}
void Player::MoveNERB(f32 dt)
{
	MoveRB(v4f(m_cameraFly->m_moveSpeed * dt, 0.f, -m_cameraFly->m_moveSpeed * dt, 1.f));
}
void Player::MoveSWRB(f32 dt)
{
	MoveRB(v4f(-m_cameraFly->m_moveSpeed * dt, 0.f, m_cameraFly->m_moveSpeed * dt, 1.f));
}
void Player::MoveSERB(f32 dt)
{
	MoveRB(v4f(m_cameraFly->m_moveSpeed * dt, 0.f, m_cameraFly->m_moveSpeed * dt, 1.f));
}
void Player::MoveWRB(f32 dt)
{
	MoveRB(v4f(-m_cameraFly->m_moveSpeed * dt, 0.f, 0.f, 1.f));
}
void Player::MoveERB(f32 dt)
{
	MoveRB(v4f(m_cameraFly->m_moveSpeed * dt, 0.f, 0.f, 1.f));
}
void Player::MoveSRB(f32 dt)
{
	MoveRB(v4f(0.f, 0.f, m_cameraFly->m_moveSpeed * dt, 1.f));
}
void Player::MoveNRB(f32 dt)
{
	MoveRB(v4f(0.f, 0.f, -m_cameraFly->m_moveSpeed * dt, 1.f));
}

void Player::MoveRB(const v4f& vec)
{
	auto RotInv = m_cameraFly->m_rotationMatrix;
	RotInv.invert();
	auto vel = math::mul(vec, RotInv) * 3.1f;
	m_rigidBody->setLinearVelocity(btVector3(
		vel.x, 
		vel.y,
		vel.z));
}

void Player::SetPosition(f32 x, f32 y, f32 z)
{
	m_position.set(x, y, z);
	m_cameraFly->m_localPosition.set(x, y, z, 0.f);

	{
		if (m_rigidBody)
		{
			if (m_rigidBody->getMotionState())
				delete m_rigidBody->getMotionState();

			g_app->m_physics->m_world->removeCollisionObject(m_rigidBody);

			if (m_rigidBody->getCollisionShape())
				delete m_rigidBody->getCollisionShape();

			delete m_rigidBody;
		}

		btScalar mass(10.01);
		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(btVector3(m_position.x, m_position.y, m_position.z));
		btVector3 localInertia(0, 0, 0);
		btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);

		auto m_collisionShape = new btCapsuleShape(0.0005, 0.0017f);
		//m_collisionShape->setMargin(0.0000001f);

		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, m_collisionShape, localInertia);

		m_rigidBody = new btRigidBody(rbInfo);
		m_rigidBody->setActivationState(DISABLE_DEACTIVATION);
		m_rigidBody->setAngularFactor(btVector3(0.f, 0.f, 0.f));
		m_rigidBody->setCcdMotionThreshold(0.00001);
		m_rigidBody->setCcdSweptSphereRadius(0.0002);
		m_rigidBody->setFriction(1.f);
		//m_rigidBody->setContactProcessingThreshold(0.01f);
		
		g_app->m_physics->m_world->addRigidBody(m_rigidBody);
	}
	Update(0.f);
}

void Player::Update(f32 dt)
{
	if (m_rigidBody)
	{
		btTransform tr;
		m_rigidBody->getMotionState()->getWorldTransform(tr);

		auto origin = tr.getOrigin();
		m_cameraFly->m_localPosition.x = origin.m_floats[0];
		m_cameraFly->m_localPosition.y = origin.m_floats[1];
		m_cameraFly->m_localPosition.z = origin.m_floats[2];
		m_position = m_cameraFly->m_localPosition;
	}

//	printf("%f %f %f\n", m_cameraFly->m_localPosition.x, m_cameraFly->m_localPosition.y, m_cameraFly->m_localPosition.z);
	g_app->m_needUpdateMapView = true;
	g_app->m_needUpdateMapCell = true;
	g_app->m_cameraWasMoved = true;
	m_cameraFly->NeedUpdate();
	m_cameraFly->OnUpdate();
}
