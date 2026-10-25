#include "UnlockCenter.hpp"
#include "UnlockLimitedCatalog.hpp"
#include "UnlockStatsCatalog.hpp"
#include "core/backend/ScriptMgr.hpp"
#include "core/commands/Command.hpp"
#include "core/commands/IntCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"
#include "game/gta/Stats.hpp"
#include "game/pointers/Pointers.hpp"

#include <algorithm>
#include <atomic>

namespace YimMenu::Features
{
	namespace
	{
		constexpr std::size_t kLimitedBatchSize = 200;
		constexpr std::size_t kCareerBatchSize = 10;

		bool RequireUnlockSession(std::string_view title)
		{
			if (Pointers.IsSessionStarted && *Pointers.IsSessionStarted)
				return true;

			Notifications::Show(std::string(title), "请先进入 GTA 在线模式。", NotificationType::Error);
			return false;
		}

		class BusyGuard
		{
			std::atomic_bool& m_Busy;
			bool m_Locked;

		public:
			explicit BusyGuard(std::atomic_bool& busy) :
			    m_Busy(busy),
			    m_Locked(!busy.exchange(true))
			{
			}

			~BusyGuard()
			{
				if (m_Locked)
					m_Busy = false;
			}

			explicit operator bool() const
			{
				return m_Locked;
			}
		};

		bool IsStatSatisfied(const UnlockEverything::StatEntry& entry)
		{
			using UnlockEverything::StatType;

			switch (entry.Type)
			{
			case StatType::Int:
			{
				const int current = Stats::GetInt(std::string(entry.Name));
				return entry.Value == -1 ? current == -1 : current >= entry.Value;
			}
			case StatType::Bool:
				return Stats::GetBool(std::string(entry.Name));
			case StatType::Float:
				return Stats::GetFloat(std::string(entry.Name)) == entry.FloatValue;
			case StatType::PackedBool:
				return Stats::GetPackedBool(entry.Value);
			case StatType::PackedInt:
				return Stats::GetPackedInt(entry.Value) >= entry.Extra;
			case StatType::PackedBoolRange:
				if (entry.Value > entry.Extra)
					return true;
				for (int index = entry.Value; index <= entry.Extra; index++)
				{
					if (!Stats::GetPackedBool(index))
						return false;
					if ((index - entry.Value + 1) % 50 == 0)
						ScriptMgr::Yield();
				}
				return true;
			case StatType::IntBit:
				return (static_cast<std::uint32_t>(Stats::GetInt(std::string(entry.Name))) & (std::uint32_t{1} << entry.Value)) != 0;
			case StatType::IntBits:
				return (Stats::GetInt(std::string(entry.Name)) & entry.Value) == entry.Value;
			}

			return true;
		}

		void ApplyStat(const UnlockEverything::StatEntry& entry)
		{
			using UnlockEverything::StatType;

			switch (entry.Type)
			{
			case StatType::Int:
				return Stats::SetInt(std::string(entry.Name), entry.Value);
			case StatType::Bool:
				return Stats::SetBool(std::string(entry.Name), true);
			case StatType::Float:
				return Stats::SetFloat(std::string(entry.Name), entry.FloatValue);
			case StatType::PackedBool:
				return Stats::SetPackedBool(entry.Value, true);
			case StatType::PackedInt:
				return Stats::SetPackedInt(entry.Value, entry.Extra);
			case StatType::PackedBoolRange:
				if (entry.Value > entry.Extra)
				{
					LOG(WARNING) << "忽略倒序打包布尔范围：" << entry.Value << " -> " << entry.Extra;
					return;
				}
				for (int index = entry.Value; index <= entry.Extra; index++)
				{
					Stats::SetPackedBool(index, true);
					if ((index - entry.Value + 1) % 50 == 0)
						ScriptMgr::Yield();
				}
				return;
			case StatType::IntBit:
			{
				const auto current = static_cast<std::uint32_t>(Stats::GetInt(std::string(entry.Name)));
				Stats::SetInt(std::string(entry.Name), static_cast<int>(current | (std::uint32_t{1} << entry.Value)));
				return;
			}
			case StatType::IntBits:
				Stats::SetInt(std::string(entry.Name), Stats::GetInt(std::string(entry.Name)) | entry.Value);
				return;
			}
		}

