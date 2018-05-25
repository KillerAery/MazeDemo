#include "d3dApp.h"
#include "d3dx11Effect.h"
#include "GeometryGenerator.h"
#include "MathHelper.h"
#include "LightHelper.h"
#include "Effects.h"
#include "Vertex.h"
#include "Camera.h"
#include "Sky.h"

#include "PhysicsWorld.h"
#include "EntityFactory.h"
#include "Player.h"
#include "MiniMap.h"
#include "SoundPlayer.h"

//#include <assimp/Importer.hpp>

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
	//结束游戏
	void GameEnd();
private:
	//创建几何形状
	void BuildShapeGeometryBuffers();
	void BuildSkullGeometryBuffers();
	//void LoadPlayerModel();
private:
	ID3D11Buffer* mShapesVB;
	ID3D11Buffer* mShapesIB;

	ID3D11Buffer* mSkullVB;
	ID3D11Buffer* mSkullIB;

	//地板纹理
	ID3D11ShaderResourceView* mGroundTexSRV;
	//女鬼纹理
	ID3D11ShaderResourceView* mGhostTexSRV;
	//结局图片纹理
	ID3D11ShaderResourceView* mEndTexSRV;

	Material mGridMat;
	Material m2DMat;

	// Define transformations from local spaces to world space.
	XMFLOAT4X4 mGridWorld;

	int mWallVertexOffset;
	int mGridVertexOffset;
	int mFullVertexOffset;

	UINT mWallIndexOffset;
	UINT mGridIndexOffset;
	UINT mFullIndexOffset;

	UINT mWallIndexCount;
	UINT mGridIndexCount;
	UINT mFullIndexCount;

	UINT mSkullIndexCount;
	//上一次鼠标点
	POINT mLastMousePos;

	//负责播放声音
	SoundPlayer mSound;

	BOOL mGhostOut;	//是否该出现鬼
	BOOL mGameEnd;	//游戏是否结束
	BOOL mButtonZdown;	//是否曾按下Z

	//出现鬼计时
	float mGhostOutTimer;
	float mGhostOutTimeLimit;
	float mGhostTimer;
	//电筒失灵计时
	float mFlashBrokenTimer;
	float mFlashBrokenTimeLimit;
	float mFlashTimer;
	int mBlinkNums;
	float mBlinkTimer;
	
	//脚步计时
	float mStepTimer;
	//结束计时
	float mEndingTimer;

	PhysicsWorld mPhysicsWorld;	//物理世界
	DirectionalLight mDirLight;	//环境（方向灯）
	SpotLight mFlashLight;	//电筒（聚光灯）
	Camera mCam;	//第一人称相机
	BOOL mLightOn;	//是否开了电筒
	shared_ptr<Sky> mSky;	//天空盒
	shared_ptr<Player> mPlayer;	//玩家
	shared_ptr<EntityFactory> mEntityFactory;	//实体生成工厂

	//shared_ptr<MiniMap> mMiniMap;	//迷你地图
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
	mGroundTexSRV(0), mGhostTexSRV(0), mSkullIndexCount(0), mEndTexSRV(0)
{
	mMainWndCaption = L"Scary Maze";

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mGridWorld, I);

	mGameEnd = false;
	mButtonZdown = false;
	mLightOn = true;
	mGhostOut = false;

	//出现鬼计时
	mGhostOutTimer = 0.0f;
	mGhostOutTimeLimit = 35.0f;
	mGhostTimer = 0.0f;
	//电筒失灵计时
	mFlashBrokenTimer = 0.0f;
	mFlashBrokenTimeLimit = 7.0f;
	mFlashTimer = 0.0f;
	//电筒闪烁
	mBlinkNums = 0;
	mBlinkTimer = 0.0f;
	//脚步计时
	mStepTimer = 0.0f;
	//结束计时
	mEndingTimer = 0.0f;

	//设置灯光属性
	mDirLight.Ambient  = XMFLOAT4(0.05f, 0.05f, 0.05f, 1.0f);
	mDirLight.Diffuse  = XMFLOAT4(0.05f, 0.05f, 0.05f, 1.0f);
	mDirLight.Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	mDirLight.Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

	//设置电筒聚光灯光属性
	mFlashLight.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mFlashLight.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mFlashLight.Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mFlashLight.Att = XMFLOAT3(1.0f, 0.0f, 0.0f);
	mFlashLight.Spot = 50.0f;
	mFlashLight.Range = 100.0f;

	//设置材料属性
	mGridMat.Ambient  = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	mGridMat.Diffuse  = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	mGridMat.Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);

	m2DMat.Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m2DMat.Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m2DMat.Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
}

