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
#include "hooks.hpp"
#include "../item_definitions.hpp"
#include "../nSkinz.hpp"
#include "../config.hpp"
#include "../sticker_changer.hpp"
#include "../SDK/declarations.hpp"
#include "../SDK.hpp"

void erase_override_if_exists_by_index(const uint64_t xuid, const int definition_index)
{
	// We have info about the item not needed to be overridden
	if(const auto original_item = game_data::get_weapon_info(definition_index))
	{
		auto& icon_override_map = g_config.get_icon_override_map();

		if (!original_item->icon)
			return;

		const auto override_entry = icon_override_map.find(xuid);

		// We are overriding its icon when not needed
		if(override_entry != end(icon_override_map))
		{
			std::unordered_map<std::string_view, std::string_view> & user_id_icon_override_map = icon_override_map.at(xuid);

			int count = user_id_icon_override_map.count(original_item->icon);

			if (count)
			{
				user_id_icon_override_map.erase(user_id_icon_override_map.at(original_item->icon)); // Remove the leftover override

				if (count == 1)
					icon_override_map.erase(xuid);
			}
		}
	}
}

std::map<sdk::C_BaseAttributableItem*, int> g_weapon_to_orgindex;

hooks::C_BaseAttributableItem_UpdateOnRemove::Fn* hooks::C_BaseAttributableItem_UpdateOnRemove::m_original;

auto __fastcall hooks::C_BaseAttributableItem_UpdateOnRemove::hooked(sdk::C_BaseAttributableItem* This, void* Edx) -> void
{
	g_weapon_to_orgindex.erase(This);
	hooks::C_BaseAttributableItem_UpdateOnRemove::m_original(This, Edx);
}

void hook_weapon_update_on_remove(sdk::C_BaseAttributableItem* thisptr) {
	static bool firstRun = true;
	if (firstRun) {
		firstRun = false;
		static vmt_multi_hook weapon_update_on_remove_hook;
		if (weapon_update_on_remove_hook.initialize_and_hook_instance(thisptr))
			weapon_update_on_remove_hook.apply_hook<hooks::C_BaseAttributableItem_UpdateOnRemove>(127);
	}
}

struct OrgModelData_s {
	int ModelIndex = -1;
	int Sequence = -1;
	int LastNewSequence = -1;
};

std::map<sdk::C_BaseEntity*, OrgModelData_s> g_weapon_to_org;
sdk::C_BaseEntity* g_lastviewmodel = nullptr;
static std::map<void*, vmt_multi_hook*> C_BaseEntity_UpdateOnRemove_hooks;

vmt_multi_hook* Get_ViewModel_Hook(sdk::C_BaseViewModel* This) {
	return C_BaseEntity_UpdateOnRemove_hooks[*(void**)This];
}

void __fastcall C_BaseEntity_UpdateOnRemove_Hook(sdk::C_BaseViewModel* This, void* Edx)
{
	if (g_lastviewmodel == This) g_lastviewmodel = nullptr;
	g_weapon_to_org.erase(This);
	auto pHook = Get_ViewModel_Hook(This);
	auto org_fn = pHook->get_original_function<void(__fastcall*)(sdk::C_BaseEntity*, void*)>(127);
	org_fn(This, Edx);
}

void hook_viewmodel(sdk::C_BaseViewModel* thisptr) {

	if (C_BaseEntity_UpdateOnRemove_hooks.contains(*(void**)thisptr)) return;

	vmt_multi_hook* pHook = new vmt_multi_hook();
	if (pHook->initialize_and_hook_instance(thisptr)) {
		pHook->hook_function(C_BaseEntity_UpdateOnRemove_Hook, 127);
		pHook->hook_function(&hooks::C_BaseViewModel_SetSequence::hooked, 219);
		C_BaseEntity_UpdateOnRemove_hooks[*(void**)thisptr] = pHook;
	}
}


