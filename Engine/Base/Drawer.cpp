#include <cmath>

#include "Drawer.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "TextureManager.h"
#include "2d/ImGuiManager.h"
#include "3d/PrimitiveDrawer.h"

#include "Math/Camera.h"
#include "Math/RenderingPipeline.h"

#include "Common/Logs.h"
#include "Common/ConvertColor.h"
#include "Common/Descriptors/SRV.h"

#include "Objects/Triangle.h"
#include "Objects/Sprite.h"
#include "Objects/Sphere.h"
#include "Objects/Billboard.h"

#define M_PI (4.0f * std::atanf(1.0f))

namespace MyEngine {

Drawer::Drawer(WinApp *winApp, DirectXCommon *dxCommon, ImGuiManager *imguiManager, TextureManager *textureManager) {
    // nullチェック
    if (!winApp) {
        Log("winApp is null.", kLogLevelFlagError);
        assert(false);
    }
    if (!dxCommon) {
        Log("dxCommon is null.", kLogLevelFlagError);
        assert(false);
    }
    if (!imguiManager) {
        Log("imguiManager is null.", kLogLevelFlagError);
        assert(false);
    }
    if (!textureManager) {
        Log("textureManager is null.", kLogLevelFlagError);
        assert(false);
    }
    // 各クラスへのポインタを設定
    winApp_ = winApp;
    dxCommon_ = dxCommon;
    imguiManager_ = imguiManager;
    textureManager_ = textureManager;

    // 2D描画用の行列を初期化
    viewMatrix2D_.MakeIdentity();
    projectionMatrix2D_ = MakeOrthographicMatrix(
        0.0f,
        0.0f,
        static_cast<float>(winApp_->GetClientWidth()),
        static_cast<float>(winApp_->GetClientHeight()),
        0.0f,
        100.0f
    );

    // 初期化完了のログを出力
    Log("Drawer Initialized.");
    LogNewLine();
}

Drawer::~Drawer() {
    // 各クラスのポインタをnullにする
    winApp_ = nullptr;
    dxCommon_ = nullptr;
    imguiManager_ = nullptr;
    // 終了処理完了のログを出力
    Log("Drawer Finalized.");
}

void Drawer::PreDraw() {
    dxCommon_->PreDraw();
    imguiManager_->BeginFrame();

    static ID3D12DescriptorHeap *descriptorHeaps[] = { SRV::GetDescriptorHeap() };
    dxCommon_->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);

    //==================================================
    // 描画の準備
    //==================================================

    static auto pipelineSet = PrimitiveDrawer::CreateGraphicsPipeline(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
    // ビューポート
    D3D12_VIEWPORT viewport{};
    // クライアント領域のサイズと一緒にして画面全体に表示
    viewport.Width = static_cast<float>(winApp_->GetClientWidth());
    viewport.Height = static_cast<float>(winApp_->GetClientHeight());
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    // シザー矩形
    D3D12_RECT scissorRect{};
    // 基本的にビューポートと同じ矩形が構成されるようにする
    scissorRect.left = 0;
    scissorRect.right = winApp_->GetClientWidth();
    scissorRect.top = 0;
    scissorRect.bottom = winApp_->GetClientHeight();

    // コマンドを積む
    dxCommon_->GetCommandList()->RSSetViewports(1, &viewport);          // ビューポートを設定
    dxCommon_->GetCommandList()->RSSetScissorRects(1, &scissorRect);    // シザー矩形を設定
    // 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
    dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    // ルートシグネチャを設定。PSOに設定しているけど別途設定が必要
    dxCommon_->GetCommandList()->SetGraphicsRootSignature(pipelineSet->rootSignature.Get());
    dxCommon_->GetCommandList()->SetPipelineState(pipelineSet->pipelineState.Get());    // PSOを設定
    // SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である。
    dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetTextureSrvHandleGPU());
}

void Drawer::PostDraw() {
    imguiManager_->EndFrame();
    dxCommon_->PostDraw();
}

