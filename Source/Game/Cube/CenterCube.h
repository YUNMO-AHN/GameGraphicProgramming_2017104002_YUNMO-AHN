#pragma once

#include "Common.h"
#include "Renderer/DataTypes.h"
#include "BaseCube.h"

class CenterCube : public BaseCube
{
public:
	CenterCube() = default;
	CenterCube(const CenterCube& other) = delete;
	CenterCube(CenterCube&& other) = delete;
	CenterCube& operator=(const CenterCube& other) = delete;
	CenterCube& operator=(CenterCube&& other) = delete;
	~CenterCube() = default;

	virtual void Update(_In_ FLOAT deltaTime) override;
};