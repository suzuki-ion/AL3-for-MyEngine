#include <cassert>
#include <format>
#include "WinApp.h"
#include "ConvertString.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

void WinApp::Initialize() {
    InitializeWindow();     // ウィンドウ初期化
    InitializeDXGI();       // DXGI初期化

    // 初期化完了のログを出力
    Log("Complete Initialize WinApp.\n");
}

void WinApp::InitializeWindow() {
    // ウィンドウプロシージャ
    wc_.lpfnWndProc = WindowProc;
    // ウィンドウクラス名
    wc_.lpszClassName = L"WindowClass";
    // インスタンスハンドル
    wc_.hInstance = GetModuleHandle(nullptr);
    // カーソル
    wc_.hCursor = LoadCursor(nullptr, IDC_ARROW);
    
    // ウィンドウクラスを登録
    RegisterClass(&wc_);

    // クライアントサイズの横幅
    kClientWidth_ = 1280;
    // クライアントサイズの縦幅
    kClientHeight_ = 720;
    
    // ウィンドウサイズを表す構造体にクライアント領域を入れる
    wrc_ = { 0, 0, kClientWidth_, kClientHeight_ };
    
    // クライアント領域を元に実際のサイズにwrcを変更してもらう
    AdjustWindowRect(&wrc_, WS_OVERLAPPEDWINDOW, false);

    // ウィンドウハンドルを作成
    hwnd_ = CreateWindow(
        wc_.lpszClassName,      // 利用するクラス名
        L"Window",              // タイトルバーの文字
        WS_OVERLAPPEDWINDOW,    // よく見るウィンドウスタイル
        CW_USEDEFAULT,          // 表示X座標
        CW_USEDEFAULT,          // 表示Y座標
        wrc_.right - wrc_.left, // ウィンドウ横幅
        wrc_.bottom - wrc_.top, // ウィンドウ縦幅
        nullptr,                // 親ウィンドウハンドル
        nullptr,                // メニューハンドル
        wc_.hInstance,          // インスタンスハンドル
        nullptr                 // オプション
    );

    // ウィンドウを表示
    ShowWindow(hwnd_, SW_SHOW);

    // 初期化完了のログを出力
    Log("Complete Initialize Window.\n");
}

void WinApp::InitializeDXGI() {
    // DXGIファクトリーの生成
    dxgiFactory_ = nullptr;
    // HRESULTはWindows系のエラーコードであり、
    // 関数が成功したかどうかをSUCCEEDEDマクロで判定できる
    hr_ = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
    // 初期化の根本的な部分でエラーが出た場合はプログラムが間違っているか、
    // どうにもできない場合が多いのでassertにしておく
    assert(SUCCEEDED(hr_));

    // アダプタを初期化
    InitializeDXGIAdapter();
    // D3D12デバイスを初期化
    InitializeD3D12Device();

    // 初期化完了のログを出力
    Log("Complete Initialize D3D12Device.\n");
}

void WinApp::InitializeDXGIAdapter() {
    // 使用するアダプタ用の変数をnullptrで初期化しておく
    useAdapter_ = nullptr;

    // 良い順にアダプタを頼む
    for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter_)) != DXGI_ERROR_NOT_FOUND; ++i) {
        // アダプタの情報を取得する
        DXGI_ADAPTER_DESC3 adapterDesc{};
        hr_ = useAdapter_->GetDesc3(&adapterDesc);

        // アダプタの情報を取得できたかどうかをチェック
        assert(SUCCEEDED(hr_));

        // ソフトウェアアダプタでなければ使用
        if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
            // 使用するアダプタの情報をログに出力。wstringの方なので変換しておく
            Log(std::format(L"Use Adapter : {}\n", adapterDesc.Description));
            break;
        }

        // ソフトウェアアダプタの場合は見なかったことにする
        useAdapter_ = nullptr;
    }

    // アダプタが見つからなかった場合はエラー
    assert(useAdapter_ != nullptr);
}

void WinApp::InitializeD3D12Device() {
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
        hr_ = D3D12CreateDevice(useAdapter_, featureLevels[i], IID_PPV_ARGS(&device_));
        // 指定した機能レベルでデバイスを生成できたかをチェック
        if (SUCCEEDED(hr_)) {
            // 生成できたらログに出力して終了
            Log(std::format("Feature Level : {}\n", featureLevelStrings[i]));
            break;
        }
    }

    // デバイスが生成できなかった場合はエラー
    assert(device_ != nullptr);
}

int WinApp::ProccessMessage() {
    if (msg_.message == WM_QUIT) {
        Log("Window Quit.\n");
        return -1;
    }

    if (PeekMessage(&msg_, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg_);
        DispatchMessage(&msg_);
        Log(std::format("Proccess Message : {}\n", msg_.message));

    } else {
        return 0;
    }
}

void WinApp::Log(const std::string &message) {
    OutputDebugStringA(message.c_str());
}

void WinApp::Log(const std::wstring &message) {
    // OutputDebugStringWでもいいらしいけど、よくわからんので変換で対応
    OutputDebugStringA(ConvertString(message).c_str());
}

LRESULT CALLBACK WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    // メッセージに応じてゲーム固有の処理を行う
    switch (msg) {
        // ウィンドウが破棄された時
        case WM_DESTROY:
            // OSに対して。アプリの終了を伝える
            PostQuitMessage(0);
            return 0;
    }

    // 標準のメッセージ処理を行う
    return DefWindowProc(hwnd, msg, wparam, lparam);
}