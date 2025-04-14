#include <Base/WinApp.h>
#include <Base/DirectXCommon.h>
#include <cassert>
#include <format>
#include <dxcapi.h>

#include "PrimitiveDrawer.h"

#pragma comment(lib, "dxcompiler.lib")

namespace MyEngine {

namespace {

// 各エンジン用クラスのグローバル変数
WinApp *sWinApp = nullptr;
DirectXCommon *sDxCommon = nullptr;

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
    sWinApp->Log(std::format(L"Begin CompileShader, path:{}, profile:{}", filePath, profile));

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
        sWinApp->Log(std::format(L"Compile Failed, path:{}, profile:{}", filePath, profile));
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
    sWinApp->Log(std::format(L"Compile Succeeded, path:{}, profile:{}", filePath, profile));
    // もう使わないリソースを解放
    shaderSource->Release();
    shaderResult->Release();

    // 実行用のバイナリを返す
    return shaderBlob;
}

} // namespace

void PrimitiveDrawer::Initialize() {
    // 初期化済みならエラー
    assert(!sWinApp);
    assert(!sDxCommon);

    // インスタンス取得
    sWinApp = WinApp::GetInstance();
    sDxCommon = DirectXCommon::GetInstance();

    // グラフィックパイプラインの初期化
    InitializeGraphicsPipeline();
    // メッシュの初期化
    InitializeMesh();

    // 初期化完了のログを出力
    sWinApp->Log("Complete Initialize PrimitiveDrawer.");
}

void PrimitiveDrawer::Finalize(){
    // メッシュの解放
    mesh_.reset();
    // パイプラインの解放
    pipelineSet_.reset();

    // 終了処理完了のログを出力
    sWinApp->Log("Complete Finalize PrimitiveDrawer.");
}

Microsoft::WRL::ComPtr<ID3D12Resource> PrimitiveDrawer::CreateBufferResources(UINT64 size) {
    // ヒープの設定
    D3D12_HEAP_PROPERTIES uploadHeapProperties{};
    uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; // UploadHeapを使う
    // リソースの設定
    D3D12_RESOURCE_DESC resourceDesc{};
    // バッファリソース。テクスチャの場合はまた別の設定をする
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resourceDesc.Width = size; // リソースのサイズ
    // バッファの場合はこれらは1にする決まり
    resourceDesc.Height = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.SampleDesc.Count = 1;
    // バッファの場合はこれにする決まり
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    // リソースの生成
    Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
    HRESULT hr = sDxCommon->GetDevice()->CreateCommittedResource(
        &uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resource));
    // リソースの生成が成功したかをチェック
    assert(SUCCEEDED(hr));

    // ログに生成したリソースのサイズを出力
    sWinApp->Log(std::format("CreateBufferResources, size:{}", size));
    return resource;
}

