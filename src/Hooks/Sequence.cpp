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
		if (it == m_SequenceMap.end()) it = m_SequenceMap.find(WEAPON_KNIFE); // use default knife as fallback.
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

extern std::map<sdk::C_BaseAttributableItem*, int> g_weapon_to_orgindex;

static auto do_sequence_remapping(sdk::C_BaseViewModel* entity, int nSequence) -> int
{
	const auto owner = get_entity_from_handle<sdk::C_BasePlayer>(entity->GetOwner());

	if (!(owner && owner->IsPlayer() && static_cast<sdk::C_BasePlayer*>(owner)->GetLifeState() == sdk::LifeState::ALIVE))
		return nSequence;

	const auto view_model_weapon = get_entity_from_handle<sdk::C_BaseAttributableItem>(entity->GetWeapon());

	if(!view_model_weapon)
		return nSequence;

	const auto weapon_info = game_data::get_weapon_info(view_model_weapon->GetItemDefinitionIndex());

	if(!weapon_info)
		return nSequence;

	const auto definition_index = view_model_weapon->GetItemDefinitionIndex();

	if (!is_knife(definition_index))
		return nSequence;

	sdk::player_info_t player_info;
	if (!g_engine->GetPlayerInfo(owner->GetIndex(), &player_info))
		return nSequence;

	const auto active_conf = g_config.get_by_definition_index(player_info.userid, WEAPON_KNIFE);
	if (nullptr == active_conf || 0 == active_conf->definition_override_index)
		return nSequence;

	int orgIndex = definition_index;
	auto it = g_weapon_to_orgindex.find(view_model_weapon);
	if (it != g_weapon_to_orgindex.end())
		orgIndex = it->second;
	return g_SequenceMap.MapSequence(orgIndex, nSequence, active_conf->definition_override_index);
}


static std::map<void*,vmt_multi_hook *> C_BaseViewModel_SetSequence_hooks;

void * g_setsequence_called = nullptr;
sdk::C_BaseViewModel* g_setsequence_This = nullptr;
int g_setsequence_value = 0;

auto __fastcall hooks::C_BaseViewModel_SetSequence::hooked(sdk::C_BaseViewModel* This, void* Edx, int nSequence) -> void
{
	auto &pHook = C_BaseViewModel_SetSequence_hooks[*(void**)This];
	g_setsequence_value = nSequence;
	g_setsequence_This = This;
	g_setsequence_called = pHook->get_original_function<void(__fastcall*)(sdk::C_BaseViewModel*, void*, int)>(219);
	int newSequence = do_sequence_remapping(This, nSequence);
	((void (__fastcall *)(sdk::C_BaseViewModel*, void*, int))g_setsequence_called)(This, Edx, newSequence);
}

void hook_C_BaseViewModel_SetSequence(sdk::C_BaseViewModel* thisptr) {

	if (C_BaseViewModel_SetSequence_hooks.contains(*(void**)thisptr)) return;

	vmt_multi_hook* pHook = new vmt_multi_hook();
	if (pHook->initialize_and_hook_instance(thisptr)) {
		pHook->hook_function(&hooks::C_BaseViewModel_SetSequence::hooked, 219);
		C_BaseViewModel_SetSequence_hooks[*(void**)thisptr] = pHook;
	}
}

bool get_fixup_view_model_index(sdk::C_BaseViewModel* view_model, int& outNewIndex, int& outOrgIndex);


bool g_modelindex_called = false;
int g_modelindex_value = 0;

auto __cdecl hooks::modelindex_proxy_fn(const sdk::CRecvProxyData* proxy_data_const, void* entity, void* output) -> void
{
	g_modelindex_called = true;
	g_modelindex_value = proxy_data_const->m_Value.m_Int;
}

auto __cdecl hooks::weapon_proxy_fn(const sdk::CRecvProxyData* proxy_data_const, void* entity, void* output) -> void
{
	const auto view_model = static_cast<sdk::C_BaseViewModel*>(entity);

	ensure_dynamic_hooks();
	hook_C_BaseViewModel_SetSequence(view_model);

	static auto original_fn = g_weapon_hook->get_original_function();

	original_fn(proxy_data_const, entity, output);
	
	if (g_modelindex_called) {
		g_modelindex_called = false;
		sdk::CRecvProxyData data;
		data.m_Value.m_Int = g_modelindex_value;

		int oldIndex;
		int newIndex;
		if (get_fixup_view_model_index(view_model, newIndex, oldIndex))
			data.m_Value.m_Int = newIndex;

		// Fake in model index call as good as we can (good enough I guess):
		g_modelindex_hook->get_original_function()(&data, view_model, &(view_model->GetModelIndex()));
	}

	if (g_setsequence_called && g_setsequence_This == view_model) {
		int newSequence = do_sequence_remapping(view_model, g_setsequence_value);
		((void(__fastcall*)(sdk::C_BaseViewModel*, void*, int))g_setsequence_called)(g_setsequence_This, 0, newSequence);
	}
	g_setsequence_called = nullptr;
}