void Drawer::Draw(Triangle *triangle) {
    // メッシュがnullptrの場合は生成
    if (triangle->mesh == nullptr) {
        triangle->mesh = PrimitiveDrawer::CreateMesh(3);
    }

    // 頂点設定
    triangle->mesh->vertexBuffer->Map(0, nullptr, reinterpret_cast<void **>(&vertexData_));
    // 頂点データを設定
    vertexData_[0] = triangle->vertexData[0];
    vertexData_[1] = triangle->vertexData[1];
    vertexData_[2] = triangle->vertexData[2];

    // マテリアル用のリソースがnullptrの場合は生成
    if (triangle->materialResource == nullptr) {
        triangle->materialResource = PrimitiveDrawer::CreateBufferResources(sizeof(Vector4));
    }
    triangle->materialResource->Map(0, nullptr, reinterpret_cast<void **>(&materialData_));
    *materialData_ = ConvertColor(triangle->color);

    // WVP用のリソースがnullptrの場合は生成
    if (triangle->wvpResource == nullptr) {
        triangle->wvpResource = PrimitiveDrawer::CreateBufferResources(sizeof(Matrix4x4));
    }
    triangle->wvpResource->Map(0, nullptr, reinterpret_cast<void **>(&wvpData_));

    // 行列を計算
    triangle->worldMatrix.MakeAffine(
        triangle->transform.scale,
        triangle->transform.rotate,
        triangle->transform.translate
    );
    // Cameraがnullptrの場合は2D描画
    if (triangle->camera == nullptr) {
        wvpMatrix2D_ = triangle->worldMatrix * (viewMatrix2D_ * projectionMatrix2D_);
        *wvpData_ = wvpMatrix2D_;
    } else {
        triangle->camera->SetWorldMatrix(triangle->worldMatrix);
        triangle->camera->CalculateMatrix();
        *wvpData_ = triangle->camera->GetWVPMatrix();
    }

    // VBVを設定
    dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &triangle->mesh->vertexBufferView);
    // マテリアルCBufferの場所を指定
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, triangle->materialResource->GetGPUVirtualAddress());
    // WVP用のCBufferの場所を指定
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, triangle->wvpResource->GetGPUVirtualAddress());
    // 描画
    dxCommon_->GetCommandList()->DrawInstanced(3, 1, 0, 0);
}

void Drawer::Draw(Sprite *sprite) {
    // メッシュがnullptrの場合は生成
    if (sprite->mesh == nullptr) {
        sprite->mesh = PrimitiveDrawer::CreateMesh(6);
    }

    // 頂点設定
    sprite->mesh->vertexBuffer->Map(0, nullptr, reinterpret_cast<void **>(&vertexData_));
    // 頂点データを設定
    vertexData_[0] = sprite->vertexData[2];
    vertexData_[1] = sprite->vertexData[0];
    vertexData_[2] = sprite->vertexData[3];
    vertexData_[3] = sprite->vertexData[0];
    vertexData_[4] = sprite->vertexData[1];
    vertexData_[5] = sprite->vertexData[3];

    // マテリアル用のリソースがnullptrの場合は生成
    if (sprite->materialResource == nullptr) {
        sprite->materialResource = PrimitiveDrawer::CreateBufferResources(sizeof(Vector4));
    }
    sprite->materialResource->Map(0, nullptr, reinterpret_cast<void **>(&materialData_));
    *materialData_ = ConvertColor(sprite->color);

    // WVP用のリソースがnullptrの場合は生成
    if (sprite->wvpResource == nullptr) {
        sprite->wvpResource = PrimitiveDrawer::CreateBufferResources(sizeof(Matrix4x4));
    }
    sprite->wvpResource->Map(0, nullptr, reinterpret_cast<void **>(&wvpData_));

    // 行列を計算
    sprite->worldMatrix.MakeAffine(
        sprite->transform.scale,
        sprite->transform.rotate,
        sprite->transform.translate
    );
    // Cameraがnullptrの場合は2D描画
    if (sprite->camera == nullptr) {
        wvpMatrix2D_ = sprite->worldMatrix * (viewMatrix2D_ * projectionMatrix2D_);
        *wvpData_ = wvpMatrix2D_;
    } else {
        sprite->camera->SetWorldMatrix(sprite->worldMatrix);
        sprite->camera->CalculateMatrix();
        *wvpData_ = sprite->camera->GetWVPMatrix();
    }

    // VBVを設定
    dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &sprite->mesh->vertexBufferView);
    // マテリアルCBufferの場所を指定
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, sprite->materialResource->GetGPUVirtualAddress());
    // WVP用のCBufferの場所を指定
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, sprite->wvpResource->GetGPUVirtualAddress());
    // 描画
    dxCommon_->GetCommandList()->DrawInstanced(6, 1, 0, 0);
}

