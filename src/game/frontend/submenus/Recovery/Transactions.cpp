#include "StatEditor.hpp"
#include "core/backend/FiberPool.hpp"
#include "core/backend/ScriptMgr.hpp"
#include "core/frontend/Notifications.hpp"
#include "core/frontend/widgets/imgui_colors.h"
#include "core/localization/Localization.hpp"
#include "game/backend/AnticheatBypass.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/Scripts.hpp"
#include "game/pointers/Pointers.hpp"
#include "types/netshop/CNetShopTransaction.hpp"
#include "types/netshop/netCatalogBaseItem.hpp"

#include <array>

namespace YimMenu::Submenus
{
	template <typename... Args>
	static std::string RuntimeFormat(std::string_view fmt, Args&&... args)
	{
		return std::vformat(fmt, std::make_format_args(args...));
	}

	static std::string TranslateTxnLabel(const char* label)
	{
		return label ? Localization::Translate(label) : "未知";
	}

	constexpr std::array<std::pair<const char*, std::uint32_t>, 28> NET_SHOP_ACTIONS =
	    {{
	        {"获取", "NET_SHOP_ACTION_ACQUIRE"_J},
	        {"添加违禁品", "NET_SHOP_ACTION_ADD_CONTRABAND"_J},
	        {"分配", "NET_SHOP_ACTION_ALLOT"_J}, // not used?
	                                              //  {"BONUS", "NET_SHOP_ACTION_BONUS"_J}, I bet someone would use this and get themselves banned
	        {"购买赌场筹码", "NET_SHOP_ACTION_BUY_CASINO_CHIPS"_J},
	        {"购买违禁品任务", "NET_SHOP_ACTION_BUY_CONTRABAND_MISSION"_J},
	        {"购买物品", "NET_SHOP_ACTION_BUY_ITEM"_J},
	        {"购买房产", "NET_SHOP_ACTION_BUY_PROPERTY"_J},
	        {"购买解锁项", "NET_SHOP_ACTION_BUY_UNLOCK"_J},
	        {"购买载具", "NET_SHOP_ACTION_BUY_VEHICLE"_J},
	        {"购买载具改装", "NET_SHOP_ACTION_BUY_VEHICLE_MODS"_J},
	        {"购买仓库", "NET_SHOP_ACTION_BUY_WAREHOUSE"_J},
	        {"创建角色外观", "NET_SHOP_ACTION_CREATE_PLAYER_APPEARANCE"_J},
	        {"删除角色", "NET_SHOP_ACTION_DELETE_CHAR"_J}, // not used
	        {"收入", "NET_SHOP_ACTION_EARN"_J},
	        {"受限服务收入", "NET_SHOP_ACTION_EARN_LIMITED_SERVICE"_J},
	        {"给予", "NET_SHOP_ACTION_GIVE"_J},
	        {"购买", "NET_SHOP_ACTION_PURCH"_J},
	        {"补偿", "NET_SHOP_ACTION_RECOUP"_J},
	        {"移除违禁品", "NET_SHOP_ACTION_REMOVE_CONTRABAND"_J},
	        {"重置产业进度", "NET_SHOP_ACTION_RESET_BUSINESS_PROGRESS"_J},
	        {"出售赌场筹码", "NET_SHOP_ACTION_SELL_CASINO_CHIPS"_J},
	        {"出售载具", "NET_SHOP_ACTION_SELL_VEHICLE"_J},
	        {"支出", "NET_SHOP_ACTION_SPEND"_J},
	        {"受限服务支出", "NET_SHOP_ACTION_SPEND_LIMITED_SERVICE"_J}, // not used
	        {"更新产业货物", "NET_SHOP_ACTION_UPDATE_BUSINESS_GOODS"_J},
	        {"更新存储数据", "NET_SHOP_ACTION_UPDATE_STORAGE_DATA"_J},
	        {"更新仓库载具", "NET_SHOP_ACTION_UPDATE_WAREHOUSE_VEHICLE"_J},
	        {"使用", "NET_SHOP_ACTION_USE"_J},
	    }};

