#include "RenderingPipeline.h"

namespace MyEngine {

Matrix4x4 MakePerspectiveFovMatrix(const float fovY, const float aspectRatio, const float nearClip, const float farClip) noexcept {
    const float cot = 1.0f / std::tan(fovY / 2.0f);
    return Matrix4x4(
        cot / aspectRatio, 0.0f, 0.0f, 0.0f,
        0.0f, cot, 0.0f, 0.0f,
        0.0f, 0.0f, farClip / (farClip - nearClip), 1.0f,
        0.0f, 0.0f, -(nearClip * farClip) / (farClip - nearClip), 0.0f
    );
}

Matrix4x4 MakeOrthographicMatrix(const float left, const float top, const float right, const float bottom, const float nearClip, const float farClip) noexcept {
    return Matrix4x4(
        2.0f / (right - left), 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f / (farClip - nearClip), 0.0f,
        (left + right) / (left - right), (top + bottom) / (bottom - top), nearClip / (nearClip - farClip), 1.0f
    );
}

Matrix4x4 MakeViewportMatrix(const float left, const float top, const float width, const float height, const float minDepth, const float maxDepth) noexcept {
    return Matrix4x4(
        width / 2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, -height / 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, maxDepth - minDepth, 0.0f,
        left + (width / 2.0f), top + (height / 2.0f), minDepth, 1.0f
    );
}

} // namespace MyEngine