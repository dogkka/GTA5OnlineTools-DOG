#include "core/commands/LoopedCommand.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/Self.hpp"
#include "game/pointers/Pointers.hpp"

namespace YimMenu::Features
{
	// resolved dynamically on the GTA5 Enhanced build (2026-09)
	// CPed + 0x10B8 -> CPedWeaponManager
	// CPedWeaponManager + 0x20 -> CWeaponInfo
	static constexpr uintptr_t OFF_WEAPON_MANAGER   = 0x10B8;
	static constexpr uintptr_t OFF_WEAPON_INFO      = 0x20;
	static constexpr uintptr_t OFF_ACCURACY_SPREAD  = 0x74;
	static constexpr uintptr_t OFF_BULLET_SPEED     = 0x11C;
	static constexpr uintptr_t OFF_RECOIL_HASH      = 0x2E4;
	static constexpr uintptr_t OFF_RECOIL_HASH_FP   = 0x2E8;
	static constexpr uintptr_t OFF_RECOIL_MIN_TIME  = 0x2EC;

	static void* GetCurrentWeaponInfo()
	{
		auto ped = Self::GetPed();
		if (!ped)
			return nullptr;

		void* ped_ptr = Pointers.HandleToPtr(ped.GetHandle());
		if (!ped_ptr)
			return nullptr;

		void* weapon_mgr = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(ped_ptr) + OFF_WEAPON_MANAGER);
		if (!weapon_mgr)
			return nullptr;

		return *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(weapon_mgr) + OFF_WEAPON_INFO);
	}

	class NoRecoil : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		void* m_WeaponInfo  = nullptr;
		uint32_t m_OgHash   = 0;
		uint32_t m_OgHashFp = 0;
		uint32_t m_OgMinTime = 0;

		void Restore()
		{
			if (!m_WeaponInfo)
				return;

			*reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(m_WeaponInfo) + OFF_RECOIL_HASH)     = m_OgHash;
			*reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(m_WeaponInfo) + OFF_RECOIL_HASH_FP)  = m_OgHashFp;
			*reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(m_WeaponInfo) + OFF_RECOIL_MIN_TIME) = m_OgMinTime;
			m_WeaponInfo = nullptr;
		}

		virtual void OnTick() override
		{
			void* wi = GetCurrentWeaponInfo();
			if (!wi)
				return;

			if (wi != m_WeaponInfo)
			{
				Restore();
				m_WeaponInfo = wi;
				m_OgHash     = *reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(wi) + OFF_RECOIL_HASH);
				m_OgHashFp   = *reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(wi) + OFF_RECOIL_HASH_FP);
				m_OgMinTime  = *reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(wi) + OFF_RECOIL_MIN_TIME);
			}

			*reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(wi) + OFF_RECOIL_HASH)     = 0;
			*reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(wi) + OFF_RECOIL_HASH_FP)  = 0;
			*reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(wi) + OFF_RECOIL_MIN_TIME) = 0;
		}

		virtual void OnDisable() override
		{
			Restore();
		}
	};

	class NoSpread : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		void* m_WeaponInfo = nullptr;
		float m_OgSpread   = 0.0f;

		void Restore()
		{
			if (!m_WeaponInfo)
				return;

			*reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(m_WeaponInfo) + OFF_ACCURACY_SPREAD) = m_OgSpread;
			m_WeaponInfo = nullptr;
		}

		virtual void OnTick() override
		{
			void* wi = GetCurrentWeaponInfo();
			if (!wi)
				return;

			if (wi != m_WeaponInfo)
			{
				Restore();
				m_WeaponInfo = wi;
				m_OgSpread   = *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(wi) + OFF_ACCURACY_SPREAD);
			}

			*reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(wi) + OFF_ACCURACY_SPREAD) = 0.0f;
		}

		virtual void OnDisable() override
		{
			Restore();
		}
	};

	class InfiniteRange : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		static constexpr float s_MaxSpeed = 9999999.0f;

		void* m_WeaponInfo = nullptr;
		float m_OgSpeed    = 0.0f;

		void Restore()
		{
			if (!m_WeaponInfo)
				return;

			*reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(m_WeaponInfo) + OFF_BULLET_SPEED) = m_OgSpeed;
			m_WeaponInfo = nullptr;
		}

		virtual void OnTick() override
		{
			void* wi = GetCurrentWeaponInfo();
			if (!wi)
				return;

			if (wi != m_WeaponInfo)
			{
				Restore();
				m_WeaponInfo = wi;
				m_OgSpeed    = *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(wi) + OFF_BULLET_SPEED);
			}

			*reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(wi) + OFF_BULLET_SPEED) = s_MaxSpeed;
		}

		virtual void OnDisable() override
		{
			Restore();
		}
	};

	static NoRecoil _NoRecoil{"norecoil", "无后坐力", "清除开火抖动（基于实测偏移，对当前武器生效）"};
	static NoSpread _NoSpread{"nospread", "无扩散", "弹道散布清零（对当前武器生效）"};
	static InfiniteRange _InfiniteRange{"infiniterange", "无限射程", "子弹速度拉满（对当前武器生效）"};
}
