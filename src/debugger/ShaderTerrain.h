#ifndef _SHADER_TERRAIN_H_
#define _SHADER_TERRAIN_H_

class ShaderTerrain : public miGPUShaderCallback
{
public:
	ShaderTerrain();
	virtual ~ShaderTerrain();

	struct cbVertex
	{
		Mat4 WVP;
		Mat4 W;
	}
	m_cbVertexData;

	miGPUShader* m_GPUShader = 0;

	void* m_cbVertex = 0;

	virtual void OnSetConstants(miGraphicsSystem* gs, miGPUDrawCommand* cmd) override;
};


#endif