void apply_config_on_attributable_item(sdk::C_BaseAttributableItem* item, item_setting* config,
	const uint64_t xuid, const unsigned xuid_low, bool isGlove)
{
	item->GetInitialized() = true;

	// Force fallback values to be used.
	item->GetItemIDHigh() = -1;
	item->GetItemIDLow() = -1;

	// Set the owner of the weapon to our lower XUID. (fixes StatTrak)
	item->GetAccountID() = xuid_low;

	if (config->entity_quality_index)
		item->GetEntityQuality() = config->entity_quality_index;

	if (config->custom_name[0])
		strcpy_s(item->GetCustomName(), config->custom_name);

	if (config->paint_kit_index)
		item->GetFallbackPaintKit() = config->paint_kit_index;

	if (config->seed)
		item->GetFallbackSeed() = config->seed;

	if (config->stat_trak)
		item->GetFallbackStatTrak() = (unsigned int)config->stat_trak;

	item->GetFallbackWear() = config->wear;

	auto& definition_index = item->GetItemDefinitionIndex();

	auto& icon_override_map = g_config.get_icon_override_map();

	if (config->definition_override_index // We need to override defindex
		&& config->definition_override_index != definition_index) // It is not yet overridden
	{
		// We have info about what we gonna override it to
		if (const auto replacement_item = game_data::get_weapon_info(config->definition_override_index))
		{
			const int old_definition_index = definition_index;
			hook_weapon_update_on_remove(item);
			g_weapon_to_orgindex.emplace(item, definition_index);


			// We didn't override 0, but some actual weapon, that we have data for
			if (old_definition_index)
			{
				if (const auto original_item = game_data::get_weapon_info(old_definition_index))
				{
					if (original_item->icon && replacement_item->icon)
						icon_override_map[xuid][original_item->icon] = replacement_item->icon;
				}
			}

			definition_index = short(config->definition_override_index);

			item->GetModelIndex() = g_model_info->GetModelIndex(isGlove ? replacement_item->worldModel : replacement_item->viewModel);
			item->ValidateModelIndex();

			if (const auto world_model = get_entity_from_handle<sdk::CBaseWeaponWorldModel>(item->GetWeaponWorldModel())) {
				world_model->GetModelIndex() = g_model_info->GetModelIndex(replacement_item->worldModel);
				world_model->ValidateModelIndex();
			}

			item->GetClientNetworkable()->PreDataUpdate(0);
			item->GetClientNetworkable()->OnPreDataChanged(0);
			item->GetClientNetworkable()->PostDataUpdate(0);
		}
	}
	else
	{
		erase_override_if_exists_by_index(xuid, definition_index);
	}

	apply_sticker_changer(item);
}

static auto get_wearable_create_fn() -> sdk::CreateClientClassFn
{
	auto clazz = g_client->GetAllClasses();

	// Please, if you gonna paste it into a cheat use classids here. I use names because they
	// won't change in the foreseeable future and i dont need high speed, but chances are
	// you already have classids, so use them instead, they are faster.
	while(fnv::hash_runtime(clazz->m_pNetworkName) != FNV("CEconWearable"))
		clazz = clazz->m_pNext;

	return clazz->m_pCreateFn;
}

static auto make_glove(int entry, int serial) -> sdk::C_BaseAttributableItem*
{
	static auto create_wearable_fn = get_wearable_create_fn();

	create_wearable_fn(entry, serial);

	const auto glove = static_cast<sdk::C_BaseAttributableItem*>(g_entity_list->GetClientEntity(entry));
	assert(glove);

	// He he
	/* {
		static auto set_abs_origin_addr = platform::find_pattern(get_client_name(), "\x55\x8B\xEC\x83\xE4\xF8\x51\x53\x56\x57\x8B\xF1", "xxxxxxxxxxxx");

		const auto set_abs_origin_fn = reinterpret_cast<void(__thiscall*)(void*, const sdk::Vector&)>(set_abs_origin_addr);

		static constexpr sdk::Vector new_pos = { 10000.f, 10000.f, 10000.f };

		set_abs_origin_fn(glove, new_pos);
	}*/

	return glove;
}

