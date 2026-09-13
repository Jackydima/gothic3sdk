#pragma once

#include <g3sdk/Script.h>
#include <g3sdk/util/Hook.h>
#include <g3sdk/util/Logging.h>
#include <g3sdk/util/Memory.h>
#include <g3sdk/util/ScriptUtil.h>
#include <g3sdk/util/Util.h>

#include <iostream>

gSScriptInit &GetScriptInit();

struct RWConfig
{
    GEU32 G_Mode = 0;

    GEU32 ReqItemAmount = 1;

    GEFloat ReqGoldMultiplicator = 1.0f;
};

#ifndef GE_DEBUG
template <typename... Args>
void print(const char *, Args...)
{}

template <typename... Args>
void println(const char *, Args...)
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
