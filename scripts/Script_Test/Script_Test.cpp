#include "Script_Test.h"
#define _USE_MATH_DEFINES
#include <math.h>

CFFGFCView *test;
CFFGFCStatic g_Label;
GEInt Top;
GEInt Right;

gSScriptInit &GetScriptInit()
{
    static gSScriptInit s_ScriptInit;
    return s_ScriptInit;
}

HotKeyTester::HotKeyTester()
{
    eCModuleAdmin::GetInstance().RegisterModule(*this);
    this->num1KeyPressed = GEFalse;
    this->num3KeyPressed = GEFalse;
    this->num5KeyPressed = GEFalse;
}

HotKeyTester::~HotKeyTester()
{}

bTPropertyObject<HotKeyTester, eCEngineComponentBase> HotKeyTester::ms_PropertyObjectInstance_HotKeyTester(GETrue);

void TestEffectMapper();
void RagdollTest();
void TestAnimationTime();
void CheckFocusBodyStats();

void TestMatrix();

void HotKeyTester::Process()
{
    if (!gCSession::GetSession().IsValid() || gCSession::GetSession().IsPaused()
        || !gCSession::GetSession().GetGUIManager())
    {
        return;
    }

    if (eCApplication::GetInstance().GetKeyboard().KeyPressed(eCInpShared::eEKeyboardStateOffset_NUM_1))
    {
        TestMatrix();
        if (!num1KeyPressed)
        {
            num1KeyPressed = GETrue;
        }
    }
    else
    {
        num1KeyPressed = GEFalse;
    }

    if (eCApplication::GetInstance().GetKeyboard().KeyPressed(eCInpShared::eEKeyboardStateOffset_NUM_3))
    {
        if (!num3KeyPressed)
        {
            num3KeyPressed = GETrue;
        }
    }
    else
    {
        num3KeyPressed = GEFalse;
    }

    if (eCApplication::GetInstance().GetKeyboard().KeyPressed(eCInpShared::eEKeyboardStateOffset_NUM_5))
    {
        if (!num5KeyPressed)
        {
            num5KeyPressed = GETrue;
        }
    }
    else
    {
        num5KeyPressed = GEFalse;
    }
}