MazeApp::~MazeApp()
{
	//关闭声音设备
	mSound.Shutdown();

	ReleaseCOM(mShapesVB);
	ReleaseCOM(mShapesIB);
	ReleaseCOM(mSkullVB);
	ReleaseCOM(mSkullIB);
	ReleaseCOM(mGroundTexSRV);
	ReleaseCOM(mEndTexSRV);

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

	//初始化声音设备
	mSound.Initialize(mhMainWnd);

	//播放BGM
	mSound.PlayWaveFile(SoundName::BGM,true);

	//载入纹理资源
	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice,
		L"Textures/ground.dds", 0, 0, &mGroundTexSRV, 0));
	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice,
		L"Textures/ghost.dds", 0, 0, &mGhostTexSRV, 0));
	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice,
		L"Textures/ending.dds", 0, 0, &mEndTexSRV, 0));


	//建造几何形状 顶点索引缓冲器
	BuildShapeGeometryBuffers();
	BuildSkullGeometryBuffers();
	//LoadPlayerModel();

	//创建天空盒
	//mSky = std::make_shared<Sky>(md3dDevice, L"Textures/sunsetcube1024.dds", 5000.0f);

	//生成物理世界
	mPhysicsWorld.init();
	
	//生成迷宫
	mEntityFactory = std::make_shared<EntityFactory>();
	mEntityFactory->init(mWallVertexOffset, mWallIndexOffset,mWallIndexCount,0,0,mSkullIndexCount);
	mEntityFactory->createMazeEntities(md3dDevice,mPhysicsWorld.getWorld());

	//生成玩家
	mPlayer = Player::create(mPhysicsWorld.getWorld(),&mCam);

	////初始化小地图
	//mMiniMap.init(md3dDevice,mPlayer,mEntityFactory);

	return true;
}

