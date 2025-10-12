#include "Logger.h"

#include <chrono>
#include <sstream>

#include "Windows.h"

// ファイルのパス
const std::string Logger::kFilePath = "Log.txt";

// コンストラクタ
Logger::Logger()
    : mFile()
{
    mFile.open( kFilePath, std::ios::out | std::ios::trunc );  // 中身削除
    if( mFile.is_open() )
    {
        OutputDebugStringA( "Log file cleared.\n" );
        mFile.close();
    }

    // 開きなおす
    mFile.open( kFilePath, std::ios::out | std::ios::app );
    if( mFile.is_open() )
    {
        OutputDebugStringA( "Log file opened.\n" );
    }
}

// デストラクタ
Logger::~Logger()
{
    if( mFile.is_open() )
    {
        mFile.close();
        OutputDebugStringA( "Log file closed.\n" );
    }
}

// ログを出力
void Logger::Log( Level level, const std::string& message )
{
    // ログファイルへ出力
    std::ostringstream oss;
    oss << "[" << GetTimestamp() << "] " << LevelToString( level ) << " " << message << "\n";
    if( mFile.is_open() )
    {
        mFile << oss.str();
        mFile.flush();  // 即書き出し
    }
    // 出力ウィンドウへ出力
    OutputDebugStringA( oss.str().c_str() );
}

// タイムスタンプを取得
std::string Logger::GetTimestamp()
{
    using namespace std::chrono;
    auto currTime = system_clock::to_time_t( system_clock::now() );
    tm localTime;
    localtime_s( &localTime, &currTime );

    std::ostringstream oss;
    oss << std::put_time( &localTime, "%Y-%m-%d %H:%M:%S" );
    return oss.str();
}

// 重要度を文字列へ変換
std::string Logger::LevelToString( Level level )
{
    switch( level )
    {
        case Level::kInfo:
            return "[INFO ]";
        case Level::kWarn:
            return "[WARN ]";
        case Level::kError:
            return "[ERROR]";
        default:
            return "[?????]";
    }
}
