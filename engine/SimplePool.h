#pragma once
#include <memory>
#include <vector>

/// <summary>
/// シンプルなプールクラス
/// </summary>
template <typename T>
class SimplePool
{
   private:
    /// <summary>
    /// アイテム
    /// </summary>
    struct Item
    {
        // 値
        T mValue = {};
        // アクティブか
        bool mIsActive = false;
        // インデックス
        uint32_t mIdx = 0;
    };

    // アイテムリスト
    std::vector<std::unique_ptr<Item>> mItems;
    // 次に割り当てるインデックス
    uint32_t mNextIdx;
    // アクティブなアイテムの数
    uint32_t mActiveCount;

   public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    SimplePool()
        : mItems()
        , mNextIdx( 0 )
        , mActiveCount( 0 )
    {
    }

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~SimplePool() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="size">アイテムの数</param>
    void Init( uint32_t size )
    {
        mItems.clear();
        mItems.reserve( size );
        for( uint32_t i = 0; i < size; ++i )
        {
            auto item = std::make_unique<Item>();
            item->mIdx = i;
            mItems.emplace_back( std::move( item ) );
        }
        mNextIdx = 0;
        mActiveCount = 0;
    }

    /// <summary>
    /// アイテムを借りる
    /// </summary>
    /// <returns>アイテム</returns>
    Item* Lend()
    {
        auto size = GetSize();
        if( mActiveCount >= size ) return nullptr;  // 空きがない

        for( uint32_t i = 0; i < size; ++i )
        {
            uint32_t idx = ( mNextIdx + i ) % size;
            if( !mItems[idx]->mIsActive )
            {
                mItems[idx]->mIsActive = true;
                mNextIdx = ( idx + 1 ) % size;
                ++mActiveCount;
                return mItems[idx].get();
            }
        }
        return nullptr;  // 空きがない
    }

    /// <summary>
    /// アイテムを返却
    /// </summary>
    /// <param name="idx">インデックス</param>
    void Return( uint32_t idx )
    {
        if( idx >= GetSize() ) return;

        if( mItems[idx]->mIsActive )
        {
            mItems[idx]->mIsActive = false;
            --mActiveCount;
        }
    }

    /// <summary>
    /// アイテムを返却
    /// </summary>
    /// <param name="item">アイテム</param>
    void Return( Item* item )
    {
        if( !item ) return;

        Return( item->mIdx );
    }

    /// <summary>
    /// 全アイテムを返却
    /// </summary>
    void ReturnAll()
    {
        for( auto& item : mItems )
        {
            item->mIsActive = false;
        }
        mNextIdx = 0;
        mActiveCount = 0;
    }

    /// <summary>アイテムリストのサイズを取得</summary>
    uint32_t GetSize() const { return static_cast<uint32_t>( mItems.size() ); }

    /// <summary>アクティブなアイテムの数を取得</summary>
    uint32_t GetActiveCount() const { return mActiveCount; }

    /// <summary>アクティブでないアイテムの数を取得</summary>
    uint32_t GetFreeCount() const { return GetSize() - mActiveCount; }
};
