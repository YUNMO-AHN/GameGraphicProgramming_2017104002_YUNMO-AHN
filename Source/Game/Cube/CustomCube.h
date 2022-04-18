#pragma once

#include "Common.h"
#include "Renderer/DataTypes.h"
#include "BaseCube.h"

class CustomCube : public BaseCube
{
public:
	CustomCube() = default;
	CustomCube(const CustomCube& other) = delete;
	CustomCube(CustomCube&& other) = delete;
	CustomCube& operator=(const CustomCube& other) = delete;
	CustomCube& operator=(CustomCube&& other) = delete;
	~CustomCube() = default;

	virtual void Update(_In_ FLOAT deltaTime) override;
private:
	FLOAT angle = 0;
};