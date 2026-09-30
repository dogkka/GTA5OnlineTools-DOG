#pragma once
#include "core/filemgr/FileMgr.hpp"

#include <map>

namespace YimMenu::Outfit
{
	struct ComponentData
	{
		std::string label;
		int drawableId = 0;
		int textureId = 0;
		int palette = -1;
		int maxDrawableId = 0;
		int maxTextureId = 0;
		bool present = false;
	};

	struct OutfitComponents
	{
		std::map<int, ComponentData> items = {
		    {1, {"面具"}},
		    {2, {"发型"}},
		    {3, {"躯干"}},
		    {4, {"裤装"}},
		    {5, {"包袋"}},
		    {6, {"鞋子"}},
		    {7, {"配饰"}},
		    {8, {"内搭"}},
		    {9, {"护甲"}},
		    {10, {"贴花"}},
		    {11, {"上衣"}}};
	};

	struct OutfitProps
	{
		std::map<int, ComponentData> items = {
		    {0, {"帽子"}},
		    {1, {"眼镜"}},
		    {2, {"耳饰"}},
		    {6, {"手表"}},
		    {7, {"手链"}}};
	};

	class OutfitEditor final
	{
	public:
		static void CheckBoundsDrawable(ComponentData& item, int lower);
		static void CheckBoundsTexture(ComponentData& item, int lower);
		static void SetSelfOutfit(const OutfitComponents& components, const OutfitProps& props, bool applyHair);
		static void RandomizeOutfit();

		static Folder CheckFolder(const std::string& folderName = {});
		static bool ApplyOutfitFromJson(const std::string& folderName, const std::string& fileName, bool applyHair);
		static bool SaveOutfit(std::string fileName, const std::string& folderName);
		static bool RefreshList(const std::string& folderName, std::vector<std::string>& folders, std::vector<std::string>& files);
	};
}
