#include "Camera.h"
#include "RenderingPipeline.h"
#include "Base/WinApp.h"
#include "Common/Logs.h"
#include <imgui.h>
#include <cmath>

namespace MyEngine {

Camera::Camera(WinApp *winApp) noexcept {
    // nullチェック
    if (!winApp) {
        Log("winApp is null.", kLogLevelFlagError);
        assert(false);
    }
    cameraScale_ = { 1.0f, 1.0f, 1.0f };
    cameraRotate_ = { 0.0f, 0.0f, 0.0f };
    cameraTranslate_ = { 0.0f, 0.0f, 0.0f };
    cameraMatrix_.SetScale(cameraScale_);
    cameraMatrix_.SetRotate(cameraRotate_);
    cameraMatrix_.SetTranslate(cameraTranslate_);
    worldMatrix_.MakeAffine({ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });
    CalculateMatrix();
}

Camera::Camera(WinApp *winApp, const Vector3 &cameraTranslate, const Vector3 &cameraRotate, const Vector3 &cameraScale) noexcept {
    winApp_ = winApp;
    cameraScale_ = cameraScale;
    cameraRotate_ = cameraRotate;
    cameraTranslate_ = cameraTranslate;
    cameraMatrix_.SetScale(cameraScale);
    cameraMatrix_.SetRotate(cameraRotate);
    cameraMatrix_.SetTranslate(cameraTranslate);
    worldMatrix_.MakeAffine({ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });
    CalculateMatrix();
}

void Camera::SetCamera(const Vector3 &cameraTranslate, const Vector3 &cameraRotate, const Vector3 &cameraScale) noexcept {
    cameraMatrix_.SetTranslate(cameraTranslate);
    cameraMatrix_.SetRotate(cameraRotate);
    cameraMatrix_.SetScale(cameraScale);
}

void Camera::SetTranslate(const Vector3 &cameraTranslate) noexcept {
    cameraMatrix_.SetTranslate(cameraTranslate);
}

void Camera::SetRotate(const Vector3 &cameraRotate) noexcept {
    cameraMatrix_.SetRotate(cameraRotate);
}

void Camera::SetScale(const Vector3 &cameraScale) noexcept {
    cameraMatrix_.SetScale(cameraScale);
}

void Camera::SetWorldMatrix(const Matrix4x4 &worldMatrix) noexcept {
    worldMatrix_ = worldMatrix;
}

void Camera::CalculateMatrix() noexcept {
    cameraMatrix_.SetScale(cameraScale_);
    cameraMatrix_.SetRotate(cameraRotate_);
    cameraMatrix_.SetTranslate(cameraTranslate_);
    viewMatrix_ = cameraMatrix_.InverseScale() * cameraMatrix_.InverseRotate() * cameraMatrix_.InverseTranslate();
    projectionMatrix_ = MakePerspectiveFovMatrix(0.45f, static_cast<float>(winApp_->GetClientWidth()) / static_cast<float>(winApp_->GetClientHeight()), 0.1f, 100.0f);
    wvpMatrix_ = worldMatrix_ * (viewMatrix_ * projectionMatrix_);
    viewportMatrix_ = MakeViewportMatrix(0.0f, 0.0f, static_cast<float>(winApp_->GetClientWidth()), static_cast<float>(winApp_->GetClientHeight()), 0.0f, 1.0f);
}

void Camera::MoveToMouse(const float translateSpeed, const float rotateSpeed, const float scaleSpeed) noexcept {
    // ImGuiウィンドウを触ってるときは操作しない
    if (ImGui::IsAnyItemActive()) {
        return;
    }

    // 左クリックで平行移動
    if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
        ImVec2 mouseDelta = ImGui::GetIO().MouseDelta;
        cameraTranslate_.x -= mouseDelta.x * translateSpeed;
        cameraTranslate_.y += mouseDelta.y * translateSpeed;
    }
    // 右クリックで回転
    if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
        ImVec2 mouseDelta = ImGui::GetIO().MouseDelta;
        cameraRotate_.x += mouseDelta.y * rotateSpeed;
        cameraRotate_.y += mouseDelta.x * rotateSpeed;
    }
    // ホイールで拡大縮小
    if (ImGui::GetIO().MouseWheel != 0.0f) {
        float mouseWheel = ImGui::GetIO().MouseWheel;
        cameraScale_.x -= mouseWheel * scaleSpeed;
        cameraScale_.y -= mouseWheel * scaleSpeed;
        cameraScale_.z -= mouseWheel * scaleSpeed;
    }
}

} // namespace MyEngine