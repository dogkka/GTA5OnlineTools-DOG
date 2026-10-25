#include "OutfitEditor.hpp"

#include "core/backend/FiberPool.hpp"
#include "core/frontend/Notifications.hpp"
#include "core/util/Strings.hpp"
#include "game/backend/Outfit.hpp"
#include "game/backend/Self.hpp"
#include "game/frontend/items/Items.hpp"
#include "core/backend/FiberPool.hpp"
#include "game/backend/Outfit.hpp"
#include "core/frontend/Notifications.hpp"
#include "game/backend/Self.hpp"
#include "game/gta/Natives.hpp"

#include "misc/cpp/imgui_stdlib.h"

#include <algorithm>
#include <cctype>
#include <mutex>

namespace YimMenu
{
	class OutfitEditorMenu final
	{
		Outfit::OutfitComponents m_Components{};
		Outfit::OutfitProps m_Props{};
		std::vector<std::string> m_Folders{};
		std::vector<std::string> m_Files{};
		std::string m_Folder{};
		std::string m_File{};
		char m_OutfitName[64]{};
		char m_NewFolder[50]{};
		bool m_ApplyHair = false;
		bool m_Initialized = false;
		std::mutex m_Mutex;

		void RefreshStats()
		{
			auto self = Self::GetPed();
			if (!self)
				return;

			Outfit::OutfitComponents components;
			Outfit::OutfitProps props;
			const auto ped = self.GetHandle();

			for (auto& [id, item] : components.items)
			{
				item.drawableId = PED::GET_PED_DRAWABLE_VARIATION(ped, id);
				item.maxDrawableId = std::max(0, PED::GET_NUMBER_OF_PED_DRAWABLE_VARIATIONS(ped, id) - 1);
				item.textureId = PED::GET_PED_TEXTURE_VARIATION(ped, id);
				item.maxTextureId = std::max(0, PED::GET_NUMBER_OF_PED_TEXTURE_VARIATIONS(ped, id, item.drawableId) - 1);
			}

			for (auto& [id, item] : props.items)
			{
				item.drawableId = PED::GET_PED_PROP_INDEX(ped, id, 0);
				item.maxDrawableId = std::max(0, PED::GET_NUMBER_OF_PED_PROP_DRAWABLE_VARIATIONS(ped, id) - 1);
				item.textureId = item.drawableId < 0 ? 0 : PED::GET_PED_PROP_TEXTURE_INDEX(ped, id);
				item.maxTextureId = item.drawableId < 0 ? 0 : std::max(0, PED::GET_NUMBER_OF_PED_PROP_TEXTURE_VARIATIONS(ped, id, item.drawableId) - 1);
			}

			std::scoped_lock lock(m_Mutex);
			m_Components = std::move(components);
			m_Props = std::move(props);
		}

		void RefreshFiles(const std::string& selectedFolder)
		{
			std::vector<std::string> folders;
			std::vector<std::string> files;
			if (!Outfit::OutfitEditor::RefreshList(selectedFolder, folders, files))
				return;

			std::scoped_lock lock(m_Mutex);
			m_Folders = std::move(folders);
			if (m_Folder == selectedFolder)
				m_Files = std::move(files);
		}

		void EnsureInitialized()
		{
			if (m_Initialized)
				return;

			m_Initialized = true;
			FiberPool::Push([this] {
				RefreshStats();
				RefreshFiles({});
			});
		}

		void RenderComponents()
		{
			std::scoped_lock lock(m_Mutex);
			ImGui::TextUnformatted("服装组件");
			for (auto& [id, item] : m_Components.items)
			{
				ImGui::PushID(id);
				ImGui::TextUnformatted(item.label.c_str());
				ImGui::SameLine(68.0f);
				ImGui::SetNextItemWidth(105.0f);
				if (ImGui::InputInt("##模型", &item.drawableId))
				{
					Outfit::OutfitEditor::CheckBoundsDrawable(item, 0);
					item.textureId = 0;
					const auto selected = item;
					FiberPool::Push([this, id, selected] {
						auto self = Self::GetPed();
						if (self)
							PED::SET_PED_COMPONENT_VARIATION(self.GetHandle(), id, selected.drawableId, 0, PED::GET_PED_PALETTE_VARIATION(self.GetHandle(), id));
						RefreshStats();
					});
				}
				ImGui::SameLine();
				ImGui::SetNextItemWidth(105.0f);
				if (ImGui::InputInt("##纹理", &item.textureId))
				{
					Outfit::OutfitEditor::CheckBoundsTexture(item, 0);
					const auto selected = item;
					FiberPool::Push([this, id, selected] {
						auto self = Self::GetPed();
						if (self)
							PED::SET_PED_COMPONENT_VARIATION(self.GetHandle(), id, selected.drawableId, selected.textureId, PED::GET_PED_PALETTE_VARIATION(self.GetHandle(), id));
						RefreshStats();
					});
				}
				ImGui::PopID();
			}
		}

		void RenderProps()
		{
			std::scoped_lock lock(m_Mutex);
			ImGui::TextUnformatted("配件");
			for (auto& [id, item] : m_Props.items)
			{
				ImGui::PushID(id + 100);
				ImGui::TextUnformatted(item.label.c_str());
				ImGui::SameLine(68.0f);
				ImGui::SetNextItemWidth(105.0f);
				if (ImGui::InputInt("##模型", &item.drawableId))
				{
					Outfit::OutfitEditor::CheckBoundsDrawable(item, -1);
					item.textureId = 0;
					const auto selected = item;
					FiberPool::Push([this, id, selected] {
						auto self = Self::GetPed();
						if (self && selected.drawableId < 0)
							PED::CLEAR_PED_PROP(self.GetHandle(), id, 1);
						else if (self)
							PED::SET_PED_PROP_INDEX(self.GetHandle(), id, selected.drawableId, 0, TRUE, 0);
						RefreshStats();
					});
				}
				ImGui::SameLine();
				ImGui::SetNextItemWidth(105.0f);
				if (ImGui::InputInt("##纹理", &item.textureId))
				{
					Outfit::OutfitEditor::CheckBoundsTexture(item, 0);
					const auto selected = item;
					FiberPool::Push([this, id, selected] {
						auto self = Self::GetPed();
						if (self && selected.drawableId >= 0)
							PED::SET_PED_PROP_INDEX(self.GetHandle(), id, selected.drawableId, selected.textureId, TRUE, 0);
						RefreshStats();
					});
				}
				ImGui::PopID();
			}
		}

