#include "Items.hpp"
#include "game/commands/PlayerCommand.hpp"
#include "core/commands/Commands.hpp"
#include "core/backend/FiberPool.hpp"
#include "core/localization/Localization.hpp"
#include "game/backend/Players.hpp"

namespace YimMenu
{
	PlayerCommandItem::PlayerCommandItem(joaat_t id, std::optional<std::string> label_override) :
	    m_Command(Commands::GetCommand<PlayerCommand>(id)),
	    m_LabelOverride(label_override)
	{
	}

	void PlayerCommandItem::Draw()
	{
		if (!m_Command)
		{
			ImGui::Text("%s", "未知！");
			return;
		}

		const auto label = Localization::TranslateLabel(m_LabelOverride.has_value() ? m_LabelOverride.value() : m_Command->GetLabel());
		const auto description = Localization::Translate(m_Command->GetDescription());
		if (ImGui::Button(label.c_str()))
		{
			FiberPool::Push([this] {
				if (Players::GetSelected().IsValid())
					m_Command->Call(Players::GetSelected());
			});
		}

		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("%s", description.c_str());
		}
	}
}
