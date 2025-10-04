#pragma once
#include <fstream>

#define LOG_INFO( message ) Logger::GetInstance().Log( Logger::Level::kInfo, message )
#define LOG_WARN( message ) Logger::GetInstance().Log( Logger::Level::kWarn, message )
#define LOG_ERROR( message ) Logger::GetInstance().Log( Logger::Level::kError, message )

/// <summary>
/// ロガー
/// </summary>
class Logger
{
   public:
    /// <summary>
    /// ログの重要度
    /// </summary>
    enum class Level
    {
        kInfo,
        kWarn,
        kError,
    };

   private:
    // ファイルのパス
    static const std::string kFilePath;
    // 出力ファイル
    std::ofstream mFile;

   public:
    /// <summary>
    /// インスタンスを取得
    /// </summary>
    /// <returns>インスタンス</returns>
    static Logger& GetInstance()
    {
        static Logger instance;
        return instance;
    }

   private:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    Logger();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~Logger();

   public:
    /// <summary>
    /// コピーコンストラクタ禁止
    /// </summary>
    Logger( const Logger& ) = delete;

    /// <summary>
    /// 代入演算子禁止
    /// </summary>
    Logger& operator=( const Logger& ) = delete;

    /// <summary>
    /// ムーブコンストラクタ禁止
    /// </summary>
    Logger( Logger&& ) = delete;

    /// <summary>
    /// ムーブ代入演算子禁止
    /// </summary>
    Logger& operator=( Logger&& ) = delete;

    /// <summary>
    /// ログを出力
    /// </summary>
    /// <param name="level">重要度</param>
    /// <param name="message">メッセージ</param>
    void Log( Level level, const std::string& message );

    private:
    /// <summary>
    /// タイムスタンプを取得
    /// </summary>
    /// <returns>タイムスタンプ</returns>
    std::string GetTimestamp();

    /// <summary>
    /// 重要度を文字列へ変換
    /// </summary>
    /// <param name="level">重要度</param>
    /// <returns>文字列</returns>
    std::string LevelToString( Level level );
};
