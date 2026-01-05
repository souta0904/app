#include "Timer.h"

// コンストラクタ
Timer::Timer( float duration )
    : mIsRunning( false )
    , mDuration( duration )
    , mRemainingTime( duration )
{
}

// 開始
void Timer::Start()
{
    mIsRunning = true;
}

// 停止
void Timer::Stop()
{
    mIsRunning = false;
}

// リセットして停止
void Timer::Reset()
{
    mRemainingTime = mDuration;
    mIsRunning = false;
}

// リセットして開始
void Timer::Restart()
{
    mRemainingTime = mDuration;
    mIsRunning = true;
}

// 更新
void Timer::Update( float deltaTime )
{
    if( !mIsRunning || IsFinished() ) return;

    mRemainingTime = std::max( 0.0f, mRemainingTime - deltaTime );
}
