#include <Windows.h>
#include <cassert>
#include <format>
#include <dxcapi.h>

#include "DirectXCommon.h"
#include "WinApp.h"

#pragma comment(lib, "dxcompiler.lib")

namespace {
    // Windowsアプリクラスのグローバル変数
    WinApp *sWinApp = nullptr;

    /// @brief シェーダーコンパイル用関数
    /// @param filePath コンパイルするシェーダーファイルへのパス
    /// @param profile コンパイルに使用するプロファイル
    /// @param dxcUtils 初期化で生成したIDxcUtilsインターフェース
    /// @param dxcCompiler 初期化で生成したIDxcCompiler3インターフェース
    /// @param includeHandler インクルードファイルを扱うためのインターフェース
    /// @return コンパイル結果
    IDxcBlob *CompileShader(const std::wstring &filePath, const wchar_t *profile,
        IDxcUtils *dxcUtils, IDxcCompiler3 *dxcCompiler, IDxcIncludeHandler *includeHandler) {
        // これからシェーダーをコンパイルする旨をログに出力
        sWinApp->Log(std::format(L"Begin CompileShader, path:{}, profile:{}\n", filePath, profile));
        
        //==================================================
        // 1. hlslファイルを読む
        //==================================================
        
        IDxcBlobEncoding *shaderSource = nullptr;
        HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
        // ファイルの読み込みに失敗した場合はエラーを出す
        assert(SUCCEEDED(hr));

        // 読み込んだファイルの内容を設定する
        DxcBuffer shaderSourceBuffer;
        shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
        shaderSourceBuffer.Size = shaderSource->GetBufferSize();
        // UTF8の文字コードであることを通知
        shaderSourceBuffer.Encoding = DXC_CP_UTF8;

        //==================================================
        // 2. コンパイルする
        //==================================================

        LPCWSTR arguments[] = {
            filePath.c_str(),           // コンパイル対象のhlslファイル名
            L"-E", L"main",             // エントリーポイントの指定。基本的にmain以外にはしない
            L"-T", profile,             // ShaderProfileの指定
            L"-Zi", L"-Qembed_debug",   // デバッグ情報を埋め込む
            L"-Od",                     // 最適化を外しておく
            L"-Zpr",                    // メモリレイアウトは行優先
        };
        // 実際にShaderをコンパイルする
        IDxcResult *shaderResult = nullptr;
        hr = dxcCompiler->Compile(
            &shaderSourceBuffer,        // 読み込んだファイル
            arguments,                  // コンパイルオプション
            _countof(arguments),        // コンパイルオプションの数
            includeHandler,             // includeが含まれた諸々
            IID_PPV_ARGS(&shaderResult) // コンパイル結果
        );
        // コンパイルエラーでなくdxcが起動できないなど致命的な状況
        assert(SUCCEEDED(hr));

        //==================================================
        // 3. 警告・エラーがでていないか確認する
        //==================================================

        // 警告・エラーが出てたらログに出して止める
        IDxcBlobUtf8 *shaderError = nullptr;
        shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
        if ((shaderError != nullptr) && (shaderError->GetStringLength() != 0)) {
            // エラーがあった場合はエラーを出力して終了
            sWinApp->Log(std::format(L"CompileShader Error: {}\n", shaderError->GetStringPointer()));
            assert(false);
        }

        //==================================================
        // 4. コンパイル結果を受け取って返す
        //==================================================

        // コンパイル結果から実行用のバイナリ部分を取得
        IDxcBlob *shaderBlob = nullptr;
        hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
        // コンパイル結果の取得に失敗した場合はエラーを出す
        assert(SUCCEEDED(hr));
        // コンパイル完了のログを出力
        sWinApp->Log(std::format(L"Compile Succeeded, path:{}, profile:{}\n", filePath, profile));
        // もう使わないリソースを解放
        shaderSource->Release();
        shaderResult->Release();

        // 実行用のバイナリを返す
        return shaderBlob;
    }
}

void DirectXCommon::Initialize(bool enableDebugLayer) {
    // 初期化済みかどうかのフラグ
    static bool isInitialized = false;
    // 初期化済みならエラーを出す
    assert(!isInitialized);
    assert(!sWinApp);

    // Windowsアプリクラスのインスタンスを取得
    sWinApp = WinApp::GetInstance();

#ifdef _DEBUG
    ID3D12Debug1 *debugController = nullptr;
    if (enableDebugLayer) {
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
            // デバッグレイヤーを有効にする
            debugController->EnableDebugLayer();
            // さらにGPU側でもチェックを行うようにする
            debugController->SetEnableGPUBasedValidation(true);
        }
    }
