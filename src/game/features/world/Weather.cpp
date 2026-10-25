#include "core/commands/Command.hpp"
#include "core/commands/ListCommand.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "game/backend/ScriptPatches.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	static std::vector<std::pair<int, const char*>> g_WeatherTypes = {
	    {0, "晴朗"},
	    {1, "万里无云"},
	    {2, "多云"},
	    {3, "阴天"},
	    {4, "下雨"},
	    {5, "雨过天晴"},
	    {6, "雷暴"},
	    {7, "雾霾"},
	    {8, "浓雾"},
	    {9, "圣诞"},
	    {10, "下雪"},
	    {11, "小雪"},
	    {12, "暴风雪"},
	    {13, "万圣节"},
	    {14, "中性"},
	    {15, "万圣节雨天"},
	    {16, "万圣节雪天"},
	};

	constexpr std::array g_WeatherCodes = {
	    "CLEAR",
	    "EXTRASUNNY",
	    "CLOUDS",
	    "OVERCAST",
	    "RAIN",
	    "CLEARING",
	    "THUNDER",
	    "SMOG",
	    "FOGGY",
	    "XMAS",
	    "SNOW",
	    "SNOWLIGHT",
	    "BLIZZARD",
	    "HALLOWEEN",
	    "NEUTRAL",
	    "RAIN_HALLOWEEN",
	    "SNOW_HALLOWEEN",
	};

	static ListCommand _Weather{"weather", "天气", "要设置或强制应用的天气。该效果仅在本地生效，其他玩家无法看到", g_WeatherTypes, 0};

	class SetWeather : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			MISC::SET_WEATHER_TYPE_PERSIST(g_WeatherCodes[_Weather.GetState()]);
		}
	};
	static SetWeather _SetWeather{"setweather", "设置天气", "设置指定天气。该效果仅在本地生效，其他玩家无法看到"};

	class ForceWeather : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		ScriptPatch m_ShopControllerWeatherPatch{};
		bool m_SnowEnabled{};

		void EnableSnow()
		{
			if (!m_SnowEnabled)
			{
				STREAMING::REQUEST_NAMED_PTFX_ASSET("core_snow");

				// TODO: this native occasionally causes crashes
				if (AUDIO::REQUEST_SCRIPT_AUDIO_BANK("SNOW_FOOTSTEPS", false, -1) && STREAMING::HAS_NAMED_PTFX_ASSET_LOADED("core_snow"))
				{
					GRAPHICS::USE_SNOW_FOOT_VFX_WHEN_UNSHELTERED(true);
					GRAPHICS::USE_SNOW_WHEEL_VFX_WHEN_UNSHELTERED(true);
					GRAPHICS::_FORCE_ALLOW_SNOW_FOOT_VFX_ON_ICE(true);
					GRAPHICS::_FORCE_GROUND_SNOW_PASS(true);

					m_SnowEnabled = true;
				}
			}
		}

		void DisableSnow()
		{
			if (m_SnowEnabled)
			{
				GRAPHICS::USE_SNOW_FOOT_VFX_WHEN_UNSHELTERED(false);
				GRAPHICS::USE_SNOW_WHEEL_VFX_WHEN_UNSHELTERED(false);
				GRAPHICS::_FORCE_ALLOW_SNOW_FOOT_VFX_ON_ICE(false);
				GRAPHICS::_FORCE_GROUND_SNOW_PASS(false);
				STREAMING::REMOVE_NAMED_PTFX_ASSET("core_snow");
				AUDIO::RELEASE_NAMED_SCRIPT_AUDIO_BANK("SNOW_FOOTSTEPS");

				m_SnowEnabled = false;
			}
		}

		virtual void OnEnable() override
		{
			// ensure shop controller can't control the weather
			if (!m_ShopControllerWeatherPatch)
			{
				m_ShopControllerWeatherPatch = ScriptPatches::AddPatch("shop_controller"_J, ScriptPointer("ShopControllerWeatherPatch", "2D 00 07 00 00 71 39 02 71").Add(5), {0x2E, 0x00, 0x00});
			}
			m_ShopControllerWeatherPatch->Enable();
		}

		virtual void OnTick() override
		{
			auto weather = _Weather.GetState();
			MISC::SET_OVERRIDE_WEATHER(g_WeatherCodes[weather]);
			if (weather == 9 || weather == 16)
				EnableSnow();
			else
				DisableSnow();
		}

		virtual void OnDisable() override
		{
			if (m_ShopControllerWeatherPatch)
			{
				m_ShopControllerWeatherPatch->Disable();
			}
			DisableSnow();
			MISC::CLEAR_OVERRIDE_WEATHER();
		}
	};

	static ForceWeather _ForceWeather{"forceweather", "强制天气", "强制应用指定天气。该效果仅在本地生效，其他玩家无法看到"};
}
