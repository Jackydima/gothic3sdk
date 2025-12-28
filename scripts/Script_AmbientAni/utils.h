#pragma once
#include "Script.h"
#include <g3sdk/util/Hook.h>
#include <g3sdk/util/Memory.h>
#include <iostream>

#ifndef GE_DEBUG
template <typename... Args>
void print(const char *format, Args... args)
{}

template <typename... Args>
void println(const char *format, Args... args)
{}
#else
template <typename... Args>
void print(const char *format, Args... args)
{
    std::fprintf(stdout, format, args...);
}
template <typename... Args>
void println(const char *format, Args... args)
{
    std::fprintf(stdout, format, args...);
    std::fprintf(stdout, "\n");
}
#endif


template <typename T>
inline T ReadPtr(LPVOID base, size_t offset = 0)
{
    return *reinterpret_cast<T *>(reinterpret_cast<uintptr_t>(base) + offset);
}

template <typename T>
inline void WritePtr(T val, LPVOID base, size_t offset = 0)
{
    *reinterpret_cast<T *>(reinterpret_cast<uintptr_t>(base) + offset) = val;
}

template <typename T>
inline T GetOffsetPtr(LPVOID base, size_t offset = 0)
{
    return reinterpret_cast<T>(reinterpret_cast<uintptr_t>(base) + offset);
}

DWORD CallWithEAX(DWORD value, DWORD adr);
