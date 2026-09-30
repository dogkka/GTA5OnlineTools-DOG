#include "Outfit.hpp"

#include "core/frontend/Notifications.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

#include <algorithm>
#include <fstream>
#include <random>

namespace
{
	constexpr std::size_t kMaxOutfitEntries = 1000;
	constexpr std::uintmax_t kMaxOutfitFileSize = 1024 * 1024;

	bool IsSafePathComponent(std::string_view value, bool allowEmpty)
	{
		if (value.empty())
			return allowEmpty;
		if (value == "." || value == ".." || value.contains(".."))
			return false;
		if (value.find_first_of("<>:\"/\\|?*") != std::string_view::npos)
			return false;
		if (value.back() == ' ' || value.back() == '.')
			return false;

		return std::ranges::none_of(value, [](unsigned char character) {
			return character < 0x20;
		});
	}

	template<typename Items>
	bool LoadItems(const nlohmann::json& source, Items& destination)
	{
		if (!source.is_object())
			return false;

		bool loadedAny = false;
		for (auto& [id, item] : destination)
		{
			const auto entry = source.find(std::to_string(id));
			if (entry == source.end())
				continue;
			if (!entry->is_object() || !entry->contains("drawable_id") || !entry->contains("texture_id")
			    || !(*entry)["drawable_id"].is_number_integer() || !(*entry)["texture_id"].is_number_integer())
				return false;

			item.drawableId = (*entry)["drawable_id"].template get<int>();
			item.textureId = (*entry)["texture_id"].template get<int>();
			if (entry->contains("palette_var") && (*entry)["palette_var"].is_number_integer())
				item.palette = (*entry)["palette_var"].template get<int>();
			item.present = true;
			loadedAny = true;
		}

		return loadedAny;
	}
}

namespace YimMenu::Outfit
{
	void OutfitEditor::CheckBoundsDrawable(ComponentData& item, int lower)
	{
		item.drawableId = std::clamp(item.drawableId, lower, std::max(lower, item.maxDrawableId));
	}

	void OutfitEditor::CheckBoundsTexture(ComponentData& item, int lower)
	{
		item.textureId = std::clamp(item.textureId, lower, std::max(lower, item.maxTextureId));
	}

	void OutfitEditor::SetSelfOutfit(const OutfitComponents& components, const OutfitProps& props, bool applyHair)
	{
		auto self = Self::GetPed();
		if (!self)
			return;

		const auto ped = self.GetHandle();
		for (const auto& [id, savedItem] : components.items)
		{
			if (!savedItem.present || (!applyHair && id == 2))
				continue;

			const auto maxDrawable = PED::GET_NUMBER_OF_PED_DRAWABLE_VARIATIONS(ped, id) - 1;
			if (maxDrawable < 0)
				continue;

			const auto drawable = std::clamp(savedItem.drawableId, 0, maxDrawable);
			const auto maxTexture = std::max(0, PED::GET_NUMBER_OF_PED_TEXTURE_VARIATIONS(ped, id, drawable) - 1);
			const auto texture = std::clamp(savedItem.textureId, 0, maxTexture);
			const auto palette = savedItem.palette < 0 ? PED::GET_PED_PALETTE_VARIATION(ped, id) : std::clamp(savedItem.palette, 0, 3);
			PED::SET_PED_COMPONENT_VARIATION(ped, id, drawable, texture, palette);
		}

		for (const auto& [id, savedItem] : props.items)
		{
			if (!savedItem.present)
				continue;
			if (savedItem.drawableId < 0)
			{
				PED::CLEAR_PED_PROP(ped, id, 1);
				continue;
			}

			const auto maxDrawable = PED::GET_NUMBER_OF_PED_PROP_DRAWABLE_VARIATIONS(ped, id) - 1;
			if (maxDrawable < 0)
				continue;

			const auto drawable = std::clamp(savedItem.drawableId, 0, maxDrawable);
			const auto maxTexture = std::max(0, PED::GET_NUMBER_OF_PED_PROP_TEXTURE_VARIATIONS(ped, id, drawable) - 1);
			const auto texture = std::clamp(savedItem.textureId, 0, maxTexture);
			PED::SET_PED_PROP_INDEX(ped, id, drawable, texture, TRUE, 0);
		}
	}