std::map<int, sdk::CBaseHandle> g_PlayerToHandle;
std::map<int, fnv::hash> g_PlayerToViewModelHash;

bool GetPlayerIndexToViewModelHash(int index, fnv::hash & hash) {
	auto it = g_PlayerToViewModelHash.find(index);
	if (it == g_PlayerToViewModelHash.end()) return false;
	hash = it->second;
	return true;
}

int do_sequence_remapping(int org_definition_index, int nSequence, int definition_override_index);

void fixup_view_model(sdk::C_BaseViewModel* view_model) {
	const auto view_model_weapon = get_entity_from_handle<sdk::C_BaseAttributableItem>(view_model->GetWeapon());

	if (view_model_weapon) {
		auto owner = get_entity_from_handle<sdk::C_BasePlayer>(view_model->GetOwner());
		if (owner) {
			/*/if (owner->EntIndex() == g_engine->GetLocalPlayer()) {
				auto obsTarget = owner->GetObserverTarget();
				if (obsTarget->IsPlayer())
					owner = static_cast<sdk::C_BasePlayer*>(obsTarget);
				else
					owner = nullptr;
			}*/
			if (owner && owner->IsPlayer() /* && static_cast<sdk::C_BasePlayer*>(owner)->GetLifeState() == sdk::LifeState::ALIVE*/) {
				sdk::player_info_t player_info;
				if (g_engine->GetPlayerInfo(owner->GetIndex(), &player_info)) {
					auto& definition_index = view_model_weapon->GetItemDefinitionIndex();
					// All knives are terrorist knives.
					const auto active_conf = g_config.get_by_definition_index(player_info.userid, is_knife(definition_index) ? WEAPON_KNIFE : definition_index);
					if (active_conf && active_conf->definition_override_index) {
						hook_weapon_update_on_remove(view_model_weapon);
						auto emplace_result = g_weapon_to_orgindex.emplace(view_model_weapon, definition_index);
						view_model_weapon->GetItemDefinitionIndex() = active_conf->definition_override_index;
						const auto override_info = game_data::get_weapon_info(active_conf->definition_override_index);
						if (override_info) {

							auto& org = g_weapon_to_org[view_model];

							int oldModelIndex = view_model->GetModelIndex();
							int oldSequence = view_model->GetSequence();

							bool bMapSequence = org.LastNewSequence == -1 || oldModelIndex != org.ModelIndex || oldSequence != org.Sequence;

							org.ModelIndex = oldModelIndex;
							org.Sequence = oldSequence;

							int newIndex = g_model_info->GetModelIndex(override_info->viewModel);
							view_model->GetModelIndex() = newIndex;

							if (bMapSequence) {
								int orgIndex = definition_index;
								auto it = g_weapon_to_orgindex.find(view_model_weapon);
								if (it != g_weapon_to_orgindex.end())
									orgIndex = it->second;
								int newSequence = do_sequence_remapping(definition_index, oldSequence, active_conf->definition_override_index);

								view_model->GetSequence() = newSequence;
								org.LastNewSequence = newSequence;
								//view_model->SetSequence(newSequence);
							}
							else {
								view_model->GetSequence() = org.LastNewSequence;
							}

							hook_viewmodel(view_model);
							g_lastviewmodel = view_model;
						}
					}
				}
			}
		}
	}
	view_model->ValidateModelIndex();
}

void On_FRAME_RENDER_START(sdk::C_BaseEntity* ent);
void On_FRAME_RENDER_END(sdk::C_BaseEntity* ent);