#else
    // リリースビルドの場合は何もしない
    static_cast<void>(enableDebugLayer);
#endif

    //==================================================
    // DirectXの初期化
    //==================================================

    InitializeDXGI();               // DXGI初期化
    InitializeCommandQueue();       // コマンドキュー初期化
    InitializeCommandAllocator();   // コマンドアロケータ初期化
    InitializeCommandList();        // コマンドリスト初期化
    InitializeSwapChain();          // スワップチェイン初期化
    InitializeRTVDescriptorHeap();  // RTVのディスクリプタヒープ初期化
    InitializeSwapChainResources(); // スワップチェインから取得したリソース初期化
    InitializeRTVHandle();          // RTVのディスクリプタヒープのハンドル初期化
    InitializeFence();              // Fence初期化
    
    // 初期化完了のログを出力
    sWinApp->Log("Complete Initialize DirectX.\n");
}

void DirectXCommon::Finalize() {
    // フェンスのイベントハンドルを閉じる
    CloseHandle(fenceEvent_);

    // 終了処理完了のログを出力
    sWinApp->Log("Complete Finalize DirectX.\n");
}

void DirectXCommon::PreDraw() {
    // レンダーターゲットのクリア
    ClearRenderTarget();
    
    // コマンドリストの内容を確定させる。すべてのコマンドを積んでからCloseすること
    HRESULT hr = commandList_->Close();
    // コマンドリストの内容を確定できたかをチェック
    assert(SUCCEEDED(hr));
}

void DirectXCommon::PostDraw() {
    // GPUにコマンドリストの実行を行わせる
    ID3D12CommandList *commandLists[] = { commandList_.Get()};
    commandQueue_->ExecuteCommandLists(1, commandLists);
    // GPUとOSに画面の交換を行うよう通知
    swapChain_->Present(1, 0);

    //==================================================
    // Fenceの処理
    //==================================================

    // Fenceの値を更新
    fenceValue_++;
    // GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
    commandQueue_->Signal(fence_.Get(), fenceValue_);

    // Fenceの値が指定したSignal値にたどり着いているか確認する
    // GetCompletedValueの初期値はFence作成時に渡した初期値
    if (fence_->GetCompletedValue() < fenceValue_) {
        // 指定したSignalにたどりついていないので、たどり着くまで待つようにイベントを設定する
        fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
        // イベントを待つ
        WaitForSingleObject(fenceEvent_, INFINITE);
    }

    // 次のフレーム用のコマンドリストを準備
    HRESULT hr = commandAllocator_->Reset();
    assert(SUCCEEDED(hr));
    hr = commandList_->Reset(commandAllocator_.Get(), nullptr);
    assert(SUCCEEDED(hr));
}

void DirectXCommon::ClearRenderTarget() {
    // これから書き込むバックバッファのindexを取得
    UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

    //==================================================
    // TransitionBarrier
    //==================================================

    // TransitionBarrierの設定
    D3D12_RESOURCE_BARRIER barrier{};
    // 今回のバリアはTransition
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    // Noneにしておく
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    // バリアを張る対象のリソース。現在のバックバッファに対して行う
    barrier.Transition.pResource = swapChainResources_[backBufferIndex].Get();
    // 遷移前（現在）のResourceState
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    // 遷移後のResourceState
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    // TransitionBarrierを張る
    commandList_->ResourceBarrier(1, &barrier);

    //==================================================
    // 画面クリア処理
    //==================================================

    // 描画先のRTVを設定
    commandList_->OMSetRenderTargets(1, &rtvHandle_[backBufferIndex], false, nullptr);
    // 指定した色で画面全体をクリア
    float clearColor[] = { 0.1f, 0.25f, 0.5f, 1.0f };
    commandList_->ClearRenderTargetView(rtvHandle_[backBufferIndex], clearColor, 0, nullptr);

    // 画面に描く処理はすべて終わり、画面に映すので、状態を遷移
    // 今回はRenderTargetからPresentにする
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    // TransitionBarrierを張る
    commandList_->ResourceBarrier(1, &barrier);
}