		template<std::size_t Size>
		class StatBatch
		{
			const std::array<UnlockEverything::StatEntry, Size>& m_Catalog;
			std::vector<std::size_t> m_Pending;
			std::size_t m_Cursor = 0;
			std::atomic_size_t m_Processed = 0;
			std::atomic_bool m_Initialized = false;
			std::atomic_bool m_Busy = false;

		public:
			explicit StatBatch(const std::array<UnlockEverything::StatEntry, Size>& catalog) :
			    m_Catalog(catalog)
			{
			}

			void Process(std::size_t batchSize, std::string_view title)
			{
				if (!RequireUnlockSession(title))
					return;

				BusyGuard guard(m_Busy);
				if (!guard)
				{
					Notifications::Show(std::string(title), "上一批仍在处理中。", NotificationType::Warning);
					return;
				}

				if (!m_Initialized)
				{
					m_Pending.clear();
					m_Pending.reserve(Size);
					for (std::size_t index = 0; index < Size; index++)
					{
						if (!IsStatSatisfied(m_Catalog[index]))
							m_Pending.push_back(index);
						if ((index + 1) % 50 == 0)
							ScriptMgr::Yield();
					}

					m_Cursor = 0;
					m_Processed = Size - m_Pending.size();
					m_Initialized = true;
				}

				if (m_Cursor >= m_Pending.size())
				{
					Notifications::Show(std::string(title), "目录中的项目已经全部满足。", NotificationType::Success);
					return;
				}

				const std::size_t end = std::min(m_Cursor + batchSize, m_Pending.size());
				while (m_Cursor < end)
				{
					ApplyStat(m_Catalog[m_Pending[m_Cursor++]]);
					++m_Processed;
				}

				LOG(INFO) << title << "：" << m_Processed.load() << "/" << Size;
				Notifications::Show(std::string(title), std::format("已处理 {}/{} 项。", m_Processed.load(), Size), NotificationType::Success);
			}

			bool Reset(std::string_view title)
			{
				BusyGuard guard(m_Busy);
				if (!guard)
				{
					Notifications::Show(std::string(title), "当前批次尚未结束，暂时无法重置。", NotificationType::Warning);
					return false;
				}

				m_Pending.clear();
				m_Cursor = 0;
				m_Processed = 0;
				m_Initialized = false;
				return true;
			}

			UnlockCenter::Progress GetProgress() const
			{
				return {m_Processed.load(), Size, m_Initialized.load(), m_Busy.load()};
			}
		};

		class LimitedBatch
		{
			std::vector<int> m_Indices;
			std::size_t m_Cursor = 0;
			std::atomic_size_t m_Processed = 0;
			std::atomic_size_t m_Total = 0;
			std::atomic_bool m_Initialized = false;
			std::atomic_bool m_Busy = false;

			template<std::size_t Size>
			void AppendRanges(const std::array<UnlockEverything::PackedBoolRange, Size>& ranges)
			{
				for (const auto& range : ranges)
				{
					if (range.From > range.To)
						continue;
					for (int index = range.From; index <= range.To; index++)
						m_Indices.push_back(index);
				}
			}

			bool Initialize()
			{
				auto ped = Self::GetPed();
				if (!ped.IsValid())
				{
					Notifications::Show("限时物品解锁", "当前玩家角色实体不可用。", NotificationType::Error);
					return false;
				}

				m_Indices.clear();
				m_Indices.reserve(8192);
				AppendRanges(UnlockEverything::CommonLimitedRanges);

				const bool isMale = PED::IS_PED_MALE(ped.GetHandle());
				if (isMale)
					AppendRanges(UnlockEverything::MaleLimitedRanges);
				else
					AppendRanges(UnlockEverything::FemaleLimitedRanges);

				AppendRanges(UnlockEverything::RecentLimitedRanges);
				m_Indices.insert(m_Indices.end(), UnlockEverything::RecentLimitedSingletons.begin(), UnlockEverything::RecentLimitedSingletons.end());
				std::ranges::sort(m_Indices);
				m_Indices.erase(std::unique(m_Indices.begin(), m_Indices.end()), m_Indices.end());
				m_Cursor = 0;
				m_Processed = 0;
				m_Total = m_Indices.size();
				m_Initialized = true;
				return true;
			}

