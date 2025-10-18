#include "Texture.h"

#include <format>

#include "DirectXTex/d3dx12.h"
#include "core/CommandList.h"
#include "core/DirectXBase.h"
#include "core/DirectXCommonSettings.h"
#include "utils/Logger.h"
#include "utils/StringHelper.h"

// コンストラクタ
Texture::Texture()
    : mState( State::Unload )
    , mTask()
    , mPath()
    , mWidth( 0 )
    , mHeight( 0 )
    , mResource( nullptr )
    , mSRVHdl( nullptr )
{
}

// デストラクタ
Texture::~Texture()
{
    auto* srvHeap = DirectXBase::GetInstance().GetSRVHeap();
    if( !srvHeap ) return;

    srvHeap->Free( mSRVHdl );
}

// テクスチャを作成
void Texture::CreateAsync( const std::string& path )
{
    mState = State::Load;
    mPath = path;

    mTask = std::async(
        std::launch::async,
        [this]()
        {
            try
            {
                DirectX::ScratchImage image = {};
                DirectX::ScratchImage mipChain = {};
                if( !Load( mPath, image, mipChain ) ||
                    !CreateResource( mipChain ) ||
                    !Upload( mipChain ) ||
                    !CreateSRV() )
                {
                    throw std::runtime_error( std::format( "Failed to create texture: {}", mPath ) );
                }
                mState = State::Ready;
            }
            catch( const std::exception& e )
            {
                mState = State::Error;
                LOG_ERROR( e.what() );
            }
        } );
}

// テクスチャをバインド
void Texture::Bind( CommandList* cmdList, uint32_t rootParamIdx )
{
    if( !cmdList ) return;

    cmdList->SetGraphicsRootDescriptorTable( rootParamIdx, mSRVHdl );
}

// テクスチャを読み込む
bool Texture::Load( const std::string& path, DirectX::ScratchImage& image, DirectX::ScratchImage& mipChain )
{
    mPath = path;
    auto wpath = StringHelper::Convert( mPath );
    auto hr = DirectX::LoadFromWICFile( wpath.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image );
    if( FAILED( hr ) ) return false;

    // ミップマップを生成
    hr = GenerateMipMaps( image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipChain );
    if( FAILED( hr ) ) return false;

    return true;
}

// リソースを作成
bool Texture::CreateResource( const DirectX::ScratchImage& mipChain )
{
    auto& metadata = mipChain.GetMetadata();
    mWidth = static_cast<uint32_t>( metadata.width );
    mHeight = static_cast<uint32_t>( metadata.height );

    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>( metadata.dimension );
    desc.Width = mWidth;
    desc.Height = mHeight;
    desc.DepthOrArraySize = static_cast<UINT16>( metadata.arraySize );
    desc.MipLevels = static_cast<UINT16>( metadata.mipLevels );
    desc.Format = metadata.format;
    desc.SampleDesc.Count = 1;

    [[maybe_unused]] auto hr = DirectXBase::GetInstance().GetDevice()->CreateCommittedResource( &DirectXCommonSettings::gHeapDefault, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS( mResource.GetAddressOf() ) );
    if( FAILED( hr ) ) return false;

    return true;
}

// データをアップロード
bool Texture::Upload( const DirectX::ScratchImage& mipChain )
{
    auto& dxBase = DirectXBase::GetInstance();
    auto device = dxBase.GetDevice();

    std::vector<D3D12_SUBRESOURCE_DATA> subresources;
    DirectX::PrepareUpload( device.Get(), mipChain.GetImages(), mipChain.GetImageCount(), mipChain.GetMetadata(), subresources );
    D3D12_RESOURCE_DESC intermediateDesc = {};
    intermediateDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    intermediateDesc.Width = GetRequiredIntermediateSize( mResource.Get(), 0, static_cast<UINT>( subresources.size() ) );
    intermediateDesc.Height = 1;
    intermediateDesc.DepthOrArraySize = 1;
    intermediateDesc.MipLevels = 1;
    intermediateDesc.SampleDesc.Count = 1;
    intermediateDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    // 中間リソースを作成
    Microsoft::WRL::ComPtr<ID3D12Resource> intermediateRes = nullptr;
    [[maybe_unused]] auto hr = device->CreateCommittedResource( &DirectXCommonSettings::gHeapUpload, D3D12_HEAP_FLAG_NONE, &intermediateDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS( intermediateRes.GetAddressOf() ) );
    if( FAILED( hr ) ) return false;

    CommandList cmdList;
    cmdList.Create();
    cmdList.Reset( 0 );

    UpdateSubresources( cmdList.GetCmdList().Get(), mResource.Get(), intermediateRes.Get(), 0, 0, static_cast<UINT>( subresources.size() ), subresources.data() );
    // リソースバリア
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Transition.pResource = mResource.Get();
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
    cmdList.ResourceBarrier( barrier );

    // コマンドリストを実行
    cmdList.Close();
    auto cmdQueue = dxBase.GetUploadQueue();
    cmdQueue->Execute( &cmdList, 0 );
    cmdQueue->WaitGPU( 0 );

    return true;
}

// シェーダーリソースビューを作成
bool Texture::CreateSRV()
{
    auto& dxBase = DirectXBase::GetInstance();

    auto* srvHeap = dxBase.GetSRVHeap();
    if( !srvHeap ) return false;

    mSRVHdl = srvHeap->Alloc();
    if( !mSRVHdl ) return false;

    auto resourceDesc = mResource->GetDesc();

    // シェーダーリソースビューを作成
    D3D12_SHADER_RESOURCE_VIEW_DESC viewDesc = {};
    viewDesc.Format = DirectX::MakeSRGB( resourceDesc.Format );
    viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    viewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    viewDesc.Texture2D.MipLevels = static_cast<UINT>( resourceDesc.MipLevels );
    dxBase.GetDevice()->CreateShaderResourceView( mResource.Get(), &viewDesc, mSRVHdl->mCPU );

    return true;
}
