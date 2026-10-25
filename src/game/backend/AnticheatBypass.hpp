#pragma once

namespace YimMenu
{
	class AnticheatBypass
	{
	private:
		static AnticheatBypass& GetInstance()
		{
			static AnticheatBypass instance;
			return instance;
		}

		void RunOnStartupImpl();
		void RunScriptImpl();

		bool m_IsFSLLoaded = false;
		bool m_BattlEyeRunning = false;
		int m_FSLVersion = -1;
		bool m_FSLProvidesLocalSaves = false;
		bool m_FSLProvidesBEBypass = false;
		bool m_IsOutdated = false;

	public:
		// game build this menu's function-restore patches were generated for
		inline static constexpr const char* PATCH_GAME_VERSION = "1158.16";

		static void RunOnStartup()
		{
			GetInstance().RunOnStartupImpl();
		}

		static void RunScript()
		{
			GetInstance().RunScriptImpl();
		}

		static bool IsFSLLoaded()
		{
			return GetInstance().m_IsFSLLoaded;
		}

		// actual battleye, as opposed to the FSL shim
		static bool IsBattlEyeRunning()
		{
			return GetInstance().m_BattlEyeRunning;
		}

		static int GetFSLVersion()
		{
			return GetInstance().m_FSLVersion;
		}

		static bool IsFSLProvidingLocalSaves()
		{
			return GetInstance().m_FSLProvidesLocalSaves;
		}

		static bool IsFSLProvidingBattlEyeBypass()
		{
			return GetInstance().m_FSLProvidesBEBypass;
		}

		// true when the running game build does not match PATCH_GAME_VERSION and
		// the encrypted/destroyed-function restore was skipped to avoid crashes
		static bool IsOutdated()
		{
			return GetInstance().m_IsOutdated;
		}

		static const char* GetSupportedGameVersion()
		{
			return PATCH_GAME_VERSION;
		}
	};
}
