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

class gSArgsFor__AI_EvadeRight : public gScrArgument
{
  public:
    ~gSArgsFor__AI_EvadeRight() override = default;

  public:
    Entity m_Self;
    Entity m_Other;
};

class gSArgsFor__AI_EvadeLeft : public gScrArgument
{
  public:
    ~gSArgsFor__AI_EvadeLeft() override = default;

  public:
    Entity m_Self;
    Entity m_Other;
};
