#include "core/commands/Command.hpp"
#include "core/commands/FloatCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

namespace YimMenu::Features
{
	static FloatCommand _DirectionalTpDistance{"directionaltpdistance", "传送距离", "方向传送每次移动的距离。"};

	class TpForward : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			auto ped = Self::GetPed();
			ped.TeleportTo(ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(ped.GetHandle(), 0, _DirectionalTpDistance.GetState(), 0));
		}
	};

	class TpBackward : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			auto ped = Self::GetPed();
			ped.TeleportTo(ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(ped.GetHandle(), 0, -_DirectionalTpDistance.GetState(), 0));
		}
	};

	class TpRight : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			auto ped = Self::GetPed();
			ped.TeleportTo(ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(ped.GetHandle(), _DirectionalTpDistance.GetState(), 0, 0));
		}
	};

	class TpLeft : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			auto ped = Self::GetPed();
			ped.TeleportTo(ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(ped.GetHandle(), -_DirectionalTpDistance.GetState(), 0, 0));
		}
	};

	class TpUp : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			auto ped = Self::GetPed();
			ped.TeleportTo(ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(ped.GetHandle(), 0, 0, _DirectionalTpDistance.GetState()));
		}
	};

	class TpDown : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			auto ped = Self::GetPed();
			ped.TeleportTo(ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(ped.GetHandle(), 0, 0, -_DirectionalTpDistance.GetState()));
		}
	};

	static TpForward  _DirectionalTpForward {"directionaltpforward",  "向前传送", "向前移动指定距离。"};
	static TpBackward _DirectionalTpBackward{"directionaltpbackward", "向后传送", "向后移动指定距离。"};
	static TpRight    _DirectionalTpRight   {"directionaltpright",    "向右传送", "向右移动指定距离。"};
	static TpLeft     _DirectionalTpLeft    {"directionaltpleft",     "向左传送", "向左移动指定距离。"};
	static TpUp       _DirectionalTpUp      {"directionaltpup",       "向上传送", "向上移动指定距离。"};
	static TpDown     _DirectionalTpDown    {"directionaltpdown",     "向下传送", "向下移动指定距离。"};
}
