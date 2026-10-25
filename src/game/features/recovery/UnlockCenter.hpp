#pragma once

#include <cstddef>

namespace YimMenu::Features::UnlockCenter
{
	struct Progress
	{
		std::size_t Processed;
		std::size_t Total;
		bool Initialized;
		bool Running;
	};

	Progress GetLimitedProgress();
	Progress GetCareerProgress();
	Progress GetAwardProgress();
}
