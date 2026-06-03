#pragma once

namespace Nebula
{

/// <summary>
/// シングルトン基底
/// </summary>
template <typename T>
class Singleton
{
   public:
    static T& GetInstance()
    {
        static T instance;
        return instance;
    }

    // コピー、代入禁止
    Singleton( const Singleton& ) = delete;
    Singleton& operator=( const Singleton& ) = delete;
    Singleton( Singleton&& ) = delete;
    Singleton& operator=( Singleton&& ) = delete;

   protected:
    // コンストラクタ、デストラクタ隠ぺい
    Singleton() = default;
    ~Singleton() = default;
};

}  // namespace Nebula
