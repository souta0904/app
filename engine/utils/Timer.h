#pragma once
#include <algorithm>

/// <summary>
/// タイマー(カウントダウン)
/// </summary>
class Timer
{
   private:
    // 計測中か
    bool mIsRunning;
    // 時間
    float mDuration;
    // 残り時間
    float mRemainingTime;

   public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="duration">時間</param>
    Timer( float duration );

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~Timer() = default;

    /// <summary>
    /// 開始
    /// </summary>
    void Start();

    /// <summary>
    /// 停止
    /// </summary>
    void Stop();

    /// <summary>
    /// リセットして停止
    /// </summary>
    void Reset();

    /// <summary>
    /// リセットして開始
    /// </summary>
    void Restart();

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="deltaTime">デルタタイム</param>
    void Update( float deltaTime );

    /// <summary>残り時間が0か</summary>
    bool IsFinished() const { return mRemainingTime <= 0.0f; }

    /// <summary>
    /// 残り時間割合を取得
    /// </summary>
    /// <returns></returns>
    float GetRate() const
    {
        if( mDuration <= 0.0f ) return 0.0f;

        return std::clamp( mRemainingTime / mDuration, 0.0f, 1.0f );
    }

    bool GetIsRunning() const { return mIsRunning; }

    float GetRemainingTime() const { return mRemainingTime; }
};
