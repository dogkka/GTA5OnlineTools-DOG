#include "core/commands/LoopedCommand.hpp"
#include "core/commands/ListCommand.hpp"
#include "game/backend/NativeHooks.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"
#include "game/pointers/Pointers.hpp"
#include "types/pad/ControllerInputs.hpp"

namespace YimMenu::Features
{
	static void IsControlPressedHook(rage::scrNativeCallContext* ctx);

	const std::vector<std::pair<int, const char*>> g_SpecialAbilityTypes = {
	    {0, "尾流加速"},
	    {2, "死亡之眼"},
	    {1, "崔佛狂怒"},
	    {3, "快照射击（瞄准头部）"},
	    {4, "辱骂"},
	};
	static ListCommand _SelectedSpecialAbility{"selspecialability", "多人模式特殊能力", "为多人模式角色启用的特殊能力", g_SpecialAbilityTypes, 0};

	class MpSpecialAbility : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnEnable() override
		{
			static auto hooked = []() {
				NativeHooks::AddHook("freemode"_J, NativeIndex::IS_CONTROL_PRESSED, &IsControlPressedHook);
				return true;
			}();

			Pointers.AbilityBarPatch->Apply();
			if (*Pointers.IsSessionStarted)
			{
				HUD::SET_ALLOW_ABILITY_BAR(true);
				HUD::SET_ABILITY_BAR_VISIBILITY(true);
				HUD::FLASH_ABILITY_BAR(2000);
			}
		}

		virtual void OnTick() override
		{
			if (!Self::GetPed() || !*Pointers.IsSessionStarted)
				return;

			auto id = Self::GetPlayer().GetId();
			if (!PLAYER::IS_SPECIAL_ABILITY_UNLOCKED(0))
				PLAYER::SPECIAL_ABILITY_UNLOCK(0, true); // the second parameter enables the cut C&C ability mode

			if (!PLAYER::IS_SPECIAL_ABILITY_ACTIVE(id, 0))
				PLAYER::SET_SPECIAL_ABILITY_MP(id, _SelectedSpecialAbility.GetState(), 0);
			HUD::SET_ALLOW_ABILITY_BAR(true);
			HUD::SET_ABILITY_BAR_VISIBILITY(true);
		}

		virtual void OnDisable() override
		{
			Pointers.AbilityBarPatch->Restore();
			PLAYER::SPECIAL_ABILITY_LOCK(0, true);
			HUD::SET_ALLOW_ABILITY_BAR(false);
		}
	};

	static MpSpecialAbility _MpSpecialAbility{"mpspecialability", "启用多人模式特殊能力", "在多人模式中启用特殊能力。该功能复用了被删减内容，可能不稳定"};

	void IsControlPressedHook(rage::scrNativeCallContext* ctx)
	{
		if (_MpSpecialAbility.GetState())
		{
			auto input = static_cast<ControllerInputs>(ctx->GetArg<int>(1));

			if (input == ControllerInputs::INPUT_SPECIAL_ABILITY_PC || input == ControllerInputs::INPUT_SPECIAL_ABILITY || input == ControllerInputs::INPUT_SPECIAL_ABILITY_SECONDARY)
				ctx->SetReturnValue(FALSE);
		}

		return ctx->SetReturnValue(PAD::IS_CONTROL_PRESSED(ctx->GetArg<int>(0), ctx->GetArg<int>(1)));
	}
}