void Drawer::Draw(Sphere *sphere) {
    // メッシュがnullptrの場合は生成
    if (sphere->mesh == nullptr) {
        sphere->mesh = PrimitiveDrawer::CreateMesh(sphere->kVertexCount);
    }
    // 頂点設定
    sphere->mesh->vertexBuffer->Map(0, nullptr, reinterpret_cast<void **>(&vertexData_));

    // 軽度分割1つ分の角度
    const float kLonEvery = M_PI * 2.0f / static_cast<float>(sphere->kSubdivision);
    // 緯度分割1つ分の角度
    const float kLatEvery = M_PI / static_cast<float>(sphere->kSubdivision);
    // 緯度の方向に分割 -π/2 ～ π/2
    for (uint32_t latIndex = 0; latIndex < sphere->kSubdivision; latIndex++) {
        const float lat = -M_PI / 2.0f + (kLatEvery * static_cast<float>(latIndex));
        // 経度の方向に分割 0 ～ 2π
        for (uint32_t lonIndex = 0; lonIndex < sphere->kSubdivision; lonIndex++) {
            // 頂点位置を計算
            const uint32_t startIndex = (latIndex * sphere->kSubdivision + lonIndex) * 6;
            const float lon = static_cast<float>(lonIndex) * kLonEvery;
            
            //--------- 頂点データを設定 ---------//

            // 左下
            vertexData_[startIndex + 0].position = {
                sphere->radius * std::cosf(lat) * std::cosf(lon),
                sphere->radius * std::sinf(lat),
                sphere->radius * std::cosf(lat) * std::sinf(lon),
                1.0f
            };
            vertexData_[startIndex + 0].texCoord = {
                static_cast<float>(lonIndex) / static_cast<float>(sphere->kSubdivision),
                1.0f - static_cast<float>(latIndex) / static_cast<float>(sphere->kSubdivision)
            };
            // 左上
            vertexData_[startIndex + 1].position = {
                sphere->radius * std::cosf(lat + kLatEvery) * std::cosf(lon),
                sphere->radius * std::sinf(lat + kLatEvery),
                sphere->radius * std::cosf(lat + kLatEvery) * std::sinf(lon),
                1.0f
            };
            vertexData_[startIndex + 1].texCoord = {
                static_cast<float>(lonIndex) / static_cast<float>(sphere->kSubdivision),
                1.0f - static_cast<float>(latIndex + 1) / static_cast<float>(sphere->kSubdivision)
            };
            // 右下
            vertexData_[startIndex + 2].position = {
                sphere->radius * std::cosf(lat) * std::cosf(lon + kLonEvery),
                sphere->radius * std::sinf(lat),
                sphere->radius * std::cosf(lat) * std::sinf(lon + kLonEvery),
                1.0f
            };
            vertexData_[startIndex + 2].texCoord = {
                static_cast<float>(lonIndex + 1) / static_cast<float>(sphere->kSubdivision),
                1.0f - static_cast<float>(latIndex) / static_cast<float>(sphere->kSubdivision)
            };
            // 左上
            vertexData_[startIndex + 3] = vertexData_[startIndex + 1];
            // 右上
            vertexData_[startIndex + 4].position = {
                sphere->radius * std::cosf(lat + kLatEvery) * std::cosf(lon + kLonEvery),
                sphere->radius * std::sinf(lat + kLatEvery),
                sphere->radius * std::cosf(lat + kLatEvery) * std::sinf(lon + kLonEvery),
                1.0f
            };
            vertexData_[startIndex + 4].texCoord = {
                static_cast<float>(lonIndex + 1) / static_cast<float>(sphere->kSubdivision),
                1.0f - static_cast<float>(latIndex + 1) / static_cast<float>(sphere->kSubdivision)
            };
            // 右下
            vertexData_[startIndex + 5] = vertexData_[startIndex + 2];
        }
    }

    // マテリアル用のリソースがnullptrの場合は生成
    if (sphere->materialResource == nullptr) {
        sphere->materialResource = PrimitiveDrawer::CreateBufferResources(sizeof(Vector4));
    }
    sphere->materialResource->Map(0, nullptr, reinterpret_cast<void **>(&materialData_));
    *materialData_ = ConvertColor(sphere->color);

    // WVP用のリソースがnullptrの場合は生成
    if (sphere->wvpResource == nullptr) {
        sphere->wvpResource = PrimitiveDrawer::CreateBufferResources(sizeof(Matrix4x4));
    }
    sphere->wvpResource->Map(0, nullptr, reinterpret_cast<void **>(&wvpData_));

    // 行列を計算
    sphere->worldMatrix.MakeAffine(
        sphere->transform.scale,
        sphere->transform.rotate,
        sphere->transform.translate
    );
    // Cameraがnullptrの場合は2D描画
    if (sphere->camera == nullptr) {
        wvpMatrix2D_ = sphere->worldMatrix * (viewMatrix2D_ * projectionMatrix2D_);
        *wvpData_ = wvpMatrix2D_;
    } else {
        sphere->camera->SetWorldMatrix(sphere->worldMatrix);
        sphere->camera->CalculateMatrix();
        *wvpData_ = sphere->camera->GetWVPMatrix();
    }

    // VBVを設定
    dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &sphere->mesh->vertexBufferView);
    // マテリアルCBufferの場所を指定
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, sphere->materialResource->GetGPUVirtualAddress());
    // WVP用のCBufferの場所を指定
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, sphere->wvpResource->GetGPUVirtualAddress());
    // 描画
    dxCommon_->GetCommandList()->DrawInstanced(sphere->kVertexCount, 1, 0, 0);
}

