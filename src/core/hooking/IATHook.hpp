#pragma once
#include "BaseHook.hpp"
#include "memory/Module.hpp"
#include "memory/PointerCalculator.hpp"

namespace YimMenu
{
	template<typename T = int*>
	class IATHook : public BaseHook
	{
	private:
		void** m_HookLocation;
		void* m_OriginalFunc;
		void* m_HookFunc;

	public:
		IATHook(const std::string_view name, Module* module, const std::string_view library, const std::string_view import, T detour);
		virtual ~IATHook() override;

		bool Enable() override;
		bool Disable() override;

		T Original() const;
	};

	template<typename T>
	inline IATHook<T>::IATHook(const std::string_view name, Module* module, const std::string_view library, const std::string_view import, T detour) :
	    BaseHook(name)
	{
		m_HookLocation = module ? module->GetImport(library, import) : nullptr;
		if (!m_HookLocation)
		{
			LOGF(FATAL, "Failed to find IAT entry for hook {}", name);
			throw std::runtime_error("Failed to find IAT entry");
		}
		m_OriginalFunc = *m_HookLocation;
		m_HookFunc = (void*)(detour);
	}

	template<typename T>
	inline IATHook<T>::~IATHook()
	{
		Disable();
	}

	template<typename T>
	inline bool IATHook<T>::Enable()
	{
		if (m_Enabled)
			return false;

		DWORD oldProtect{};
		if (!VirtualProtect(m_HookLocation, sizeof(void*), PAGE_READWRITE, &oldProtect))
		{
			LOGF(FATAL, "Failed to make IAT entry writable for hook {}: {}", Name(), GetLastError());
			return false;
		}

		*m_HookLocation = m_HookFunc;
		m_Enabled = true;

		DWORD currentProtect{};
		if (!VirtualProtect(m_HookLocation, sizeof(void*), oldProtect, &currentProtect))
		{
			LOGF(FATAL, "Failed to restore IAT protection for hook {}: {}", Name(), GetLastError());
			return false;
		}

		return true;
	}

	template<typename T>
	inline bool IATHook<T>::Disable()
	{
		if (!m_Enabled)
			return false;

		DWORD oldProtect{};
		if (!VirtualProtect(m_HookLocation, sizeof(void*), PAGE_READWRITE, &oldProtect))
		{
			LOGF(FATAL, "Failed to make IAT entry writable while disabling hook {}: {}", Name(), GetLastError());
			return false;
		}

		*m_HookLocation = m_OriginalFunc;
		m_Enabled = false;

		DWORD currentProtect{};
		if (!VirtualProtect(m_HookLocation, sizeof(void*), oldProtect, &currentProtect))
		{
			LOGF(FATAL, "Failed to restore IAT protection while disabling hook {}: {}", Name(), GetLastError());
			return false;
		}

		return true;
	}

	template<typename T>
	inline T IATHook<T>::Original() const
	{
		return reinterpret_cast<T>(m_OriginalFunc);
	}
}
