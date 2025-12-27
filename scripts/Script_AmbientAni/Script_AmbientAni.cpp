#include "Script_AmbientAni.h"

gSScriptInit &GetScriptInit()
{
    static gSScriptInit s_ScriptInit;
    return s_ScriptInit;
}

static bCString g_Zombie = "Zombie";

// Helper struct for allocating correct size of AniObject!
struct AniObject_Type
{
    char buffer[0x1C];
};

// SuperHACK!!!
// We want to adjust the Animations-String in the GetMotionDataEntity
// In the regular game the character animation is stored in the esi-register that is usually preserved for the callee (scary to use but whatever)
LPVOID Call_GetMotionDataEntity(LPVOID CharacterAnimation, LPVOID AniObjectPtr,
                                gEAniState p_aniState,
                                gEUseType p_UseTypeLeft, gEUseType p_UseTypeRight, gEPose p_Pose, gEAction p_Action,
                                gEPhase p_Phase, gEDirection p_Direction, GEInt p_Variation,
                                gEAmbientAction p_AmbientAction)
{
    LPVOID result;
    DWORD GetMotionDataEntity_Func = RVA_Game(0xd94b0);

    __asm {
        push esi // preserve non-volatile register
        mov  esi, CharacterAnimation

        push p_AmbientAction
        push p_Variation
        push p_Direction
        push p_Phase
        push p_Action
        push p_Pose
        push p_UseTypeRight
        push p_UseTypeLeft
        push p_aniState
        push AniObjectPtr

        call GetMotionDataEntity_Func // GetMotionDataEntity

        mov result, eax // return value
        pop esi
    }

    return result;
}