void Drawer::Draw(BillBoard *billboard) {
    // メッシュがnullptrの場合は生成
    if (billboard->mesh == nullptr) {
        billboard->mesh = PrimitiveDrawer::CreateMesh(6);
    }

    // 頂点設定
    billboard->mesh->vertexBuffer->Map(0, nullptr, reinterpret_cast<void **>(&vertexData_));
    // 頂点データを設定
    vertexData_[0] = billboard->vertexData[2];
    vertexData_[1] = billboard->vertexData[0];
    vertexData_[2] = billboard->vertexData[3];
    vertexData_[3] = billboard->vertexData[0];
    vertexData_[4] = billboard->vertexData[1];
    vertexData_[5] = billboard->vertexData[3];

    // マテリアル用のリソースがnullptrの場合は生成
    if (billboard->materialResource == nullptr) {
        billboard->materialResource = PrimitiveDrawer::CreateBufferResources(sizeof(Vector4));
    }
    billboard->materialResource->Map(0, nullptr, reinterpret_cast<void **>(&materialData_));
    *materialData_ = ConvertColor(billboard->color);

    // WVP用のリソースがnullptrの場合は生成
    if (billboard->wvpResource == nullptr) {
        billboard->wvpResource = PrimitiveDrawer::CreateBufferResources(sizeof(Matrix4x4));
    }
    billboard->wvpResource->Map(0, nullptr, reinterpret_cast<void **>(&wvpData_));

    // カメラと同じ向きにBillBoardを向ける
    billboard->transform.rotate = billboard->camera->GetRotate();

    // 行列を計算
    billboard->worldMatrix.MakeAffine(
        billboard->transform.scale,
        billboard->transform.rotate,
        billboard->transform.translate
    );

    // BillBoardはカメラを必ず設定しないとなのでnullptrの場合はエラー
    if (billboard->camera == nullptr) {
        Log("BillBoard camera is null.", kLogLevelFlagError);
        assert(false);
    }
    billboard->camera->SetWorldMatrix(billboard->worldMatrix);
    billboard->camera->CalculateMatrix();
    *wvpData_ = billboard->camera->GetWVPMatrix();

    // VBVを設定
    dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &billboard->mesh->vertexBufferView);
    // マテリアルCBufferの場所を指定
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, billboard->materialResource->GetGPUVirtualAddress());
    // WVP用のCBufferの場所を指定
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, billboard->wvpResource->GetGPUVirtualAddress());
    // 描画
    dxCommon_->GetCommandList()->DrawInstanced(6, 1, 0, 0);
}

} // namespace MyEngine