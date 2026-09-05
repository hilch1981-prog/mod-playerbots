#include "PlayerLifecycleBridge.h"
#include "PlayerUpdateBridge.h"
#include "ScriptMgr.h"

class ChipaPlayerbotUpdateScript : public PlayerScript
{
public:
    ChipaPlayerbotUpdateScript() : PlayerScript("ChipaPlayerbotUpdateScript") { }

    void OnLogin(Player* player) override
    {
        chipa::playerbots::DispatchPlayerLogin(player);
    }

    void OnLogout(Player* player) override
    {
        chipa::playerbots::DispatchPlayerLogout(player);
    }

    void OnUpdate(Player* player, uint32 diff) override
    {
        chipa::playerbots::DispatchPlayerUpdate(player, static_cast<std::uint32_t>(diff));
    }
};

void AddChipaPlayerbotUpdateScript()
{
    new ChipaPlayerbotUpdateScript();
}
