#pragma once
#include "Script.h"

#include <g3sdk/util/Hook.h>
#include <g3sdk/util/Logging.h>
#include <g3sdk/util/Memory.h>

#include <iostream>

#define IDC_STATIC_EXP_BAR 20843
#define IDC_STATIC_EXP 20844

class __declspec(dllexport) EXPGUI : public gCHUDPage
{
  public:
    GEBool OnInitDialog(void) override;
    // public: void   SetEntity ( eCEntity* , gEHUDEntity ) override;
  public:
    void OnCreate() override;

  public:
    void DoDataExchange(CFFGFCDataExchange *) override;

  public:
    void OnPaint() override;

  public:
    ~EXPGUI();

  public:
    CFFGFCProgressBar expBar;
    CFFGFCStatic backGround;
    CFFGFCStatic foreGround;
    CFFGFCWnd expLocation;
};