static mCFunctionHook Hook_Pre_GetMotionDataEntity;
// Rebuilt function of Pre_GetMotionDataEntity of Game.dll
LPVOID Pre_GetMotionDataEntity(gEAniState p_aniState, gEUseType p_UseTypeLeft, gEUseType p_UseTypeRight,
    gEPose p_Pose,
    gEAction p_Action, gEPhase p_Phase, gEDirection p_Direction, GEInt p_Variation,
                               gEAmbientAction p_AmbientAction)
{
    LPVOID CharacterAnimation = Hook_Pre_GetMotionDataEntity.GetEsi<LPVOID>();
    if (CharacterAnimation == NULL)
    {
        return Hook_Pre_GetMotionDataEntity.GetOriginalFunction(&Pre_GetMotionDataEntity)(
            p_aniState, p_UseTypeLeft, p_UseTypeRight, p_Pose, p_Action, p_Phase, p_Direction, p_Variation,
            p_AmbientAction);
    }

    gCCharacterMovement_PS *CharacterMovement = ReadPtr<gCCharacterMovement_PS *>(CharacterAnimation, 0x14);
    //println("CharacterMovement PTR: %x", CharacterMovement);
    //println("CharacterAnimation PTR: %x", CharacterAnimation);
    
    LPVOID AniObjectPtr = GetOffsetPtr<LPVOID>(CharacterAnimation, 0x40);
    //println("AniObjectPtr PTR: %x", AniObjectPtr);

    if (ReadPtr<LPVOID>(AniObjectPtr) == NULL)
    {
        bCString aniActorName;

        eCEntity* selfEntity = CharacterMovement->GetEntity();
        if (selfEntity == NULL)
        {
            return NULL;
        }
        //println("Self = %s", selfEntity->GetName());
        eCVisualAnimation_PS *vA = (eCVisualAnimation_PS *)selfEntity->GetPropertySet(eEPropertySetType_Animation);
        if (vA == NULL)
        {
            return NULL;
        }

        bCString resourceString = vA->GetResourceFilePath();
        //println("resourceString PTR: %s", resourceString.GetText());
        resourceString.GetWord(1, "_", aniActorName, GEFalse, GEFalse);

        gCNPC_PS *NPC = (gCNPC_PS *)selfEntity->GetPropertySet(eEPropertySetType_NPC);
        if (NPC == NULL)
        {
            return NULL;
        }

        gEBearing bearing = NPC->GetBearing();

        if (bearing == gEBearing_Babe)
        {
            aniActorName.SetText("Babe");
        }

        if (bearing == gEBearing_Zombie)
        {
            aniActorName.SetText("Zombie");
        }

        // 
        // CUSTOMIZED SECTION
        // 

        if (NPC->GetSpecies() == gESpecies_Zombie)
        {
            aniActorName.SetText("Zombie");
        }

        //
        // CUSTOMIZED SECTION END
        // 
        

        GEInt tableAmount = *(GEInt *)RVA_Game(0x3f46dc);
        //println("tableAmount PTR: %d", tableAmount);
        GEInt entryIndex = 0;


        if (0 < tableAmount)
        {
            bCString *currentActorString1;
            bCString *currentActorString2;
            LPVOID currentTable;
            do
            {
                LPVOID currentTable = ReadPtr<LPVOID>(ReadPtr<LPVOID>((LPVOID)RVA_Game(0x3f46d8)),entryIndex * 0x4);
                //println("CurrentTableEntry PTR: %x", currentTable);
                
                currentActorString1 = GetOffsetPtr<bCString *>(currentTable, 0x14);
                //println("currentActorString1: %s", currentActorString1->GetText());
                
                currentActorString2 = GetOffsetPtr<bCString *>(currentTable, 0x18);
                //println("currentActorString2: %s", currentActorString2->GetText());
                if (currentActorString1->CompareNoCase(aniActorName) == 0
                    && currentActorString2->CompareNoCase(aniActorName) == 0)
                {
                    //println("Found compatible Table Entry!");
                    
                    WritePtr(currentTable, AniObjectPtr);
                    //println("AniObjectPtr: %x", ReadPtr<LPVOID>(AniObjectPtr));

                    // Probably a reference counter for clean up!
                    GEInt *currentTableReferenceCounter = GetOffsetPtr<GEInt *>(currentTable, 0x10);
                    //println("currentTableReferenceCounter: %d", *currentTableReferenceCounter);
                    *currentTableReferenceCounter += 1;
                    break;
                }

                entryIndex++;
            } while (entryIndex < tableAmount);
        }

        if (ReadPtr<LPVOID>(AniObjectPtr) == NULL)
        {
            // Hack for Memory creation!
            LPVOID newAniObject = (LPVOID)GE_NEW(AniObject_Type);
            if (newAniObject == NULL)
            {
                //println("New Ani Object is null :(");
                WritePtr<LPVOID>(NULL, AniObjectPtr);
            }
            else
            {
                // Game.dll function for constructor method!
                // Custom call convetion with (eax) -> eax
                WritePtr(CallWithEAX((DWORD)newAniObject, RVA_Game(0xd9450)), AniObjectPtr);

                //println("AniObjectPtr: %x", ReadPtr<LPVOID>(AniObjectPtr));
            }
            bCString *currentActorString1 = GetOffsetPtr<bCString *>(ReadPtr<LPVOID>(AniObjectPtr), 0x14);
            bCString *currentActorString2 = GetOffsetPtr<bCString *>(ReadPtr<LPVOID>(AniObjectPtr), 0x18);
            
            currentActorString1->SetText(aniActorName);
            //println("Final ActorString1: %s", currentActorString1->GetText());
            currentActorString2->SetText(aniActorName);
            //println("Final ActorString2: %s", currentActorString2->GetText());

            // Probably a reference counter for clean up!

            GEInt *currentTableReferenceCounterPtr = GetOffsetPtr<GEInt *>(ReadPtr<LPVOID>(AniObjectPtr), 0x10);
            //println("currentTableReferenceCounter PTR: %x", currentTableReferenceCounterPtr);
            *currentTableReferenceCounterPtr = 1;

            GEInt *amountPtr = reinterpret_cast<GEInt *>(RVA_Game(0x3f46dc));
            //println("amountPtr: %x", amountPtr);
            GEInt amount = *amountPtr + 1;
            
            GEInt localArg = amount == 0 ? -1 : 0;

            using HelpFunction = void (__fastcall *) (GEInt p_Int1, GEInt p_Int2);

            HelpFunction HelpFunction_Inst = (HelpFunction)RVA_Game(0xdd020);
            HelpFunction_Inst(localArg, amount);
            //println("Helper Function done!");
            *amountPtr = amount;

            LPVOID cachedTablesPtr = ReadPtr<LPVOID>((LPVOID)RVA_Game(0x3f46d8));
            //println("cachedTablesPtr PTR: %x", cachedTablesPtr);
            // 
            //*(LPVOID*)((DWORD)(cachedTablesPtr)-0x4 + amount * 0x4) = ReadPtr<LPVOID>(AniObjectPtr);
            WritePtr(ReadPtr<LPVOID>(AniObjectPtr), cachedTablesPtr, -0x4 + amount * 0x4);

            //println("Added Entry into the table!");
        }
    }

    LPVOID resourceReturned = Call_GetMotionDataEntity(CharacterAnimation, ReadPtr<LPVOID>(AniObjectPtr),
                                                        p_aniState, p_UseTypeLeft, p_UseTypeRight, p_Pose, p_Action,
                                                        p_Phase, p_Direction, p_Variation, p_AmbientAction);
    if (resourceReturned != NULL)
    {
        using HelpFuntion = LPVOID(__thiscall*)(void*);
        void **vtable = *(void ***)resourceReturned;
        HelpFuntion helpFunction_Inst = (HelpFuntion)vtable[0x70 / 4];
        //println("FUNCTION DONE WITH SUCCESS!!!!!");
        return helpFunction_Inst(resourceReturned);
    }

    //println("FUNCTION DONE WITH NOOOOOOOO SUCCESS!!!!!");
    return NULL;
}