	constexpr std::array<std::pair<const char*, std::uint32_t>, 60> NET_SHOP_CATEGORIES =
	    {{
	        {"胡须", "CATEGORY_BEARD"_J},
	        {"腮红", "CATEGORY_BLUSHER"_J},
	        {"赌场筹码原因", "CATEGORY_CASINO_CHIP_REASON"_J},
	        {"赌场筹码", "CATEGORY_CASINO_CHIPS"_J},
	        {"胸毛", "CATEGORY_CHEST_HAIR"_J},
	        {"服饰", "CATEGORY_CLOTH"_J},
	        {"隐形眼镜", "CATEGORY_CONTACTS"_J},
	        {"违禁品标记", "CATEGORY_CONTRABAND_FLAGS"_J},
	        {"违禁品任务", "CATEGORY_CONTRABAND_MISSION"_J},
	        {"违禁品数量", "CATEGORY_CONTRABAND_QNTY"_J},
	        {"货币类型", "CATEGORY_CURRENCY_TYPE"_J},
	        {"数据存储", "CATEGORY_DATA_STORAGE"_J},
	        {"装饰", "CATEGORY_DECORATION"_J},
	        {"收入货币", "CATEGORY_EARN_CURRENCY"_J},
	        {"眉毛", "CATEGORY_EYEBROWS"_J},
	        {"脸部彩绘", "CATEGORY_FACEPAINT"_J},
	        {"发型", "CATEGORY_HAIR"_J},
	        {"库存胡须", "CATEGORY_INVENTORY_BEARD"_J},
	        {"库存腮红", "CATEGORY_INVENTORY_BLUSHER"_J},
	        {"库存胸毛", "CATEGORY_INVENTORY_CHEST_HAIR"_J},
	        {"库存隐形眼镜", "CATEGORY_INVENTORY_CONTACTS"_J},
	        {"库存违禁品任务", "CATEGORY_INVENTORY_CONTRABAND_MISSION"_J},
	        {"库存货币", "CATEGORY_INVENTORY_CURRENCY"_J},
	        {"库存眉毛", "CATEGORY_INVENTORY_EYEBROWS"_J},
	        {"库存脸部彩绘", "CATEGORY_INVENTORY_FACEPAINT"_J},
	        {"库存发型", "CATEGORY_INVENTORY_HAIR"_J},
	        {"库存物品", "CATEGORY_INVENTORY_ITEM"_J}, // unused
	        {"库存口红", "CATEGORY_INVENTORY_LIPSTICK"_J},
	        {"库存妆容", "CATEGORY_INVENTORY_MKUP"_J},
	        {"库存已付价格", "CATEGORY_INVENTORY_PRICE_PAID"_J},
	        {"库存房产", "CATEGORY_INVENTORY_PROPERTIE"_J},
	        {"库存房产内饰", "CATEGORY_INVENTORY_PROPERTY_INTERIOR"_J},
	        {"库存载具", "CATEGORY_INVENTORY_VEHICLE"_J},
	        {"库存载具改装", "CATEGORY_INVENTORY_VEHICLE_MOD"_J},
	        {"库存仓库", "CATEGORY_INVENTORY_WAREHOUSE"_J},
	        {"库存仓库内饰", "CATEGORY_INVENTORY_WAREHOUSE_INTERIOR"_J},
	        {"口红", "CATEGORY_LIPSTICK"_J},
	        {"商店", "CATEGORY_MART"_J},
	        {"妆容", "CATEGORY_MKUP"_J},
	        {"价格修正", "CATEGORY_PRICE_MODIFIER"_J},
	        {"价格覆盖", "CATEGORY_PRICE_OVERRIDE"_J},
	        {"房产", "CATEGORY_PROPERTIE"_J}, // seriously how tf did "propertie" get past code reviews?
	        {"房产内饰", "CATEGORY_PROPERTY_INTERIOR"_J},
	        {"服务", "CATEGORY_SERVICE"_J},
	        {"已解锁服务", "CATEGORY_SERVICE_UNLOCKED"_J},
	        {"有限服务", "CATEGORY_SERVICE_WITH_LIMIT"_J},
	        {"有阈值服务", "CATEGORY_SERVICE_WITH_THRESHOLD"_J},
	        {"系统", "CATEGORY_SYSTEM"_J},
	        {"纹身", "CATEGORY_TATTOO"_J},
	        {"解锁", "CATEGORY_UNLOCK"_J},
	        {"载具", "CATEGORY_VEHICLE"_J},
	        {"载具改装", "CATEGORY_VEHICLE_MOD"_J},
	        {"载具升级", "CATEGORY_VEHICLE_UPGRADE"_J},
	        {"自动贩卖机", "CATEGORY_VENDING_MACHINE"_J},
	        {"仓库", "CATEGORY_WAREHOUSE"_J},
	        {"仓库内饰", "CATEGORY_WAREHOUSE_INTERIOR"_J},
	        {"仓库载具索引", "CATEGORY_WAREHOUSE_VEHICLE_INDEX"_J},
	        {"武器", "CATEGORY_WEAPON"_J},
	        {"武器弹药", "CATEGORY_WEAPON_AMMO"_J},
	        {"武器配件", "CATEGORY_WEAPON_MOD"_J},
	    }};

