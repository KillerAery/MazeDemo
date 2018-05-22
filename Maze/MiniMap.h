#pragma once

#include "EntityFactory.h"
#include "Player.h"

//小地图
//TODO 效率太低，弃用
class MiniMap
{
	std::weak_ptr<EntityFactory> r_wallFactory;
	std::weak_ptr<Player> r_player;
	//纹理贴图资源
	ID3D11ShaderResourceView* mMiniWallTexSRV;
	ID3D11ShaderResourceView* mMiniPlayerTexSRV;
	ID3D11ShaderResourceView* mMiniEndTexSRV;

	//顶点，索引 相关信息
	//缓冲区
	ID3D11Buffer* mMiniWallVB;
	ID3D11Buffer* mMiniWallIB;
	//位移、定点数
	UINT mOneVertexCount;
	UINT mOneIndexCount;
public:
	MiniMap();
	~MiniMap();
	void init(ID3D11Device* d3dDevice, std::shared_ptr<Player>& player, std::shared_ptr<EntityFactory>& factroy);
	//绘制2D小地图
	void draw(ID3D11DeviceContext* d3dImmediateContext);
protected:
	//生成各个图块相应的 顶点+索引
	//TODO 很蠢的方法 严重减少效率
	void buildMiniWallVertexIndex(ID3D11Device* d3dDevice);
};

