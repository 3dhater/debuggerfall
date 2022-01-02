#include "mi/MainSystem/MainSystem.h"
#include "mi/GraphicsSystem/util.h"
#include "mi/Classes/material.h"

#include "application.h"
#include "ShaderTerrain.h"

extern Application* g_app;

ShaderTerrain::ShaderTerrain()
{

}

ShaderTerrain::~ShaderTerrain()
{
	if (m_GPUShader)
		m_GPUShader->Release();
}

void ShaderTerrain::OnSetConstants(miGraphicsSystem* gs, miGPUDrawCommand* cmd)
{
	if(cmd->m_material->m_maps[0].m_GPUTexture)
		cmd->m_shader->SetTexture(cmd->m_material->m_maps[0].m_GPUTexture, 0);

	m_cbVertexData.W = *cmd->m_matWorld;
	m_cbVertexData.WVP = *cmd->m_matWVP;

	cmd->m_shader->MapConstantBuffer(m_cbVertex, &m_cbVertexData);
	cmd->m_shader->SetConstantBuffer(m_cbVertex, 0, 0);
}
