#include <Windows.h>

#include "core/Core.h"

// エントリポイント
int WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int )
{
    Nebula::Core core;
    if( !core.Initialize( 1920, 1080, L"Game" ) ) return -1;
    core.Run();
    core.Terminate();

    return 0;
}
