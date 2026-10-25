#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Players.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/Object.hpp"

namespace YimMenu::Features
{
	// UFO 悬停：在选中玩家头顶维持一架旋转的飞碟（本地可见）
	class UfoEvent : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		int m_Saucer = 0;
		int m_Timer  = 0;

		void Cleanup()
		{
			if (m_Saucer && ENTITY::DOES_ENTITY_EXIST(m_Saucer))
			{
				OBJECT::DELETE_OBJECT(&m_Saucer);
			}

			m_Saucer = 0;
		}

		virtual void OnTick() override
		{
			auto player = Players::GetSelected();
			if (!player.IsValid() || player.IsLocal())
			{
				Cleanup();
				return;
			}

			if (++m_Timer < 15)
				return;

			m_Timer = 0;

			auto target = player.GetPed();
			if (!target)
				return;

			const auto pos = target.GetPosition();

			if (!m_Saucer || !ENTITY::DOES_ENTITY_EXIST(m_Saucer))
			{
				std::uint32_t model = 0;
				for (auto candidate : {"prop_cs_saucer_01"_J, "p_cs_saucer_01_s"_J})
				{
					if (STREAMING::IS_MODEL_IN_CDIMAGE(candidate))
					{
						model = candidate;
						break;
					}
				}

				if (!model)
					return;

				if (auto saucer = Object::Create(model, rage::fvector3{pos.x, pos.y, pos.z + 25.0f}))
					m_Saucer = saucer.GetHandle();
				return;
			}

			ENTITY::SET_ENTITY_COORDS(m_Saucer, pos.x, pos.y, pos.z + 25.0f, false, false, false, true);
			ENTITY::SET_ENTITY_ROTATION(m_Saucer, 0.0f, 0.0f, static_cast<float>(MISC::GET_GAME_TIMER() % 36000) * 0.01f, 2, true);
		}

		virtual void OnDisable() override
		{
			Cleanup();
		}
	};

	static UfoEvent _UfoEvent{"ufo", "UFO 悬停", "在选中玩家头顶维持一架旋转飞碟（本地可见）"};
}
