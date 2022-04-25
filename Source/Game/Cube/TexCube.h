#pragma once

#include "Common.h"
#include "BaseCube.h"

class TexCube : public BaseCube
{
public:
    TexCube(const std::filesystem::path& textureFilePath);
    TexCube(const TexCube& other) = delete;
    TexCube(TexCube&& other) = delete;
    TexCube& operator=(const TexCube& other) = delete;
    TexCube& operator=(TexCube&& other) = delete;
    ~TexCube() = default;

    virtual void Update(_In_ FLOAT deltaTime) override;
};