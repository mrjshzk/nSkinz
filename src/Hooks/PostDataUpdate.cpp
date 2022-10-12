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

void erase_override_if_exists_by_index(int xuid_lo, int xuid_hi, const int definition_index)
{
	uint64_t xuid = (uint64_t)((uint32_t)xuid_lo) << 32 | (uint64_t)(uint32_t)xuid_hi;

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

void apply_config_on_attributable_item(sdk::C_BaseAttributableItem* item, item_setting* config, int xuid_lo, int xuid_hi, bool isGlove)
{
	uint64_t xuid = (uint64_t)((uint32_t)xuid_lo) << 32 | (uint64_t)(uint32_t)xuid_hi;

	item->GetInitialized() = true;

	// Force fallback values to be used.
	item->GetItemIDHigh() = -1;
	item->GetItemIDLow() = -1;

	// Set the owner of the weapon to our lower XUID. (fixes StatTrak)
	item->GetAccountID() = xuid_lo;

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
		erase_override_if_exists_by_index(xuid_lo, xuid_hi, definition_index);
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

void patch_weapon(sdk::C_BaseAttributableItem* weapon) {

	auto& definition_index = weapon->GetItemDefinitionIndex();

	// All knives are terrorist knives.
	if (const auto active_conf = g_config.get_from_xuid_by_definition_index(weapon->GetOriginalOwnerXuidLow(), weapon->GetOriginalOwnerXuidHigh(), is_knife(definition_index) ? WEAPON_KNIFE : definition_index))
		apply_config_on_attributable_item(weapon, active_conf, weapon->GetOriginalOwnerXuidLow(), weapon->GetOriginalOwnerXuidHigh(), false);
	else
		erase_override_if_exists_by_index(weapon->GetOriginalOwnerXuidLow(), weapon->GetOriginalOwnerXuidHigh(), definition_index);
}


void hook_viewmodel(sdk::C_BaseViewModel* thisptr);

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

	/*
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

			patch_weapon(weapon);
		}
	}
	
	auto view_model = get_entity_from_handle<sdk::C_BaseViewModel>(local->GetViewModel());

	if (nullptr == view_model) return;

	hook_viewmodel(view_model);

	const auto view_model_weapon = get_entity_from_handle<sdk::C_BaseAttributableItem>(view_model->GetWeapon());

	if (view_model_weapon) {
		patch_weapon(view_model_weapon);

		auto item = (sdk::C_BaseAttributableItem*)view_model_weapon;

		auto& definition_index = view_model_weapon->GetItemDefinitionIndex();
		// All knives are terrorist knives.
		const auto active_conf = g_config.get_from_xuid_by_definition_index(item->GetOriginalOwnerXuidLow(), item->GetOriginalOwnerXuidHigh(), is_knife(definition_index) ? WEAPON_KNIFE : definition_index);
		if (active_conf && active_conf->definition_override_index) {
			hook_weapon_update_on_remove(view_model_weapon);
			auto emplace_result = g_weapon_to_orgindex.emplace(view_model_weapon, definition_index);
			view_model_weapon->GetItemDefinitionIndex() = active_conf->definition_override_index;
			const auto override_info = game_data::get_weapon_info(active_conf->definition_override_index);
			if (override_info) {
				view_model->GetModelIndex() = g_model_info->GetModelIndex(override_info->viewModel);
			}
		}
	}*/
}

void patch_view_model(sdk::C_BaseViewModel * view_model) {
	if (nullptr == view_model) return;

	hook_viewmodel(view_model);

	const auto view_model_weapon = get_entity_from_handle<sdk::C_BaseAttributableItem>(view_model->GetWeapon());

	if (view_model_weapon) {
		patch_weapon(view_model_weapon);

		auto item = (sdk::C_BaseAttributableItem*)view_model_weapon;

		auto& definition_index = view_model_weapon->GetItemDefinitionIndex();
		// All knives are terrorist knives.
		const auto active_conf = g_config.get_from_xuid_by_definition_index(item->GetOriginalOwnerXuidLow(), item->GetOriginalOwnerXuidHigh(), is_knife(definition_index) ? WEAPON_KNIFE : definition_index);
		if (active_conf && active_conf->definition_override_index) {
			hook_weapon_update_on_remove(view_model_weapon);
			auto emplace_result = g_weapon_to_orgindex.emplace(view_model_weapon, definition_index);
			view_model_weapon->GetItemDefinitionIndex() = active_conf->definition_override_index;
			const auto override_info = game_data::get_weapon_info(active_conf->definition_override_index);
			if (override_info) {
				view_model->GetModelIndex() = g_model_info->GetModelIndex(override_info->viewModel);
			}
		}
	}
}
