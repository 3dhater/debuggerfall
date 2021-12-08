#include "mixer.lib.h"
#include "mixer.lib.mesh.h"
#include "mixer.lib.gpuMesh.h"

#include "application.h"
#include "Player.h"

extern Application* g_app;

Player::Player() 
{
	m_cameraFly = new FlyCamera;
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