void TestMatrix()
{
    Entity Player = Entity::GetPlayer();
    Entity PlayerTarget = Player.NPC.GetCurrentTarget();

    if (PlayerTarget == None || !PlayerTarget.NPC.IsValid())
        return;

    eCVisualAnimation_PS *VA = static_cast<eCVisualAnimation_PS *>(PlayerTarget.Animation.m_pEngineEntityPropertySet);
    bCString actor = VA->GetActor()->GetActorName();
    actor.GetWord(1, "_", actor, GETrue, GETrue);
    if (actor.CompareFast("Boar"))
        actor = bCString("Keiler");
    if (actor.CompareFast("Alligator"))
        actor = bCString("Aligator");
    bCString spineName = actor + "_Spine_Spine_1";
    bCMatrix targetBoneMatrix;
    GEBool gotBoneMatrix = VA->GetBoneMatrix(spineName, targetBoneMatrix);
    if (!gotBoneMatrix)
        return;

    Entity Camera = Entity::GetCameraEntity();
    gCCameraAI_PS *Camera_PS = static_cast<gCCameraAI_PS *>(Camera.Camera.m_pEngineEntityPropertySet);
    auto &CameraBase = eCApplication::GetInstance().GetCurrentCamera();
    GEFloat FOV = CameraBase.GetFieldOfView();

    if (Camera == None)
        return;

    auto SensX = Camera_PS->GetMouseSensitivityX();
    auto SensY = Camera_PS->GetMouseSensitivityY();

    std::cout << "MouseSense x: " << SensX << "\ty: " << SensY << "\n";
    std::cout << "FOV: " << FOV * 360.0f / (2 * static_cast<GEFloat>(M_PI)) << "\n";

    bCMatrix ViewMatrix = Camera.GetGameEntity()->GetWorldMatrix();
    bCVector TargetPoint = targetBoneMatrix.GetTranslation();

    ViewMatrix.Invert();

    // MANUAL INVERT
    /*bCMatrix3 InvertedRotationMatrix;
    InvertedRotationMatrix.AccessXAxis() = ViewMatrix.GetXAxis();
    InvertedRotationMatrix.AccessYAxis() = ViewMatrix.GetYAxis();
    InvertedRotationMatrix.AccessZAxis() = ViewMatrix.GetZAxis();
    InvertedRotationMatrix.Transpose();

    bCVector4 XAxis = {InvertedRotationMatrix.GetXAxis().GetX(), InvertedRotationMatrix.GetXAxis().GetY(),
    InvertedRotationMatrix.GetXAxis().GetZ(), 0}; bCVector4 YAxis = {InvertedRotationMatrix.GetYAxis().GetX(),
    InvertedRotationMatrix.GetYAxis().GetY(), InvertedRotationMatrix.GetYAxis().GetZ(), 0}; bCVector4 ZAxis =
    {InvertedRotationMatrix.GetZAxis().GetX(), InvertedRotationMatrix.GetZAxis().GetY(),
    InvertedRotationMatrix.GetZAxis().GetZ(), 0}; bCVector4 Translation = { -InvertedRotationMatrix.GetXAxis().GetX() *
    ViewMatrix.GetTranslation().GetX()
        + -InvertedRotationMatrix.GetYAxis().GetX() * ViewMatrix.GetTranslation().GetY()
        + -InvertedRotationMatrix.GetZAxis().GetX() * ViewMatrix.GetTranslation().GetZ(),
        -InvertedRotationMatrix.GetXAxis().GetY() * ViewMatrix.GetTranslation().GetX()
        + -InvertedRotationMatrix.GetYAxis().GetY() * ViewMatrix.GetTranslation().GetY()
        + -InvertedRotationMatrix.GetZAxis().GetY() * ViewMatrix.GetTranslation().GetZ(),
        -InvertedRotationMatrix.GetXAxis().GetZ() * ViewMatrix.GetTranslation().GetX()
        + -InvertedRotationMatrix.GetYAxis().GetZ() * ViewMatrix.GetTranslation().GetY()
        + -InvertedRotationMatrix.GetZAxis().GetZ() * ViewMatrix.GetTranslation().GetZ(),
        1.0f
    };

    ViewMatrix.AccessXAxis4() = XAxis;
    ViewMatrix.AccessYAxis4() = YAxis;
    ViewMatrix.AccessZAxis4() = ZAxis;
    ViewMatrix.AccessTranslation4() = Translation;*/
    // MANUAL INVERT

    GEFloat LocalX = ViewMatrix.GetXAxis4().GetX() * TargetPoint.GetX()
                   + ViewMatrix.GetYAxis4().GetX() * TargetPoint.GetY()
                   + ViewMatrix.GetZAxis4().GetX() * TargetPoint.GetZ() + ViewMatrix.GetTranslation4().GetX();

    GEFloat LocalY = ViewMatrix.GetXAxis4().GetY() * TargetPoint.GetX()
                   + ViewMatrix.GetYAxis4().GetY() * TargetPoint.GetY()
                   + ViewMatrix.GetZAxis4().GetY() * TargetPoint.GetZ() + ViewMatrix.GetTranslation4().GetY();

    GEFloat LocalZ = ViewMatrix.GetXAxis4().GetZ() * TargetPoint.GetX()
                   + ViewMatrix.GetYAxis4().GetZ() * TargetPoint.GetY()
                   + ViewMatrix.GetZAxis4().GetZ() * TargetPoint.GetZ() + ViewMatrix.GetTranslation4().GetZ();

    GEFloat LocalW = ViewMatrix.GetXAxis4().GetW() * TargetPoint.GetX()
                   + ViewMatrix.GetYAxis4().GetW() * TargetPoint.GetY()
                   + ViewMatrix.GetZAxis4().GetW() * TargetPoint.GetZ() + ViewMatrix.GetTranslation4().GetW();

    bCVector4 LocalPoint = {LocalX, LocalY, LocalZ, LocalW};

    std::cout << "ViewMatrix x:= " << ViewMatrix.AccessTranslation().GetX()
              << "\ty=" << ViewMatrix.AccessTranslation().AccessY()
              << "\tz=" << ViewMatrix.AccessTranslation().AccessZ() << "\n";
    std::cout << "ViewMatrix-Z-Axis x:= " << ViewMatrix.AccessZAxis().AccessX()
              << "\ty=" << ViewMatrix.AccessZAxis().AccessY() << "\tz=" << ViewMatrix.AccessZAxis().AccessZ() << "\n";

    std::cout << "LocalDirectionVec x:= " << LocalPoint.GetX() << "\ty=" << LocalPoint.GetY()
              << "\tz=" << LocalPoint.GetZ() << "\tw= " << LocalPoint.GetW() << "\n";
    
    if (LocalPoint.GetZ() <= 0.1f) 
        return;

    GEFloat aspectRatio = static_cast<GEFloat>(Right) / Top;

    GEFloat ScreenY =
        (LocalPoint.GetY() * (-Top / 2.0f)) / (LocalPoint.GetZ() * tan(CameraBase.GetFieldOfView() / 2.0f))
        + Top / 2.0f;
    GEFloat ScreenX =
        (LocalPoint.GetX() * (Right / 2.0f)) / (LocalPoint.GetZ() * tan(CameraBase.GetFieldOfView() / 2.0f) * 16 / 9)
        + Right / 2.0f;

    std::cout << "ScreenVec x:= " << ScreenX << "\ty=" << ScreenY << "\n";

    bCRect rect = {bCPoint(ScreenX - 20, ScreenY - 20), bCPoint(ScreenX + 20, ScreenY + 20)};
    g_Label.MoveWindow(rect);
    g_Label.SetWindowTextA(bCUnicodeString(PlayerTarget.GetName()));
}

