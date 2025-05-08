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
    viewport.Width = 1280.0f /*static_cast<float>(winApp_->GetClientWidth())*/;
    viewport.Height = 720.0f /*static_cast<float>(winApp_->GetClientHeight())*/;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    // シザー矩形
    D3D12_RECT scissorRect{};
    // 基本的にビューポートと同じ矩形が構成されるようにする
    scissorRect.left = 0;
    scissorRect.right = 1280.0f /*winApp_->GetClientWidth()*/;
    scissorRect.top = 0;
    scissorRect.bottom = 720.0f /*winApp_->GetClientHeight()*/;

    // コマンドを積む
    dxCommon_->GetCommandList()->RSSetViewports(1, &viewport);          // ビューポートを設定
    dxCommon_->GetCommandList()->RSSetScissorRects(1, &scissorRect);    // シザー矩形を設定
    // 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
    dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    // ルートシグネチャを設定。PSOに設定しているけど別途設定が必要
    dxCommon_->GetCommandList()->SetGraphicsRootSignature(pipelineSet->rootSignature.Get());
    dxCommon_->GetCommandList()->SetPipelineState(pipelineSet->pipelineState.Get());    // PSOを設定
}

void Drawer::PostDraw() {
    imguiManager_->EndFrame();
    dxCommon_->PostDraw();
}

void Drawer::SetLight(DirectionalLight *light) {
    // 光源のリソースを生成
    static auto directionalLightResource = PrimitiveDrawer::CreateBufferResources(sizeof(DirectionalLight));
    directionalLightResource->Map(0, nullptr, reinterpret_cast<void **>(&directionalLightData_));
    // 光源のデータを設定
    directionalLightData_->color = ConvertColor(light->color);
    directionalLightData_->direction = light->direction;
    directionalLightData_->intensity = light->intensity;

    // CBufferの場所を指定
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());
}

void Drawer::Draw(Triangle *triangle) {
    // 法線を設定
    static Vector3 position[3];
    if (triangle->camera == nullptr || triangle->material.enableLighting == false) {
        for (int i = 0; i < 3; i++) {
            triangle->mesh->vertexBufferMap[i].normal = { 0.0f, 0.0f, -1.0f };
        }
    } else {
        if (triangle->normalType == kNormalTypeVertex) {
            for (int i = 0; i < 3; i++) {
                triangle->mesh->vertexBufferMap[i].normal =
                    Vector3(triangle->mesh->vertexBufferMap[i].position);
            }
        } else if (triangle->normalType == kNormalTypeFace) {
            position[0] = Vector3(triangle->mesh->vertexBufferMap[0].position);
            position[1] = Vector3(triangle->mesh->vertexBufferMap[1].position);
            position[2] = Vector3(triangle->mesh->vertexBufferMap[2].position);
            for (int i = 0; i < 3; i++) {
                triangle->mesh->vertexBufferMap[i].normal =
                    (position[1] - position[0]).Cross(position[2] - position[1]).Normalize();
            }
        }
    }

    // マテリアルを設定
    triangle->materialMap->color = ConvertColor(triangle->material.color);
    triangle->materialMap->enableLighting = triangle->material.enableLighting;
    triangle->materialMap->uvTransform.MakeAffine(
        triangle->uvTransform.scale,
        triangle->uvTransform.rotate,
        triangle->uvTransform.translate
    );

    // 行列を計算
    triangle->worldMatrix.MakeAffine(
        triangle->transform.scale,
        triangle->transform.rotate,
        triangle->transform.translate
    );
    // Cameraがnullptrの場合は2D描画
    if (triangle->camera == nullptr) {
        wvpMatrix2D_ = triangle->worldMatrix * (viewMatrix2D_ * projectionMatrix2D_);
        triangle->transformationMatrixMap->wvp = wvpMatrix2D_;
        triangle->transformationMatrixMap->world = triangle->worldMatrix;
    } else {
        triangle->camera->SetWorldMatrix(triangle->worldMatrix);
        triangle->camera->CalculateMatrix();
        triangle->transformationMatrixMap->wvp = triangle->camera->GetWVPMatrix();
        triangle->transformationMatrixMap->world = triangle->worldMatrix;
    }

    // SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である。
    if (triangle->useTextureIndex != -1) {
        dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetTexture(triangle->useTextureIndex).srvHandleGPU);
    } else {
        // テクスチャを使用しない場合は0を設定
        dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetTexture(0).srvHandleGPU);
    }

    // VBVを設定
    dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &triangle->mesh->vertexBufferView);
    // IBVを設定
    dxCommon_->GetCommandList()->IASetIndexBuffer(&triangle->mesh->indexBufferView);
    // マテリアルCBufferの場所を指定
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, triangle->materialResource->GetGPUVirtualAddress());
    // TransformationMatrix用のCBufferの場所を指定
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, triangle->transformationMatrixResource->GetGPUVirtualAddress());
    // 描画
    dxCommon_->GetCommandList()->DrawIndexedInstanced(3, 1, 0, 0, 0);
}