	// more guardrails to prevent some stupid stuff
	const std::unordered_set<std::uint32_t> BANNED_ITEM_HASHES =
	    {
	        "SERVICE_EARN_DEBUG"_J,
	        "SERVICE_EARN_JBONUS"_J,
	        "SERVICE_EARN_JBONUS_MODEL"_J,
	        "SERVICE_EARN_JBONUS_NO_DEATH"_J,
	        "SERVICE_EARN_JBONUS_NO_FALL"_J,
	        "SERVICE_EARN_JBONUS_NOT_SEEN"_J,
	        "SERVICE_EARN_JBONUS_SE"_J,
	        "SERVICE_EARN_EVOLVE"_J,
	        "SERVICE_EARN_IMPULSE"_J,
	};

	static std::string CategoryNameFromHash(std::uint32_t hash)
	{
		for (auto& cat : NET_SHOP_CATEGORIES)
			if (cat.second == hash)
				return cat.first;

		return "未知";
	}

	struct TransactionItemHash
	{
		char m_Name[128]{};
		std::uint32_t m_Hash;
		std::uint32_t m_IntendedCategory;
		int m_IntendedPrice;
		bool m_IsValid;
	};

	struct TransactionCategoryHash
	{
		const char* m_Name = nullptr;
		std::uint32_t m_Hash;
	};

	struct BasketItem
	{
		TransactionItemHash m_PrimaryItem;
		TransactionItemHash m_SecondaryItem;
		std::uint32_t m_Quantity = 1;
		int m_StatValue = 1;
		int m_Price;
	};

	struct TransactionInfo
	{
		enum class Type
		{
			BASKET,
			SERVICE
		};

		Type m_Type = Type::BASKET;
		TransactionCategoryHash m_Category{"房产", "CATEGORY_PROPERTIE"_J};
		TransactionCategoryHash m_Action{"购买房产", "NET_SHOP_ACTION_BUY_PROPERTY"_J};
		struct
		{
			std::vector<BasketItem> m_BasketItems = {BasketItem{}};
		} m_Basket;
		struct
		{
			TransactionItemHash m_Item;
			int m_Price;
		} m_Service;
	};