void CheckFocusBodyStats()
{
    Entity Player = Entity::GetPlayer();
    Entity Focus = Player.Focus.GetFocusEntity();
    if (Focus == None)
        return;
    gCInventory_PS *Inv = (gCInventory_PS *)Focus.Inventory.m_pEngineEntityPropertySet;
    if (Inv == nullptr)
        return;

    std::cout << "Body Entity Valid? " << Inv->GetDefaultSlot(gESlot_Body)->AccessTemplate().GetEntity()->IsValid()
              << "\n";
    if (!Inv->GetDefaultSlot(gESlot_Body)->AccessTemplate().GetEntity()->IsValid())
        return;
    auto TemplateEntity = Inv->GetDefaultSlot(gESlot_Body)->AccessTemplate().GetEntity();
    std::cout << "Body Name " << TemplateEntity->GetName() << "\n";
    gCItem_PS *BodyItem = (gCItem_PS *)TemplateEntity->GetPropertySet(eEPropertySetType_Item);
    if (!BodyItem->IsValid())
        return;
    std::cout << BodyItem->AccessModAttrib1Tag() << BodyItem->AccessModAttrib1Value() << "\n";
    std::cout << BodyItem->AccessModAttrib2Tag() << BodyItem->AccessModAttrib2Value() << "\n";
    std::cout << BodyItem->AccessModAttrib3Tag() << BodyItem->AccessModAttrib3Value() << "\n";
    std::cout << BodyItem->AccessModAttrib4Tag() << BodyItem->AccessModAttrib4Value() << "\n";
    std::cout << BodyItem->AccessModAttrib5Tag() << BodyItem->AccessModAttrib5Value() << "\n";
    std::cout << BodyItem->AccessModAttrib6Tag() << BodyItem->AccessModAttrib6Value() << "\n";
}

