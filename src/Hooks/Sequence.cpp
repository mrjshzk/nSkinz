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
#include "../nSkinz.hpp"
#include "../config.hpp"
#include "../SDK/declarations.hpp"

static auto random_sequence(const int low, const int high) -> int
{
	return rand() % (high - low + 1) + low;
}

enum class Activity_e : int {
	ACT_UNKNOWN = -1,
	ACT_VM_DRAW = 0,
	ACT_VM_IDLE,
	ACT_VM_MISSCENTER,
	ACT_VM_HITCENTER,
	ACT_VM_SWINGHIT,
	ACT_VM_MISSCENTER2,
	ACT_VM_HITCENTER2,
	ACT_VM_SWINGHARD,
	ACT_VM_IDLE_LOWERED
};

struct Sequence_s {
	Activity_e m_Activity;
	int m_Wheight;

	Sequence_s(Activity_e activity, int weight) : m_Activity(activity), m_Wheight(weight) {}
};

class SequenceMap {
public:
	SequenceMap() {
		/*
		{
			auto& weapon = m_SequenceMap[WEAPON_KNIFE];
			weapon.emplace_back(Activity_e::ACT_VM_DRAW, 1);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE, 3);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHARD, 1);
		}*/
		{
			auto& weapon = m_SequenceMap[WEAPON_KNIFE_BAYONET];
			weapon.emplace_back(Activity_e::ACT_VM_DRAW, 1);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE, 3);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHARD, 1);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE_LOWERED, 1); //?
		}
		{
			auto& weapon = m_SequenceMap[WEAPON_KNIFE_BUTTERFLY];
			weapon.emplace_back(Activity_e::ACT_VM_DRAW, 1);
			weapon.emplace_back(Activity_e::ACT_VM_DRAW, 4);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE, 3);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHARD, 1);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE_LOWERED, 1);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE_LOWERED, 1);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE_LOWERED, 1);
		}
		{
			auto& weapon = m_SequenceMap[WEAPON_KNIFE_CANIS];
			weapon.emplace_back(Activity_e::ACT_VM_DRAW, 5);
			weapon.emplace_back(Activity_e::ACT_VM_DRAW, 1);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE, 3);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHARD, 1);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE_LOWERED, 1);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE_LOWERED, 3);
		}
		{
			auto& weapon = m_SequenceMap[WEAPON_KNIFE_CORD];
			weapon.emplace_back(Activity_e::ACT_VM_DRAW, 5);
			weapon.emplace_back(Activity_e::ACT_VM_DRAW, 1);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE, 3);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHARD, 1);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE_LOWERED, 1);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE_LOWERED, 4);
		}
		{
			auto& weapon = m_SequenceMap[WEAPON_KNIFE_CSS];
			weapon.emplace_back(Activity_e::ACT_VM_DRAW, 2);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE, 3);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHARD, 1);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE_LOWERED, 5);
			weapon.emplace_back(Activity_e::ACT_UNKNOWN, 0);
			weapon.emplace_back(Activity_e::ACT_UNKNOWN, 0);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE_LOWERED, 1);
		}
		{
			auto& weapon = m_SequenceMap[WEAPON_KNIFE]; //WEAPON_KNIFE_CT?
			weapon.emplace_back(Activity_e::ACT_VM_DRAW, 1);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE, 3);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHARD, 1);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE_LOWERED, 1);
		}
		{
			auto& weapon = m_SequenceMap[WEAPON_KNIFE_T];
			weapon.emplace_back(Activity_e::ACT_VM_DRAW, 1);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE, 3);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHARD, 1);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE_LOWERED, 1);
		}
		{
			auto& weapon = m_SequenceMap[WEAPON_KNIFE_FALCHION];
			weapon.emplace_back(Activity_e::ACT_VM_DRAW, 2);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE, 40);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER2, 10);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHARD, 1);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE_LOWERED, 100);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE_LOWERED, 1);
		}
		{
			auto& weapon = m_SequenceMap[WEAPON_KNIFE_FLIP];
			weapon.emplace_back(Activity_e::ACT_VM_DRAW, 1);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE, 3);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHARD, 1);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE_LOWERED, 1); //?
		}
		/*
		{
			const auto hash = FNV("models/weapons/v_knife_gg.mdl");
			auto& weapon = m_SequenceMap[hash];
			weapon.emplace_back(Activity_e::ACT_VM_DRAW, 1);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE, 3);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHARD, 1);
		}*/
		/*
		{
			const auto hash = FNV("models/weapons/v_knife_ghost.mdl");
			auto& weapon = m_SequenceMap[hash];
			weapon.emplace_back(Activity_e::ACT_VM_DRAW, 1);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE, 3);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHARD, 1);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE_LOWERED, 1); //?
		}*/
		{
			auto& weapon = m_SequenceMap[WEAPON_KNIFE_GUT];
			weapon.emplace_back(Activity_e::ACT_VM_DRAW, 1);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE, 3);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHARD, 1);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE_LOWERED, 1); //?
		}
		{
			auto& weapon = m_SequenceMap[WEAPON_KNIFE_GYPSY_JACKKNIFE];
			weapon.emplace_back(Activity_e::ACT_VM_DRAW, 2);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE, 3);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHARD, 1);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE_LOWERED, 1); //?
		}
		{
			auto& weapon = m_SequenceMap[WEAPON_KNIFE_KARAMBIT];
			weapon.emplace_back(Activity_e::ACT_VM_DRAW, 1);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE, 3);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHARD, 1);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE_LOWERED, 1); //?
		}
		{
			auto& weapon = m_SequenceMap[WEAPON_KNIFE_M9_BAYONET];
			weapon.emplace_back(Activity_e::ACT_VM_DRAW, 1);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE, 3);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHARD, 1);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE_LOWERED, 1); //?
		}
		{
			auto& weapon = m_SequenceMap[WEAPON_KNIFE_OUTDOOR];
			weapon.emplace_back(Activity_e::ACT_VM_DRAW, 1);
			weapon.emplace_back(Activity_e::ACT_VM_DRAW, 10);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE, 3);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHARD, 1);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE_LOWERED, 1);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE_LOWERED, 4);
		}
		{
			auto& weapon = m_SequenceMap[WEAPON_KNIFE_PUSH];
			weapon.emplace_back(Activity_e::ACT_VM_DRAW, 2);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE, 40);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHARD, 1);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE_LOWERED, 1);
		}
		{
			auto& weapon = m_SequenceMap[WEAPON_KNIFE_SKELETON];
			weapon.emplace_back(Activity_e::ACT_VM_DRAW, 5);
			weapon.emplace_back(Activity_e::ACT_VM_DRAW, 1);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE, 3);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHARD, 1);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE_LOWERED, 5);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE_LOWERED, 1);
		}
		{
			auto& weapon = m_SequenceMap[WEAPON_KNIFE_STILETTO];
			weapon.emplace_back(Activity_e::ACT_VM_DRAW, 2);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE, 3);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHARD, 1);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE_LOWERED, 1);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE_LOWERED, 3);
		}
		{
			auto& weapon = m_SequenceMap[WEAPON_KNIFE_SURVIVAL_BOWIE];
			weapon.emplace_back(Activity_e::ACT_VM_DRAW, 2);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE, 40);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHARD, 1);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE_LOWERED, 100);
		}
		{
			auto& weapon = m_SequenceMap[WEAPON_KNIFE_TACTICAL];
			weapon.emplace_back(Activity_e::ACT_VM_DRAW, 1);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE, 3);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHARD, 1);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE_LOWERED, 1); //?
		}
		{
			auto& weapon = m_SequenceMap[WEAPON_KNIFE_URSUS];
			weapon.emplace_back(Activity_e::ACT_VM_DRAW, 3);
			weapon.emplace_back(Activity_e::ACT_VM_DRAW, 1);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE, 3);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHARD, 1);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE_LOWERED, 1);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE_LOWERED, 3);
		}
		{
			auto& weapon = m_SequenceMap[WEAPON_KNIFE_WIDOWMAKER];
			weapon.emplace_back(Activity_e::ACT_VM_DRAW, 2);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE, 3);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHIT, 1);
			weapon.emplace_back(Activity_e::ACT_VM_MISSCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_HITCENTER2, 1);
			weapon.emplace_back(Activity_e::ACT_VM_SWINGHARD, 1);
			weapon.emplace_back(Activity_e::ACT_UNKNOWN, 0);
			weapon.emplace_back(Activity_e::ACT_UNKNOWN, 0);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE_LOWERED, 3);
			weapon.emplace_back(Activity_e::ACT_VM_IDLE_LOWERED, 1);
		}
		
		for (auto it = m_SequenceMap.begin(); it != m_SequenceMap.end(); ++it) {
			int sequence = 0;
			for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
				m_ActivityWeights[it->first][it2->m_Activity] += it2->m_Wheight;
				m_ActivitySequences[it->first][it2->m_Activity].emplace(sequence, it2->m_Wheight);
				++sequence;
			}
		}
	}

	int MapSequence(int org_definition_index, int sequence, int override_definition_index) {

		if (org_definition_index == override_definition_index)
			return sequence;

		auto it = m_SequenceMap.find(org_definition_index);
		if (it != m_SequenceMap.end()) it = m_SequenceMap.find(WEAPON_KNIFE); // use default knife as fallback.
		if (it != m_SequenceMap.end()) {
			if (0 <= sequence && it->second.size() > (size_t)sequence) {
				Activity_e activity = it->second[sequence].m_Activity;
				auto it2 = m_ActivityWeights.find(override_definition_index);
				if (it2 != m_ActivityWeights.end()) {

					// source and target known.

					auto it3 = it2->second.find(activity);
					if (it3 != it2->second.end() && 0 < it3->second) {
						int rand = random_sequence(0, it3->second);
						int newSequence = 0;
						auto &map = m_ActivitySequences[override_definition_index][activity];
						if (0 < map.size()) {
							auto itMap = map.begin();
							do {
								if(itMap == map.end()) itMap = map.begin();
								rand -= itMap->second;
								newSequence = itMap->first;
								++itMap;
							} while (0 < rand);
							return newSequence;
						}

					}
				}
				return 0;
			}
		}

		return sequence;
	}