	static void ProcessTransaction(const TransactionInfo& info)
	{
		FiberPool::Push([&info] {
			int txn_id{-1};
			bool txn_failed{false};
			CNetShopTransaction* txn{nullptr};

			// set up transaction
			Scripts::RunAsScript(Scripts::FindScriptThread("shop_controller"_J), [&] {
				if (info.m_Type == TransactionInfo::Type::BASKET)
				{
					if (NETSHOPPING::NET_GAMESERVER_BASKET_IS_ACTIVE())
						NETSHOPPING::NET_GAMESERVER_BASKET_END();

					if (!NETSHOPPING::NET_GAMESERVER_BASKET_START(&txn_id, info.m_Category.m_Hash, info.m_Action.m_Hash, 4))
					{
						Notifications::Show("交易", "创建购物篮失败", NotificationType::Error);
						txn_failed = true;
						NETSHOPPING::NET_GAMESERVER_BASKET_END();
						return;
					}

					struct NETSHOPPING_BASKET_ITEM
					{
						SCR_HASH PrimaryHash;
						SCR_HASH SecondaryHash;
						SCR_INT Value;
						SCR_INT StatValue;
					};
					static_assert(sizeof(NETSHOPPING_BASKET_ITEM) == 4 * 8);

					for (auto& item : info.m_Basket.m_BasketItems)
					{
						NETSHOPPING_BASKET_ITEM scr_item{};
						scr_item.PrimaryHash = item.m_PrimaryItem.m_Hash;
						scr_item.SecondaryHash = item.m_SecondaryItem.m_Hash;
						scr_item.Value = item.m_Price;
						scr_item.StatValue = item.m_StatValue;

						if (!NETSHOPPING::NET_GAMESERVER_BASKET_ADD_ITEM(&scr_item, item.m_Quantity))
						{
							Notifications::Show("交易",
							    RuntimeFormat("添加 {}（x{}）到购物篮失败", item.m_PrimaryItem.m_Name, item.m_Quantity),
							    NotificationType::Error);
							txn_failed = true;
							NETSHOPPING::NET_GAMESERVER_BASKET_END();
							return;
						}
					}
				}
				else
				{
					if (!NETSHOPPING::NET_GAMESERVER_BEGIN_SERVICE(&txn_id, info.m_Category.m_Hash, info.m_Service.m_Item.m_Hash, info.m_Action.m_Hash, info.m_Service.m_Price, 4))
					{
						Notifications::Show("交易", "创建服务失败", NotificationType::Error);
						txn_failed = true;
						return;
					}
				}

				if (info.m_Type == TransactionInfo::Type::BASKET)
				{
					txn = Pointers.GetActiveBasket(*Pointers.TransactionMgr, &txn_id); // TODO: add support for services too
				}

				if (!NETSHOPPING::NET_GAMESERVER_CHECKOUT_START(txn_id))
				{
					Notifications::Show("交易", "开始结账失败", NotificationType::Error);
					txn_failed = true;
					return;
				}
			});

			if (txn && txn->m_Running)
			{
				while (txn->m_Status == 0 || txn->m_Status == 1)
					ScriptMgr::Yield();

				if (txn->m_Status == 3)
				{
					Notifications::Show("交易", "交易完成", NotificationType::Success);
				}
				else
				{
					Notifications::Show("交易", "交易失败", NotificationType::Error);
				}
			}
		});
	}

	static bool HasGTAPlusMembership()
	{
		if (!NETWORK::NETWORK_HAS_SC_MEMBERSHIP_INFO())
			return false;

		struct SC_MEMBERSHIP_INFO
		{
			SCR_BOOL HasMembership;
			SCR_INT StartTime; // very likely, metric says "s" and "e"
			SCR_INT EndTime;
		};

		SC_MEMBERSHIP_INFO info{};
		NETWORK::NETWORK_GET_SC_MEMBERSHIP_INFO(&info);

		return info.HasMembership;
	}

	static int GetItemPrice(rage::netCatalogBaseItem* item)
	{
		if (item->m_MembershipPrice == -1 || !HasGTAPlusMembership())
			return item->m_Price;
		else
			return item->m_MembershipPrice;
	}

	// and only service
	static bool IsCategoryService(std::uint32_t hash)
	{
		return hash == "CATEGORY_SERVICE"_J || hash == "CATEGORY_SERVICE_WITH_LIMIT"_J || hash == "CATEGORY_SERVICE_WITH_THRESHOLD"_J;
	}

	// but can be used for basket as well
	static bool IsActionService(std::uint32_t hash)
	{
		return hash == "NET_SHOP_ACTION_EARN"_J || hash == "NET_SHOP_ACTION_SPEND"_J;
	}

	static bool IsPriceModifier(std::uint32_t hash)
	{
		return hash == "CATEGORY_PRICE_MODIFIER"_J || hash == "CATEGORY_PRICE_OVERRIDE"_J;
	}

