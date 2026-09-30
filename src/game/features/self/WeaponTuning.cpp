#include "core/commands/FloatCommand.hpp"
#include "core/commands/IntCommand.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/pointers/Pointers.hpp"

namespace YimMenu::Features
{
	// verified live on GTA5 Enhanced (2026-09)
	static constexpr uintptr_t OFF_CLIP_SIZE      = 0x70;
	static constexpr uintptr_t OFF_DAMAGE         = 0xB0;
	static constexpr uintptr_t OFF_TIME_BETWEEN   = 0x13C;
	static constexpr uintptr_t OFF_WEAPON_RANGE   = 0x28C;

	static void* GetWeaponInfo()
	{
		auto ped = Self::GetPed();
		if (!ped)
			return nullptr;

		void* ped_ptr = Pointers.HandleToPtr(ped.GetHandle());
		if (!ped_ptr)
			return nullptr;

		void* weapon_mgr = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(ped_ptr) + 0x10B8);
		if (!weapon_mgr)
			return nullptr;

		return *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(weapon_mgr) + 0x20);
	}

	static IntCommand _BigClipSize{"bigclipsize", "弹匣容量", "覆盖后的弹匣容量", 10, 9999, 999};

	class BigClip : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		void* m_WeaponInfo = nullptr;
		uint32_t m_OgClip  = 0;

		void Restore()
		{
			if (!m_WeaponInfo)
				return;

			*reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(m_WeaponInfo) + OFF_CLIP_SIZE) = m_OgClip;
			m_WeaponInfo = nullptr;
		}

		virtual void OnTick() override
		{
			void* wi = GetWeaponInfo();
			if (!wi)
				return;

			if (wi != m_WeaponInfo)
			{
				Restore();
				m_WeaponInfo = wi;
				m_OgClip     = *reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(wi) + OFF_CLIP_SIZE);
			}

			*reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(wi) + OFF_CLIP_SIZE) = static_cast<uint32_t>(_BigClipSize.GetState());
		}

		virtual void OnDisable() override
		{
			Restore();
		}
	};

	static FloatCommand _DamageMultiplier{"damagemultiplier", "伤害倍率", "武器伤害的放大倍数", 1.0f, 100.0f, 5.0f};

	class DamageMultiplier : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		void* m_WeaponInfo = nullptr;
		float m_OgDamage   = 0.0f;

		void Restore()
		{
			if (!m_WeaponInfo)
				return;

			*reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(m_WeaponInfo) + OFF_DAMAGE) = m_OgDamage;
			m_WeaponInfo = nullptr;
		}

		virtual void OnTick() override
		{
			void* wi = GetWeaponInfo();
			if (!wi)
				return;

			if (wi != m_WeaponInfo)
			{
				Restore();
				m_WeaponInfo = wi;
				m_OgDamage   = *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(wi) + OFF_DAMAGE);
			}

			*reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(wi) + OFF_DAMAGE) = m_OgDamage * _DamageMultiplier.GetState();
		}

		virtual void OnDisable() override
		{
			Restore();
		}
	};

	static FloatCommand _FireRateMultiplier{"firerate", "射速倍率", "武器射速的放大倍数", 1.0f, 50.0f, 5.0f};

	class FastFire : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		void* m_WeaponInfo    = nullptr;
		float m_OgTimeBetween = 0.0f;

		void Restore()
		{
			if (!m_WeaponInfo)
				return;

			*reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(m_WeaponInfo) + OFF_TIME_BETWEEN) = m_OgTimeBetween;
			m_WeaponInfo = nullptr;
		}

		virtual void OnTick() override
		{
			void* wi = GetWeaponInfo();
			if (!wi)
				return;

			if (wi != m_WeaponInfo)
			{
				Restore();
				m_WeaponInfo    = wi;
				m_OgTimeBetween = *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(wi) + OFF_TIME_BETWEEN);
			}

			const float multiplier = _FireRateMultiplier.GetState();
			const float new_time   = m_OgTimeBetween > 0.0f ? m_OgTimeBetween / multiplier : 0.0f;
			*reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(wi) + OFF_TIME_BETWEEN) = new_time;
		}

		virtual void OnDisable() override
		{
			Restore();
		}
	};

	static FloatCommand _RangeValue{"weaponrange", "射程覆盖", "武器射程（米）", 10.0f, 3000.0f, 500.0f};

	class LongRange : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		void* m_WeaponInfo = nullptr;
		float m_OgRange    = 0.0f;

		void Restore()
		{
			if (!m_WeaponInfo)
				return;

			*reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(m_WeaponInfo) + OFF_WEAPON_RANGE) = m_OgRange;
			m_WeaponInfo = nullptr;
		}

		virtual void OnTick() override
		{
			void* wi = GetWeaponInfo();
			if (!wi)
				return;

			if (wi != m_WeaponInfo)
			{
				Restore();
				m_WeaponInfo = wi;
				m_OgRange    = *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(wi) + OFF_WEAPON_RANGE);
			}

			*reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(wi) + OFF_WEAPON_RANGE) = _RangeValue.GetState();
		}

		virtual void OnDisable() override
		{
			Restore();
		}
	};

	static BigClip _BigClip{"bigclip", "大容量弹匣", "重新装弹后弹匣容量变为设定值"};
	static DamageMultiplier _DamageMultiplierFeature{"damageboost", "伤害强化", "按倍数放大武器伤害"};
	static FastFire _FastFire{"fastfire", "极速射击", "按倍数缩短射击间隔"};
	static LongRange _LongRange{"longrange", "超远射程", "把武器射程覆盖为设定值"};
}