		public:
			void Process()
			{
				if (!RequireUnlockSession("限时物品解锁"))
					return;

				BusyGuard guard(m_Busy);
				if (!guard)
				{
					Notifications::Show("限时物品解锁", "上一批仍在处理中。", NotificationType::Warning);
					return;
				}

				if (!m_Initialized)
				{
					if (!Initialize())
						return;
				}

				if (m_Cursor >= m_Indices.size())
				{
					Notifications::Show("限时物品解锁", "目录中的物品已经全部处理。", NotificationType::Success);
					return;
				}

				const std::size_t end = std::min(m_Cursor + kLimitedBatchSize, m_Indices.size());
				while (m_Cursor < end)
				{
					const int index = m_Indices[m_Cursor++];
					if (!Stats::GetPackedBool(index))
						Stats::SetPackedBool(index, true);
					m_Processed = m_Cursor;
					if (m_Cursor % 50 == 0)
						ScriptMgr::Yield();
				}

				LOG(INFO) << "限时物品解锁：" << m_Processed.load() << "/" << m_Total.load();
				Notifications::Show("限时物品解锁", std::format("已处理 {}/{} 项。", m_Processed.load(), m_Total.load()), NotificationType::Success);
			}

			bool Reset()
			{
				BusyGuard guard(m_Busy);
				if (!guard)
				{
					Notifications::Show("重置解锁进度", "限时物品批次尚未结束。", NotificationType::Warning);
					return false;
				}

				m_Indices.clear();
				m_Cursor = 0;
				m_Processed = 0;
				m_Total = 0;
				m_Initialized = false;
				return true;
			}

			UnlockCenter::Progress GetProgress() const
			{
				return {m_Processed.load(), m_Total.load(), m_Initialized.load(), m_Busy.load()};
			}
		};

		LimitedBatch g_LimitedBatch;
		StatBatch g_CareerBatch(UnlockEverything::careerStats);
		StatBatch g_AwardBatch(UnlockEverything::awardStats);
	}

	namespace UnlockCenter
	{
		Progress GetLimitedProgress()
		{
			return g_LimitedBatch.GetProgress();
		}

		Progress GetCareerProgress()
		{
			return g_CareerBatch.GetProgress();
		}

		Progress GetAwardProgress()
		{
			return g_AwardBatch.GetProgress();
		}
	}

	class UnlockLimitedItems final : public Command
	{
		using Command::Command;

		void OnCall() override
		{
			g_LimitedBatch.Process();
		}
	};

	class UnlockVehiclePaints final : public Command
	{
		using Command::Command;

		void OnCall() override
		{
			if (!RequireUnlockSession("载具涂装解锁"))
				return;

			for (int index = 0; index <= 20; index++)
				Stats::SetInt(std::format("MPPLY_XMASLIVERIES{}", index), -1);
			Stats::SetInt("MPPLY_XMAS22CPAINT0", -1);
			Stats::SetInt("MPPLY_XMAS22CPAINT1", -1);
			Stats::SetInt("MPPLY_SUM23WHEELCPAINT0", -1);
			Stats::SetInt("MPPLY_SUM23WHEELCPAINT1", -1);
			Notifications::Show("载具涂装解锁", "节日涂装与特殊轮毂颜色已解锁。", NotificationType::Success);
		}
	};

	class SkipBusinessIntros final : public Command
	{
		using Command::Command;

