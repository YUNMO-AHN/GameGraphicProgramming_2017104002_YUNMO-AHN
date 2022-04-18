#pragma once

#include "Common.h"
#include "Renderer/DataTypes.h"
#include "BaseCube.h"

class OrbitCube : public BaseCube
{
public:
	OrbitCube() = default;
	OrbitCube(const OrbitCube& other) = delete;
	OrbitCube(OrbitCube&& other) = delete;
	OrbitCube& operator=(const OrbitCube& other) = delete;
	OrbitCube& operator=(OrbitCube&& other) = delete;
	~OrbitCube() = default;

	virtual void Update(_In_ FLOAT deltaTime) override;
private:
	FLOAT angle = 0;
};