	bool ShouldSwapItemParams(std::uint32_t hash)
	{
		switch (hash)
		{
		case "CATEGORY_INVENTORY_ITEM"_J:
		case "CATEGORY_INVENTORY_VEHICLE"_J:
		case "CATEGORY_INVENTORY_VEHICLE_MOD"_J:
		case "CATEGORY_INVENTORY_PROPERTIE"_J:
		case "CATEGORY_INVENTORY_BEARD"_J:
		case "CATEGORY_INVENTORY_MKUP"_J:
		case "CATEGORY_INVENTORY_HAIR"_J:
		case "CATEGORY_INVENTORY_EYEBROWS"_J:
		case "CATEGORY_INVENTORY_CHEST_HAIR"_J:
		case "CATEGORY_INVENTORY_CONTACTS"_J:
		case "CATEGORY_INVENTORY_FACEPAINT"_J:
		case "CATEGORY_INVENTORY_BLUSHER"_J:
		case "CATEGORY_INVENTORY_LIPSTICK"_J:
		case "CATEGORY_INVENTORY_PROPERTY_INTERIOR"_J:
		case "CATEGORY_INVENTORY_WAREHOUSE"_J:
		case "CATEGORY_INVENTORY_CONTRABAND_MISSION"_J:
		case "CATEGORY_CONTRABAND_MISSION"_J:
		case "CATEGORY_CONTRABAND_QNTY"_J:
		case "CATEGORY_INVENTORY_WAREHOUSE_INTERIOR"_J:
		case "CATEGORY_WAREHOUSE_VEHICLE_INDEX"_J:
			return true;
		}

		return false;
	}

	static void OnTransactionTypeChanged(TransactionInfo& info)
	{
		if (info.m_Type == TransactionInfo::Type::SERVICE)
		{
			if (!IsCategoryService(info.m_Category.m_Hash))
				info.m_Category = {"服务", "CATEGORY_SERVICE"_J};
			if (!IsActionService(info.m_Action.m_Hash))
				info.m_Action = {"收入", "NET_SHOP_ACTION_EARN"_J};
			info.m_Basket.m_BasketItems.clear();
		}
		else
		{
			if (IsCategoryService(info.m_Category.m_Hash))
				info.m_Category = {"房产", "CATEGORY_PROPERTIE"_J};
			if (IsActionService(info.m_Action.m_Hash))
				info.m_Action = {"购买房产", "NET_SHOP_ACTION_BUY_PROPERTY"_J};
			info.m_Basket.m_BasketItems.push_back({}); // ensure we always have one
		}
	}

