#include "PlayerUpdateBridge.h"
#include "ScriptMgr.h"

class ChipaPlayerbotUpdateScript : public PlayerScript
{
public:
    ChipaPlayerbotUpdateScript() : PlayerScript("ChipaPlayerbotUpdateScript") { }

    void OnUpdate(Player* player, uint32 diff) override
    {
        chipa::playerbots::DispatchPlayerUpdate(player, static_cast<std::uint32_t>(diff));
    }
};

void AddChipaPlayerbotUpdateScript()
{
    new ChipaPlayerbotUpdateScript();
}
