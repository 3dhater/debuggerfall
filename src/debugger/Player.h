#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "MapCell.h"

class btRigidBody;
class Player
{
public:
	Player();
	~Player();

	s32 m_cellID = -1;
	CellData m_cellData;

	btRigidBody* m_rigidBody = 0;

	v3f m_position;
	miCameraFly* m_cameraFly = 0;

	void MoveLeft(f32 dt);
	void MoveRight(f32 dt);
	void MoveUp(f32 dt);
	void MoveDown(f32 dt);
	void MoveBackward(f32 dt);
	void MoveForward(f32 dt);

	void MoveWRB(f32 dt);
	void MoveERB(f32 dt);
	void MoveSRB(f32 dt);
	void MoveNRB(f32 dt);
	void MoveNWRB(f32 dt);
	void MoveNERB(f32 dt);
	void MoveSWRB(f32 dt);
	void MoveSERB(f32 dt);
	void MoveRB(const v4f& v);

	void SetPosition(f32 x, f32 y, f32 z);
	void Update(f32 dt);
};


#endif