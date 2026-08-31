#pragma once

#include <iostream>

#include <g3sdk/Script.h>
#include <g3sdk/util/Hook.h>
#include <g3sdk/util/Logging.h>
#include <g3sdk/util/Memory.h>
#include <g3sdk/util/ScriptUtil.h>

gSScriptInit & GetScriptInit();


class HotKeyTester :
	public eCEngineComponentBase
{
public: virtual void	Process ( void );
public: virtual			~HotKeyTester ( void );

private:
	static bTPropertyObject<HotKeyTester , eCEngineComponentBase> ms_PropertyObjectInstance_HotKeyTester;

public:
	HotKeyTester ( void );

private:
	HotKeyTester ( HotKeyTester const& );
	GEBool num1KeyPressed;
	GEBool num3KeyPressed;
	GEBool num5KeyPressed;
	HotKeyTester const& operator = ( HotKeyTester const& );
};