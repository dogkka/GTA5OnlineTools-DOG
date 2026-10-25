#include <iterator>
#include <vector>

#include "core/commands/Command.hpp"
#include "core/commands/ListCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	static const struct
	{
		std::uint32_t Model;
		const char* Name;
	} g_SwapModels[] = {
	    {"a_c_chop"_J, "狗（小查）"},
	    {"a_c_mtlion"_J, "山狮"},
	    {"a_c_panther"_J, "黑豹"},
	    {"a_c_sharktiger"_J, "虎鲨"},
	    {"a_c_chickenhawk"_J, "鹰"},
	    {"a_c_cow"_J, "奶牛"},
	    {"a_c_pig"_J, "猪"},
	    {"a_c_deer"_J, "鹿"},
	    {"u_m_y_zombie_01"_J, "丧尸"},
	    {"s_m_y_cop_01"_J, "警察"},
	    {"s_m_m_doctor_01"_J, "医生"},
	    {"s_m_m_security_01"_J, "保安"},
	};

	static std::vector<std::pair<int, const char*>> MakeSwapModelList()
	{
		std::vector<std::pair<int, const char*>> list;
		for (int i = 0; i < (int)std::size(g_SwapModels); i++)
			list.push_back({i, g_SwapModels[i].Name});
		return list;
	}

	static ListCommand _SwapModelSelector{"swapmodelselect", "模型", "选择要变身的模型", MakeSwapModelList(), 0};

	class ApplySwapModel : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			const int index = _SwapModelSelector.GetState();
			if (index < 0 || index >= (int)std::size(g_SwapModels))
				return;

			const auto model = g_SwapModels[index].Model;
			if (!STREAMING::IS_MODEL_IN_CDIMAGE(model))
			{
				Notifications::Show("变身", "该模型在当前版本不可用。", NotificationType::Warning);
				return;
			}

			if (!STREAMING::HAS_MODEL_LOADED(model))
			{
				STREAMING::REQUEST_MODEL(model);
				Notifications::Show("变身", "模型加载中，1~2 秒后再点一次「变身」。", NotificationType::Info);
				return;
			}

			PLAYER::SET_PLAYER_MODEL(PLAYER::PLAYER_ID(), model);
			PED::SET_PED_DEFAULT_COMPONENT_VARIATION(Self::GetPed().GetHandle());
			STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(model);

			Notifications::Show("变身", std::string("已变身：") + g_SwapModels[index].Name, NotificationType::Success);
		}
	};

	class RevertSwapModel : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			PLAYER::SET_PLAYER_MODEL(PLAYER::PLAYER_ID(), "mp_m_freemode_01"_J);
			PED::SET_PED_DEFAULT_COMPONENT_VARIATION(Self::GetPed().GetHandle());
			Notifications::Show("变身", "已恢复默认模型（外观可能需要去服装店重新保存）。", NotificationType::Info);
		}
	};

	static ApplySwapModel _ApplySwapModel{"applyswapmodel", "变身", "变身成上面选择的模型"};
	static RevertSwapModel _RevertSwapModel{"revertswapmodel", "恢复人形", "变回默认角色模型"};
}