private:
	std::map<int, std::vector<Sequence_s>> m_SequenceMap;
	std::map<int, std::map<Activity_e,int>> m_ActivityWeights;
	std::map<int, std::map<Activity_e,std::map<int,int>>> m_ActivitySequences;
} g_SequenceMap;

static auto do_sequence_remapping(sdk::CRecvProxyData* data, sdk::C_BaseViewModel* entity) -> void
{
	const auto owner = get_entity_from_handle<sdk::C_BasePlayer>(entity->GetOwner());

	if (!(owner && owner->IsPlayer() && static_cast<sdk::C_BasePlayer*>(owner)->GetLifeState() == sdk::LifeState::ALIVE))
		return;

	const auto view_model_weapon = get_entity_from_handle<sdk::C_BaseAttributableItem>(entity->GetWeapon());

	if(!view_model_weapon)
		return;

	const auto weapon_info = game_data::get_weapon_info(view_model_weapon->GetItemDefinitionIndex());

	if(!weapon_info)
		return;

	const auto definition_index = view_model_weapon->GetItemDefinitionIndex();

	if (!is_knife(definition_index))
		return;

	sdk::player_info_t player_info;
	if (!g_engine->GetPlayerInfo(owner->GetIndex(), &player_info))
		return;

	const auto active_conf = g_config.get_by_definition_index(player_info.userid, WEAPON_KNIFE);
	if (nullptr == active_conf || 0 == active_conf->definition_override_index)
		return;

	auto& sequence = data->m_Value.m_Int;
	sequence = g_SequenceMap.MapSequence(active_conf->org_definition_index, sequence, active_conf->definition_override_index);
}

