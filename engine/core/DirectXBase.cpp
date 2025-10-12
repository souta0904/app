#include "DirectXBase.h"

#include <format>

#include "DirectXCommonSettings.h"
#include "Window.h"
#include "utils/Logger.h"
#include "utils/StringHelper.h"

#pragma comment( lib, "d3d12.lib" )
#pragma comment( lib, "dxgi.lib" )
#pragma comment( lib, "dxguid.lib" )

// コンストラクタ
DirectXBase::DirectXBase()
    : mFactory( nullptr )
    , mDevice( nullptr )
    , mCmdQueue( nullptr )
    , mSwapChain( nullptr )
    , mCmdList( nullptr )
    , mRTVHeap()
    , mDSVHeap()
    , mSRVHeap()
    , mBackBuffs()
    , mRTVHdls()
    , mDepthBuff( nullptr )
    , mDSVHdl( nullptr )
    , mBackBuffIdx( 0 )
    , mClearColor( 0.0f, 0.0f, 0.5f )
    , mAdapterName()
{
}

// 初期化
bool DirectXBase::Init()
{
    LOG_INFO( std::format( "Back Buffer Count: {}", kBackBuffCount ) );

    if( !DirectXCommonSettings::Init() ) return false;

    if( !CreateDevice() ) return false;

    if( !CreateCmdQueue() ) return false;

    if( !CreateSwapChain() ) return false;

    if( !CreateCmdList() ) return false;

    if( !CreateDescriptorHeaps() ) return false;

    if( !CreateRTV() ) return false;

    if( !CreateDSV() ) return false;

    return true;
}

// 終了処理
void DirectXBase::Term()
{
    mCmdQueue->WaitGPUTerm( mBackBuffIdx );

    // デスクリプタハンドルを解放
    if( mRTVHeap )
    {
        for( auto& hdl : mRTVHdls )
        {
            if( !hdl ) continue;

            mRTVHeap->Free( hdl );
        }
    }
    if( mDSVHeap && mDSVHdl )
    {
        mDSVHeap->Free( mDSVHdl );
    }

    DirectXCommonSettings::Term();
}

// 描画開始
void DirectXBase::BeginDraw()
{
    // コマンドをリセット
    mCmdList->Reset( mBackBuffIdx );

    // 表示からレンダーターゲットへ
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Transition.pResource = mBackBuffs[mBackBuffIdx].Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    mCmdList->ResourceBarrier( barrier );

    // レンダーターゲットをセット
    mCmdList->SetRenderTarget( mRTVHdls[mBackBuffIdx], mDSVHdl );

    // レンダーターゲット・深度バッファをクリア
    mCmdList->ClearRenderTargetView( mRTVHdls[mBackBuffIdx], &mClearColor.r );
    mCmdList->ClearDepthStencilView( mDSVHdl );

    // SRVデスクリプタヒープをセット
    mCmdList->SetDescriptorHeap( mSRVHeap.get() );
}

// 描画終了
void DirectXBase::EndDraw()
{
    // レンダーターゲットから表示へ
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Transition.pResource = mBackBuffs[mBackBuffIdx].Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    mCmdList->ResourceBarrier( barrier );

    // コマンドを実行して待つ
    mCmdList->Close();
    mCmdQueue->Execute( mCmdList.get(), mBackBuffIdx );
    mSwapChain->Present( 1, 0 );
    mBackBuffIdx = mSwapChain->GetCurrentBackBufferIndex();
    mCmdQueue->WaitGPU( mBackBuffIdx );
}

#pragma region 作成処理

