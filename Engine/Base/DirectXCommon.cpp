#include <Windows.h>
#include <cassert>
#include <format>

#include "DirectXCommon.h"
#include "WinApp.h"

void DirectXCommon::Initialize() {
    // Windowsアプリクラスのインスタンスを取得
    winApp_ = WinApp::GetInstance();

    // DXGIの初期化
    InitializeDXGI();
    // コマンドキューの初期化
    InitializeCommandQueue();

    // 初期化完了のログを出力
    winApp_->Log("Complete Initialize DirectX.\n");
}

void DirectXCommon::InitializeDXGI() {
    // DXGIファクトリーの生成
    dxgiFactory_ = nullptr;
    // HRESULTはWindows系のエラーコードであり、
    // 関数が成功したかどうかをSUCCEEDEDマクロで判定できる
    HRESULT hr_ = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
    // 初期化の根本的な部分でエラーが出た場合はプログラムが間違っているか、
    // どうにもできない場合が多いのでassertにしておく
    assert(SUCCEEDED(hr_));

    // アダプタを初期化
    InitializeDXGIAdapter();
    // D3D12デバイスを初期化
    InitializeD3D12Device();

    // 初期化完了のログを出力
    winApp_->Log("Complete Initialize DXGI.\n");
}

void DirectXCommon::InitializeDXGIAdapter() {
    // 使用するアダプタ用の変数をnullptrで初期化しておく
    useAdapter_ = nullptr;

    // 良い順にアダプタを頼む
    for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter_)) != DXGI_ERROR_NOT_FOUND; ++i) {
        // アダプタの情報を取得する
        DXGI_ADAPTER_DESC3 adapterDesc{};
        HRESULT hr_ = useAdapter_->GetDesc3(&adapterDesc);

        // アダプタの情報を取得できたかどうかをチェック
        assert(SUCCEEDED(hr_));

        // ソフトウェアアダプタでなければ使用
        if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
            // 使用するアダプタの情報をログに出力。wstringの方なので変換しておく
            winApp_->Log(std::format(L"Use Adapter : {}\n", adapterDesc.Description));
            break;
        }

        // ソフトウェアアダプタの場合は見なかったことにする
        useAdapter_ = nullptr;
    }

    // アダプタが見つからなかった場合はエラー
    assert(useAdapter_ != nullptr);
}

void DirectXCommon::InitializeD3D12Device() {
    device_ = nullptr;
    // 機能レベルとログ出力用の文字列
    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_12_2,
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0
    };
    const char *featureLevelStrings[] = {
        "12.2",
        "12.1",
        "12.0"
    };

    // 高い順に生成できるか試す
    for (size_t i = 0; i < _countof(featureLevels); ++i) {
        // 使用してるアダプタでデバイスを生成
        HRESULT hr_ = D3D12CreateDevice(useAdapter_, featureLevels[i], IID_PPV_ARGS(&device_));
        // 指定した機能レベルでデバイスを生成できたかをチェック
        if (SUCCEEDED(hr_)) {
            // 生成できたらログに出力して終了
            winApp_->Log(std::format("Feature Level : {}\n", featureLevelStrings[i]));
            break;
        }
    }

    // デバイスが生成できなかった場合はエラー
    assert(device_ != nullptr);
}

void DirectXCommon::InitializeCommandQueue() {
    // コマンドキューの生成
    commandQueue_ = nullptr;
    D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
    HRESULT hr_ = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));
    // コマンドキューの生成が成功したかをチェック
    assert(SUCCEEDED(hr_));

}