// Replacement function that will be called when the view model animation sequence changes.
auto __cdecl hooks::sequence_proxy_fn(const sdk::CRecvProxyData* proxy_data_const, void* entity, void* output) -> void
{
	const auto view_model = static_cast<sdk::C_BaseViewModel*>(entity);

	// Ensure our other dynamic object hooks are in place.
	// Must do this from a game thread.
	ensure_dynamic_hooks();

	static auto original_fn = g_sequence_hook->get_original_function();

	// Remove the constness from the proxy data allowing us to make changes.
	const auto proxy_data = const_cast<sdk::CRecvProxyData*>(proxy_data_const);

	do_sequence_remapping(proxy_data, view_model);

	// Call the original function with our edited data.
	original_fn(proxy_data_const, entity, output);
}

void post_data_update_end(sdk::C_BasePlayer* local);
void erase_override_if_exists_by_index(const uint64_t xuid, const int definition_index);
void apply_config_on_attributable_item(sdk::C_BaseAttributableItem* item, item_setting* config,
	const uint64_t xuid, const unsigned xuid_low);


bool g_modelindex_called = false;
int g_modelindex_value = 0;

auto __cdecl hooks::modelindex_proxy_fn(const sdk::CRecvProxyData* proxy_data_const, void* entity, void* output) -> void
{
	// The problem is that model index is set before we know the weapon handle, so we cache it off and fix it up after weapon handle is set,
	// assuming that weapon handle changes when model index changes.

	g_modelindex_called = true;
	g_modelindex_value = proxy_data_const->m_Value.m_Int;
}