void On_FRAME_NET_UPDATE_POSTDATAUPDATE_START(sdk::C_BasePlayer* local)
{
	const auto local_index = local->GetIndex();

	/*if(auto player_resource = *g_player_resource)
	{
		player_resource->GetCoins()[local_index] = 890;
		player_resource->GetMusicKits()[local_index] = 3;
		player_resource->GetRanks()[local_index] = 1;
		player_resource->GetWins()[local_index] = 1337;
	}*/

	sdk::player_info_t player_info;
	if (!g_engine->GetPlayerInfo(local_index, &player_info))
		return;

	// Handle glove config
	{
		const auto wearables = local->GetWearables();

		const auto glove_config = g_config.get_by_definition_index(player_info.userid, GLOVE_T_SIDE);

		static std::map<uint64_t, sdk::CBaseHandle> glove_handles;
		if (glove_handles.find(player_info.xuid) == glove_handles.end())  glove_handles[player_info.xuid] = sdk::INVALID_EHANDLE_INDEX;

		auto cur_glove_handle = glove_handles[player_info.xuid];

		auto glove = get_entity_from_handle<sdk::C_BaseAttributableItem>(wearables[0]);

		if (!glove) // There is no glove
		{
			// Try to get our last created glove
			const auto our_glove = get_entity_from_handle<sdk::C_BaseAttributableItem>(cur_glove_handle);

			if (our_glove) // Our glove still exists
			{
				wearables[0] = cur_glove_handle;
				glove = our_glove;
			}
		}

		if (local->GetLifeState() != sdk::LifeState::ALIVE)
		{
			// We are dead but we have a _own_ glove, destroy it
			if (glove && cur_glove_handle != sdk::INVALID_EHANDLE_INDEX)
			{
				glove->GetClientNetworkable()->SetDestroyedOnRecreateEntities();
				glove->GetClientNetworkable()->Release();
			}

			return;
		}

		if (glove_config && glove_config->definition_override_index)
		{
			// We don't have a glove, but we should
			if (!glove)
			{
				const auto entry = g_entity_list->GetHighestEntityIndex() + 1;
				const auto serial = rand() % 0x1000;

				glove = make_glove(entry, serial);

				glove->GetNetworkMoveParent() = local->GetRefEHandle();
				glove->GetOwnerEntity() = local->GetRefEHandle();
				glove->GiveToPlayer(local);

				wearables[0] = glove->GetRefEHandle();

				// Let's store it in case we somehow lose it.
				glove_handles[player_info.xuid] = wearables[0];

				local->GetBody() = 1;
			}

			apply_config_on_attributable_item(glove, glove_config, player_info.xuid, player_info.xuid_low, true);
		}
	}

	// Handle weapon configs
	{
		auto& weapons = local->GetWeapons();

		for (auto weapon_handle : weapons)
		{
			if (weapon_handle == sdk::INVALID_EHANDLE_INDEX)
				break;

			auto weapon = get_entity_from_handle<sdk::C_BaseAttributableItem>(weapon_handle);

			if (!weapon)
				continue;

			auto& definition_index = weapon->GetItemDefinitionIndex();

			// All knives are terrorist knives.
			if (const auto active_conf = g_config.get_by_definition_index(player_info.userid, is_knife(definition_index) ? WEAPON_KNIFE : definition_index))
				apply_config_on_attributable_item(weapon, active_conf, player_info.xuid, player_info.xuid_low, false);
			else
				erase_override_if_exists_by_index(player_info.xuid, definition_index);
		}
	}

	const auto view_model = get_entity_from_handle<sdk::C_BaseViewModel>(local->GetViewModel());
	if (view_model) {
		fixup_view_model(view_model);
		//On_FRAME_RENDER_END(view_model);
		//On_FRAME_RENDER_START(view_model);
	}
}

void On_FRAME_RENDER_START(sdk::C_BaseEntity* ent) {
	const char* className = ent->GetClassname();
	if (0 == strcmp("predicted_viewmodel", className)) {
		const auto view_model = static_cast<sdk::C_BaseViewModel *>(ent);
		if (view_model) {
			fixup_view_model(view_model);
		}
	}
}

void On_FRAME_RENDER_END(sdk::C_BaseEntity* ent) {
	auto it = g_weapon_to_org.find(ent);
	if (it != g_weapon_to_org.end()) {
		auto view_model = (sdk::C_BaseViewModel*)ent;
		view_model->GetModelIndex() = it->second.ModelIndex;
		view_model->GetSequence() = it->second.Sequence;
		//view_model->ValidateModelIndex();
		//g_weapon_to_org.erase(ent);
	}
}