	void OutfitEditor::RandomizeOutfit()
	{
		auto self = Self::GetPed();
		if (!self)
			return;

		const auto ped = self.GetHandle();
		std::random_device randomDevice;
		std::mt19937 generator(randomDevice());

		for (const auto& [id, _] : OutfitComponents{}.items)
		{
			const auto maxDrawable = PED::GET_NUMBER_OF_PED_DRAWABLE_VARIATIONS(ped, id) - 1;
			if (maxDrawable < 0)
				continue;

			const auto drawable = std::uniform_int_distribution<>(0, maxDrawable)(generator);
			const auto maxTexture = std::max(0, PED::GET_NUMBER_OF_PED_TEXTURE_VARIATIONS(ped, id, drawable) - 1);
			const auto texture = std::uniform_int_distribution<>(0, maxTexture)(generator);
			PED::SET_PED_COMPONENT_VARIATION(ped, id, drawable, texture, PED::GET_PED_PALETTE_VARIATION(ped, id));
		}

		for (const auto& [id, _] : OutfitProps{}.items)
		{
			const auto maxDrawable = PED::GET_NUMBER_OF_PED_PROP_DRAWABLE_VARIATIONS(ped, id) - 1;
			if (maxDrawable < 0)
				continue;

			const auto drawable = std::uniform_int_distribution<>(0, maxDrawable)(generator);
			const auto maxTexture = std::max(0, PED::GET_NUMBER_OF_PED_PROP_TEXTURE_VARIATIONS(ped, id, drawable) - 1);
			const auto texture = std::uniform_int_distribution<>(0, maxTexture)(generator);
			PED::SET_PED_PROP_INDEX(ped, id, drawable, texture, TRUE, 0);
		}
	}

	Folder OutfitEditor::CheckFolder(const std::string& folderName)
	{
		if (!IsSafePathComponent(folderName, true))
			throw std::invalid_argument("服装文件夹名称无效。");

		return FileMgr::GetProjectFolder(std::filesystem::path("saved_outfits") / folderName);
	}

	bool OutfitEditor::ApplyOutfitFromJson(const std::string& folderName, const std::string& fileName, bool applyHair)
	{
		if (!IsSafePathComponent(fileName, false))
		{
			Notifications::Show("服装编辑器", "服装文件名无效。", NotificationType::Error);
			return false;
		}

		std::filesystem::path path;
		try
		{
			path = CheckFolder(folderName).GetFile(fileName).Path();
		}
		catch (const std::exception& exception)
		{
			LOG(WARNING) << exception.what();
			Notifications::Show("服装编辑器", "服装路径无效。", NotificationType::Error);
			return false;
		}

		std::error_code error;
		const auto size = std::filesystem::file_size(path, error);
		if (error || size > kMaxOutfitFileSize)
		{
			Notifications::Show("服装编辑器", "服装文件不存在或超过 1 MB。", NotificationType::Error);
			return false;
		}

		std::ifstream input(path, std::ios::binary);
		const auto json = nlohmann::json::parse(input, nullptr, false);
		if (json.is_discarded() || !json.is_object() || !json.contains("model") || !json["model"].is_number_integer()
		    || !json.contains("components") || !json.contains("props"))
		{
			Notifications::Show("服装编辑器", "服装文件格式无效。", NotificationType::Error);
			return false;
		}

		auto self = Self::GetPed();
		if (!self || json["model"].get<Hash>() != self.GetModel())
		{
			Notifications::Show("服装编辑器", "服装文件与当前角色模型不匹配。", NotificationType::Error);
			return false;
		}

		OutfitComponents components;
		OutfitProps props;
		if (!LoadItems(json["components"], components.items) || !LoadItems(json["props"], props.items))
		{
			Notifications::Show("服装编辑器", "服装组件数据无效。", NotificationType::Error);
			return false;
		}

		SetSelfOutfit(components, props, applyHair);
		Notifications::Show("服装编辑器", "已应用所选服装。", NotificationType::Success);
		return true;
	}

