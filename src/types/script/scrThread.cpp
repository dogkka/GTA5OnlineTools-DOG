#include "scrThread.hpp"
#include "types/rage/tlsContext.hpp"

namespace rage
{
	rage::scrThread* scrThread::GetRunningThread()
	{
		return rage::tlsContext::Get()->m_CurrentScriptThread;
	}
}