#include "core/commands/BoolCommand.hpp"

namespace YimMenu::Features
{
	static BoolCommand _ScriptEventProtection{"scripteventprotection", "脚本事件防护", "拦截恶意的脚本事件攻击（声音轰炸、假通知、强制传送、踢出载具等）", true};
}
