#include "core/backend/FiberPool.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/Players.hpp"
#include "game/commands/PlayerCommand.hpp"
#include "game/gta/Entity.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/Object.hpp"

#include <cstdlib>
#include <vector>

namespace YimMenu::Features
{
	static std::vector<int> g_FakeBags{};

	static void DropBags(Player player, int bagCount)
	{
		auto ped = player.GetPed();
		if (!ped)
			return;

		// 先清理已消失的旧袋子，避免列表无界增长
		std::erase_if(g_FakeBags, [](int h) { return h == 0 || !ENTITY::DOES_ENTITY_EXIST(h); });

		const auto pos = ped.GetPosition();
		for (int i = 0; i < bagCount; i++)
		{
			const float x = pos.x + (float)(rand() % 20 - 10) / 10.0f;
			const float y = pos.y + (float)(rand() % 20 - 10) / 10.0f;
			const float z = pos.z + 1.25f;

			auto bag = Object::Create("p_poly_bag_01_s"_J, rage::fvector3{x, y, z});
			if (!bag)
				continue;

			ENTITY::APPLY_FORCE_TO_ENTITY(bag.GetHandle(), 1, 0.0f, 0.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0, true, false, false, false, false);
			g_FakeBags.push_back(bag.GetHandle());
		}

		// 上限保护：最多保留 60 个袋子，超出时删除最旧的
		while (g_FakeBags.size() > 60)
		{
			const int oldest = g_FakeBags.front();
			if (oldest && ENTITY::DOES_ENTITY_EXIST(oldest))
				Entity(oldest).Delete();
			g_FakeBags.erase(g_FakeBags.begin());
		}

		// 钱声：附近玩家（含目标）都能听到
		AUDIO::PLAY_SOUND_FROM_COORD(-1, "LOCAL_PLYR_CASH_COUNTER_COMPLETE", pos.x, pos.y, pos.z, "DLC_HEISTS_GENERAL_FRONTEND_SOUNDS", true, 0, false);
	}

	class FakeMoneyDrop : public PlayerCommand
	{
		using PlayerCommand::PlayerCommand;

		virtual void OnCall(Player player) override
		{
			if (!player.IsValid() || player.IsLocal())
				return;

			DropBags(player, 6);
			Notifications::Show("假钱袋", "已在该玩家脚下撒下假钱袋。", NotificationType::Success);
		}
	};

	class LoopFakeMoneyDrop : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		int m_Timer = 0;

		virtual void OnTick() override
		{
			if (++m_Timer < 240) // 约 4 秒
				return;
			m_Timer = 0;

			std::erase_if(g_FakeBags, [](int h) { return h == 0 || !ENTITY::DOES_ENTITY_EXIST(h); });

			auto player = Players::GetSelected();
			if (!player.IsValid() || player.IsLocal())
				return;

			DropBags(player, 2);
		}

		virtual void OnDisable() override
		{
			for (int h : g_FakeBags)
			{
				if (h && ENTITY::DOES_ENTITY_EXIST(h))
					Entity(h).Delete();
			}
			g_FakeBags.clear();
		}
	};

	static FakeMoneyDrop _FakeMoneyDrop{"fakemoneydrop", "假钱袋", "在目标脚下撒一堆假钱袋并播放收钱音效（整蛊用）。"};
	static LoopFakeMoneyDrop _LoopFakeMoneyDrop{"loopfakedrop", "循环假钱袋", "持续给选中玩家撒假钱袋，关闭时自动清理。"};
}
