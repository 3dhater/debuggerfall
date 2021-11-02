#ifndef _APP_H_
#define _APP_H_

class MapCell;
class Player;

class ApplicationGUI
{
public:
	ApplicationGUI();
	~ApplicationGUI();

	miGUIContext* m_context = 0;
	miGUIFont* m_fontDefault = 0;
	miGUIPanel* m_panel_terrain = 0;
	
	miGUIPanel* m_panel_debug = 0;
	miGUIText* m_debug_text_FPS = 0;
	miGUIText* m_debug_text_position = 0;
	miGUIText* m_debug_text_cameraCellID = 0;

	void Init();
};

struct miCameraFrustum
{
	v4f m_planes[6u];

	bool PointInFrustum(const v4f& point)
	{

		if ((
			m_planes[0].x * point.x +
			m_planes[0].y * point.y +
			m_planes[0].z * point.z +
			m_planes[0].w)
			< 0.f)
			return false;
		if ((
			m_planes[1].x * point.x +
			m_planes[1].y * point.y +
			m_planes[1].z * point.z +
			m_planes[1].w)
			< 0.f)
			return false;
		if ((
			m_planes[2].x * point.x +
			m_planes[2].y * point.y +
			m_planes[2].z * point.z +
			m_planes[2].w)
			< 0.f)
			return false;
		if ((
			m_planes[3].x * point.x +
			m_planes[3].y * point.y +
			m_planes[3].z * point.z +
			m_planes[3].w)
			< 0.f)
			return false;
		if ((
			m_planes[4].x * point.x +
			m_planes[4].y * point.y +
			m_planes[4].z * point.z +
			m_planes[4].w)
			< 0.f)
			return false;
		if ((
			m_planes[5].x * point.x +
			m_planes[5].y * point.y +
			m_planes[5].z * point.z +
			m_planes[5].w)
			< 0.f)
			return false;
		return true;
	}

	bool SphereInFrustum(f32 radius, const v4f& position)
	{
		for (u32 i = 0u; i < 6u; ++i)
		{
			if (m_planes[i].x * position.x +
				m_planes[i].y * position.y +
				m_planes[i].z * position.z +
				m_planes[i].w <= -radius)
				return false;
		}
		return true;
		/*float dist01 = std::min(DistanceToPlane(m_planes[0], position), DistanceToPlane(m_planes[1], position));
		float dist23 = std::min(DistanceToPlane(m_planes[2], position), DistanceToPlane(m_planes[3], position));
		float dist45 = std::min(DistanceToPlane(m_planes[4], position), DistanceToPlane(m_planes[5], position));
		return std::min(std::min(dist01, dist23), dist45) + radius;*/
	}

	bool AABBInFrustum(const Aabb& aabb, const v4f& position)
	{
		v3f mmin = aabb.m_min + position;
		v3f mmax = aabb.m_max + position;
		for (u32 i = 0; i < 6; ++i)
		{
			if ((m_planes[i].x * mmin.x + m_planes[i].y * mmin.y + m_planes[i].z * mmin.z + m_planes[i].w) >= 0.f) continue;
			if ((m_planes[i].x * mmin.x + m_planes[i].y * mmin.y + m_planes[i].z * mmax.z + m_planes[i].w) >= 0.f) continue;
			if ((m_planes[i].x * mmin.x + m_planes[i].y * mmax.y + m_planes[i].z * mmin.z + m_planes[i].w) >= 0.f) continue;
			if ((m_planes[i].x * mmax.x + m_planes[i].y * mmin.y + m_planes[i].z * mmin.z + m_planes[i].w) >= 0.f) continue;
			if ((m_planes[i].x * mmin.x + m_planes[i].y * mmax.y + m_planes[i].z * mmax.z + m_planes[i].w) >= 0.f) continue;
			if ((m_planes[i].x * mmax.x + m_planes[i].y * mmin.y + m_planes[i].z * mmax.z + m_planes[i].w) >= 0.f) continue;
			if ((m_planes[i].x * mmax.x + m_planes[i].y * mmax.y + m_planes[i].z * mmin.z + m_planes[i].w) >= 0.f) continue;
			if ((m_planes[i].x * mmax.x + m_planes[i].y * mmax.y + m_planes[i].z * mmax.z + m_planes[i].w) >= 0.f) continue;
			return false;
		}
		return true;
	}

	enum FrustumSide
	{
		RIGHT = 0,		// The RIGHT side of the frustum
		LEFT = 1,		// The LEFT	 side of the frustum
		BOTTOM = 2,		// The BOTTOM side of the frustum
		TOP = 3,		// The TOP side of the frustum
		BACK = 4,		// The BACK	side of the frustum
		FRONT = 5			// The FRONT side of the frustum
	};

	void NormalizePlane(v4f& plane)
	{
		// Here we calculate the magnitude of the normal to the plane (point A B C)
		// Remember that (A, B, C) is that same thing as the normal's (X, Y, Z).
		// To calculate magnitude you use the equation:  magnitude = sqrt( x^2 + y^2 + z^2)
		float magnitude = (float)sqrt(plane.x * plane.x +
			plane.y * plane.y +
			plane.z * plane.z);

		// Then we divide the plane's values by it's magnitude.
		// This makes it easier to work with.
		plane.x /= magnitude;
		plane.y /= magnitude;
		plane.z /= magnitude;
		plane.w /= magnitude;
	}

