#pragma once
#include <windows.h>

namespace application_meta
{
	inline HINSTANCE application_base; // Windows gives us this in our WinMain, this is just the base address of our module (the .exe file)
}