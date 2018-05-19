//***************************************************************************************
// Effects.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include "Effects.h"

#pragma region Effect
Effect::Effect(ID3D11Device* device, const std::wstring& filename)
	: mFX(0)
{
	std::ifstream fin(filename, std::ios::binary);

	fin.seekg(0, std::ios_base::end);
	int size = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);
	std::vector<char> compiledShader(size);

	fin.read(&compiledShader[0], size);
	fin.close();
	
	HR(D3DX11CreateEffectFromMemory(&compiledShader[0], size, 
		0, device, &mFX));
}

Effect::~Effect()
{
	ReleaseCOM(mFX);
}
#pragma endregion

#pragma region BasicEffect
BasicEffect::BasicEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	Light1Tech    = mFX->GetTechniqueByName("Light1");
	Light1TexTech = mFX->GetTechniqueByName("Light1Tex");
	Light1TexAlphaClipTech = mFX->GetTechniqueByName("Light1TexAlphaClip");
	Light1FogTech    = mFX->GetTechniqueByName("Light1Fog");
	Light1TexFogTech = mFX->GetTechniqueByName("Light1TexFog");
	Light1TexAlphaClipFogTech = mFX->GetTechniqueByName("Light1TexAlphaClipFog");

	WorldViewProj     = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	World             = mFX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	TexTransform      = mFX->GetVariableByName("gTexTransform")->AsMatrix();
	EyePosW           = mFX->GetVariableByName("gEyePosW")->AsVector();
	FogColor          = mFX->GetVariableByName("gFogColor")->AsVector();
	FogStart          = mFX->GetVariableByName("gFogStart")->AsScalar();
	FogRange          = mFX->GetVariableByName("gFogRange")->AsScalar();
	DirLight          = mFX->GetVariableByName("gDirLight");
	SpLight			  = mFX->GetVariableByName("gSpotLight");
	Mat               = mFX->GetVariableByName("gMaterial");
	DiffuseMap        = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
}

BasicEffect::~BasicEffect()
{
}
#pragma endregion

#pragma region Effects

BasicEffect* Effects::BasicFX = 0;

void Effects::InitAll(ID3D11Device* device)
{
	//编译着色器文件所用代码
//	DWORD shaderFlags = 0;
//#if defined(DEBUG) || defined(_DEBUG)
//	shaderFlags |= D3D10_SHADER_DEBUG;
//	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
//#endif
//	ID3D10Blob* compiledShader = 0;
//	ID3D10Blob* compilationMsgs = 0;
//	HRESULT hr = D3DX11CompileFromFile(L"FX/Basic.fx", 0,
//		0, 0, "fx_5_0", shaderFlags,
//		0, 0, &compiledShader, &compilationMsgs, 0);
//	// compilationMsgs can store errors or warnings.
//	if (compilationMsgs != 0)
//	{
//		MessageBoxA(0, (char*)compilationMsgs->GetBufferPointer(), 0, 0);
//		ReleaseCOM(compilationMsgs);
//	}
//	// Even if there are no compilationMsgs, check to make sure there
//	// were no other errors.
//	if (FAILED(hr))
//	{
//		DXTrace(__FILE__, (DWORD)__LINE__, hr,
//			L"D3DX11CompileFromFile", true);
//	}
//
//	ID3DX11Effect* mFX;
//	HR(D3DX11CreateEffectFromMemory(
//		compiledShader->GetBufferPointer(),
//		compiledShader->GetBufferSize(),
//		0, device, &mFX));
//	// Done with compiled shader.
//	ReleaseCOM(compiledShader);

	BasicFX = new BasicEffect(device, L"FX/Basic.fxo");
}

void Effects::DestroyAll()
{
	SafeDelete(BasicFX);
}
#pragma endregion