	bool OutfitEditor::SaveOutfit(std::string fileName, const std::string& folderName)
	{
		if (fileName.ends_with(".json"))
			fileName.resize(fileName.size() - 5);
		if (!IsSafePathComponent(fileName, false))
		{
			Notifications::Show("服装编辑器", "请输入有效的服装名称。", NotificationType::Warning);
			return false;
		}

		auto self = Self::GetPed();
		if (!self)
		{
			Notifications::Show("服装编辑器", "未找到玩家角色。", NotificationType::Error);
			return false;
		}

		std::filesystem::path path;
		try
		{
			path = CheckFolder(folderName).GetFile(fileName + ".json").Path();
		}
		catch (const std::exception& exception)
		{
			LOG(WARNING) << exception.what();
			Notifications::Show("服装编辑器", "服装保存路径无效。", NotificationType::Error);
			return false;
		}

		const auto ped = self.GetHandle();
		nlohmann::json components = nlohmann::json::object();
		for (const auto& [id, _] : OutfitComponents{}.items)
		{
			components[std::to_string(id)] = {
			    {"drawable_id", PED::GET_PED_DRAWABLE_VARIATION(ped, id)},
			    {"texture_id", PED::GET_PED_TEXTURE_VARIATION(ped, id)},
			    {"palette_var", PED::GET_PED_PALETTE_VARIATION(ped, id)}};
		}

		nlohmann::json props = nlohmann::json::object();
		for (const auto& [id, _] : OutfitProps{}.items)
		{
			props[std::to_string(id)] = {
			    {"drawable_id", PED::GET_PED_PROP_INDEX(ped, id, 0)},
			    {"texture_id", PED::GET_PED_PROP_TEXTURE_INDEX(ped, id)}};
		}

		nlohmann::json json = {
		    {"components", std::move(components)},
		    {"props", std::move(props)},
		    {"model", self.GetModel()}};

		auto temporaryPath = path;
		temporaryPath += ".tmp";
		std::error_code fileError;
		{
			std::ofstream output(temporaryPath, std::ios::binary | std::ios::trunc);
			if (!output)
			{
				Notifications::Show("服装编辑器", "无法创建服装文件。", NotificationType::Error);
				return false;
			}
			output << json.dump(4);
			if (!output)
			{
				output.close();
				std::filesystem::remove(temporaryPath, fileError);
				Notifications::Show("服装编辑器", "写入服装文件失败。", NotificationType::Error);
				return false;
			}
		}

		if (!MoveFileExW(temporaryPath.c_str(), path.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
		{
			std::filesystem::remove(temporaryPath, fileError);
			Notifications::Show("服装编辑器", "替换服装文件失败。", NotificationType::Error);
			return false;
		}

		Notifications::Show("服装编辑器", "服装已保存。", NotificationType::Success);
		return true;
	}

	bool OutfitEditor::RefreshList(const std::string& folderName, std::vector<std::string>& folders, std::vector<std::string>& files)
	{
		folders.clear();
		files.clear();

		try
		{
			std::error_code error;
			for (const auto& entry : std::filesystem::directory_iterator(CheckFolder().Path(), error))
			{
				if (error || folders.size() >= kMaxOutfitEntries)
					break;
				if (entry.is_directory(error) && !error)
					folders.push_back(entry.path().filename().generic_string());
			}
			if (error)
				throw std::filesystem::filesystem_error("无法读取服装文件夹", error);

			for (const auto& entry : std::filesystem::directory_iterator(CheckFolder(folderName).Path(), error))
			{
				if (error || files.size() >= kMaxOutfitEntries)
					break;
				if (entry.is_regular_file(error) && !error && entry.path().extension() == ".json")
					files.push_back(entry.path().filename().generic_string());
			}
			if (error)
				throw std::filesystem::filesystem_error("无法读取服装文件", error);
		}
		catch (const std::exception& exception)
		{
			LOG(WARNING) << exception.what();
			Notifications::Show("服装编辑器", "刷新服装列表失败。", NotificationType::Error);
			return false;
		}

		std::ranges::sort(folders);
		std::ranges::sort(files);
		return true;
	}
}
