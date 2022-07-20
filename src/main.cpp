#include <sdk/SexySDK.hpp>
#include <callbacks/callbacks.hpp>

void purple_peg_hit()
{
	Sexy::Board::Reset();
	Sexy::SoundMgr::AddSound(Sexy::Assets::get(Sexy::Asset::SOUND_PENALTY));
}

void __declspec(naked) purple_peg_hit_hook()
{
	__asm
	{
		cmp[ecx + 10h], ebx;
		jnz loc_46FE4D;
		call purple_peg_hit;

	loc_46FE4D:
		push 0x0046FE4D;
		retn;
	}
}

void init()
{
	jump(0x0046FD82, purple_peg_hit_hook);
}

DWORD WINAPI OnAttachImpl(LPVOID lpParameter)
{
	init();
	return 0;
}

DWORD WINAPI OnAttach(LPVOID lpParameter)
{
	__try
	{
		return OnAttachImpl(lpParameter);
	}
	__except (0)
	{
		FreeLibraryAndExitThread((HMODULE)lpParameter, 0xDECEA5ED);
	}

	return 0;
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
		CreateThread(nullptr, 0, OnAttach, hModule, 0, nullptr);
		return true;
	}

	return false;
}