// デバイスを作成
bool DirectXBase::CreateDevice()
{
#ifdef _DEBUG
    // デバッグレイヤーを有効にする
    Microsoft::WRL::ComPtr<ID3D12Debug1> debugController = nullptr;
    if( SUCCEEDED( D3D12GetDebugInterface( IID_PPV_ARGS( debugController.GetAddressOf() ) ) ) )
    {
        debugController->EnableDebugLayer();
        debugController->SetEnableGPUBasedValidation( true );
        LOG_INFO( "Enable debug layer." );
    }
#endif

    // DXGIファクトリーを作成
    auto hr = CreateDXGIFactory( IID_PPV_ARGS( mFactory.GetAddressOf() ) );
    if( FAILED( hr ) )
    {
        LOG_ERROR( "Failed to create DXGIFactory." );
        return false;
    }
    else
    {
        LOG_INFO( "DXGIFactory created successfully." );
    }

    // パフォーマンスが高いアダプタから取得
    Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter = nullptr;
    for( uint32_t i = 0;
         mFactory->EnumAdapterByGpuPreference( i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS( &useAdapter ) ) != DXGI_ERROR_NOT_FOUND;
         ++i )
    {
        DXGI_ADAPTER_DESC3 desc = {};
        hr = useAdapter->GetDesc3( &desc );
        if( SUCCEEDED( hr ) )
        {
            // ソフトウェアアダプタは使わない
            if( !( desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE ) )
            {
                mAdapterName = desc.Description;
                LOG_INFO( StringHelper::Convert( std::format( L"Use Adapter: {}", mAdapterName ) ) );
                break;
            }
        }
    }
    if( !useAdapter ) return false;

    // 機能レベル
    D3D_FEATURE_LEVEL featureLevels[] =
        {
            D3D_FEATURE_LEVEL_12_2,
            D3D_FEATURE_LEVEL_12_1,
            D3D_FEATURE_LEVEL_12_0,
        };
    const char* str[] = { "12.2", "12.1", "12.0" };
    for( uint32_t i = 0; i < _countof( featureLevels ); ++i )
    {
        // デバイスを作成
        hr = D3D12CreateDevice( useAdapter.Get(), featureLevels[i], IID_PPV_ARGS( mDevice.GetAddressOf() ) );
        if( SUCCEEDED( hr ) )
        {
            LOG_INFO( std::format( "Feature Level: {}", str[i] ) );
            break;
        }
    }
    if( !mDevice )
    {
        LOG_ERROR( "Failed to create D3D12Device." );
        return false;
    }
    else
    {
        LOG_INFO( "D3D12Device created successfully." );
    }

#ifdef _DEBUG
    Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue = nullptr;
    if( SUCCEEDED( mDevice->QueryInterface( IID_PPV_ARGS( infoQueue.GetAddressOf() ) ) ) )
    {
        // 警告・エラーは停止する
        infoQueue->SetBreakOnSeverity( D3D12_MESSAGE_SEVERITY_CORRUPTION, true );
        infoQueue->SetBreakOnSeverity( D3D12_MESSAGE_SEVERITY_ERROR, true );
        infoQueue->SetBreakOnSeverity( D3D12_MESSAGE_SEVERITY_WARNING, true );

        // どうしようもないエラーは無視する
        D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
        D3D12_MESSAGE_ID ids[] = { D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE };
        D3D12_INFO_QUEUE_FILTER filter = {};
        filter.DenyList.NumSeverities = _countof( severities );
        filter.DenyList.pSeverityList = severities;
        filter.DenyList.NumIDs = _countof( ids );
        filter.DenyList.pIDList = ids;
        infoQueue->PushStorageFilter( &filter );
    }
#endif

    return true;
}

// コマンドキューを作成
bool DirectXBase::CreateCmdQueue()
{
    mCmdQueue = std::make_unique<CommandQueue>();
    if( !mCmdQueue->Create( kBackBuffCount ) )
    {
        LOG_ERROR( "Failed to create CommandQueue." );
        return false;
    }
    else
    {
        LOG_INFO( "CommandQueue created successfully." );
    }

    return true;
}

