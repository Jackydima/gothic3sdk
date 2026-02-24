#pragma once

#include <g3sdk/Script.h>
#include <g3sdk/util/Hook.h>
#include <g3sdk/util/Logging.h>
#include <g3sdk/util/Memory.h>

#include <iostream>

class MyView : public CFFGFCView
{
  public:
    GEBool OnInitDialog(void) override;

  protected:
    virtual GEInt WindowProc(GEUInt, GEUInt, GEUInt);

  public:
    void OnPaint() override;
    void DoDataExchange(CFFGFCDataExchange *) override;
    ~MyView(void);

  public:
    MyView(MyView const &);
    MyView(void);

  public:
    Entity Focus;
    CFFGFCStatic focusStaminaBack;
    CFFGFCProgressBar staminaBar;
};

void InitGUI();
void DoIt(CFFGFCWnd *a0);