	static void SetTransactionError(std::string_view message)
	{
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Text, ImGui::Colors::Red.Value);
		ImGui::Text("%s", message.data());
		ImGui::PopStyleColor();
	}

	static void SetTransactionWarning(std::string_view message)
	{
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Text, ImGui::Colors::Yellow.Value);
		ImGui::Text("%s", message.data());
		ImGui::PopStyleColor();
	}

	static bool EditTransactionItem(std::string_view label, TransactionInfo& info, TransactionItemHash& item, bool& is_valid, bool required = true, bool validate_category = true)
	{
		bool modified = false;
		const auto translatedLabel = Localization::TranslateLabel(label);

		ImGui::SetNextItemWidth(340.0f);
		if (
		    required ?
		        ImGui::InputText(translatedLabel.c_str(), item.m_Name, sizeof(item.m_Name)) :
		        ImGui::InputTextWithHint(translatedLabel.c_str(), "可选", item.m_Name, sizeof(item.m_Name)))
		{
			item.m_Hash = Joaat(item.m_Name);
			if (auto cat_item = Pointers.GetCatalogItem(Pointers.NetCatalog, &item.m_Hash))
			{
				modified = true;
				item.m_IsValid = true;
				item.m_IntendedCategory = cat_item->m_CategoryHash;
				item.m_IntendedPrice = GetItemPrice(cat_item);
			}
			else
			{
				item.m_IsValid = false;
			}
		}

		bool empty = item.m_Name[0] == 0 && !required;

		if (!item.m_IsValid && !empty)
		{
			SetTransactionError("未找到该物品！");
			is_valid = false;
			return false;
		}

		// TODO: maybe not check this every tick?
		if (BANNED_ITEM_HASHES.contains(item.m_Hash))
		{
			SetTransactionError("出于安全考虑，此物品已被屏蔽");
			is_valid = false;
			return false;
		}

		if (validate_category && !IsPriceModifier(item.m_IntendedCategory) && item.m_IntendedCategory != info.m_Category.m_Hash && !empty)
		{
			if (info.m_Type == TransactionInfo::Type::SERVICE)
			{
				SetTransactionError(RuntimeFormat("物品分类 {} 与交易分类 {} 不匹配", CategoryNameFromHash(item.m_IntendedCategory), info.m_Category.m_Name));
				is_valid = false;
				return false;
			}
			else
			{
				SetTransactionWarning(RuntimeFormat("物品分类 {} 与交易分类 {} 不匹配", CategoryNameFromHash(item.m_IntendedCategory), info.m_Category.m_Name));
			}
		}

		return modified;
	}

	static void RenderBasketEditor(TransactionInfo& info, bool& txn_valid)
	{
		std::optional<int> item_to_delete = std::nullopt;

		for (int i = 0; i < info.m_Basket.m_BasketItems.size(); i++)
		{
			auto& item = info.m_Basket.m_BasketItems[i];
			ImGui::PushID(i);
			if (ShouldSwapItemParams(info.m_Category.m_Hash))
			{
				if (EditTransactionItem("物品", info, item.m_SecondaryItem, txn_valid, true, false)) // TODO: validate if this corresponds to the correct non-inventory category
					item.m_Price = item.m_SecondaryItem.m_IntendedPrice;
				EditTransactionItem("库存槽位", info, item.m_PrimaryItem, txn_valid, true, true);
			}
			else
			{
				if (EditTransactionItem("物品", info, item.m_PrimaryItem, txn_valid, true, true))
					item.m_Price = item.m_PrimaryItem.m_IntendedPrice;
				EditTransactionItem("次要物品", info, item.m_SecondaryItem, txn_valid, false, false); // TODO: is this ever used outside of inventory stuff?
			}

			ImGui::SetNextItemWidth(180.0f);
			if (ImGui::InputScalar("数量", ImGuiDataType_U32, &item.m_Quantity))
			{
				if (item.m_Quantity == 0)
					item_to_delete = i; // assume the user wants this item gone
			}

			ImGui::SetNextItemWidth(180.0f);
			ImGui::InputInt("价格", &item.m_Price);

			ImGui::SetNextItemWidth(180.0f);
			ImGui::InputInt("统计值", &item.m_StatValue); // I'm not actually sure what this does ngl

			if (info.m_Basket.m_BasketItems.size() > 1 && ImGui::Button("删除"))
				item_to_delete = i;
			ImGui::PopID();

			if (info.m_Basket.m_BasketItems.size() != (i + 1))
				ImGui::Separator();
		}

		if (item_to_delete.has_value())
			info.m_Basket.m_BasketItems.erase(std::next(info.m_Basket.m_BasketItems.begin(), *item_to_delete));

		if (ImGui::Button("添加物品"))
		{
			info.m_Basket.m_BasketItems.push_back({});
		}
	}

	static void RenderServiceEditor(TransactionInfo& info, bool& txn_valid)
	{
		if (EditTransactionItem("物品", info, info.m_Service.m_Item, txn_valid))
			info.m_Service.m_Price = info.m_Service.m_Item.m_IntendedPrice;
		if (info.m_Service.m_Item.m_IntendedPrice != 0 || info.m_Action.m_Hash != "NET_SHOP_ACTION_EARN"_J)
		{
			ImGui::SetNextItemWidth(180.0f);
			ImGui::InputInt("价格", &info.m_Service.m_Price);
			if (info.m_Service.m_Price > info.m_Service.m_Item.m_IntendedPrice && info.m_Action.m_Hash == "NET_SHOP_ACTION_EARN"_J)
			{
				SetTransactionError(RuntimeFormat("物品价格超过允许的最大值（{}）", info.m_Service.m_Item.m_IntendedPrice));
				txn_valid = false;
			}
		}
	}

	std::shared_ptr<Category> BuildTransactionsMenu()
	{
		auto menu = std::make_shared<Category>("交易");
		auto normal = std::make_shared<Group>("执行器");

		normal->AddItem(std::make_unique<ImGuiItem>([] {
			if (!NativeInvoker::AreHandlersCached())
				return ImGui::TextDisabled("%s", "原生函数缓存尚未完成。");

			if (AnticheatBypass::IsFSLProvidingLocalSaves())
				return ImGui::TextDisabled("%s", "启用 FSL 本地存档时，交易功能不可用。");

			if (!NETSHOPPING::NET_GAMESERVER_CATALOG_IS_VALID())
				return ImGui::TextDisabled("%s", "目录尚未加载。");

			ImGui::TextWrapped("%s", "警告：使用此工具的一切后果由你自行承担。如果你不了解自己在做什么，很可能会被封禁。");

			static TransactionInfo info{};
			bool txn_valid{true};

			ImGui::SetNextItemWidth(180.0f);
			static const std::array<const char*, 2> transactionTypeKeys = {"购物篮", "服务"};
			if (ImGui::BeginCombo("类型", Localization::Translate(transactionTypeKeys[static_cast<int>(info.m_Type)]).c_str()))
			{
				for (int i = 0; i < static_cast<int>(transactionTypeKeys.size()); ++i)
				{
					const bool selected = static_cast<int>(info.m_Type) == i;
					if (ImGui::Selectable(Localization::Translate(transactionTypeKeys[i]).c_str(), selected))
					{
						info.m_Type = static_cast<TransactionInfo::Type>(i);
						OnTransactionTypeChanged(info);
					}

					if (selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			ImGui::SetNextItemWidth(250.0f);
			const auto selectedCategory = TranslateTxnLabel(info.m_Category.m_Name);
			if (ImGui::BeginCombo("分类", selectedCategory.c_str()))
			{
				for (auto& item : NET_SHOP_CATEGORIES)
				{
					if ((info.m_Type != TransactionInfo::Type::SERVICE) ^ IsCategoryService(item.second))
					{
						const auto translatedCategory = Localization::Translate(item.first);
						if (ImGui::Selectable(translatedCategory.c_str(), item.second == info.m_Category.m_Hash))
						{
							info.m_Category.m_Name = item.first;
							info.m_Category.m_Hash = item.second;
						}

						if (item.second == info.m_Category.m_Hash)
							ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			ImGui::SetNextItemWidth(250.0f);
			const auto selectedAction = TranslateTxnLabel(info.m_Action.m_Name);
			if (ImGui::BeginCombo("操作", selectedAction.c_str()))
			{
				for (auto& item : NET_SHOP_ACTIONS)
				{
					if (info.m_Type != TransactionInfo::Type::SERVICE || IsActionService(item.second))
					{
						const auto translatedAction = Localization::Translate(item.first);
						if (ImGui::Selectable(translatedAction.c_str(), item.second == info.m_Action.m_Hash))
						{
							info.m_Action.m_Name = item.first;
							info.m_Action.m_Hash = item.second;
						}

						if (item.second == info.m_Action.m_Hash)
							ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			ImGui::Separator();

			if (info.m_Type == TransactionInfo::Type::BASKET)
			{
				RenderBasketEditor(info, txn_valid);
			}
			else
			{
				RenderServiceEditor(info, txn_valid);
			}

			ImGui::Separator();

			ImGui::BeginDisabled(!txn_valid);
			if (ImGui::Button("触发"))
				FiberPool::Push([] {
					ProcessTransaction(info);
				});
			if (!txn_valid && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
				ImGui::SetTooltip("%s", "当前交易无效。请确认所有字段都已正确填写。");
			ImGui::EndDisabled();
		}));

		menu->AddItem(std::move(normal));
		return menu;
	}
}
