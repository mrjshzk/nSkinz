/* This file is part of nSkinz by namazso, licensed under the MIT license:
*
* MIT License
*
* Copyright (c) namazso 2018
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
#include "nSkinz.hpp"
#include "Hooks/hooks.hpp"
#include "render.hpp"
#include "kit_parser.hpp"
#include "update_check.hpp"
#include "config.hpp"

sdk::IBaseClientDLL* g_client;
sdk::IClientEntityList* g_entity_list;
sdk::IVEngineClient* g_engine;
sdk::IVModelInfoClient* g_model_info;
sdk::IGameEventManager2* g_game_event_manager;
sdk::ILocalize* g_localize;
sdk::IInputSystem* g_input_system;

sdk::CBaseClientState** g_client_state;
sdk::C_CS_PlayerResource** g_player_resource;

//vmt_smart_hook*				g_client_hook;
//vmt_smart_hook*				g_game_event_manager_hook;

recv_prop_hook* g_modelindex_hook;
recv_prop_hook* g_weapon_hook;
recv_prop_hook* g_sequence_hook;

auto ensure_dynamic_hooks() -> void
{
	// find by xref to "CHudSaveStatus"
	const auto hss = get_vfunc<char*>(g_client, 87);
	const auto hud = *(void**)(hss + 7);
	const auto off = *(int32_t*)(hss + 12);
	const auto fn = (void* (__thiscall*)(void*, const char*))(hss + 16 + off);
	const auto notice_hud = fn(hud, "CCSGO_HudDeathNotice");
	if (notice_hud)
	{
		static vmt_multi_hook notice_hook;
		if (notice_hook.initialize_and_hook_instance(notice_hud))
			notice_hook.apply_hook<hooks::SFHudDeathNoticeAndBotStatus_FireGameEvent>(1);
	}
}

template <class T>
auto get_interface(const char* module, const char* name) -> T*
{
	return reinterpret_cast<T*>(platform::get_interface(module, name));
}

auto get_client_name() -> const char*
{
	static const char* name = nullptr;
	if (!name)
		name = platform::get_module_info("client_panorama.dll").first ? "client_panorama.dll" : "client.dll";
	return name;
}

void On_FRAME_NET_UPDATE_POSTDATAUPDATE_START(sdk::C_BasePlayer* local);

hooks::IBaseClientDLL_FrameStageNotify::Fn* hooks::IBaseClientDLL_FrameStageNotify::m_original;

void MapViewModel(sdk::C_BaseViewModel* view_model);
void UnmapViewModel(sdk::C_BaseViewModel* view_model);
void patch_weapon(sdk::C_BaseAttributableItem* weapon);

auto __fastcall hooks::IBaseClientDLL_FrameStageNotify::hooked(sdk::IBaseClientDLL* thisptr, void*, sdk::ClientFrameStage_t curStage) -> void
{
	switch (curStage) {
	case sdk::FRAME_NET_UPDATE_POSTDATAUPDATE_START:
	{
		for (int idx = 0; idx <= sdk::MAX_PLAYERS; ++idx)
		{
			if (auto ent = g_entity_list->GetClientEntity(idx))
			{
				if (auto bent = ent->GetBaseEntity())
				{
					if (bent->IsPlayer())
					{
						On_FRAME_NET_UPDATE_POSTDATAUPDATE_START(static_cast<sdk::C_BasePlayer*>(bent));
					}
				}
			}
		}
		for (int idx = 0; idx <= g_entity_list->GetMaxEntities(); ++idx)
		{
			if (auto ent = g_entity_list->GetClientEntity(idx))
			{
				if (auto bent = ent->GetBaseEntity())
				{
					const char* className = bent->GetClassname();

					if (className == strstr(className, "weapon_")) {
						auto weapon = static_cast<sdk::C_BaseAttributableItem*>(bent);
						patch_weapon(weapon);
					}
					else if (0 == strcmp("predicted_viewmodel", className))
					{
						auto view_model = static_cast<sdk::C_BaseViewModel*>(bent);
						MapViewModel(view_model);
					}
				}
			}
		}
		m_original(thisptr, nullptr, curStage);
	} break;
	case sdk::FRAME_NET_UPDATE_END:
	{
		for (int idx = 0; idx <= g_entity_list->GetMaxEntities(); ++idx)
		{
			if (auto ent = g_entity_list->GetClientEntity(idx))
			{
				if (auto bent = ent->GetBaseEntity())
				{
					const char* className = bent->GetClassname();

					if (0 == strcmp("predicted_viewmodel", className))
					{
						auto view_model = static_cast<sdk::C_BaseViewModel*>(bent);
						MapViewModel(view_model);
					}
				}
			}
		}
		m_original(thisptr, nullptr, curStage);
	} break;
	case sdk::FRAME_NET_UPDATE_START:
	{
		m_original(thisptr, nullptr, curStage);
		for (int idx = 0; idx <= g_entity_list->GetMaxEntities(); ++idx)
		{
			if (auto ent = g_entity_list->GetClientEntity(idx))
			{
				if (auto bent = ent->GetBaseEntity())
				{
					if (0 == strcmp("predicted_viewmodel", bent->GetClassname()))
					{
						UnmapViewModel(static_cast<sdk::C_BaseViewModel*>(bent));
					}
				}
			}
		}
	} break;
	default:
		m_original(thisptr, nullptr, curStage);
	}

}

auto initialize(void* instance) -> void
{
	g_client = get_interface<sdk::IBaseClientDLL>(get_client_name(), CLIENT_DLL_INTERFACE_VERSION);
	g_entity_list = get_interface<sdk::IClientEntityList>(get_client_name(), VCLIENTENTITYLIST_INTERFACE_VERSION);
	g_engine = get_interface<sdk::IVEngineClient>("engine.dll", VENGINE_CLIENT_INTERFACE_VERSION);
	g_model_info = get_interface<sdk::IVModelInfoClient>("engine.dll", VMODELINFO_CLIENT_INTERFACE_VERSION);
	g_game_event_manager = get_interface<sdk::IGameEventManager2>("engine.dll", INTERFACEVERSION_GAMEEVENTSMANAGER2);
	g_localize = get_interface<sdk::ILocalize>("localize.dll", ILOCALIZE_CLIENT_INTERFACE_VERSION);
	g_input_system = get_interface<sdk::IInputSystem>("inputsystem.dll", INPUTSYSTEM_INTERFACE_VERSION);

	g_client_state = *reinterpret_cast<sdk::CBaseClientState***>(get_vfunc<std::uintptr_t>(g_engine, 12) + 0x10);

	run_update_check();

	// Get skins
	game_data::initialize_kits();

	g_config.load();

	render::initialize();

	//g_client_hook = new vmt_smart_hook(g_client);
	//g_client_hook->apply_hook<hooks::FrameStageNotify>(36);

	//g_game_event_manager_hook = new vmt_smart_hook(g_game_event_manager);
	//g_game_event_manager_hook->apply_hook<hooks::FireEventClientSide>(9);

	//const auto modelindex_prop = sdk::C_BaseViewModel::GetModelIndexProp();
	//g_modelindex_hook = new recv_prop_hook(modelindex_prop, &hooks::modelindex_proxy_fn);

	//const auto weapon_prop = sdk::C_BaseViewModel::GetWeaponProp();
	//g_weapon_hook = new recv_prop_hook(weapon_prop, &hooks::weapon_proxy_fn);

	//const auto squence_prop = sdk::C_BaseViewModel::GetSequenceProp();
	//g_sequence_hook = new recv_prop_hook(squence_prop, &hooks::sequence_proxy_fn);

	const auto team_arr_prop = sdk::C_CS_PlayerResource::GetTeamProp();
	const auto team_prop = team_arr_prop->m_pDataTable->m_pProps;
	const auto proxy_addr = std::uintptr_t(team_prop->m_ProxyFn);
	g_player_resource = *reinterpret_cast<sdk::C_CS_PlayerResource***>(proxy_addr + 0x10);

	static vmt_multi_hook baseclientdll_hook;
	if (baseclientdll_hook.initialize_and_hook_instance(g_client))
		baseclientdll_hook.apply_hook<hooks::IBaseClientDLL_FrameStageNotify>(37);
}

// If we aren't unloaded correctly (like when you close csgo)
// we should just leak the hooks, since the hooked instances
// might be already destroyed
auto uninitialize() -> void
{
	render::uninitialize();

	//delete g_client_hook;
	//delete g_game_event_manager_hook;

	delete g_sequence_hook;
}
