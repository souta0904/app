#pragma once

/// <summary>
/// ストップウォッチ
/// </summary>
class Stopwatch
{
   private:
    // 計測中か
    bool mIsRunning;
    // 経過時間
    float mElapsedTime;

   public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    Stopwatch();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~Stopwatch() = default;

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

    bool GetIsRunning() const { return mIsRunning; }

    float GetElapsedTime() const { return mElapsedTime; }
};
