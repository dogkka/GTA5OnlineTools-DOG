#include "core/commands/Command.hpp"
#include "core/commands/ListCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/ScriptGlobal.hpp"
#include "game/pointers/Pointers.hpp"

#include <string>
#include <vector>

namespace YimMenu::Features
{
	// 数据来源：Tutones-Menu（同版本 Enhanced 1.73 / b1158.13）的请求表。
	// 基址 Global_2733326 = 我们的 FreemodeGeneral；偏移为该构建实测值（与 Stand 旧版不同，已校正）。
	static constexpr std::size_t kRequestServicesGlobal = 2733326;

	struct ServiceEntry
	{
		const char* label;
		std::size_t offset;
	};

	static const ServiceEntry kServices[] = {
	    {"空袭", 3580},
	    {"直升机火力支援", 3579},
	    {"弹药空投", 538},
	    {"船只接送", 539},
	    {"直升机接送", 540},
	    {"直升机接送（豪华 SuperVolito）", 547},
	    {"MOC 防暴车", 577},
	    {"复仇者", 585},
	    {"恐怖雷达（Terrorbyte）", 591},
	    {"虎鲸潜艇（Kosatka）", 613},
	    {"橡皮艇（Dinghy）", 626},
	    {"酸实验室", 592},
	    {"酸实验室摩托", 648},
	    {"保释办公室运输车", 362},
	    {"弹道装备", 548},
	    {"RC 小坦克", 5832},
	    {"RC 犀牛坦克", 5833},
	    {"出租车", 509},
	    {"牛鲨睾丸素", 546},
	    {"卡约岛直升机支援", 490},
	};

	static std::vector<std::pair<int, const char*>> BuildServiceList()
	{
		std::vector<std::pair<int, const char*>> list;
		for (int i = 0; i < (int)(sizeof(kServices) / sizeof(kServices[0])); i++)
			list.emplace_back(i, kServices[i].label);
		return list;
	}

	static bool WriteRequestFlag(std::size_t offset)
	{
		auto flag = ScriptGlobal(kRequestServicesGlobal).At(offset);
		if (!flag.CanAccess())
			return false;

		auto* ptr = flag.As<int*>();
		if (!ptr)
			return false;

		*ptr = 1;
		return *ptr == 1; // 读回校验，与参考实现一致
	}

	static void RequestEntry(const ServiceEntry& entry)
	{
		if (!Pointers.IsSessionStarted || !*Pointers.IsSessionStarted)
		{
			Notifications::Show("请求服务", "请先进入 GTA 在线战局。", NotificationType::Warning);
			return;
		}

		// 豪华直升机：先写 547（SuperVolito 标记）再写 540（接送请求）
		if (entry.offset == 547)
			WriteRequestFlag(547);

		const std::size_t target = entry.offset == 547 ? 540 : entry.offset;
		if (WriteRequestFlag(target))
			Notifications::Show("请求服务", std::string("已请求：") + entry.label, NotificationType::Success);
		else
			Notifications::Show("请求服务", std::string("请求失败：") + entry.label, NotificationType::Error);
	}

	static ListCommand _ServiceSelect{"servicelist", "服务类型", "选择要请求的互动菜单服务（1.73 精确请求标志）。", BuildServiceList(), 0};

	class RequestService : public Command
	{
		using Command::Command;

		virtual void OnCall() override
		{
			const int idx = _ServiceSelect.GetState();
			if (idx < 0 || idx >= (int)(sizeof(kServices) / sizeof(kServices[0])))
				return;
			RequestEntry(kServices[idx]);
		}
	};

	class QuickAirstrike : public Command
	{
		using Command::Command;

		virtual void OnCall() override { RequestEntry(kServices[0]); }
	};

	class QuickBackupHeli : public Command
	{
		using Command::Command;

		virtual void OnCall() override { RequestEntry(kServices[1]); }
	};

	class QuickAmmoDrop : public Command
	{
		using Command::Command;

		virtual void OnCall() override { RequestEntry(kServices[2]); }
	};

	class QuickHeliPickup : public Command
	{
		using Command::Command;

		virtual void OnCall() override { RequestEntry(kServices[4]); }
	};

	static RequestService _RequestService{"requestservice", "请求服务", "向互动菜单发送所选服务请求（1.73 精确请求标志）。"};
	static QuickAirstrike _QuickAirstrike{"svcairstrike", "空袭", "一键请求空袭。"};
	static QuickBackupHeli _QuickBackupHeli{"svcbackupheli", "直升机火力支援", "一键请求武装直升机支援。"};
	static QuickAmmoDrop _QuickAmmoDrop{"svcammodrop", "弹药空投", "一键请求弹药空投。"};
	static QuickHeliPickup _QuickHeliPickup{"svcheli", "直升机接送", "一键请求直升机接送。"};
}
