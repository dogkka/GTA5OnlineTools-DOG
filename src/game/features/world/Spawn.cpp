#include "core/commands/StringCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "core/localization/Localization.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Object.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	static StringCommand _PedModelName{"pedmodelname", "行人模型", "要生成的行人模型名称。"};
	static StringCommand _ObjectModelName{"objectmodelname", "物体模型", "要生成的物体模型名称。"};


	class SpawnPed : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			auto model = _PedModelName.GetString();
			if (!model.length())
			{
				Notifications::Show(Localization::Translate("生成行人"), Localization::Translate("未提供模型名称。"), NotificationType::Error);
				return;
			}

			auto modelHash = Joaat(model);
			if (STREAMING::IS_MODEL_IN_CDIMAGE(modelHash))
			{
				Ped::Create(modelHash, Self::GetPed().GetPosition(), Self::GetPed().GetHeading());
			}
			else
			{
				Notifications::Show(Localization::Translate("生成行人"), Localization::Translate("提供的模型名称无效。"), NotificationType::Error);
			}
		}
	};

	class SpawnObject : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			auto model = _ObjectModelName.GetString();
			if (!model.length())
			{
				Notifications::Show(Localization::Translate("生成物体"), Localization::Translate("未提供模型名称。"), NotificationType::Error);
				return;
			}

			auto modelHash = Joaat(model);
			if (STREAMING::IS_MODEL_IN_CDIMAGE(modelHash))
			{
				Object::Create(modelHash, Self::GetPed().GetPosition());
			}
			else
			{
				Notifications::Show(Localization::Translate("生成物体"), Localization::Translate("提供的模型名称无效。"), NotificationType::Error);
			}
		}
	};

	static SpawnPed _SpawnPed{"spawnped", "生成行人", "在你当前位置生成一个行人。"};
	static SpawnObject _SpawnObject{"spawnobject", "生成物体", "在你当前位置生成一个物体。"};
}
