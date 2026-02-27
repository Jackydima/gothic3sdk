#pragma once

#include <Script/gs_scriptargs.h>

class gSArgsFor__AI_EvadeBackward : public gScrArgument
{
  public:
    ~gSArgsFor__AI_EvadeBackward() override = default;

  public:
    Entity m_Self;
    Entity m_Other;
};
