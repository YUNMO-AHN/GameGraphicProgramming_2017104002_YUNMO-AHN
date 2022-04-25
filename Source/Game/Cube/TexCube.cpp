#include "TexCube.h"

TexCube::TexCube(const std::filesystem::path& textureFilePath) : BaseCube(textureFilePath)
{
}

void TexCube::Update(_In_ FLOAT deltaTime)
{
    static FLOAT s_totalTime = 0.0f;
    s_totalTime += deltaTime;

    m_world = XMMatrixTranslation(4.0f, XMScalarSin(s_totalTime), 0.0f) * XMMatrixRotationY(s_totalTime);
}