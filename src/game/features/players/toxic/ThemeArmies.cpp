#include <cmath>
#include <span>
#include <vector>

#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Players.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/Ped.hpp"

namespace YimMenu::Features
{
	// 主题军团基类：围着选中的玩家刷固定阵容的 NPC
	class ThemeArmyBase : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		static constexpr size_t s_MaxUnits = 8;

		std::vector<int> m_Units;
		int m_Timer = 0;

	protected:
		virtual std::span<const std::uint32_t> Models() const = 0;

		void Cleanup()
		{
			for (const int handle : m_Units)
			{
				if (ENTITY::DOES_ENTITY_EXIST(handle))
					Ped(handle).Delete();
			}

			m_Units.clear();
		}

		virtual void OnTick() override
		{
			auto player = Players::GetSelected();
			if (!player.IsValid() || player.IsLocal())
			{
				Cleanup();
				return;
			}

			if (++m_Timer < 120)
				return;

			m_Timer = 0;

			auto target = player.GetPed();
			if (!target)
				return;

			std::erase_if(m_Units, [](int handle) {
				return !ENTITY::DOES_ENTITY_EXIST(handle) || PED::IS_PED_DEAD_OR_DYING(handle, true);
			});

			if (m_Units.size() >= s_MaxUnits)
				return;

			std::uint32_t model = 0;
			for (auto candidate : Models())
			{
				if (STREAMING::IS_MODEL_IN_CDIMAGE(candidate))
				{
					model = candidate;
					break;
				}
			}

			if (model == 0)
				return;

			const auto pos = target.GetPosition();

			auto unit = Ped::Create(model, rage::fvector3{pos.x + (float)(rand() % 12 - 6), pos.y + (float)(rand() % 12 - 6), pos.z + 0.5f});
			if (!unit)
				return;

			const float angle = (float)m_Units.size() / (float)s_MaxUnits * 6.28318f;

			TASK::TASK_FOLLOW_TO_OFFSET_OF_ENTITY(unit.GetHandle(),
			    target.GetHandle(),
			    cosf(angle) * 2.5f,
			    sinf(angle) * 2.5f,
			    0.0f,
			    2.0f,
			    -1,
			    1.5f,
			    true);
			PED::SET_PED_KEEP_TASK(unit.GetHandle(), true);

			m_Units.push_back(unit.GetHandle());
		}

		virtual void OnDisable() override
		{
			Cleanup();
		}
	};

	class AlienArmy : public ThemeArmyBase
	{
		using ThemeArmyBase::ThemeArmyBase;

	protected:
		std::span<const std::uint32_t> Models() const override
		{
			static const std::uint32_t models[] = {"s_m_m_movalien_01"_J, "s_m_m_movspace_01"_J};
			return models;
		}
	};

	class AstronautArmy : public ThemeArmyBase
	{
		using ThemeArmyBase::ThemeArmyBase;

	protected:
		std::span<const std::uint32_t> Models() const override
		{
			static const std::uint32_t models[] = {"s_m_m_movspace_01"_J};
			return models;
		}
	};

	class MarineArmy : public ThemeArmyBase
	{
		using ThemeArmyBase::ThemeArmyBase;

	protected:
		std::span<const std::uint32_t> Models() const override
		{
			static const std::uint32_t models[] = {"s_m_m_marine_01"_J, "s_m_y_marine_01"_J, "s_m_y_marine_02"_J};
			return models;
		}
	};

	static AlienArmy _AlienArmy{"alienarmy", "外星人大军", "一群外星人围着选中的玩家" };
	static AstronautArmy _AstronautArmy{"astronautarmy", "宇航员军团", "一群宇航员围着选中的玩家"};
	static MarineArmy _MarineArmy{"marinearmy", "陆战队清场", "一队陆战队员围着选中的玩家"};
}
