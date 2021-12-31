#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "MapCell.h"

class Player
{
public:
	Player();
	~Player();

	s32 m_cellID = -1;
	CellData m_cellData;


	v3f m_position;
	miCameraFly* m_cameraFly = 0;

	void MoveLeft(f32 dt);
	void MoveRight(f32 dt);
	void MoveUp(f32 dt);
	void MoveDown(f32 dt);
	void MoveBackward(f32 dt);
	void MoveForward(f32 dt);
};


#endif