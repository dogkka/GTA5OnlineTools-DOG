#include "core/commands/Command.hpp"
#include "core/commands/Commands.hpp"
#include "core/commands/StringCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "core/util/Joaat.hpp"

namespace YimMenu::Features
{
	static StringCommand _CommandInput{"commandinput", "命令名", "输入命令名称或中文标签，然后点击执行"};

	class ExecuteCommand : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			const auto input = _CommandInput.GetString();
			if (input.empty())
			{
				Notifications::Show("命令执行器", "请输入命令名。", NotificationType::Warning);
				return;
			}

			// by internal name first
			if (auto cmd = Commands::GetCommand(Joaat(input)))
			{
				cmd->Call();
				Notifications::Show("命令执行器", "已执行：" + std::string(cmd->GetLabel()), NotificationType::Success);
				return;
			}

			// then by (localized) label
			for (auto& [hash, cmd] : Commands::GetCommands())
			{
				if (cmd->GetLabel() == input)
				{
					cmd->Call();
					Notifications::Show("命令执行器", "已执行：" + std::string(cmd->GetLabel()), NotificationType::Success);
					return;
				}
			}

			Notifications::Show("命令执行器", "未找到命令：" + input, NotificationType::Warning);
		}
	};

	static ExecuteCommand _ExecuteCommand{"executecommand", "执行命令", "执行上面输入的命令"};
}