	void CalculateFrustum(Mat4& P, Mat4& V)
	{
		f32* proj = P.getPtr();
		f32* modl = V.getPtr();
		float   clip[16]; //clipping planes

		clip[0] = modl[0] * proj[0] + modl[1] * proj[4] + modl[2] * proj[8] + modl[3] * proj[12];
		clip[1] = modl[0] * proj[1] + modl[1] * proj[5] + modl[2] * proj[9] + modl[3] * proj[13];
		clip[2] = modl[0] * proj[2] + modl[1] * proj[6] + modl[2] * proj[10] + modl[3] * proj[14];
		clip[3] = modl[0] * proj[3] + modl[1] * proj[7] + modl[2] * proj[11] + modl[3] * proj[15];

		clip[4] = modl[4] * proj[0] + modl[5] * proj[4] + modl[6] * proj[8] + modl[7] * proj[12];
		clip[5] = modl[4] * proj[1] + modl[5] * proj[5] + modl[6] * proj[9] + modl[7] * proj[13];
		clip[6] = modl[4] * proj[2] + modl[5] * proj[6] + modl[6] * proj[10] + modl[7] * proj[14];
		clip[7] = modl[4] * proj[3] + modl[5] * proj[7] + modl[6] * proj[11] + modl[7] * proj[15];

		clip[8] = modl[8] * proj[0] + modl[9] * proj[4] + modl[10] * proj[8] + modl[11] * proj[12];
		clip[9] = modl[8] * proj[1] + modl[9] * proj[5] + modl[10] * proj[9] + modl[11] * proj[13];
		clip[10] = modl[8] * proj[2] + modl[9] * proj[6] + modl[10] * proj[10] + modl[11] * proj[14];
		clip[11] = modl[8] * proj[3] + modl[9] * proj[7] + modl[10] * proj[11] + modl[11] * proj[15];

		clip[12] = modl[12] * proj[0] + modl[13] * proj[4] + modl[14] * proj[8] + modl[15] * proj[12];
		clip[13] = modl[12] * proj[1] + modl[13] * proj[5] + modl[14] * proj[9] + modl[15] * proj[13];
		clip[14] = modl[12] * proj[2] + modl[13] * proj[6] + modl[14] * proj[10] + modl[15] * proj[14];
		clip[15] = modl[12] * proj[3] + modl[13] * proj[7] + modl[14] * proj[11] + modl[15] * proj[15];


		//RIGHT 
		m_planes[RIGHT].x = (clip[3] - clip[0]);
		m_planes[RIGHT].y = (clip[7] - clip[4]);
		m_planes[RIGHT].z = (clip[11u] - clip[8]);
		m_planes[RIGHT].w = (clip[15u] - clip[12u]);
		NormalizePlane(m_planes[RIGHT]);

		//LEFT  
		m_planes[LEFT].x = (clip[3] + clip[0]);
		m_planes[LEFT].y = (clip[7] + clip[4]);
		m_planes[LEFT].z = (clip[11u] + clip[8]);
		m_planes[LEFT].w = (clip[15u] + clip[12u]);
		NormalizePlane(m_planes[LEFT]);

		//BOTTOM  
		m_planes[BOTTOM].x = (clip[3] + clip[1]);
		m_planes[BOTTOM].y = (clip[7] + clip[5]);
		m_planes[BOTTOM].z = (clip[11u] + clip[9]);
		m_planes[BOTTOM].w = (clip[15u] + clip[13u]);
		NormalizePlane(m_planes[BOTTOM]);

		//TOP  
		m_planes[TOP].x = (clip[3] - clip[1]);
		m_planes[TOP].y = (clip[7] - clip[5]);
		m_planes[TOP].z = (clip[11u] - clip[9]);
		m_planes[TOP].w = (clip[15u] - clip[13u]);
		NormalizePlane(m_planes[TOP]);

		//FAR  
		m_planes[BACK].x = (clip[3] - clip[2]);
		m_planes[BACK].y = (clip[7] - clip[6]);
		m_planes[BACK].z = (clip[11u] - clip[10u]);
		m_planes[BACK].w = (clip[15u] - clip[14u]);
		NormalizePlane(m_planes[BACK]);

		//NEAR  
		m_planes[FRONT].x = (clip[3] + clip[2]);
		m_planes[FRONT].y = (clip[7] + clip[6]);
		m_planes[FRONT].z = (clip[11u] + clip[10u]);
		m_planes[FRONT].w = (clip[15u] + clip[14u]);
		NormalizePlane(m_planes[FRONT]);
	}
};

class FlyCamera
{
	bool m_needUpdate = true;
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
	f32 m_aspect = 800.f / 600.f;
	f32 m_near = 0.01f;
	f32 m_far = 1000.f;


	miCameraFrustum m_frust;

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
		{
			m_rotationMatrix = RX * m_rotationMatrix * RY;
			m_needUpdate = true;
		}
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

		m_needUpdate = true;
	}
	void OnUpdate() {
		if (!m_needUpdate)
			return;

		m_needUpdate = false;
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

		m_frust.CalculateFrustum(m_projection, m_view);
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
	ApplicationGUI* m_GUI = 0;

	void OnCreate(const char*);
	void MainLoop();
	void WriteLog(const char* message);
	miPopup* ShowPopup();
	void ShowGUITab(u32);
	void OpenMap();
	void GenerateWorld();
	void ReadWorld();

	MapCell* m_testMapCell = 0;
	miArray<MapCell*> m_mapCells;
	miArray<MapCell*> m_visibleMapCells;
	void DrawMapCell(MapCell*);
	void FrustumCullMap();

	Player* m_player = 0;
	void FindCurrentCellID();

	bool m_cameraWasMoved = true;
	void FindLODs();
};

#endif