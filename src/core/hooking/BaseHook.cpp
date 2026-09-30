#include "BaseHook.hpp"

#include <MinHook.h>

namespace YimMenu
{
	BaseHook::BaseHook(const std::string_view name) :
	    m_Name(name),
	    m_Enabled(false)
	{
	}

	std::vector<BaseHook*>& BaseHook::Hooks()
	{
		return m_Hooks;
	}

	bool BaseHook::EnableAll()
	{
		bool success = true;
		for (auto hook : m_Hooks)
			success = hook->Enable() && success;
		return success;
	}

	bool BaseHook::DisableAll()
	{
		bool success = true;
		for (auto hook : m_Hooks)
			success = hook->Disable() && success;
		return success;
	}

	void BaseHook::CommitQueuedStates(bool success)
	{
		for (auto hook : m_Hooks)
			hook->CommitQueuedState(success);
	}

	void BaseHook::ReconcileEnabledStates(bool enabled)
	{
		for (auto hook : m_Hooks)
			hook->ReconcileEnabledState(enabled);
	}
}
