#ifndef _APP_H_
#define _APP_H_

class FlyCamera
{
public:
	FlyCamera() {}
	~FlyCamera(){}

	Mat4 m_view;
	Mat4 m_projection;
	Mat4 m_viewProjection;
	Mat4 m_rotationMatrix;
	v4f m_localPosition;
	f32 m_moveSpeed = 10.f;
	f32 m_moveSpeedDefault = 10.f;
	f32 m_fov = 1.74533f;
	f32 m_aspect = 1.3333333f;
	f32 m_near = 0.01f;
	f32 m_far = 1000.f;

	void Rotate(const v2f& mouseDelta, f32 dt) {
		f32 speed = 4.4f;
		Mat4 RX;
		Mat4 RY;
		bool update = false;
		if (mouseDelta.x != 0.f)
		{
			update = true;
			RY.setRotation(Quat(v4f(0.f, math::degToRad(-mouseDelta.x) * dt * speed, 0.f, 0.f)));
		}
		if (mouseDelta.y != 0.f)
		{
			update = true;
			RX.setRotation(Quat(v4f(math::degToRad(-mouseDelta.y) * dt * speed, 0.f, 0.f, 0.f)));
		}

		if (update)
			m_rotationMatrix = RX * m_rotationMatrix * RY;
	}

	void MoveLeft(f32 dt){
		MoveCamera(v4f(-m_moveSpeed * dt, 0.f, 0.f, 1.f));
	}
	void MoveRight(f32 dt){
		MoveCamera(v4f(m_moveSpeed * dt, 0.f, 0.f, 1.f));
	}
	void MoveUp(f32 dt){
		MoveCamera(v4f(0.f, m_moveSpeed * dt, 0.f, 1.f));
	}
	void MoveDown(f32 dt){
		MoveCamera(v4f(0.f, -m_moveSpeed * dt, 0.f, 1.f));
	}
	void MoveBackward(f32 dt){
		MoveCamera(v4f(0.f, 0.f, m_moveSpeed * dt, 1.f));
	}
	void MoveForward(f32 dt){
		MoveCamera(v4f(0.f, 0.f, -m_moveSpeed * dt, 1.f));
	}
	void MoveCamera(const v4f& vec)
	{
		auto RotInv = m_rotationMatrix;
		RotInv.invert();
		auto vel = math::mul(vec, RotInv);
		m_localPosition += vel; // m_localPosition is just vec4 for position
	}
	void OnUpdate() {
		math::makePerspectiveRHMatrix(
			m_projection,
			m_fov,
			m_aspect,
			m_near,
			m_far);

		auto V = math::mul(-m_localPosition, m_rotationMatrix);
		m_view = m_rotationMatrix;
		m_view[3] = V;
		m_view[3].w = 1.f;
		m_viewProjection = m_projection * m_view;
	}
};

class Application
{
public:
	Application();
	~Application();

	miInputContext* m_inputContext = nullptr;
	miLibContext* m_libContext = nullptr;
	miWindow* m_mainWindow = nullptr;
	f32 m_dt = 0.f;
	miVideoDriver* m_gpu = nullptr;
	v2f m_gpuDepthRange;
	FlyCamera* m_activeCamera = 0;
	FlyCamera* m_cameraFly = 0;

	void OnCreate(const char*);
	void MainLoop();
	void WriteLog(const char* message);
	
};

#endif