static mCFunctionHook Hook_GetMotionDataEntity;
LPVOID GetMotionDataEntity_Ext(LPVOID p_ThisObject, gEAniState p_aniState, gEUseType p_UseTypeLeft, gEUseType p_UseTypeRight,
                                  gEPose p_Pose,
    gEAction p_Action, gEPhase p_Phase, gEDirection p_Direction,
    GEInt p_Variation, gEAmbientAction p_AmbientAction)
{
    LPVOID characterAnimationPtr = Hook_GetMotionDataEntity.GetEsi<LPVOID>();
    if (p_ThisObject == NULL || characterAnimationPtr == NULL)
    {
        return Hook_GetMotionDataEntity.GetOriginalFunction(&GetMotionDataEntity_Ext)(
            p_ThisObject, p_aniState, p_UseTypeLeft, p_UseTypeRight, p_Pose, p_Action, p_Phase, p_Direction,
            p_Variation, p_AmbientAction);
    }
    gCCharacterMovement_PS* characterMovementPtr = *(gCCharacterMovement_PS**)((DWORD)(characterAnimationPtr) + 0x14);
    if (characterMovementPtr == NULL)
    {
        return Hook_GetMotionDataEntity.GetOriginalFunction(&GetMotionDataEntity_Ext)(
            p_ThisObject, p_aniState, p_UseTypeLeft, p_UseTypeRight, p_Pose, p_Action, p_Phase, p_Direction,
            p_Variation, p_AmbientAction);
    }
   
    Entity Self = Entity(characterMovementPtr->GetEntity());
    if (Self == None)
    {
        return Hook_GetMotionDataEntity.GetOriginalFunction(&GetMotionDataEntity_Ext)(
            p_ThisObject, p_aniState, p_UseTypeLeft, p_UseTypeRight, p_Pose, p_Action, p_Phase, p_Direction,
            p_Variation, p_AmbientAction);
    }

    bCString *actorAniString1 = (bCString *)((DWORD)(p_ThisObject) + 0x14);
    bCString *actorAniString2 = (bCString *)((DWORD)(p_ThisObject) + 0x18);
    //println("actorAniString1 Val: %s", actorAniString1->GetText());
    //println("actorAniString2 Val: %s", actorAniString2->GetText());


    //
    // CUSTOMIZED SECION
    //
    
    // 
    // CUSTOMIZED SECION END
    // 

    // TODO: Redo this function aswell!!

    LPVOID returnVal = Hook_GetMotionDataEntity.GetOriginalFunction(&GetMotionDataEntity_Ext)(
        p_ThisObject, p_aniState, p_UseTypeLeft, p_UseTypeRight, p_Pose, p_Action, p_Phase, p_Direction, p_Variation,
        p_AmbientAction);

    //println("Return Val: %x", returnVal);

    //println("Done!");
    return returnVal;
}


extern "C" __declspec(dllexport) gSScriptInit const *GE_STDCALL ScriptInit(void)
{

    Hook_Pre_GetMotionDataEntity.Hook(RVA_Game(0xda260), &Pre_GetMotionDataEntity, mCBaseHook::mEHookType_Mixed,
                                      mERegisterType_Esi);

    Hook_GetMotionDataEntity.Hook(RVA_Game(0xd94b0), &GetMotionDataEntity_Ext, mCBaseHook::mEHookType_Mixed, mERegisterType_Esi);
    
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
#ifdef GE_DEBUG
        AllocConsole();
        freopen_s((FILE **)stdout, "CONOUT$", "w", stdout);
        ::DisableThreadLibraryCalls(hModule);
#endif
        break;
    case DLL_PROCESS_DETACH: break;
    }
    return TRUE;
}
