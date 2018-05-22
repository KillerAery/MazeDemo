#pragma once

#include "EntityFactory.h"
#include "Player.h"

//С��ͼ
//TODO Ч��̫�ͣ�����
class MiniMap
{
	std::weak_ptr<EntityFactory> r_wallFactory;
	std::weak_ptr<Player> r_player;
	//������ͼ��Դ
	ID3D11ShaderResourceView* mMiniWallTexSRV;
	ID3D11ShaderResourceView* mMiniPlayerTexSRV;
	ID3D11ShaderResourceView* mMiniEndTexSRV;

	//���㣬���� �����Ϣ
	//������
	ID3D11Buffer* mMiniWallVB;
	ID3D11Buffer* mMiniWallIB;
	//λ�ơ�������
	UINT mOneVertexCount;
	UINT mOneIndexCount;
public:
	MiniMap();
	~MiniMap();
	void init(ID3D11Device* d3dDevice, std::shared_ptr<Player>& player, std::shared_ptr<EntityFactory>& factroy);
	//����2DС��ͼ
	void draw(ID3D11DeviceContext* d3dImmediateContext);
protected:
	//���ɸ���ͼ����Ӧ�� ����+����
	//TODO �ܴ��ķ��� ���ؼ���Ч��
	void buildMiniWallVertexIndex(ID3D11Device* d3dDevice);
};

