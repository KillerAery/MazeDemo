#include "MiniMap.h"



MiniMap::MiniMap():
mMiniEndTexSRV(nullptr), mMiniPlayerTexSRV(nullptr),mMiniWallTexSRV(nullptr),
mMiniWallIB(nullptr),mMiniWallVB(nullptr)
{

}


MiniMap::~MiniMap()
{
	ReleaseCOM(mMiniEndTexSRV);
	ReleaseCOM(mMiniPlayerTexSRV);
	ReleaseCOM(mMiniWallTexSRV);
	ReleaseCOM(mMiniWallIB);
	ReleaseCOM(mMiniWallVB);
}

void MiniMap::init(ID3D11Device* d3dDevice, std::shared_ptr<Player>& player, std::shared_ptr<EntityFactory>& factroy) {
	r_player = player;
	r_wallFactory = factroy;
	//生成迷你地图 顶点索引
	buildMiniWallVertexIndex(d3dDevice);
	//载入纹理资源
	HR(D3DX11CreateShaderResourceViewFromFile(d3dDevice,
		L"Textures/miniEnd.dds", 0, 0, &mMiniEndTexSRV, 0));
	HR(D3DX11CreateShaderResourceViewFromFile(d3dDevice,
		L"Textures/miniWall.dds", 0, 0, &mMiniWallTexSRV, 0));
	HR(D3DX11CreateShaderResourceViewFromFile(d3dDevice,
		L"Textures/miniPlayer.dds", 0, 0, &mMiniPlayerTexSRV, 0));

}

void MiniMap::draw(ID3D11DeviceContext* d3dImmediateContext){
	//墙不存在，不需要渲染墙坐标图片
	if (r_player.expired()) return;

	auto wallFactory = r_wallFactory.lock();

	ID3DX11EffectTechnique* texTech = Effects::BasicFX->Show2DTech;

	D3DX11_TECHNIQUE_DESC techDesc;
	texTech->GetDesc(&techDesc);

	UINT stride = sizeof(Vertex::Basic32);
	UINT offset = 0;

	for (UINT passIndex = 0; passIndex < techDesc.Passes; ++passIndex) {
		d3dImmediateContext->IASetVertexBuffers(0, 1, &mMiniWallVB, &stride, &offset);
		d3dImmediateContext->IASetIndexBuffer(mMiniWallIB, DXGI_FORMAT_R32_UINT, 0);

		int height = WallMaze::WallHeight;
		int width = WallMaze::WallWidth;

		Effects::BasicFX->SetWorld(XMMatrixIdentity());
		Effects::BasicFX->SetWorldInvTranspose(XMMatrixIdentity());
		Effects::BasicFX->SetWorldViewProj(XMMatrixIdentity());
		Effects::BasicFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));
		Effects::BasicFX->SetDiffuseMap(mMiniWallTexSRV);
		texTech->GetPassByIndex(passIndex)->Apply(0, d3dImmediateContext);

		for (int y = 0; y < height; ++y)
			for (int x = 0; x < width; ++x)
			{
				//如果没有墙，则跳过渲染一次墙图片
				if (!wallFactory->wallAlive[x][y])continue;
				d3dImmediateContext->DrawIndexed(mOneIndexCount,mOneIndexCount*(y*width+x),mOneVertexCount*(y*width+x));
			}
		//玩家存在，才渲染玩家坐标图片
		if (!r_player.expired()) {

		}

	}


}

void MiniMap::buildMiniWallVertexIndex(ID3D11Device* d3dDevice){
	GeometryGenerator::MeshData miniWall[WallMaze::WallWidth][WallMaze::WallHeight];

	int width = WallMaze::WallWidth;
	int height = WallMaze::WallHeight;

	float dw = 0.5f / width;
	float dh = 0.5f / height;

	GeometryGenerator geoGen;

	for (int y = 0; y < height; ++y)
		for (int x = 0; x < width; ++x)
			geoGen.Create2Dshow(miniWall[x][y],1.0f+x*dw,1.0f+y*dh,dw,dh);

	mOneVertexCount = miniWall[0][0].Vertices.size();
	mOneIndexCount = miniWall[0][0].Indices.size();

	UINT totalVertexCount =
		mOneVertexCount*height*width;

	UINT totalIndexCount =
		mOneIndexCount*height*width;

	std::vector<Vertex::Basic32> vertices(totalVertexCount);

	UINT k = 0;

	for (int y = 0; y < height; ++y)
		for (int x = 0; x < width; ++x)
	for (size_t i = 0; i < mOneVertexCount; ++i, ++k)
	{
		vertices[k].Pos = miniWall[x][y].Vertices[i].Position;
		vertices[k].Normal = miniWall[x][y].Vertices[i].Normal;
		vertices[k].Tex = miniWall[x][y].Vertices[i].TexC;
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Basic32) * totalVertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(d3dDevice->CreateBuffer(&vbd, &vinitData, &mMiniWallVB));

	std::vector<UINT> indices;

	for (int y = 0; y < height; ++y)
		for (int x = 0; x < width; ++x)
	indices.insert(indices.end(), miniWall[x][y].Indices.begin(), miniWall[x][y].Indices.end());

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * totalIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	HR(d3dDevice->CreateBuffer(&ibd, &iinitData, &mMiniWallIB));
}