void MazeApp::OnResize()
{
	D3DApp::OnResize();

	mCam.SetLens(0.25f * MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
}

void MazeApp::UpdateScene(float dt)
{
	//判断游戏结束——关闭之
	if (mGameEnd) {
		mEndingTimer += dt;
		if(mEndingTimer >= 5.0f) {
			PostQuitMessage(0);
		}
	}

	mGhostOutTimer += dt;
	mGhostTimer += dt;
	mStepTimer += dt;
	mFlashBrokenTimer += dt;

	//随机灯光坏掉 7~17秒
	if (mFlashBrokenTimer >=  mFlashBrokenTimeLimit) {
		mFlashBrokenTimeLimit = (rand() % 10 + 7);
		mFlashBrokenTimer = 0.0f;

		mBlinkNums = 5;
	}

	if (mBlinkNums) {
		mBlinkTimer += dt;
		if (mBlinkTimer > 0.05f) {
			mBlinkTimer = 0.0f;
			mLightOn = !mLightOn;
			mBlinkNums--;
		}
	}


	//随机 20~50 秒 出现一次女鬼
	if (mGhostOutTimer > mGhostOutTimeLimit) {
		mGhostOutTimeLimit = (rand() % 30 + 20);
		mGhostOutTimer = 0.0f;

		mGhostTimer = 0.0f;
		mGhostOut = true;
	}
	//一次女鬼出现 0.15s
	else if (mGhostTimer > 0.3f){
		mGhostOut = false;
	}

	//更新手电筒 的位置+方向
	mFlashLight.Position = mCam.GetPosition();
	XMStoreFloat3(&mFlashLight.Direction, XMVector3Normalize(mCam.GetLookXM()));

	//更新墙
	for (auto& i : mEntityFactory->walls)
		i->update(dt);
	//更新骷髅头
	for (auto& i : mEntityFactory->skulls)
		i->update(dt);


	bool haveWASD = false;
	//玩家脚步
	if (GetAsyncKeyState('W') & 0x8000){
		mPlayer->walk(Player::ButtonDown::W_Down);
		haveWASD = true;
	}
	if (GetAsyncKeyState('S') & 0x8000) {
		mPlayer->walk(Player::ButtonDown::S_Down);
		haveWASD = true;
	}
	if (GetAsyncKeyState('A') & 0x8000){
		mPlayer->walk(Player::ButtonDown::A_Down);
		haveWASD = true;
	}

	if (GetAsyncKeyState('D') & 0x8000) {
		mPlayer->walk(Player::ButtonDown::D_Down);
		haveWASD = true;
	}

	if (haveWASD) {
		if (mStepTimer > 0.7f) {
			static int i = 0;
			mSound.PlayWaveFile(i);
			i = !i;
			mStepTimer = 0.0f;
		}
	}

	//按了Esc 结束游戏
	if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
		PostQuitMessage(0);
	//TODO 跳跃
	if (GetAsyncKeyState(' ') & 0x8000)
		mPlayer->jump();
	//判断  手电筒开关
	if (GetAsyncKeyState('Z') & 0x8000)
	{
		mButtonZdown = true;
	}
	else if (mButtonZdown) {
		mButtonZdown = false;
		mLightOn = !mLightOn;
	}

	//设置灯光
	mFlashLight.Ambient = XMFLOAT4(mLightOn, mLightOn, mLightOn, 1.0f);
	mFlashLight.Diffuse = XMFLOAT4(mLightOn, mLightOn, mLightOn, 1.0f);
	mFlashLight.Specular = XMFLOAT4(mLightOn, mLightOn, mLightOn, 1.0f);

	//更新玩家
	mPlayer->update(dt);

	//如果玩家到达终点，结束游戏
	if (mEntityFactory->posInEnd(XMLoadFloat3(&mPlayer->getPosition()))){
		GameEnd();
	}

	//物理世界模拟步长
	mPhysicsWorld.stepWorld(dt);
}

void MazeApp::DrawScene()
{
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Black));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

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
	Effects::BasicFX->SetSpotLight(mFlashLight);

	ID3DX11EffectTechnique* activeTexTech = Effects::BasicFX->Light1TexTech;
	ID3DX11EffectTechnique* activeSkullTech = Effects::BasicFX->Light1Tech;

	XMVECTOR relativeVector;
	XMFLOAT3 distanceSqVector;

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
		Effects::BasicFX->SetDiffuseMap(mGroundTexSRV);

		activeTexTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mGridIndexCount, mGridIndexOffset, mGridVertexOffset);

		//绘制墙
		for (auto & wall : mEntityFactory->walls) {
			relativeVector = XMVector3LengthSq(XMLoadFloat3(&wall->getPosition()) - XMLoadFloat3(&mPlayer->getPosition()));
			XMStoreFloat3(&distanceSqVector, relativeVector);
			//在一定范围才渲染
			if(distanceSqVector.x < 1300.0f)
			wall->draw(md3dImmediateContext,view,proj,p);
		}

		//如果出现女鬼
		if (mGhostOut) {
			//绘制2D 女鬼 图片
			Effects::BasicFX->SetWorld(XMMatrixIdentity());
			Effects::BasicFX->SetWorldInvTranspose(XMMatrixIdentity());
			Effects::BasicFX->SetWorldViewProj(XMMatrixIdentity());
			Effects::BasicFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));
			Effects::BasicFX->SetMaterial(m2DMat);
			Effects::BasicFX->SetDiffuseMap(mGhostTexSRV);
			Effects::BasicFX->Show2DTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
			md3dImmediateContext->DrawIndexed(mFullIndexCount, mFullIndexOffset, mFullVertexOffset);
		}

		//如果结局
		if (mGameEnd) {
			//绘制2D 结局 图片
			Effects::BasicFX->SetWorld(XMMatrixIdentity());
			Effects::BasicFX->SetWorldInvTranspose(XMMatrixIdentity());
			Effects::BasicFX->SetWorldViewProj(XMMatrixIdentity());
			Effects::BasicFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));
			Effects::BasicFX->SetMaterial(m2DMat);
			Effects::BasicFX->SetDiffuseMap(mEndTexSRV);
			Effects::BasicFX->Show2DTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
			md3dImmediateContext->DrawIndexed(mFullIndexCount, mFullIndexOffset, mFullVertexOffset);
		}
    }

	//绘制骷髅头
	activeSkullTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mSkullVB, &stride, &offset);
		md3dImmediateContext->IASetIndexBuffer(mSkullIB, DXGI_FORMAT_R32_UINT, 0);

		for (auto & skull : mEntityFactory->skulls) {
			relativeVector = XMVector3LengthSq(XMLoadFloat3(&skull->getPosition()) - XMLoadFloat3(&mPlayer->getPosition()));
			XMStoreFloat3(&distanceSqVector, relativeVector);
			//在一定范围才渲染
			if (distanceSqVector.x < 1300.0f)
			skull->draw(md3dImmediateContext, view, proj, p);
		}
	}

	//绘制2D miniMap 图片
	//mMiniMap.draw(md3dImmediateContext);

	//绘制天空盒
	//mSky->Draw(md3dImmediateContext, mCam);

	// restore default states, as the SkyFX changes them in the effect file.
	//md3dImmediateContext->RSSetState(0);
	//md3dImmediateContext->OMSetDepthStencilState(0, 0);

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
	//游戏结束，则无需封锁鼠标
	if (mGameEnd)return;

	RECT rect;
	GetWindowRect(mhMainWnd, &rect);

	//封锁鼠标以获得FPS视野
	if (x > 700 || x < 100 || y > 600 || y < 100) {
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
	GeometryGenerator::MeshData full;

	GeometryGenerator geoGen;
	geoGen.CreateBox(3.0f,6.0f,3.0f, wall);
	geoGen.CreateGrid(WallMaze::WallWidth*3.0f, WallMaze::WallHeight*3.0f, 60, 40, grid);
	geoGen.Create2Dshow(full, 0.5f, 0.5f, 1.0f, 1.0f);

	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	mWallVertexOffset = 0;
	mGridVertexOffset = wall.Vertices.size();
	mFullVertexOffset = mGridVertexOffset + grid.Vertices.size();
	// Cache the index count of each object.
	mWallIndexCount = wall.Indices.size();
	mGridIndexCount = grid.Indices.size();
	mFullIndexCount = full.Indices.size();
	// Cache the starting index for each object in the concatenated index buffer.
	mWallIndexOffset = 0;
	mGridIndexOffset = mWallIndexCount;
	mFullIndexOffset = mGridIndexOffset + mGridIndexCount;

	UINT totalVertexCount =
		wall.Vertices.size() +
		grid.Vertices.size() +
		full.Vertices.size();

	UINT totalIndexCount =
		mWallIndexCount +
		mGridIndexCount +
		mFullIndexCount;

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
	for (size_t i = 0; i < grid.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = grid.Vertices[i].Position;
		vertices[k].Normal = grid.Vertices[i].Normal;
		vertices[k].Tex = grid.Vertices[i].TexC;
	}
	for (size_t i = 0; i < full.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = full.Vertices[i].Position;
		vertices[k].Normal = full.Vertices[i].Normal;
		vertices[k].Tex = full.Vertices[i].TexC;
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
	indices.insert(indices.end(), full.Indices.begin(), full.Indices.end());

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
		fin >> vertices[i].Pos.x  >> vertices[i].Pos.y >> vertices[i].Pos.z;
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

void MazeApp::GameEnd(){
	mGameEnd = true;
}

//void MazeApp::LoadPlayerModel(){
//	Assimp::Importer importer;
//	VertexTpye * vertices;
//	unsigned long* indices;
//	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
//	D3D11_SUBRESOURCE_DATA vertexData, indexData;
//
//	const aiScene* scene = importer.ReadFile(filename, aiProcessPreset_TargetRealtime_Quality | aiProcess_FlipUVs);
//
//	if (!scene)
//	{
//		MessageBoxA(NULL, importer.GetErrorString(), "Error", MB_OK);
//		return;
//	}
//
//	scene->mNumMeshes;
//	
//	int m = 0;
//	//第一趟扫描，得到顶点和索引计数 
//	for (m = 0; m < scene->mNumMeshes; ++m)
//	{
//		//第m个mesh 
//		aiMesh* aiMesh = scene->mMeshes[m];
//
//		m_vertexCount += aiMesh->mNumVertices;
//		m_indexCount += aiMesh->mNumFaces * 3;
//	}
//
//	// 创建顶点临时缓冲. 
//	vertices = new VertexType[m_vertexCount];
//	if (!vertices)
//	{
//		return false;
//	}
//
//	// 创建索引临时缓冲. 
//	indices = new unsigned long[m_indexCount];
//	if (!indices)
//	{
//		return false;
//	}
//
//	//临时的顶点和索引指针 
//	int index1 = 0;
//	int index2 = 0;
//	int i = 0;
//
//	//第二趟循环，得到每个顶点和索引的值 
//	for (m = 0; m < scene->mNumMeshes; ++m)
//	{
//		//第m个mesh 
//		aiMesh* aiMesh = scene->mMeshes[m];
//
//		if (!aiMesh->HasNormals() || !aiMesh->HasTextureCoords(0))
//		{
//			MessageBox(NULL, L"模型文件中没有纹理坐标或者法向信息", L"Error", MB_OK);
//			return;
//		}
//
//		int vertexCount = aiMesh->mNumVertices;
//		for (i = 0; i < vertexCount; ++i)
//		{
//			vertices[index1].position = D3DXVECTOR3(aiMesh->mVertices[i].x, aiMesh->mVertices[i].y, aiMesh->mVertices[i].z);
//			vertices[index1].normal = D3DXVECTOR3(aiMesh->mNormals[i].x, aiMesh->mNormals[i].y, aiMesh->mNormals[i].z);
//			vertices[index1].texture = D3DXVECTOR2(aiMesh->mTextureCoords[0][i].x, aiMesh->mTextureCoords[0][i].y);
//			vertices[index1].Kd = D3DXVECTOR4(1.0, 1.0, 1.0, 1.0);
//			vertices[index1].Ks = D3DXVECTOR4(0.2, 0.2, 0.2, 1.0);
//			index1++;
//		}
//
//		for (i = 0; i < aiMesh->mNumFaces; ++i)
//		{
//			const aiFace& Face = aiMesh->mFaces[i];
//			//assert(Face.mNumIndices == 3); 
//			indices[index2] = Face.mIndices[0];
//			index2++;
//			indices[index2] = Face.mIndices[1];
//			index2++;
//			indices[index2] = Face.mIndices[2];
//			index2++;
//
//		}
//	}
//	// 设置顶点缓冲描述 
//	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
//	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
//	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	vertexBufferDesc.CPUAccessFlags = 0;
//	vertexBufferDesc.MiscFlags = 0;
//	vertexBufferDesc.StructureByteStride = 0;
//
//	// 指向保存顶点数据的临时缓冲. 
//	vertexData.pSysMem = vertices;
//	vertexData.SysMemPitch = 0;
//	vertexData.SysMemSlicePitch = 0;
//
//	// 创建顶点缓冲. 
//	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
//	if (FAILED(result))
//	{
//		HR(result);
//		return;
//	}
//
//	// 设置索引缓冲描述. 
//	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
//	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
//	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
//	indexBufferDesc.CPUAccessFlags = 0;
//	indexBufferDesc.MiscFlags = 0;
//	indexBufferDesc.StructureByteStride = 0;
//
//	// 指向存临时索引缓冲. 
//	indexData.pSysMem = indices;
//	indexData.SysMemPitch = 0;
//	indexData.SysMemSlicePitch = 0;
//
//	// 创建索引缓冲. 
//	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
//	if (FAILED(result))
//	{
//		HR(result);
//		return;
//	}
//
//	// 释放临时缓冲. 
//	delete[] vertices;
//	vertices = 0;
//
//	delete[] indices;
//	indices = 0;
//
//}