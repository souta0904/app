#pragma once
#include <string>

class CommandList;
struct DescriptorHandle;

/// <summary>
/// エディタの基盤
/// </summary>
class EditorBase
{
   private:
    // フォントのパス
    static const std::string kFontPath;
    // フォントのサイズ
    static const float kFontSize;

    // imguiが使用する
    DescriptorHandle* mSRVHdl;

   public:
    /// <summary>
    /// インスタンスを取得
    /// </summary>
    /// <returns>インスタンス</returns>
    static EditorBase& GetInstance()
    {
        static EditorBase instance;
        return instance;
    }

   private:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    EditorBase();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~EditorBase() = default;

   public:
    /// <summary>
    /// コピーコンストラクタ禁止
    /// </summary>
    EditorBase( const EditorBase& ) = delete;

    /// <summary>
    /// 代入演算子禁止
    /// </summary>
    EditorBase& operator=( const EditorBase& ) = delete;

    /// <summary>
    /// ムーブコンストラクタ禁止
    /// </summary>
    EditorBase( EditorBase&& ) = delete;

    /// <summary>
    /// ムーブ代入演算子禁止
    /// </summary>
    EditorBase& operator=( EditorBase&& ) = delete;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <returns>成否</returns>
    bool Init();

    /// <summary>
    /// 終了処理
    /// </summary>
    void Term();

    /// <summary>
    /// エディタ開始
    /// </summary>
    void Begin();

    /// <summary>
    /// エディタ終了
    /// </summary>
    void End();

    /// <summary>
    /// 描画
    /// </summary>
    /// <param name="cmdList">コマンドリスト</param>
    void Draw( CommandList* cmdList );
};