auto __cdecl hooks::weapon_proxy_fn(const sdk::CRecvProxyData* proxy_data_const, void* entity, void* output) -> void
{
	const auto view_model = static_cast<sdk::C_BaseViewModel*>(entity);

	ensure_dynamic_hooks();
	static auto original_fn = g_weapon_hook->get_original_function();

	original_fn(proxy_data_const, entity, output);
	
	if (g_modelindex_called) {
		g_modelindex_called = false;
		sdk::CRecvProxyData data;
		data.m_Value.m_Int = g_modelindex_value;

		const auto view_model_weapon = get_entity_from_handle<sdk::C_BaseAttributableItem>(view_model->GetWeapon());

		if (view_model_weapon) {
			const auto owner = get_entity_from_handle<sdk::C_BasePlayer>(view_model->GetOwner());
			if (owner && owner->IsPlayer() && static_cast<sdk::C_BasePlayer*>(owner)->GetLifeState() == sdk::LifeState::ALIVE) {
				sdk::player_info_t player_info;
				if (g_engine->GetPlayerInfo(owner->GetIndex(), &player_info)) {

					auto& definition_index = view_model_weapon->GetItemDefinitionIndex();
					// All knives are terrorist knives.
					const auto active_conf = g_config.get_by_definition_index(player_info.userid, is_knife(definition_index) ? WEAPON_KNIFE : definition_index);
					if (active_conf && active_conf->definition_override_index) {
						if(active_conf->definition_override_index != definition_index)
							active_conf->org_definition_index = definition_index;
						view_model_weapon->GetItemDefinitionIndex() = active_conf->definition_override_index;
						const auto override_info = game_data::get_weapon_info(active_conf->definition_override_index);
						if (override_info) {
							const auto override_model_index = g_model_info->GetModelIndex(override_info->viewModel);
							data.m_Value.m_Int = override_model_index;
						}
					}
				}
			}
		}

		// Fake in model index call as good as we can (good enough I guess):
		g_modelindex_hook->get_original_function()(&data, view_model, &(view_model->GetModelIndex()));
	}
}