void DirectXCommon::InitializeDXGI() {
    // DXGIファクトリーの生成
    dxgiFactory_ = nullptr;
    // HRESULTはWindows系のエラーコードであり、
    // 関数が成功したかどうかをSUCCEEDEDマクロで判定できる
    HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
    // 初期化の根本的な部分でエラーが出た場合はプログラムが間違っているか、
    // どうにもできない場合が多いのでassertにしておく
    assert(SUCCEEDED(hr));

    // アダプタを初期化
    InitializeDXGIAdapter();
    // D3D12デバイスを初期化
    InitializeD3D12Device();

    // 初期化完了のログを出力
    sWinApp->Log("Complete Initialize DXGI.\n");

    // DirectX12のエラーが出た際に止まるようにする
#ifdef _DEBUG
    ID3D12InfoQueue *infoQueue = nullptr;
    if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
        // ヤバいエラーの時に止まる
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        // エラーの時に止まる
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        // 警告の時に止まる
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
        
        // 抑制するメッセージのID
        D3D12_MESSAGE_ID denyIds[] = {
            // Windows11でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
            // https://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
            D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
        };
        // 抑制するレベル
        D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
        D3D12_INFO_QUEUE_FILTER filter{};
        filter.DenyList.NumIDs = _countof(denyIds);
        filter.DenyList.pIDList = denyIds;
        filter.DenyList.NumSeverities = _countof(severities);
        filter.DenyList.pSeverityList = severities;
        // 指定したメッセージの表示を抑制する
        infoQueue->PushStorageFilter(&filter);

        // 解放
        infoQueue->Release();
    }
#endif
}

void DirectXCommon::InitializeDXGIAdapter() {
    // 使用するアダプタ用の変数をnullptrで初期化しておく
    useAdapter_ = nullptr;

    // 良い順にアダプタを頼む
    for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter_)) != DXGI_ERROR_NOT_FOUND; ++i) {
        // アダプタの情報を取得する
        DXGI_ADAPTER_DESC3 adapterDesc{};
        HRESULT hr = useAdapter_->GetDesc3(&adapterDesc);

        // アダプタの情報を取得できたかどうかをチェック
        assert(SUCCEEDED(hr));

        // ソフトウェアアダプタでなければ使用
        if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
            // 使用するアダプタの情報をログに出力。wstringの方なので変換しておく
            sWinApp->Log(std::format(L"Use Adapter : {}\n", adapterDesc.Description));
            break;
        }

        // ソフトウェアアダプタの場合は見なかったことにする
        useAdapter_ = nullptr;
    }

    // アダプタが見つからなかった場合はエラー
    assert(useAdapter_ != nullptr);

    // 初期化完了のログを出力
    sWinApp->Log("Complete Initialize DXGI Adapter.\n");
}

void DirectXCommon::InitializeD3D12Device() {
    // D3D12デバイスの生成
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
        HRESULT hr = D3D12CreateDevice(useAdapter_.Get(), featureLevels[i], IID_PPV_ARGS(&device_));
        // 指定した機能レベルでデバイスを生成できたかをチェック
        if (SUCCEEDED(hr)) {
            // 生成できたらログに出力して終了
            sWinApp->Log(std::format("Feature Level : {}\n", featureLevelStrings[i]));
            break;
        }
    }

    // デバイスが生成できなかった場合はエラー
    assert(device_ != nullptr);

    // 初期化完了のログを出力
    sWinApp->Log("Complete Initialize D3D12 Device.\n");
}

void DirectXCommon::InitializeCommandQueue() {
    // コマンドキューの生成
    commandQueue_ = nullptr;
    D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
    HRESULT hr = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));
    // コマンドキューの生成が成功したかをチェック
    assert(SUCCEEDED(hr));

    // 初期化完了のログを出力
    sWinApp->Log("Complete Initialize Command Queue.\n");
}

void DirectXCommon::InitializeCommandAllocator() {
    // コマンドアロケータの生成
    commandAllocator_ = nullptr;
    HRESULT hr = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
    // コマンドアロケータの生成が成功したかをチェック
    assert(SUCCEEDED(hr));

    // 初期化完了のログを出力
    sWinApp->Log("Complete Initialize Command Allocator.\n");
}

void DirectXCommon::InitializeCommandList() {
    // コマンドリストの生成
    commandList_ = nullptr;
    HRESULT hr = device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr, IID_PPV_ARGS(&commandList_));
    // コマンドリストの生成が成功したかをチェック
    assert(SUCCEEDED(hr));
}

