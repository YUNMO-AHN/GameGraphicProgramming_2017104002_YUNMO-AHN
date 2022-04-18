#include "CustomCube.h"

void CustomCube::Update(_In_ FLOAT deltaTime) {

	angle += deltaTime;

	if (angle >= 360) {
		angle -= 360;
	}

	XMMATRIX mSpin = XMMatrixIdentity();
	XMMATRIX mTranslation = XMMatrixIdentity();
	XMMATRIX mScale = XMMatrixIdentity();
	XMMATRIX mOrbit = XMMatrixIdentity();

	mTranslation = XMMatrixTranslation(0.0f, 3.0f, 0.0f);
	mSpin *= XMMatrixRotationY(-angle * 0.5f);
	mScale = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	mOrbit = XMMatrixRotationX(angle * 3.0f);

	m_world = mScale * mSpin * mTranslation * mOrbit;
}