void Drawer::Draw(Sprite *sprite) {
    // 法線を設定
    static Vector3 position[3];
    if (sprite->camera == nullptr || sprite->material.enableLighting == false) {
        for (int i = 0; i < 4; i++) {
            sprite->mesh->vertexBufferMap[i].normal = { 0.0f, 0.0f, -1.0f };
        }
    } else {
        if (sprite->normalType == kNormalTypeVertex) {
            for (int i = 0; i < 4; i++) {
                sprite->mesh->vertexBufferMap[i].normal =
                    Vector3(sprite->mesh->vertexBufferMap[i].position);
            }
        } else if (sprite->normalType == kNormalTypeFace) {
            position[0] = Vector3(sprite->mesh->vertexBufferMap[0].position);
            position[1] = Vector3(sprite->mesh->vertexBufferMap[1].position);
            position[2] = Vector3(sprite->mesh->vertexBufferMap[2].position);
            for (int i = 0; i < 4; i++) {
                sprite->mesh->vertexBufferMap[sprite->mesh->indexBufferMap[i]].normal =
                    (position[1] - position[0]).Cross(position[2] - position[1]).Normalize();
            }
        }
    }

    // マテリアルを設定
    sprite->materialMap->color = ConvertColor(sprite->material.color);
    sprite->materialMap->enableLighting = sprite->material.enableLighting;
    sprite->materialMap->uvTransform.MakeAffine(
        sprite->uvTransform.scale,
        sprite->uvTransform.rotate,
        sprite->uvTransform.translate
    );

    // 行列を計算
    sprite->worldMatrix.MakeAffine(
        sprite->transform.scale,
        sprite->transform.rotate,
        sprite->transform.translate
    );
    // Cameraがnullptrの場合は2D描画
    if (sprite->camera == nullptr) {
        wvpMatrix2D_ = sprite->worldMatrix * (viewMatrix2D_ * projectionMatrix2D_);
        sprite->transformationMatrixMap->wvp = wvpMatrix2D_;
        sprite->transformationMatrixMap->world = sprite->worldMatrix;
    } else {
        sprite->camera->SetWorldMatrix(sprite->worldMatrix);
        sprite->camera->CalculateMatrix();
        sprite->transformationMatrixMap->wvp = sprite->camera->GetWVPMatrix();
        sprite->transformationMatrixMap->world = sprite->worldMatrix;
    }

    // SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である。
    if (sprite->useTextureIndex != -1) {
        dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetTexture(sprite->useTextureIndex).srvHandleGPU);
    } else {
        // テクスチャを使用しない場合は0を設定
        dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetTexture(0).srvHandleGPU);
    }

    // VBVを設定
    dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &sprite->mesh->vertexBufferView);
    // IBVを設定
    dxCommon_->GetCommandList()->IASetIndexBuffer(&sprite->mesh->indexBufferView);
    // マテリアルCBufferの場所を指定
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, sprite->materialResource->GetGPUVirtualAddress());
    // TransformationMatrix用のCBufferの場所を指定
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, sprite->transformationMatrixResource->GetGPUVirtualAddress());
    // 描画
    dxCommon_->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void Drawer::Draw(Sphere *sphere) {
    static Vector3 position[3];
    // 緯度の方向に分割
    for (uint32_t latIndex = 0; latIndex < sphere->kSubdivision; latIndex++) {
        // 経度の方向に分割
        for (uint32_t lonIndex = 0; lonIndex < sphere->kSubdivision; lonIndex++) {
            // インデックスを計算
            const uint32_t startIndex = (latIndex * sphere->kSubdivision + lonIndex) * 6;
            // 頂点位置を計算
            const uint32_t vertexIndex = (latIndex * sphere->kSubdivision + lonIndex) * 4;

            //--------- 法線を設定 ---------//

            if (sphere->camera == nullptr || sphere->material.enableLighting == false) {
                for (int i = 0; i < 4; i++) {
                    sphere->mesh->vertexBufferMap[vertexIndex + i].normal = { 0.0f, 0.0f, -1.0f };
                }
                continue;
            }
            if (sphere->normalType == kNormalTypeVertex) {
                sphere->mesh->vertexBufferMap[vertexIndex + 0].normal =
                    Vector3(sphere->mesh->vertexBufferMap[vertexIndex + 0].position);
                sphere->mesh->vertexBufferMap[vertexIndex + 1].normal =
                    Vector3(sphere->mesh->vertexBufferMap[vertexIndex + 1].position);
                sphere->mesh->vertexBufferMap[vertexIndex + 2].normal = 
                    Vector3(sphere->mesh->vertexBufferMap[vertexIndex + 2].position);
                sphere->mesh->vertexBufferMap[vertexIndex + 3].normal =
                    Vector3(sphere->mesh->vertexBufferMap[vertexIndex + 3].position);

            } else if (sphere->normalType == kNormalTypeFace) {
                position[0] = Vector3(sphere->mesh->vertexBufferMap[vertexIndex + 0].position);
                position[1] = Vector3(sphere->mesh->vertexBufferMap[vertexIndex + 1].position);
                position[2] = Vector3(sphere->mesh->vertexBufferMap[vertexIndex + 2].position);
                for (int i = 0; i < 4; i++) {
                    sphere->mesh->vertexBufferMap[vertexIndex + i].normal =
                        (position[1] - position[0]).Cross(position[2] - position[1]).Normalize();
                }
            }
        }
    }
    // 面法線の場合、真下の面の法線だけ別で計算する
    if (sphere->normalType == kNormalTypeFace) {
        for (int i = 0; i < sphere->kSubdivision; i++) {
            const uint32_t vertexIndex = i * 4;
            position[0] = Vector3(sphere->mesh->vertexBufferMap[vertexIndex + 1].position);
            position[1] = Vector3(sphere->mesh->vertexBufferMap[vertexIndex + 3].position);
            position[2] = Vector3(sphere->mesh->vertexBufferMap[vertexIndex + 2].position);
            for (int i = 0; i < 4; i++) {
                sphere->mesh->vertexBufferMap[vertexIndex + i].normal =
                    (position[1] - position[0]).Cross(position[2] - position[1]).Normalize();
            }
        }
    }

    // マテリアルを設定
    sphere->materialMap->color = ConvertColor(sphere->material.color);
    sphere->materialMap->enableLighting = sphere->material.enableLighting;
    sphere->materialMap->uvTransform.MakeAffine(
        sphere->uvTransform.scale,
        sphere->uvTransform.rotate,
        sphere->uvTransform.translate
    );

    // 行列を計算
    sphere->worldMatrix.MakeAffine(
        sphere->transform.scale * sphere->radius,
        sphere->transform.rotate,
        sphere->transform.translate
    );
    // Cameraがnullptrの場合は2D描画
    if (sphere->camera == nullptr) {
        wvpMatrix2D_ = sphere->worldMatrix * (viewMatrix2D_ * projectionMatrix2D_);
        sphere->transformationMatrixMap->wvp = wvpMatrix2D_;
        sphere->transformationMatrixMap->world = sphere->worldMatrix;
    } else {
        sphere->camera->SetWorldMatrix(sphere->worldMatrix);
        sphere->camera->CalculateMatrix();
        sphere->transformationMatrixMap->wvp = sphere->camera->GetWVPMatrix();
        sphere->transformationMatrixMap->world = sphere->worldMatrix;
    }

    // SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である。
    if (sphere->useTextureIndex != -1) {
        dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetTexture(sphere->useTextureIndex).srvHandleGPU);
    } else {
        // テクスチャを使用しない場合は0を設定
        dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetTexture(0).srvHandleGPU);
    }

    // IBVを設定
    dxCommon_->GetCommandList()->IASetIndexBuffer(&sphere->mesh->indexBufferView);
    // VBVを設定
    dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &sphere->mesh->vertexBufferView);
    // マテリアルCBufferの場所を指定
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, sphere->materialResource->GetGPUVirtualAddress());
    // TransformationMatrix用のCBufferの場所を指定
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, sphere->transformationMatrixResource->GetGPUVirtualAddress());
    // 描画
    dxCommon_->GetCommandList()->DrawIndexedInstanced(sphere->kIndexCount, 1, 0, 0, 0);
}