void DirectXCommon::InitializeSwapChain() {
    // スワップチェインの生成
    swapChain_ = nullptr;
    
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
    swapChainDesc.Width = sWinApp->GetClientWidth();                // 画面の幅。ウィンドウのクライアント領域と同じにする
    swapChainDesc.Height = sWinApp->GetClientHeight();              // 画面の高さ。ウィンドウのクライアント領域と同じにする
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;              // 色の形式。8bitのRGBA
    swapChainDesc.SampleDesc.Count = 1;                             // マルチサンプルしない
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;    // レンダーターゲットとして使用
    swapChainDesc.BufferCount = 2;                                  // バッファ数。ダブルバッファ
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;       // モニタに映したら中身を破棄

    // コマンドキュー、ウィンドウハンドル、設定を渡してスワップチェインを生成
    HRESULT hr = dxgiFactory_->CreateSwapChainForHwnd(commandQueue_.Get(), sWinApp->GetWindowHandle(), &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1 **>(swapChain_.GetAddressOf()));
    // スワップチェインの生成が成功したかをチェック
    assert(SUCCEEDED(hr));

    // 初期化完了のログを出力
    sWinApp->Log("Complete Initialize SwapChain.\n");
}

void DirectXCommon::InitializeRTVDescriptorHeap() {
    // レンダーターゲットビューのディスクリプタヒープの生成
    rtvDescriptorHeap_ = nullptr;

    D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc{};
    rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;    // レンダーターゲットビュー用
    rtvDescriptorHeapDesc.NumDescriptors = 2;                       // レンダーターゲットビューの数。ダブルバッファ用に2つ。多くても別に構わない
    HRESULT hr = device_->CreateDescriptorHeap(&rtvDescriptorHeapDesc, IID_PPV_ARGS(&rtvDescriptorHeap_));

    // レンダーターゲットビューのディスクリプタヒープの生成が成功したかをチェック
    assert(SUCCEEDED(hr));

    // 初期化完了のログを出力
    sWinApp->Log("Complete Initialize RTV Descriptor Heap.\n");
}

void DirectXCommon::InitializeSwapChainResources() {
    // スワップチェインからのリソースの生成
    for (UINT i = 0; i < 2; ++i) {
        swapChainResources_[i] = nullptr;
        HRESULT hr = swapChain_->GetBuffer(i, IID_PPV_ARGS(&swapChainResources_[i]));
        // リソースの生成が成功したかをチェック
        assert(SUCCEEDED(hr));
    }

    // 初期化完了のログを出力
    sWinApp->Log("Complete Initialize SwapChain Resources.\n");
}

void DirectXCommon::InitializeRTVHandle() {
    // RTVの設定
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;       // 出力結果をSRGBに変換して書き込む
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;  // 2Dテクスチャとして書き込む

    // ディスクリプタの先頭を取得
    D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();

    // RTVの1つ目を作成。1つ目は最初のところに作る。作る場所をこちらで設定してあげる必要がある
    rtvHandle_[0] = rtvStartHandle;
    device_->CreateRenderTargetView(swapChainResources_[0].Get(), &rtvDesc, rtvHandle_[0]);

    // 2つ目のディスクリプタハンドルを得る(自力で)
    rtvHandle_[1].ptr = rtvHandle_[0].ptr + device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    // RTVの2つ目を作成。
    device_->CreateRenderTargetView(swapChainResources_[1].Get(), &rtvDesc, rtvHandle_[1]);
}

void DirectXCommon::InitializeFence() {
    // フェンスの生成。初期値0で作る
    fence_ = nullptr;
    fenceValue_ = 0;
    HRESULT hr = device_->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
    // フェンスの生成が成功したかをチェック
    assert(SUCCEEDED(hr));

    // フェンスのSignalを持つためのイベントを作成する
    fenceEvent_ = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    // フェンスのイベントハンドルの生成が成功したかをチェック
    assert(fenceEvent_ != nullptr);

    // 初期化完了のログを出力
    sWinApp->Log("Complete Initialize Fence.\n");
}

void DirectXCommon::InitializeDXC() {
    //dxcCompilerを初期化
    IDxcUtils *dxcUtils = nullptr;
    IDxcCompiler3 *dxcCompiler = nullptr;
    HRESULT hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
    assert(SUCCEEDED(hr));
    hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
    assert(SUCCEEDED(hr));

    // 現時点でincludeはしないが、includeに対応するための設定を行っておく
    IDxcIncludeHandler *includeHandler = nullptr;
    hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
    assert(SUCCEEDED(hr));

    // シェーダーコンパイル

    // 初期化完了のログを出力
    sWinApp->Log("Complete Initialize DXC.\n");
}

DirectXCommon::D3DResourceLeakChecker::~D3DResourceLeakChecker() {
    // リソースリークチェック
    Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
    if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
        debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
        debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
        debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
    }
}