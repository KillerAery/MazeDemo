//***************************************************************************************
// CameraDemo.cpp by Frank Luna (C) 2011 All Rights Reserved.
//
// Demonstrates first person camera.
//
// Controls:
//		Hold the left mouse button down and move the mouse to rotate.
//      Hold the right mouse button down to zoom in and out.
//      Press '1', '2', '3' for 1, 2, or 3 lights enabled.
//
//***************************************************************************************

#include "d3dApp.h"
#include "d3dx11Effect.h"
#include "GeometryGenerator.h"
#include "MathHelper.h"
#include "LightHelper.h"
#include "Effects.h"
#include "Vertex.h"
#include "Camera.h"

#include "WallFactory.h"
#include "PhysicsWorld.h"

class MazeApp : public D3DApp 
{
public:
	MazeApp(HINSTANCE hInstance);
	~MazeApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene(); 

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	//创建几何形状
	void BuildShapeGeometryBuffers();
	void BuildSkullGeometryBuffers();
private:
	ID3D11Buffer* mShapesVB;
	ID3D11Buffer* mShapesIB;

	ID3D11Buffer* mSkullVB;
	ID3D11Buffer* mSkullIB;

	ID3D11ShaderResourceView* mFloorTexSRV;
	ID3D11ShaderResourceView* mStoneTexSRV;
	ID3D11ShaderResourceView* mBrickTexSRV;

	Material mGridMat;
	Material mSkullMat;

	// Define transformations from local spaces to world space.
	XMFLOAT4X4 mGridWorld;
	XMFLOAT4X4 mSkullWorld;

	int mWallVertexOffset;
	int mGridVertexOffset;

	UINT mWallIndexOffset;
	UINT mGridIndexOffset;

	UINT mWallIndexCount;
	UINT mGridIndexCount;
	UINT mSkullIndexCount;

	POINT mLastMousePos;

	//物理世界
	PhysicsWorld mPhysicsWorld;

	//环境（方向灯）
	DirectionalLight mDirLight;
	//电筒（聚光灯）
	SpotLight mSpotLight;
	//第一人称相机
	Camera mCam;
	//是否开了电筒
	BOOL mLightOn;

	//迷宫生成器
	WallFactory mMazeGenerator;

};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	MazeApp theApp(hInstance);
	
	if( !theApp.Init() )
		return 0;

	return theApp.Run();
}
 

MazeApp::MazeApp(HINSTANCE hInstance)
: D3DApp(hInstance), mShapesVB(0), mShapesIB(0), mSkullVB(0), mSkullIB(0), 
  mFloorTexSRV(0), mStoneTexSRV(0), mBrickTexSRV(0),
  mSkullIndexCount(0), mLightOn(true)
{
	mMainWndCaption = L"Maze Demo";
	//设置相机一开始的地方
	mCam.SetPosition(0.0f, 20.0f, -15.0f);

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mGridWorld, I);

	XMMATRIX skullScale = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	XMMATRIX skullOffset = XMMatrixTranslation(0.0f, 1.0f, 0.0f);
	XMStoreFloat4x4(&mSkullWorld, XMMatrixMultiply(skullScale, skullOffset));

	//设置灯光属性
	mDirLight.Ambient  = XMFLOAT4(0.05f, 0.05f, 0.05f, 1.0f);
	mDirLight.Diffuse  = XMFLOAT4(0.05f, 0.05f, 0.05f, 1.0f);
	mDirLight.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mDirLight.Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

	//设置电筒聚光灯光属性
	mSpotLight.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mSpotLight.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mSpotLight.Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mSpotLight.Att = XMFLOAT3(1.0f, 0.0f, 0.0f);
	mSpotLight.Spot = 96.0f;
	mSpotLight.Range = 100.0f;

	//设置材料属性
	mGridMat.Ambient  = XMFLOAT4(0.48f, 0.55f, 0.46f, 1.0f);
	mGridMat.Diffuse  = XMFLOAT4(0.48f, 0.55f, 0.46f, 1.0f);
	mGridMat.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

	mSkullMat.Ambient  = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	mSkullMat.Diffuse  = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	mSkullMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);
}

MazeApp::~MazeApp()
{
	ReleaseCOM(mShapesVB);
	ReleaseCOM(mShapesIB);
	ReleaseCOM(mSkullVB);
	ReleaseCOM(mSkullIB);
	ReleaseCOM(mFloorTexSRV);
	ReleaseCOM(mStoneTexSRV);
	ReleaseCOM(mBrickTexSRV);

	Effects::DestroyAll();
	InputLayouts::DestroyAll(); 
}

