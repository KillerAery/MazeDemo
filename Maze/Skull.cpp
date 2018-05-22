#include "Skull.h"

Skull::Skull()
{
	//设置材料属性
	mMat.Ambient = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	mMat.Diffuse = XMFLOAT4(0.48f, 0.55f, 0.46f, 1.0f);
	mMat.Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);
}


Skull::~Skull()
{
}

shared_ptr<Skull> Skull::create() {
	shared_ptr<Skull> wall = std::make_shared<Skull>();
	return wall;
}

void Skull::draw(ID3D11DeviceContext* d3dImmediateContext, CXMMATRIX view, CXMMATRIX proj, int passIndex) {					
	//绘制骷髅模型
	XMMATRIX scale = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	//XMMATRIX rotation = XMMatrixRotationQuaternion(XMLoadFloat3(&m_rotation));
	XMMATRIX offset = XMMatrixTranslation(m_position.x ,m_position.y ,m_position.z);

	XMMATRIX world = scale*offset;
	XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
	XMMATRIX worldViewProj = world * view * proj;

	Effects::BasicFX->SetWorld(world);
	Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
	Effects::BasicFX->SetWorldViewProj(worldViewProj);
	Effects::BasicFX->SetTexTransform(XMMatrixIdentity());
	Effects::BasicFX->SetMaterial(mMat);
	Effects::BasicFX->SetDiffuseMap(0);

	ID3DX11EffectTechnique* activeTexTech = Effects::BasicFX->Light1Tech;
	activeTexTech->GetPassByIndex(passIndex)->Apply(0, d3dImmediateContext);

	d3dImmediateContext->DrawIndexed(mIndexCount, mIndexOffset, mVertexOffset);

}

void Skull::update(float dt) {

}