// スワップチェインを作成
bool DirectXBase::CreateSwapChain()
{
    if( !mFactory || !mCmdQueue ) return false;

    auto& window = Window::GetInstance();

    DXGI_SWAP_CHAIN_DESC1 desc = {};
    desc.Width = window.GetWidth();
    desc.Height = window.GetHeight();
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = kBackBuffCount;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    [[maybe_unused]] auto hr = mFactory->CreateSwapChainForHwnd( mCmdQueue->GetCmdQueue().Get(), window.GetHWnd(), &desc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>( mSwapChain.GetAddressOf() ) );
    if( FAILED( hr ) )
    {
        LOG_ERROR( "Failed to create DXGISwapChain." );
        return false;
    }
    else
    {
        LOG_INFO( "DXGISwapChain created successfully." );
    }

    return true;
}

// コマンドリストを作成
bool DirectXBase::CreateCmdList()
{
    mCmdList = std::make_unique<CommandList>();
    if( !mCmdList->Create( kBackBuffCount ) )
    {
        LOG_ERROR( "Failed to create CommandList." );
        return false;
    }
    else
    {
        LOG_INFO( "CommandList created successfully." );
    }

    return true;
}

// デスクリプタヒープを作成
bool DirectXBase::CreateDescriptorHeaps()
{
    mRTVHeap = std::make_unique<DescriptorHeap>();
    if( !mRTVHeap->Create( DescriptorHeap::Type::kRTV, 128, false ) ) return false;

    mDSVHeap = std::make_unique<DescriptorHeap>();
    if( !mDSVHeap->Create( DescriptorHeap::Type::kDSV, 64, false ) ) return false;

    mSRVHeap = std::make_unique<DescriptorHeap>();
    if( !mSRVHeap->Create( DescriptorHeap::Type::kCBV_SRV_UAV, 4096, true ) ) return false;

    return true;
}

// レンダーターゲットビューを作成
bool DirectXBase::CreateRTV()
{
    if( !mDevice || !mSwapChain || !mRTVHeap ) return false;

    for( uint32_t i = 0; i < kBackBuffCount; ++i )
    {
        // スワップチェインからバックバッファを取得
        [[maybe_unused]] auto hr = mSwapChain->GetBuffer( i, IID_PPV_ARGS( mBackBuffs[i].GetAddressOf() ) );
        if( FAILED( hr ) ) return false;

        mRTVHdls[i] = mRTVHeap->Alloc();
        if( !mRTVHdls[i] ) return false;

        // レンダーターゲットビューを作成
        D3D12_RENDER_TARGET_VIEW_DESC viewDesc = {};
        viewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
        mDevice->CreateRenderTargetView( mBackBuffs[i].Get(), &viewDesc, mRTVHdls[i]->mCPU );
    }

    return true;
}

// 深度ステンシルビューを作成
bool DirectXBase::CreateDSV()
{
    if( !mDevice || !mDSVHeap ) return false;

    auto& window = Window::GetInstance();

    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    desc.Width = window.GetWidth();
    desc.Height = window.GetHeight();
    desc.DepthOrArraySize = 1;
    desc.Format = DXGI_FORMAT_D32_FLOAT;
    desc.SampleDesc.Count = 1;
    desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE clearValue = {};
    clearValue.Format = DXGI_FORMAT_D32_FLOAT;
    clearValue.DepthStencil.Depth = 1.0f;

    // 深度バッファを作成
    [[maybe_unused]] auto hr = mDevice->CreateCommittedResource( &DirectXCommonSettings::gHeapPropertiesDefault, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearValue, IID_PPV_ARGS( mDepthBuff.GetAddressOf() ) );
    if( FAILED( hr ) ) return false;

    mDSVHdl = mDSVHeap->Alloc();
    if( !mDSVHdl ) return false;

    // 深度ステンシルビューを作成
    D3D12_DEPTH_STENCIL_VIEW_DESC viewDesc = {};
    viewDesc.Format = DXGI_FORMAT_D32_FLOAT;
    viewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    mDevice->CreateDepthStencilView( mDepthBuff.Get(), &viewDesc, mDSVHdl->mCPU );

    return true;
}

#pragma endregion
