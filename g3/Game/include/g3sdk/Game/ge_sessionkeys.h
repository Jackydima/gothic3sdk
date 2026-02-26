#pragma once

#include <Game/GameEnum.h>

#include <Engine.h>

class gCSession;

class GE_DLLIMPORT gCSessionKeys : public bCObjectRefBase
{
    // clang-format off
    public: virtual void CopyFrom(gCSessionKeys const &);
    public: bEResult Write(bCOStream &) override;
    public: bEResult Read(bCIStream &) override;
    public: void Destroy() override;
    public: bEResult Create() override;
    public: ~gCSessionKeys() override;
    // clang-format on

  public:
      using THIS_CLASS = gCSessionKeys;
      using BASE_CLASS = bCObjectRefBase;

    public:
      static bCPropertyObjectBase const *__stdcall GetRootObject();

    public:
      gCSessionKeys();
      //gCSessionKeys(gCSessionKeys const &);

    public:
      gCSessionKeys const &operator=(gCSessionKeys const &);

    private:
      static bTPropertyObject<gCSessionKeys, bCObjectRefBase> ms_PropertyObjectInstance_gCSessionKeys;

  public:
    explicit gCSessionKeys(gCSession &);

  protected:
    gCSessionKeys(gCSessionKeys const &);

  public:
    bCOStream &operator>>(bCOStream &);
    bCIStream &operator<<(bCIStream &);

  public:
    void AssignKey(gESessionKey, bCUnicodeString &, bTPtrArray<eCPhysicalKey *> &);
    void CreateDefaultKeys();
    void CreateINIKeys();
    eCPhysicalKey *GetAssignedKey(gESessionKey, GEInt);
    eCPhysicalKey *GetPressedKey();
    void Invalidate();

  protected:
    void AssignSingleKey(gESessionKey, bCString, eSSetupEngine::SPhysicalKeys &);

  private:
    GE_UNIMPL_MEMBERS(4)
};

GE_ASSERT_SIZEOF(gCSessionKeys, 0x10)
