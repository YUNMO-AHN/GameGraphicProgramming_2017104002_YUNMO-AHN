#include "OrbitCube.h"

void OrbitCube::Update(_In_ FLOAT deltaTime) {

	angle += deltaTime;

	if (angle >= 360) {
		angle -= 360;
	}

	XMMATRIX mSpin = XMMatrixIdentity();
	XMMATRIX mTranslate = XMMatrixIdentity();
	XMMATRIX mOrbit = XMMatrixIdentity();
	XMMATRIX mScale = XMMatrixIdentity();

	mSpin *= XMMatrixRotationZ(-angle);
	mTranslate = XMMatrixTranslation(-4.0f, 0.0f, 0.0f);
	mOrbit *= XMMatrixRotationY(-angle * 2.0f);
	mScale = XMMatrixScaling(0.3f, 0.3f, 0.3f);

	m_world = mScale * mSpin * mTranslate * mOrbit;
}