bool MazeApp::Init()
{
	if(!D3DApp::Init())
		return false;

	// Must init Effects first since InputLayouts depend on shader signatures.
	Effects::InitAll(md3dDevice);
	InputLayouts::InitAll(md3dDevice);

	//载入纹理资源
	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, 
		L"Textures/floor.dds", 0, 0, &mFloorTexSRV, 0 ));

	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, 
		L"Textures/stone.dds", 0, 0, &mStoneTexSRV, 0 ));

	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, 
		L"Textures/bricks.dds", 0, 0, &mBrickTexSRV, 0 ));

	//建造几何形状 顶点索引缓冲器
	BuildShapeGeometryBuffers();
	BuildSkullGeometryBuffers();

	//生成物理世界
	mPhysicsWorld.init();
	
	//生成迷宫
	mMazeGenerator.init();
	mMazeGenerator.generateMazeWalls(md3dDevice,mPhysicsWorld.getWorld(),mWallVertexOffset,mWallIndexOffset,mWallIndexCount);

	return true;
}

void MazeApp::OnResize()
{
	D3DApp::OnResize();

	mCam.SetLens(0.25f * MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
}

void MazeApp::UpdateScene(float dt)
{
	//相机走动
	if( GetAsyncKeyState('W') & 0x8000 )
		mCam.Walk(10.0f*dt);
	if( GetAsyncKeyState('S') & 0x8000 )
		mCam.Walk(-10.0f*dt);
	if( GetAsyncKeyState('A') & 0x8000 )
		mCam.Strafe(-10.0f*dt);
	if( GetAsyncKeyState('D') & 0x8000 )
		mCam.Strafe(10.0f*dt);

	//开关 手电筒
	if (GetAsyncKeyState('Z') & 0x8000)
	{
		mLightOn = !mLightOn;
		mSpotLight.Ambient = XMFLOAT4(mLightOn, mLightOn, mLightOn, 1.0f);
		mSpotLight.Diffuse = XMFLOAT4(mLightOn, mLightOn, mLightOn, 1.0f);
		mSpotLight.Specular = XMFLOAT4(mLightOn, mLightOn, mLightOn, 1.0f);
	}
	//更新手电筒 的位置+方向
	mSpotLight.Position = mCam.GetPosition();
	XMStoreFloat3(&mSpotLight.Direction, XMVector3Normalize(mCam.GetLookXM()));

	for (auto& i : mMazeGenerator.walls)
		i->update(dt);

	//物理世界模拟步长
	mPhysicsWorld.stepWorld(dt);
}

void MazeApp::DrawScene()
{
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Black));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	md3dImmediateContext->IASetInputLayout(InputLayouts::Basic32);
    md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
 
	UINT stride = sizeof(Vertex::Basic32);
    UINT offset = 0;

	mCam.UpdateViewMatrix();
 
	XMMATRIX view     = mCam.View();
	XMMATRIX proj     = mCam.Proj();
	XMMATRIX viewProj = mCam.ViewProj();

	Effects::BasicFX->SetEyePosW(mCam.GetPosition());
	/*---灯光设置---*/
	Effects::BasicFX->SetDirLight(mDirLight);
	/*---手电筒聚光灯设置---*/
	Effects::BasicFX->SetSpotLight(mSpotLight);

	ID3DX11EffectTechnique* activeTexTech = Effects::BasicFX->Light1TexTech;
	ID3DX11EffectTechnique* activeSkullTech = Effects::BasicFX->Light1Tech;

    D3DX11_TECHNIQUE_DESC techDesc;
    activeTexTech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mShapesVB, &stride, &offset);
		md3dImmediateContext->IASetIndexBuffer(mShapesIB, DXGI_FORMAT_R32_UINT, 0);

		XMMATRIX world;
		XMMATRIX worldInvTranspose;
		XMMATRIX worldViewProj;

		//绘制网格地板
		world = XMLoadFloat4x4(&mGridWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world * view * proj;

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetTexTransform(XMMatrixScaling(6.0f, 8.0f, 1.0f));
		Effects::BasicFX->SetMaterial(mGridMat);
		Effects::BasicFX->SetDiffuseMap(mFloorTexSRV);

		activeTexTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mGridIndexCount, mGridIndexOffset, mGridVertexOffset);

		//绘制墙
		for (auto & wall : mMazeGenerator.walls) {
			wall->draw(md3dImmediateContext,view,proj,p);
		}

    }

	//activeSkullTech->GetDesc( &techDesc );
	//for(UINT p = 0; p < techDesc.Passes; ++p)
	//   {
	//	// Draw the skull.

	//	md3dImmediateContext->IASetVertexBuffers(0, 1, &mSkullVB, &stride, &offset);
	//	md3dImmediateContext->IASetIndexBuffer(mSkullIB, DXGI_FORMAT_R32_UINT, 0);

	//	XMMATRIX world = XMLoadFloat4x4(&mSkullWorld);
	//	XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
	//	XMMATRIX worldViewProj = world*view*proj;

	//	Effects::BasicFX->SetWorld(world);
	//	Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
	//	Effects::BasicFX->SetWorldViewProj(worldViewProj);
	//	Effects::BasicFX->SetMaterial(mSkullMat);

	//	activeSkullTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
	//	md3dImmediateContext->DrawIndexed(mSkullIndexCount, 0, 0);
	//}

	HR(mSwapChain->Present(0, 0));
}

void MazeApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void MazeApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void MazeApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	RECT rect;
	GetWindowRect(mhMainWnd, &rect);

	if (x > 750 || x < 50 || y > 650 || y < 50) {
		mLastMousePos.x = 400;
		mLastMousePos.y = 300;
		SetCursorPos(mLastMousePos.x + rect.left , mLastMousePos.y + rect.top);
		return;
	}

	float dx = XMConvertToRadians(0.1f*static_cast<float>(x - mLastMousePos.x));
	float dy = XMConvertToRadians(0.1f*static_cast<float>(y - mLastMousePos.y));

	mCam.Pitch(dy);
	mCam.RotateY(dx);

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void MazeApp::BuildShapeGeometryBuffers()
{
	GeometryGenerator::MeshData wall;
	GeometryGenerator::MeshData grid;

	GeometryGenerator geoGen;
	geoGen.CreateBox(2.0f,5.0f,2.0f, wall);
	geoGen.CreateGrid(WallWidth*2.0f, WallHeight*2.0f, 60, 40, grid);

	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	mWallVertexOffset = 0;
	mGridVertexOffset = wall.Vertices.size();

	// Cache the index count of each object.
	mWallIndexCount = wall.Indices.size();
	mGridIndexCount = grid.Indices.size();

	// Cache the starting index for each object in the concatenated index buffer.
	mWallIndexOffset = 0;
	mGridIndexOffset = mWallIndexCount;
	
	UINT totalVertexCount = 
		wall.Vertices.size() + 
		grid.Vertices.size();

	UINT totalIndexCount = 
		mWallIndexCount + 
		mGridIndexCount;

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	std::vector<Vertex::Basic32> vertices(totalVertexCount);

	UINT k = 0;
	for(size_t i = 0; i < wall.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos    = wall.Vertices[i].Position;
		vertices[k].Normal = wall.Vertices[i].Normal;
		vertices[k].Tex    = wall.Vertices[i].TexC;
	}

	for(size_t i = 0; i < grid.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos    = grid.Vertices[i].Position;
		vertices[k].Normal = grid.Vertices[i].Normal;
		vertices[k].Tex    = grid.Vertices[i].TexC;
	}

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex::Basic32) * totalVertexCount;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &vertices[0];
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mShapesVB));

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	std::vector<UINT> indices;
	indices.insert(indices.end(), wall.Indices.begin(), wall.Indices.end());
	indices.insert(indices.end(), grid.Indices.begin(), grid.Indices.end());

	D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(UINT) * totalIndexCount;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &indices[0];
    HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mShapesIB));
}
 
void MazeApp::BuildSkullGeometryBuffers()
{
	std::ifstream fin("Models/skull.txt");
	
	if(!fin)
	{
		MessageBox(0, L"Models/skull.txt not found.", 0, 0);
		return;
	}

	UINT vcount = 0;
	UINT tcount = 0;
	std::string ignore;

	fin >> ignore >> vcount;
	fin >> ignore >> tcount;
	fin >> ignore >> ignore >> ignore >> ignore;
	
	std::vector<Vertex::Basic32> vertices(vcount);
	for(UINT i = 0; i < vcount; ++i)
	{
		fin >> vertices[i].Pos.x >> vertices[i].Pos.y >> vertices[i].Pos.z;
		fin >> vertices[i].Normal.x >> vertices[i].Normal.y >> vertices[i].Normal.z;
	}

	fin >> ignore;
	fin >> ignore;
	fin >> ignore;

	mSkullIndexCount = 3*tcount;
	std::vector<UINT> indices(mSkullIndexCount);
	for(UINT i = 0; i < tcount; ++i)
	{
		fin >> indices[i*3+0] >> indices[i*3+1] >> indices[i*3+2];
	}

	fin.close();

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Basic32) * vcount;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &vertices[0];
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mSkullVB));

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * mSkullIndexCount;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
    HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mSkullIB));
}