void Drawer::Draw(BillBoard *billboard) {
    // 法線を設定
    static Vector3 position[3];
    if (billboard->material.enableLighting == false) {
        for (int i = 0; i < 4; i++) {
            billboard->mesh->vertexBufferMap[i].normal = { 0.0f, 0.0f, -1.0f };
        }
    } else {
        if (billboard->normalType == kNormalTypeVertex) {
            for (int i = 0; i < 4; i++) {
                billboard->mesh->vertexBufferMap[i].normal =
                    Vector3(billboard->mesh->vertexBufferMap[i].position);
            }
        } else if (billboard->normalType == kNormalTypeFace) {
            position[0] = Vector3(billboard->mesh->vertexBufferMap[0].position);
            position[1] = Vector3(billboard->mesh->vertexBufferMap[1].position);
            position[2] = Vector3(billboard->mesh->vertexBufferMap[2].position);
            for (int i = 0; i < 4; i++) {
                billboard->mesh->vertexBufferMap[i].normal =
                    (position[1] - position[0]).Cross(position[2] - position[1]).Normalize();
            }
        }
    }

    // マテリアルを設定
    billboard->materialMap->color = ConvertColor(billboard->material.color);
    billboard->materialMap->enableLighting = billboard->material.enableLighting;
    billboard->materialMap->uvTransform.MakeAffine(
        billboard->uvTransform.scale,
        billboard->uvTransform.rotate,
        billboard->uvTransform.translate
    );

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
    billboard->transformationMatrixMap->wvp = billboard->camera->GetWVPMatrix();
    billboard->transformationMatrixMap->world = billboard->worldMatrix;

    // SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である。
    if (billboard->useTextureIndex != -1) {
        dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetTexture(billboard->useTextureIndex).srvHandleGPU);
    } else {
        // テクスチャを使用しない場合は0を設定
        dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetTexture(0).srvHandleGPU);
    }

    // VBVを設定
    dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &billboard->mesh->vertexBufferView);
    // IBVを設定
    dxCommon_->GetCommandList()->IASetIndexBuffer(&billboard->mesh->indexBufferView);
    // マテリアルCBufferの場所を指定
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, billboard->materialResource->GetGPUVirtualAddress());
    // TransformationMatrix用のCBufferの場所を指定
    dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, billboard->transformationMatrixResource->GetGPUVirtualAddress());
    // 描画
    dxCommon_->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

} // namespace MyEngine