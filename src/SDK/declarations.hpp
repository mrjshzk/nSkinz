#pragma once
#include <cstddef>
#include <array>

template <typename Fn = void*>
Fn get_vfunc(void* class_base, const std::size_t index)
{
	return (*reinterpret_cast<Fn**>(class_base))[index];
}

namespace sdk
{
	class C_BaseEntity;
	class ClientClass;
	class ClientClass;
	class IClientAlphaProperty;
	class IClientEntity;
	class IClientEntity;
	class IClientNetworkable;
	class IClientNetworkable;
	class IClientRenderable;
	class IClientThinkable;
	class IClientUnknown;
	class ICollideable;
	class IGameEvent;
	class KeyValues;
	class RecvProp;
	class RecvTable;

	struct AppSystemInfo_t;
	struct CRecvProxyData;
	struct bf_read;
	struct bf_read;
	struct bf_write;
	struct model_t;

	using CBaseHandle = unsigned long;
	using CreateClientClassFn = IClientNetworkable* (*)(int, int);
	using CreateEventFn = IClientNetworkable* (*)();
	using CreateInterfaceFn = void* (*)(const char*, int*);
	using LocalizeStringIndex_t = unsigned;
	using RecvVarProxyFn = void(__cdecl *)(const CRecvProxyData*, void*, void*);
	using Vector = std::array<float, 3>;


	constexpr static auto MAX_EDICT_BITS = 11;

	constexpr static auto NUM_NETWORKED_EHANDLE_SERIAL_NUMBER_BITS = 10;
	constexpr static auto NUM_NETWORKED_EHANDLE_BITS = MAX_EDICT_BITS + NUM_NETWORKED_EHANDLE_SERIAL_NUMBER_BITS;
	constexpr static auto INVALID_NETWORKED_EHANDLE_VALUE = (1 << NUM_NETWORKED_EHANDLE_BITS) - 1;

	constexpr static auto NUM_SERIAL_NUM_BITS = 16; // (32 - NUM_ENT_ENTRY_BITS)
	constexpr static auto NUM_SERIAL_NUM_SHIFT_BITS = 32 - NUM_SERIAL_NUM_BITS;
	constexpr static auto INVALID_EHANDLE_INDEX = 0xFFFFFFFF;
	constexpr static auto MAX_PLAYER_NAME_LENGTH = 128;
	constexpr static auto SIGNED_GUID_LEN = 32;
	constexpr static auto MAX_WEAPONS = 48;
	constexpr static auto MAX_PLAYERS = 65; // Only cstrike and csgo

	enum class LifeState
	{
		ALIVE = 0,
		DYING,
		DEAD,
		RESPAWNABLE,
		DISCARDBODY,
	};
}