#include "Self.hpp"

#include "core/util/Joaat.hpp"
#include "game/frontend/items/Items.hpp"
#include "Self/OutfitEditor.hpp"
#include "Self/Weapons.hpp"

namespace YimMenu::Submenus
{
	Self::Self() :
		#define ICON_FA_USER "\xef\x80\x87"
	    Submenu::Submenu("个人", ICON_FA_USER)
	{
		auto main = std::make_shared<Category>("基础");
		auto globalsGroup = std::make_shared<Group>("状态", -1);
		auto extraGroup = std::make_shared<Group>("辅助", 2);
		auto movementGroup = std::make_shared<Group>("移动", -1);
		auto toolsGroup = std::make_shared<Group>("日常工具", -1);
		auto tools2Group = std::make_shared<Group>("进阶工具", -1);
		auto specialAbilityGroup = std::make_shared<Group>("特殊能力");
		auto wantedGroup = std::make_shared<Group>("通缉");
		auto funGroup = std::make_shared<Group>("娱乐", -1);
		auto cameraGroup = std::make_shared<Group>("相机", -1);
		auto hudPanelGroup = std::make_shared<Group>("界面", -1);
		auto qolGroup = std::make_shared<Group>("体验优化", -1);
		auto serviceGroup = std::make_shared<Group>("请求服务", -1);

		globalsGroup->AddItem(std::make_shared<BoolCommandItem>("godmode"_J));
		globalsGroup->AddItem(std::make_shared<BoolCommandItem>("invis"_J));
		globalsGroup->AddItem(std::make_shared<ConditionalItem>("invis"_J, std::make_shared<BoolCommandItem>("localvis"_J)));
		globalsGroup->AddItem(std::make_shared<BoolCommandItem>("otr"_J));
		globalsGroup->AddItem(std::make_shared<BoolCommandItem>("noragdoll"_J));
		globalsGroup->AddItem(std::make_shared<BoolCommandItem>("noidlekick"_J));
		globalsGroup->AddItem(std::make_shared<BoolCommandItem>("unlimitedoxygen"_J));
		globalsGroup->AddItem(std::make_shared<BoolCommandItem>("formatmoney"_J));
		globalsGroup->AddItem(std::make_shared<BoolCommandItem>("mobileradio"_J));
		globalsGroup->AddItem(std::make_shared<BoolCommandItem>("keepplayerclean"_J));
		globalsGroup->AddItem(std::make_shared<BoolCommandItem>("passivemode"_J));

		extraGroup->AddItem(std::make_shared<BoolCommandItem>("disablecriticalhits"_J));
		extraGroup->AddItem(std::make_shared<BoolCommandItem>("nofalldamage"_J));
		extraGroup->AddItem(std::make_shared<BoolCommandItem>("nightvision"_J));
		extraGroup->AddItem(std::make_shared<BoolCommandItem>("thermalvision"_J));
		extraGroup->AddItem(std::make_shared<BoolCommandItem>("supermelee"_J));
		extraGroup->AddItem(std::make_shared<ConditionalItem>("supermelee"_J, std::make_shared<IntCommandItem>("supermeleepower"_J, "力度")));
		extraGroup->AddItem(std::make_shared<BoolCommandItem>("mentalstate"_J));
		extraGroup->AddItem(std::make_shared<ConditionalItem>("mentalstate"_J, std::make_shared<IntCommandItem>("mentalstateset"_J, "精神值")));

		toolsGroup->AddItem(std::make_shared<CommandItem>("skipcutscene"_J));
		toolsGroup->AddItem(std::make_shared<CommandItem>("skipconversation"_J));
		toolsGroup->AddItem(std::make_shared<CommandItem>("suicide"_J));
		toolsGroup->AddItem(std::make_shared<CommandItem>("heal"_J));
		toolsGroup->AddItem(std::make_shared<CommandItem>("cleardamage"_J));
		toolsGroup->AddItem(std::make_shared<CommandItem>("fillinventory"_J));
		toolsGroup->AddItem(std::make_shared<CommandItem>("openwardrobe"_J));

		tools2Group->AddItem(std::make_shared<BoolCommandItem>("fastrespawn"_J));
		tools2Group->AddItem(std::make_shared<CommandItem>("cleanplayer"_J));
		tools2Group->AddItem(std::make_shared<BoolCommandItem>("bodyguards"_J));
		tools2Group->AddItem(std::make_shared<BoolCommandItem>("flamethrower"_J));
		tools2Group->AddItem(std::make_shared<BoolCommandItem>("orbitalstrike"_J));

		auto swapGroup = std::make_shared<Group>("变身", -1);
		swapGroup->AddItem(std::make_shared<ListCommandItem>("swapmodelselect"_J));
		swapGroup->AddItem(std::make_shared<CommandItem>("applyswapmodel"_J));
		swapGroup->AddItem(std::make_shared<CommandItem>("revertswapmodel"_J));

		auto clearWanted = std::make_shared<Group>("", 1);
		clearWanted->AddItem(std::make_shared<ConditionalItem>("neverwanted"_J, std::make_shared<CommandItem>("clearwanted"_J), true));
		clearWanted->AddItem(std::make_shared<BoolCommandItem>("neverwanted"_J));
		auto setWanted = std::make_shared<Group>("", 1);
		setWanted->AddItem(std::make_shared<IntCommandItem>("wantedslider"_J));
		setWanted->AddItem(std::make_shared<ConditionalItem>("freezewanted"_J, std::make_shared<CommandItem>("setwanted"_J), true));
		setWanted->AddItem(std::make_shared<BoolCommandItem>("freezewanted"_J));
		wantedGroup->AddItem(std::make_shared<ConditionalItem>("freezewanted"_J, clearWanted, true));
		wantedGroup->AddItem(std::make_shared<ConditionalItem>("neverwanted"_J, setWanted, true));

		movementGroup->AddItem(std::make_shared<BoolCommandItem>("standonvehicles"_J));
		movementGroup->AddItem(std::make_shared<BoolCommandItem>("disableactionmode"_J));
		movementGroup->AddItem(std::make_shared<BoolCommandItem>("superrun"_J));
		movementGroup->AddItem(std::make_shared<ConditionalItem>("superrun"_J, std::make_shared<FloatCommandItem>("moverateoverride"_J, std::nullopt, false)));
		movementGroup->AddItem(std::make_shared<ConditionalItem>("superrun"_J, std::make_shared<FloatCommandItem>("runsprintswimmultiplier"_J, std::nullopt, false)));
		movementGroup->AddItem(std::make_shared<BoolCommandItem>("superjump"_J));
		movementGroup->AddItem(std::make_shared<BoolCommandItem>("noclip"_J));
		movementGroup->AddItem(std::make_shared<ConditionalItem>("noclip"_J, std::make_shared<FloatCommandItem>("noclipspeed"_J)));
		movementGroup->AddItem(std::make_shared<BoolCommandItem>("freecam"_J));
		movementGroup->AddItem(std::make_shared<ConditionalItem>("freecam"_J, std::make_shared<FloatCommandItem>("freecamspeed"_J)));

		funGroup->AddItem(std::make_shared<BoolCommandItem>("flamewings"_J));
		funGroup->AddItem(std::make_shared<ConditionalItem>("flamewings"_J, std::make_shared<FloatCommandItem>("flamewingssize"_J)));
		funGroup->AddItem(std::make_shared<ConditionalItem>("flamewings"_J, std::make_shared<FloatCommandItem>("flamewingsspread"_J)));
		funGroup->AddItem(std::make_shared<ConditionalItem>("flamewings"_J, std::make_shared<FloatCommandItem>("flamewingsheight"_J)));
		funGroup->AddItem(std::make_shared<ConditionalItem>("flamewings"_J, std::make_shared<BoolCommandItem>("flamewingsburst"_J)));
		funGroup->AddItem(std::make_shared<BoolCommandItem>("broomflight"_J));
		funGroup->AddItem(std::make_shared<BoolCommandItem>("overheadtag"_J));
		funGroup->AddItem(std::make_shared<ConditionalItem>("overheadtag"_J, std::make_shared<StringCommandItem>("overheadtagtext"_J)));
		funGroup->AddItem(std::make_shared<ConditionalItem>("overheadtag"_J, std::make_shared<FloatCommandItem>("overheadtagsize"_J)));
		funGroup->AddItem(std::make_shared<ConditionalItem>("overheadtag"_J, std::make_shared<ListCommandItem>("overheadtagcolor"_J)));
		funGroup->AddItem(std::make_shared<ConditionalItem>("overheadtag"_J, std::make_shared<BoolCommandItem>("overheadtagglow"_J)));
		funGroup->AddItem(std::make_shared<BoolCommandItem>("ptfxtrails"_J));
		funGroup->AddItem(std::make_shared<ConditionalItem>("ptfxtrails"_J, std::make_shared<ListCommandItem>("ptfxtrailparticle"_J)));
		funGroup->AddItem(std::make_shared<ConditionalItem>("ptfxtrails"_J, std::make_shared<ListCommandItem>("ptfxtrailbone"_J)));
		funGroup->AddItem(std::make_shared<ConditionalItem>("ptfxtrails"_J, std::make_shared<FloatCommandItem>("ptfxtrailscale"_J)));
		funGroup->AddItem(std::make_shared<ConditionalItem>("ptfxtrails"_J, std::make_shared<FloatCommandItem>("ptfxtrailoffset"_J)));
		funGroup->AddItem(std::make_shared<ConditionalItem>("ptfxtrails"_J, std::make_shared<IntCommandItem>("ptfxtrailinterval"_J, "间隔")));
		funGroup->AddItem(std::make_shared<ListCommandItem>("animscenariosel"_J));
		funGroup->AddItem(std::make_shared<CommandItem>("animscenario"_J));
		funGroup->AddItem(std::make_shared<ListCommandItem>("animdancesel"_J));
		funGroup->AddItem(std::make_shared<CommandItem>("animdance"_J));
		funGroup->AddItem(std::make_shared<ListCommandItem>("animlistsel"_J));
		funGroup->AddItem(std::make_shared<CommandItem>("animplay"_J));
		funGroup->AddItem(std::make_shared<CommandItem>("animstop"_J));
		funGroup->AddItem(std::make_shared<BoolCommandItem>("aestheticlight"_J));
		funGroup->AddItem(std::make_shared<ConditionalItem>("aestheticlight"_J, std::make_shared<ColorCommandItem>("aestheticcolour"_J)));
		funGroup->AddItem(std::make_shared<ConditionalItem>("aestheticlight"_J, std::make_shared<FloatCommandItem>("aestheticrange"_J)));
		funGroup->AddItem(std::make_shared<ConditionalItem>("aestheticlight"_J, std::make_shared<FloatCommandItem>("aestheticintensity"_J)));
		funGroup->AddItem(std::make_shared<ConditionalItem>("aestheticlight"_J, std::make_shared<ListCommandItem>("aestheticplace"_J)));

		qolGroup->AddItem(std::make_shared<BoolCommandItem>("nostuntjump"_J));
		qolGroup->AddItem(std::make_shared<BoolCommandItem>("qolnoidlecam"_J));
		qolGroup->AddItem(std::make_shared<BoolCommandItem>("noflightmusic"_J));
		qolGroup->AddItem(std::make_shared<BoolCommandItem>("nowantedmusic"_J));
		qolGroup->AddItem(std::make_shared<BoolCommandItem>("nocinematiccam"_J));

		serviceGroup->AddItem(std::make_shared<ListCommandItem>("servicelist"_J));
		serviceGroup->AddItem(std::make_shared<CommandItem>("requestservice"_J));
		serviceGroup->AddItem(std::make_shared<CommandItem>("svcairstrike"_J));
		serviceGroup->AddItem(std::make_shared<CommandItem>("svcbackupheli"_J));
		serviceGroup->AddItem(std::make_shared<CommandItem>("svcammodrop"_J));
		serviceGroup->AddItem(std::make_shared<CommandItem>("svcheli"_J));

		cameraGroup->AddItem(std::make_shared<BoolCommandItem>("camerapack"_J));
		cameraGroup->AddItem(std::make_shared<ConditionalItem>("camerapack"_J, std::make_shared<ListCommandItem>("camviewmode"_J)));
		cameraGroup->AddItem(std::make_shared<ConditionalItem>("camerapack"_J, std::make_shared<IntCommandItem>("camvehzoom"_J, "镜头远近")));
		cameraGroup->AddItem(std::make_shared<ConditionalItem>("camerapack"_J, std::make_shared<BoolCommandItem>("camnoshake"_J)));
		cameraGroup->AddItem(std::make_shared<ConditionalItem>("camerapack"_J, std::make_shared<FloatCommandItem>("camfov"_J)));

		hudPanelGroup->AddItem(std::make_shared<BoolCommandItem>("playerbar"_J));
		hudPanelGroup->AddItem(std::make_shared<ConditionalItem>("playerbar"_J, std::make_shared<FloatCommandItem>("playerbarx"_J, "位置 X")));
		hudPanelGroup->AddItem(std::make_shared<ConditionalItem>("playerbar"_J, std::make_shared<FloatCommandItem>("playerbary"_J, "位置 Y")));
		hudPanelGroup->AddItem(std::make_shared<ConditionalItem>("playerbar"_J, std::make_shared<FloatCommandItem>("playerbarscale"_J, "大小")));
		hudPanelGroup->AddItem(std::make_shared<ConditionalItem>("playerbar"_J, std::make_shared<BoolCommandItem>("playerbartags"_J)));

		specialAbilityGroup->AddItem(std::make_shared<BoolCommandItem>("infspecialability"_J));
		auto specialInMp = std::make_shared<Group>("", 1);
		specialInMp->AddItem(std::make_shared<BoolCommandItem>("mpspecialability"_J));
		specialInMp->AddItem(std::make_shared<ConditionalItem>("mpspecialability"_J, std::make_shared<ListCommandItem>("selspecialability"_J, "##specialselect")));
		specialAbilityGroup->AddItem(std::move(specialInMp));

		main->AddItem(globalsGroup);
		main->AddItem(extraGroup);
		main->AddItem(toolsGroup);
		main->AddItem(tools2Group);
		main->AddItem(swapGroup);
		main->AddItem(specialAbilityGroup);
		main->AddItem(wantedGroup);
		main->AddItem(movementGroup);
		main->AddItem(funGroup);
		main->AddItem(cameraGroup);
		main->AddItem(hudPanelGroup);
		main->AddItem(qolGroup);
		main->AddItem(serviceGroup);
		AddCategory(std::move(main));

		AddCategory(BuildWeaponsMenu());

		auto outfitEditorCategory = YimMenu::CreateOutfitsMenu();
		AddCategory(std::move(outfitEditorCategory));
	}
}