std::unique_ptr<PrimitiveDrawer::Mesh> PrimitiveDrawer::CreateMesh(UINT vertexCount) {
    // 頂点バッファの生成
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer =
        CreateBufferResources(sizeof(Vector4) * vertexCount);

    //==================================================
    // 頂点バッファの設定
    //==================================================

    // 頂点バッファビューを作成する
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
    // リソースの先頭アドレスから使う
    vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
    // 使用するリソースのサイズ
    vertexBufferView.SizeInBytes = sizeof(Vector4) * vertexCount;
    // 1頂点あたりのサイズ
    vertexBufferView.StrideInBytes = sizeof(Vector4);

    // 頂点リソースにデータを書き込む
    Vector4 *vertexData = nullptr;
    // 書き込むためのアドレスを取得
    vertexBuffer->Map(0, nullptr, reinterpret_cast<void **>(&vertexData));
    // 左下
    vertexData[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
    // 上
    vertexData[1] = { 0.0f, 0.5f, 0.0f, 1.0f };
    // 右下
    vertexData[2] = { 0.5f, -0.5f, 0.0f, 1.0f };

    // ビューポート
    D3D12_VIEWPORT viewport{};
    // クライアント領域のサイズと一緒にして画面全体に表示
    viewport.Width = static_cast<float>(sWinApp->GetClientWidth());
    viewport.Height = static_cast<float>(sWinApp->GetClientHeight());
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    // シザー矩形
    D3D12_RECT scissorRect{};
    // 基本的にビューポートと同じ矩形が構成されるようにする
    scissorRect.left = 0;
    scissorRect.right = sWinApp->GetClientWidth();
    scissorRect.top = 0;
    scissorRect.bottom = sWinApp->GetClientHeight();

    // メッシュを返す
    auto mesh = std::make_unique<Mesh>();
    mesh->vertexBuffer = vertexBuffer;
    mesh->vertexBufferView = vertexBufferView;
    return mesh;
}

std::unique_ptr<PrimitiveDrawer::PipeLineSet> PrimitiveDrawer::CreateGraphicsPipeline(D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType) {
    //==================================================
    // ルートシグネチャの生成
    //==================================================

    D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
    descriptionRootSignature.Flags =
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    // RootParameter作成。複数設定できるので配列。今回は結果1つだけなので長さ1の配列
    D3D12_ROOT_PARAMETER rootParameters[1]{};
    rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;    // CBVを使う
    rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // シェーダーは頂点シェーダー
    rootParameters[0].Descriptor.ShaderRegister = 0;                    // レジスタ番号0とバインド
    descriptionRootSignature.pParameters = rootParameters;              // ルートパラメータ配列へのポインタ
    descriptionRootSignature.NumParameters = _countof(rootParameters);  // 配列の長さ

    // シリアライズしてバイナリにする
    Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
    HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature,
        D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
    if (FAILED(hr)) {
        sWinApp->Log(reinterpret_cast<char *>(errorBlob->GetBufferPointer()));
        assert(false);
    }

    // バイナリを元に生成
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
    hr = sDxCommon->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
        signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
    assert(SUCCEEDED(hr));

    //==================================================
    // InputLayoutの設定
    //==================================================

    D3D12_INPUT_ELEMENT_DESC inputElementDescs[1] = {};
    inputElementDescs[0].SemanticName = "POSITION";
    inputElementDescs[0].SemanticIndex = 0;
    inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
    inputLayoutDesc.pInputElementDescs = inputElementDescs;
    inputLayoutDesc.NumElements = _countof(inputElementDescs);

    //==================================================
    // BlendStateの設定
    //==================================================

    D3D12_BLEND_DESC blendDesc{};
    // すべての色要素を書き込む
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    //==================================================
    // RaseterizerStateの設定
    //==================================================

    /*今回は
    ・裏面をカリング（ピクセルにしない）
    ・内部を塗りつぶす
    という設定をする*/

    D3D12_RASTERIZER_DESC rasterizerDesc{};
    // 裏面（時計回り）を表示しない
    rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
    // 三角形の中を塗りつぶす
    rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

    //==================================================
    // Shaderをコンパイルする
    //==================================================

    //dxcCompilerを初期化
    Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils = nullptr;
    Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler = nullptr;
    hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
    assert(SUCCEEDED(hr));
    hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
    assert(SUCCEEDED(hr));

    // 現時点でincludeはしないが、includeに対応するための設定を行っておく
    IDxcIncludeHandler *includeHandler = nullptr;
    hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
    assert(SUCCEEDED(hr));

    // シェーダーコンパイル
    Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = CompileShader(
        L"Engine/Shader/Object3d.VS.hlsl", L"vs_6_0",
        dxcUtils.Get(), dxcCompiler.Get(), includeHandler);
    Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = CompileShader(
        L"Engine/Shader/Object3d.PS.hlsl", L"ps_6_0",
        dxcUtils.Get(), dxcCompiler.Get(), includeHandler);

    //==================================================
    // PSOを生成する
    //==================================================

    D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
    graphicsPipelineStateDesc.pRootSignature = rootSignature.Get(); // ルートシグネチャ
    graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;        // InputLayout
    graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
        vertexShaderBlob->GetBufferSize() };                        // VertexShader
    graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
        pixelShaderBlob->GetBufferSize() };                         // PixelShader
    graphicsPipelineStateDesc.BlendState = blendDesc;               // BlendState
    graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;     // RasterizerState
    // 書き込むRTVの情報
    graphicsPipelineStateDesc.NumRenderTargets = 1;
    graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    // 利用するトポロジ（形状）のタイプ
    graphicsPipelineStateDesc.PrimitiveTopologyType = topologyType;
    // どのように画面に色を打ち込むかの設定
    graphicsPipelineStateDesc.SampleDesc.Count = 1;
    graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
    // 生成
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState = nullptr;
    hr = sDxCommon->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
        IID_PPV_ARGS(&pipelineState));
    assert(SUCCEEDED(hr));

    //==================================================
    // 生成したものをセットにして返す
    //==================================================

    auto pipelineSet = std::make_unique<PipeLineSet>();
    pipelineSet->rootSignature = rootSignature;
    pipelineSet->pipelineState = pipelineState;
    return pipelineSet;
}

void PrimitiveDrawer::Reset() {

}

void PrimitiveDrawer::InitializeGraphicsPipeline() {
    // パイプラインの生成。今回は三角形を描画するためのパイプラインを生成する
    pipelineSet_ = CreateGraphicsPipeline(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

    // 初期化完了のログを出力
    sWinApp->Log("Complete Initialize Graphics Pipeline.");
}

void PrimitiveDrawer::InitializeMesh() {
    mesh_ = CreateMesh(3);

    // 初期化完了のログを出力
    sWinApp->Log("Complete Initialize Mesh.");
}

} // namespace MyEngine