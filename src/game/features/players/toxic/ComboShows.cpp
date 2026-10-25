#include "core/backend/ScriptMgr.hpp"
#include "core/commands/ListCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/Self.hpp"
#include "game/commands/PlayerCommand.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/Object.hpp"
#include "types/fx/ExplosionType.hpp"
#include "types/script/ScriptEvent.hpp"

#include <chrono>
#include <cstdlib>
#include <cstring>
#include <string>

namespace YimMenu::Features
{
	static ListCommand _ComboIndex{"comboindex", "整活组合", "选择一键整活的内容", {{0, "庆典派对"}, {1, "末日审判"}, {2, "送葬一条龙"}}, 0};

	// 一键整活宏：把横幅 / 烟花 / 礼物雨 / 音效串成连播
	class ComboShow : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		static void SendEvent(int64_t* args, int arg_count, int bits)
		{
			SCRIPT::_SEND_TU_SCRIPT_EVENT_NEW(1, args, arg_count, bits, static_cast<Hash>(args[0]));
		}

		static void SendBanner(Player player, const std::string& text)
		{
			const int id   = player.GetId();
			const int bits = 1 << id;

			int64_t args[12]{};
			args[0] = static_cast<int64_t>(ScriptEventIndex::GtaBanner);
			args[1] = Self::GetPlayer().GetId();
			args[2] = bits;
			args[3] = 1;
			strncpy(reinterpret_cast<char*>(&args[4]), text.c_str(), 56);

			SendEvent(args, 12, bits);
		}

		static void SendSound(Player player, const std::string& sound)
		{
			const int id   = player.GetId();
			const int bits = 1 << id;

			int64_t args[12]{};
			args[0] = static_cast<int64_t>(ScriptEventIndex::SoundSpam);
			args[1] = Self::GetPlayer().GetId();
			args[2] = bits;
			strncpy(reinterpret_cast<char*>(&args[4]), sound.c_str(), 56);

			SendEvent(args, 12, bits);
		}

		static void Fireworks(rage::fvector3 pos, int count)
		{
			for (int i = 0; i < count; i++)
			{
				const float x = pos.x + (float)(rand() % 9 - 4);
				const float y = pos.y + (float)(rand() % 9 - 4);
				const float z = pos.z + 3.0f + (float)(rand() % 6);
				FIRE::ADD_EXPLOSION(x, y, z, (int)ExplosionType::FIREWORK, 1.0f, true, false, 1.0f, true);
			}
		}

		static void GiftBoxes(rage::fvector3 pos, int count)
		{
			for (int i = 0; i < count; i++)
			{
				const float x = pos.x + (float)(rand() % 12 - 6);
				const float y = pos.y + (float)(rand() % 12 - 6);
				const float z = pos.z + 7.0f + (float)(rand() % 6);
				if (auto gift = Object::Create("hei_prop_heist_box"_J, rage::fvector3{x, y, z}))
					ENTITY::SET_ENTITY_VELOCITY(gift.GetHandle(), (float)(rand() % 3 - 1), (float)(rand() % 3 - 1), -8.0f);
			}
		}

		static void PianoDrop(rage::fvector3 pos)
		{
			if (auto piano = Object::Create("prop_piano_01"_J, rage::fvector3{pos.x, pos.y, pos.z + 15.0f}))
				ENTITY::SET_ENTITY_VELOCITY(piano.GetHandle(), 0.0f, 0.0f, -45.0f);
		}

		virtual void OnCall(Player player) override
		{
			auto target = player.GetPed();
			if (!target)
				return;

			const auto pos = target.GetPosition();

			switch (_ComboIndex.GetState())
			{
			case 0: // 庆典派对
				SendBanner(player, "派对时间到！");
				Fireworks(pos, 4);
				ScriptMgr::Yield(std::chrono::milliseconds(400));
				GiftBoxes(pos, 8);
				ScriptMgr::Yield(std::chrono::milliseconds(400));
				SendSound(player, "CELL_APTINVYACHT");
				ScriptMgr::Yield(std::chrono::milliseconds(400));
				Fireworks(pos, 4);
				break;

			case 1: // 末日审判
				SendBanner(player, "检测到作弊者，正在处理…");
				for (int i = 0; i < 3; i++)
				{
					FIRE::ADD_EXPLOSION(pos.x + (float)(rand() % 13 - 6), pos.y + (float)(rand() % 13 - 6), pos.z + 2.0f,
					    (int)ExplosionType::GRENADE, 0.0f, true, false, 2.0f, true);
					ScriptMgr::Yield(std::chrono::milliseconds(300));
				}
				SendSound(player, "COP_HELI_CAM_ZOOM");
				ScriptMgr::Yield(std::chrono::milliseconds(300));
				FIRE::ADD_EXPLOSION(pos.x, pos.y, pos.z + 5.0f, (int)ExplosionType::FIREWORK, 1.0f, true, false, 1.0f, true);
				break;

			default: // 送葬一条龙
				SendBanner(player, "沉痛悼念");
				PianoDrop(pos);
				ScriptMgr::Yield(std::chrono::milliseconds(600));
				Fireworks(pos, 2);
				ScriptMgr::Yield(std::chrono::milliseconds(600));
				SendSound(player, "Remote_Ring");
				break;
			}

			Notifications::Show("整活宏", "组合已播放完毕。", NotificationType::Success);
		}
	};

	static ComboShow _ComboShow{"comboshow", "一键整活", "按所选组合，把横幅/烟花/礼物雨/音效串成连播", 0, false};
}
