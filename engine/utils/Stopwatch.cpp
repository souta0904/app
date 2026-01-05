#include "Stopwatch.h"

// コンストラクタ
Stopwatch::Stopwatch()
    : mIsRunning( false )
    , mElapsedTime( 0.0f )
{
}

// 開始
void Stopwatch::Start()
{
    mIsRunning = true;
}

// 停止
void Stopwatch::Stop()
{
    mIsRunning = false;
}

// リセットして停止
void Stopwatch::Reset()
{
    mElapsedTime = 0.0f;
    mIsRunning = false;
}

// リセットして開始
void Stopwatch::Restart()
{
    mElapsedTime = 0.0f;
    mIsRunning = true;
}

// 更新
void Stopwatch::Update( float deltaTime )
{
    if( !mIsRunning ) return;

    mElapsedTime += deltaTime;
}