		void RenderSavedOutfits()
		{
			std::scoped_lock lock(m_Mutex);

			ImGui::SetNextItemWidth(300.0f);
			if (ImGui::BeginCombo("文件夹", m_Folder.empty() ? "根目录" : m_Folder.c_str()))
			{
				if (ImGui::Selectable("根目录", m_Folder.empty()))
				{
					m_Folder.clear();
					m_File.clear();
					FiberPool::Push([this] {
						RefreshFiles({});
					});
				}

				for (const auto& folderName : m_Folders)
				{
					if (ImGui::Selectable(folderName.c_str(), m_Folder == folderName))
					{
						m_Folder = folderName;
						m_File.clear();
						FiberPool::Push([this, folderName] {
							RefreshFiles(folderName);
						});
					}
				}
				ImGui::EndCombo();
			}

			static std::string search;
			ImGui::SetNextItemWidth(300.0f);
			if (ImGui::InputTextWithHint("##服装搜索", "搜索已保存服装", &search))
			{
				std::ranges::transform(search, search.begin(), [](unsigned char character) {
					return static_cast<char>(std::tolower(character));
				});
			}

			if (ImGui::BeginListBox("##已保存服装", ImVec2(300.0f, 260.0f)))
			{
				for (const auto& fileName : m_Files)
				{
					auto lowerName = fileName;
					std::ranges::transform(lowerName, lowerName.begin(), [](unsigned char character) {
						return static_cast<char>(std::tolower(character));
					});
					if (lowerName.contains(search) && ImGui::Selectable(fileName.c_str(), m_File == fileName))
						m_File = fileName;
				}
				ImGui::EndListBox();
			}

			ImGui::SameLine();
			ImGui::BeginGroup();
			if (ImGui::Button("刷新列表"))
			{
				const auto selectedFolder = m_Folder;
				FiberPool::Push([this, selectedFolder] {
					RefreshFiles(selectedFolder);
				});
			}

			ImGui::Checkbox("应用发型", &m_ApplyHair);
			if (ImGui::Button("应用所选服装"))
			{
				const auto selectedFolder = m_Folder;
				const auto selectedFile = m_File;
				const auto applyHair = m_ApplyHair;
				m_ApplyHair = false;
				FiberPool::Push([this, selectedFolder, selectedFile, applyHair] {
					if (Outfit::OutfitEditor::ApplyOutfitFromJson(selectedFolder, selectedFile, applyHair))
						RefreshStats();
				});
			}

			ImGui::Spacing();
			ImGui::TextUnformatted("服装名称");
			ImGui::SetNextItemWidth(250.0f);
			ImGui::InputText("##服装名称", m_OutfitName, IM_ARRAYSIZE(m_OutfitName));

			if (m_Folder.empty())
			{
				ImGui::TextUnformatted("新文件夹（可选）");
				ImGui::SetNextItemWidth(250.0f);
				ImGui::InputText("##服装文件夹", m_NewFolder, IM_ARRAYSIZE(m_NewFolder));
			}

			if (ImGui::Button("保存当前服装"))
			{
				const auto fileName = TrimString(m_OutfitName);
				const auto requestedFolder = TrimString(m_NewFolder);
				const auto targetFolder = m_Folder.empty() && !requestedFolder.empty() ? requestedFolder : m_Folder;
				m_OutfitName[0] = '\0';

				if (fileName.empty())
				{
					Notifications::Show("服装编辑器", "服装名称不能为空。", NotificationType::Warning);
				}
				else
				{
					FiberPool::Push([this, fileName, targetFolder] {
						if (!Outfit::OutfitEditor::SaveOutfit(fileName, targetFolder))
							return;

						{
							std::scoped_lock stateLock(m_Mutex);
							m_Folder = targetFolder;
							m_NewFolder[0] = '\0';
						}
						RefreshFiles(targetFolder);
					});
				}
			}
			ImGui::EndGroup();
		}

	public:
		void Draw()
		{
			EnsureInitialized();

			if (ImGui::Button("刷新当前数据"))
			{
				FiberPool::Push([this] {
					RefreshStats();
				});
			}
			ImGui::SameLine();
			if (ImGui::Button("随机服装"))
			{
				FiberPool::Push([this] {
					Outfit::OutfitEditor::RandomizeOutfit();
					RefreshStats();
				});
			}

			ImGui::Columns(2, "服装编辑列", false);
			RenderComponents();
			ImGui::NextColumn();
			RenderProps();
			ImGui::Columns(1);

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
			RenderSavedOutfits();
		}
	};

	std::shared_ptr<Category> CreateOutfitsMenu()
	{
		static OutfitEditorMenu editor;
		auto category = std::make_shared<Category>("服装编辑器");
		category->AddItem(std::make_shared<ImGuiItem>([] {
			if (!NativeInvoker::AreHandlersCached())
				return ImGui::TextDisabled("Native 尚未缓存。");
			if (!Self::GetPed())
				return ImGui::TextDisabled("未找到玩家角色。");

			editor.Draw();
		}));
		return category;
	}
}
