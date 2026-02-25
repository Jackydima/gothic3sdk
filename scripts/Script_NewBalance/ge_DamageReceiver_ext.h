#pragma once

#include <g3sdk/Game.h>
#include <g3sdk/Script.h>
#include <g3sdk/util/Hook.h>
#include <g3sdk/util/Memory.h>
#include <g3sdk/util/Module.h>
#include <g3sdk/util/Util.h>

class __declspec(dllexport) gCDamageReceiver_PS_Ext : public gCDamageReceiver_PS
{
  public:
    GE_DEFINE_PROPERTY(gCDamageReceiver_PS, GEU32, m_PoisonDamage, PoisonDamage)
    GE_DEFINE_PROPERTY(gCDamageReceiver_PS, GEU32, m_VulnerableState, VulnerableState)

  public:
    gCDamageReceiver_PS_Ext(void);

  protected:
    void Invalidate(void);
};


GE_ASSERT_PROPERTY(gCDamageReceiver_PS_Ext, m_PoisonDamage, 0x54, 0x4)
GE_ASSERT_PROPERTY(gCDamageReceiver_PS_Ext, m_VulnerableState, 0x58, 0x4)
GE_ASSERT_SIZEOF(gCDamageReceiver_PS_Ext, 0x54 + 0x8)
