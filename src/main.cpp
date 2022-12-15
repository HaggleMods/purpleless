#include <sdk/SexySDK.hpp>
#include <callbacks/callbacks.hpp>

bool display_ready = false;
bool can_display = false;
int resets = 0;
bool hard_reset = false;

bool get_boolean(const char* bool_text)
{
	if (!std::strcmp(bool_text, "true")) return true;
	else return false;
}


void purple_peg_hit()
{
	if (hard_reset)
	{
		Sexy::ThunderballApp::DoToMenu();
	}
	else
	{
		Sexy::Board::Reset();
		Sexy::SoundMgr::AddSound(Sexy::Assets::get(Sexy::Asset::SOUND_PENALTY));
	}
	++resets;
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

	callbacks::on(callbacks::type::begin_turn_2, []()
	{
		display_ready = true;
		can_display = true;
	});

	callbacks::on(callbacks::type::finish_options_dialog, []()
	{
		if (display_ready) can_display = true;
	});

	callbacks::on(callbacks::type::do_level_done, []()
	{
		if (display_ready) can_display = false;
	});

	callbacks::on(callbacks::type::do_options_dialog, []()
	{
		if (display_ready) can_display = false;
	});

	callbacks::on(callbacks::type::do_to_menu, []()
	{
		if (display_ready)
		{
			display_ready = false;
			can_display = false;
			resets = 0;
		}
	});

	callbacks::on(callbacks::type::main_loop, []()
	{
		if (display_ready && can_display)
		{
			Sexy::FloatingText_* reset_counter = (Sexy::FloatingText_*)Sexy::LogicMgr::AddStandardText(
				Sexy::Format("Resets: %i", resets),
				510.0f,
				30.0f,
				14
			);

			reset_counter->unk_1 = 1;
			reset_counter->float_offset_start = 0.0f;
			reset_counter->color = 0xE33D3D;
		}
	});


	ini_t* ini;

	//This runs before the mod loader switches the directory back, maybe look into a mod init import function eventually for haggle
	if (!std::filesystem::exists("mods/purpleless.ini"))
	{
		//Check again because the file path changes
		if (!std::filesystem::exists("mods/purpleless.ini"))
		{
			std::printf("%s\n", std::filesystem::current_path().string().c_str());
			const char* ini_default = ""
				"[settings]\n"
				"hard-reset = false";

			ini = ini_create(ini_default, strlen(ini_default));
			ini_save(ini, "mods/purpleless.ini");
		}
	}
	else if (std::filesystem::exists("mods/purpleless.ini"))
	{
		ini = ini_load("mods/purpleless.ini");
	}

	hard_reset = get_boolean(ini_get(ini, "settings", "hard-reset"));
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
