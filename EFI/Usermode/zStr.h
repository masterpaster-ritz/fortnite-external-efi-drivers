#pragma once
#define DIRECTINPUT_VERSION 0x0800
#undef _HAS_STD_BYTE 
#include <dinput.h>
#include <tchar.h>
#include <Dwmapi.h>
#include <Windows.h>
#include <Tlhelp32.h>
#include <string>
#include <windef.h>
#include <cmath>
#include <windef.h>
#include <Psapi.h> 
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <iostream>
#include <vector>

#define COMPILE_SEED 87345632764276848

template <int X>
struct EnsureCompileTime
{
	enum : int
	{
		Value = X
	};
};

constexpr int LinearCongruentGenerator(int rounds)
{
	return 1013904223 + 1664525 * ((rounds > 0) ? LinearCongruentGenerator(rounds - 1) : COMPILE_SEED & 0xFFFFFFFF);
}

#define Random() EnsureCompileTime<LinearCongruentGenerator(10)>::Value
#define RandomNumber(Min, Max) (Min + (Random() % (Max - Min + 1)))

const BYTE g_xorKey = static_cast<BYTE>(RandomNumber(0, 0xFF));

template <int... Pack> struct IndexList {};

template <typename IndexList, int Right> struct Append;

template <int... Left, int Right> struct Append<IndexList<Left...>, Right>
{
	typedef IndexList<Left..., Right> Result;
};

template <int N> struct ConstructIndexList
{
	typedef typename Append<typename ConstructIndexList<N - 1>::Result, N - 1>::Result Result;
};

template <> struct ConstructIndexList<0>
{
	typedef IndexList<> Result;
};

constexpr char EncryptCharacter(const char character, int index)
{
	return character ^ (g_xorKey + index);
}

constexpr wchar_t EncryptWCharacter(const wchar_t character, int index)
{
	return character ^ (g_xorKey + index);
}

template <typename IndexList> class MetaString;

template <int... Index> class MetaString<IndexList<Index...> >
{
public:
	__forceinline constexpr MetaString(const char* const value)
		: m_value{ EncryptCharacter(value[Index], Index)... }
	{
	}

	char* Decrypt()
	{
		for (int t = 0; t < sizeof...(Index); t++)
		{
			m_value[t] = m_value[t] ^ (g_xorKey + t);
		}

		m_value[sizeof...(Index)] = '\0';

		return m_value;
	}

private:
	char m_value[sizeof...(Index) + 1];
};

template <typename IndexList> class MetaWString;

template <int... Index> class MetaWString<IndexList<Index...> >
{
public:
	__forceinline constexpr MetaWString(const wchar_t* const value)
		: m_value{ EncryptWCharacter(value[Index], Index)... }
	{
	}

	wchar_t* Decrypt()
	{
		for (int t = 0; t < sizeof...(Index); t++)
		{
			m_value[t] = m_value[t] ^ (g_xorKey + t);
		}

		m_value[sizeof...(Index)] = '\0';

		return m_value;
	}

private:
	wchar_t m_value[sizeof...(Index) + 1];
};

#define z(x) (MetaString<ConstructIndexList<sizeof(x) - 1>::Result>(x).Decrypt()) 
#define METAWSTRING(x) (MetaWString<ConstructIndexList<sizeof(x) - 1>::Result>(x).Decrypt()) 
