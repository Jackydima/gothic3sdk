#include "EXP_GUI.h"

GEBool EXPGUI::OnInitDialog(void)
{
    CFFGFCBitmap barBack;
    barBack.Create("G3_HUD_EXP_Back.tga");
    bCRect rect;
    this->expLocation.GetWindowRect(rect);
    this->expLocation.ShowWindow(GEFalse);

    this->backGround.Create("", 0x10, rect, this, 1000);
    this->backGround.SetBitmap(barBack.GetHandle());
    this->backGround.ShowWindow(GETrue);
    // this->backGround.BringWindowToTop ( );

    this->expBar.Create("", 268435456, bCRect(5, 2, 8 + 195, 2 + 16), &this->backGround,
                        1001); // Local location of Barbackground
    this->expBar.SetBkColor(0);
    this->expBar.SetBarColor(0xAAAA00AA); // Color ARGB
    this->expBar.SetRange(0, 100);
    GEInt val = 0;
    this->expBar.SetPos(val);
    this->expBar.ShowWindow(GETrue);

    CFFGFCBitmap barFront;
    barFront.Create("G3_HUD_EXP_Front.tga");
    this->foreGround.Create("", 0x10, rect, this, 1000);
    this->foreGround.SetBitmap(barFront.GetHandle());
    this->foreGround.ShowWindow(GETrue);
    this->foreGround.BringWindowToTop();

    gCHUDPage::OnInitDialog();
    return GETrue;
}

void EXPGUI::OnCreate()
{
    // std::cout << "initiliazed EXPGUI\n";
}

void EXPGUI::DoDataExchange(CFFGFCDataExchange *data)
{
    // std::cout << "DataExchange\n";
}

GEInt NeededXP(GEInt level)
{
    return static_cast<GEInt>(500 * ((level + 2) * 0.5f) * (level + 1));
}

void EXPGUI::OnPaint()
{
    Entity Player = Entity::GetPlayer();
    if (Player == None)
        return;

    GEInt XP = Player.PlayerMemory.GetProperty<PSPlayerMemory::PropertyXP>();
    GEInt currentLevel = Player.NPC.GetProperty<PSNpc::PropertyLevel>();
    GEInt val = static_cast<GEInt>(static_cast<GEFloat>(static_cast<GEFloat>(XP - NeededXP(currentLevel - 1))
                                                        / (NeededXP(currentLevel) - NeededXP(currentLevel - 1)))
                                   * 100.0f);
    this->expBar.SetPos(val);
}

EXPGUI::~EXPGUI()
{}
