#pragma once
#include "Vector3.h"
#include "Matrix4x4.h"

namespace MyEngine {

struct AffineMatrix {
    AffineMatrix() noexcept = default;
    AffineMatrix(const Matrix4x4 &scale, const Matrix4x4 &rotate, const Matrix4x4 &translate) noexcept
        : scaleMatrix(scale), rotateMatrix(rotate), translateMatrix(translate), worldMatrix(scale *rotate *translate)
    {}
    AffineMatrix(const Vector3 &scale, const Vector3 &rotate, const Vector3 &translate) noexcept {
        scaleMatrix.MakeScale(scale);
        rotateMatrix.MakeRotate(rotate);
        translateMatrix.MakeTranslate(translate);
        worldMatrix = scaleMatrix * rotateMatrix * translateMatrix;
    }
    AffineMatrix(const AffineMatrix &affine) noexcept
        : scaleMatrix(affine.scaleMatrix), rotateMatrix(affine.rotateMatrix), translateMatrix(affine.translateMatrix), worldMatrix(affine.worldMatrix)
    {}

    AffineMatrix &operator=(const AffineMatrix &affine) noexcept {
        scaleMatrix = affine.scaleMatrix;
        rotateMatrix = affine.rotateMatrix;
        translateMatrix = affine.translateMatrix;
        worldMatrix = affine.worldMatrix;
        return *this;
    }
    AffineMatrix &operator*=(const AffineMatrix &affine) noexcept {
        scaleMatrix *= affine.scaleMatrix;
        rotateMatrix *= affine.rotateMatrix;
        translateMatrix *= affine.translateMatrix;
        worldMatrix = scaleMatrix * rotateMatrix * translateMatrix;
        return *this;
    }
    AffineMatrix &operator*=(const Matrix4x4 &mat) noexcept {
        scaleMatrix *= mat;
        rotateMatrix *= mat;
        translateMatrix *= mat;
        worldMatrix = scaleMatrix * rotateMatrix * translateMatrix;
        return *this;
    }

    void SetScale(const Vector3 &scale) noexcept {
        scaleMatrix.MakeScale(scale);
        worldMatrix = scaleMatrix * rotateMatrix * translateMatrix;
    }
    void SetRotate(const Vector3 &rotate) noexcept {
        rotateMatrix.MakeRotate(rotate);
        worldMatrix = scaleMatrix * rotateMatrix * translateMatrix;
    }
    void SetTranslate(const Vector3 &translate) noexcept {
        translateMatrix.MakeTranslate(translate);
        worldMatrix = scaleMatrix * rotateMatrix * translateMatrix;
    }

    Matrix4x4 InverseScale() const noexcept {
        return Matrix4x4(
            1.0f / scaleMatrix.m[0][0], 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f / scaleMatrix.m[1][1], 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f / scaleMatrix.m[2][2], 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }
    Matrix4x4 InverseRotate() const noexcept {
        return Matrix4x4(
            rotateMatrix.m[0][0], rotateMatrix.m[1][0], rotateMatrix.m[2][0], 0.0f,
            rotateMatrix.m[0][1], rotateMatrix.m[1][1], rotateMatrix.m[2][1], 0.0f,
            rotateMatrix.m[0][2], rotateMatrix.m[1][2], rotateMatrix.m[2][2], 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }
    Matrix4x4 InverseTranslate() const noexcept {
        return Matrix4x4(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            -translateMatrix.m[3][0], -translateMatrix.m[3][1], -translateMatrix.m[3][2], 1.0f
        );
    }

    Matrix4x4 scaleMatrix;
    Matrix4x4 rotateMatrix;
    Matrix4x4 translateMatrix;
    Matrix4x4 worldMatrix;
};

} // namespace MyEngine