void RagdollTest()
{
    Entity Player = Entity::GetPlayer();
    eCVisualAnimation_PS *va = (eCVisualAnimation_PS *)Player.Animation.m_pEngineEntityPropertySet;
    // va->SetRagDollEnabled ( GETrue , 0.0 , GETrue );
    bCMatrix Test;
    GEBool Worked = va->GetBoneMatrix("Spine_Spine_1", Test);
    if (Worked)
        va->AddForceAtPos(Player.GetPose().AccessZAxis() * -55000, Test.GetTranslation());
    else
        va->AddForceAtPos(Player.GetPose().AccessZAxis() * -55000, Player.GetPosition());
    Entity::GetPlayer().SetMovementMode(gECharMovementMode_RagDollDead);
}

void TestAnimationTime()
{
    Entity Player = Entity::GetPlayer();
    eCVisualAnimation_PS *va = (eCVisualAnimation_PS *)Player.Animation.m_pEngineEntityPropertySet;
    // bCString* ptrCurrentMotionDescription = ( bCString* )( *( GEU32* )( ( GEU32 )va + 0xE8 ) + 0x4 );
    bCString ptrCurrentMotionDescription = va->GetMotionDesc((eCWrapper_emfx2Actor::eEMotionType)0).GetMotionFilename();

    auto actor = va->GetActor();
    std::cout << "MaxTime: " << actor->GetMaxTime((eCWrapper_emfx2Actor::eEMotionType)0)
              << "\tPlayTime: " << actor->GetPlayTime((eCWrapper_emfx2Actor::eEMotionType)0)
              << "\nAni: " << ptrCurrentMotionDescription.GetText() << "\n";
}

static mCFunctionHook Hook_GetString;
bCUnicodeString GetString(bCString *p_String1, bCString *p_String2)
{
    bCUnicodeString returnValue = Hook_GetString.GetOriginalFunction(&GetString)(p_String1, p_String2);
    // std::cout << "String: " << returnValue.GetAnsiText() << "\n";
    if (returnValue.IsEmpty())
    {
        bCString s = p_String1->GetText();
        // Check for Prequels, then remove them
        s.Delete(0, 3);
        returnValue = s.GetText();
    }
    return returnValue;
}

extern "C" __declspec(dllexport) gSScriptInit const *GE_STDCALL ScriptInit(void)
{
    GetScriptAdmin().LoadScriptDLL("Script_Game.dll");
    static bCAccessorCreator HotKeyTester(bTClassName<HotKeyTester>::GetUnmangled());

    CFFGFCWnd *dW = ((CFFGFCWnd *)0)->GetDesktopWindow();
    bCRect rect;
    // bCRect(static_cast<GEInt>(ScreenX)-5, static_cast<GEInt>(ScreenY)-5, static_cast<GEInt>(ScreenX)+5,
    // static_cast<GEInt>(ScreenY)+5)
    g_Label.Create(L"", 0x10, rect, dW, static_cast<GEU32>(-1));
    g_Label.ShowWindow(GETrue);
    g_Label.BringWindowToTop();

    dW->GetWindowRect(rect);
    std::cout << "DestkopWindow TopLeft x: " << rect.GetTopLeft().GetX() << "\ty: " << rect.GetTopLeft().GetY() << "\n";
    std::cout << "DestkopWindow ButtomRight x: " << rect.GetBottomRight().GetX()
              << "\ty: " << rect.GetBottomRight().GetY() << "\n";
    Top = rect.GetBottomRight().GetY();
    Right = rect.GetBottomRight().GetX();
    return &GetScriptInit();
}

//
// Entry Point
//

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID)
{
    switch (dwReason)
    {
        case DLL_PROCESS_ATTACH:
            ::DisableThreadLibraryCalls(hModule);
            AllocConsole();
            freopen_s((FILE **)stdout, "CONOUT$", "w", stdout);
            break;
        case DLL_PROCESS_DETACH: break;
    }
    return TRUE;
}