		void OnCall() override
		{
			if (!RequireUnlockSession("跳过产业引导"))
				return;

			static constexpr std::array packedBools = {
			    18139,
			    22106,
			    22080,
			    28155,
			    22193,
			    42038,
			    9537,
			    9540,
			    9539,
			    28310,
			    28256,
			    27090,
			    27089,
			    25008,
			    25009,
			    31737,
			    54768,
			};

			for (const int index : packedBools)
				Stats::SetPackedBool(index, true);
			Notifications::Show("跳过产业引导", "17 项产业与场所引导已标记为完成。", NotificationType::Success);
		}
	};

	class ProcessCareerUnlocks final : public Command
	{
		using Command::Command;

		void OnCall() override
		{
			g_CareerBatch.Process(kCareerBatchSize, "职业进度解锁");
		}
	};

	class ProcessAwardUnlock final : public Command
	{
		using Command::Command;

		void OnCall() override
		{
			g_AwardBatch.Process(1, "奖章解锁");
		}
	};

	static IntCommand _AchievementId{"achievementid", "成就编号", "选择要解锁的平台成就编号（1-77）。", 1, 77, 1};

	class UnlockAchievement final : public Command
	{
		using Command::Command;

		void OnCall() override
		{
			if (!RequireUnlockSession("平台成就解锁"))
				return;

			const int achievementId = _AchievementId.GetState();
			if (achievementId < 1 || achievementId > 77)
			{
				Notifications::Show("平台成就解锁", "成就编号必须位于 1 到 77。", NotificationType::Error);
				return;
			}

			if (PLAYER::GIVE_ACHIEVEMENT_TO_PLAYER(achievementId))
				Notifications::Show("平台成就解锁", std::format("成就 {} 的解锁请求已提交。", achievementId), NotificationType::Success);
			else
				Notifications::Show("平台成就解锁", "平台未接受该成就请求。", NotificationType::Warning);
		}
	};

	class ResetUnlockProgress final : public Command
	{
		using Command::Command;

		void OnCall() override
		{
			if (g_LimitedBatch.GetProgress().Running || g_CareerBatch.GetProgress().Running || g_AwardBatch.GetProgress().Running)
			{
				Notifications::Show("重置解锁进度", "当前仍有批次在处理，请稍后重试。", NotificationType::Warning);
				return;
			}

			const bool limitedReset = g_LimitedBatch.Reset();
			const bool careerReset = g_CareerBatch.Reset("重置解锁进度");
			const bool awardReset = g_AwardBatch.Reset("重置解锁进度");
			if (limitedReset && careerReset && awardReset)
				Notifications::Show("重置解锁进度", "批处理游标已重置，下次会重新扫描。", NotificationType::Success);
		}
	};

	static UnlockLimitedItems _UnlockLimitedItems{"unlocklimiteditems", "处理下一批限时物品", "每次最多处理 200 项限时服装、收藏品及相关打包解锁。"};
	static UnlockVehiclePaints _UnlockVehiclePaints{"unlockvehiclepaints", "解锁特殊载具涂装", "解锁节日载具涂装和特殊轮毂颜色。"};
	static SkipBusinessIntros _SkipBusinessIntros{"skipbusinessintros", "跳过产业与场所引导", "跳过设施、夜总会、游戏厅、改装铺等 17 项引导流程。"};
	static ProcessCareerUnlocks _ProcessCareerUnlocks{"processcareerunlocks", "处理下一批职业进度", "首次扫描后，每次最多处理 10 个尚未满足的职业进度项目。"};
	static ProcessAwardUnlock _ProcessAwardUnlock{"processawardunlock", "处理下一项奖章", "首次扫描后，每次只处理一个尚未满足的奖章项目。"};
	static UnlockAchievement _UnlockAchievement{"unlockachievement", "解锁所选平台成就", "提交所选编号的平台成就解锁请求。"};
	static ResetUnlockProgress _ResetUnlockProgress{"resetunlockprogress", "重置批处理进度", "清除本次运行的批处理游标，并在下次操作时重新扫描。"};
}
