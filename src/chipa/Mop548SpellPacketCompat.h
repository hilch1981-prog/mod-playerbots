#ifndef CHIPA_MOP548_SPELL_PACKET_COMPAT_H
#define CHIPA_MOP548_SPELL_PACKET_COMPAT_H

namespace chipa
{
namespace mop548
{
// MoP 5.4.8 target-native readers for the two spell notifications consumed
// by PlayerbotAI::HandleBotOutgoingPacket. The GUID mask/byte orders mirror
// the authoritative MOP_V2_Repack Spell.cpp packet producers; callers keep
// the target ObjectGuid/WorldPacket types so no modern AzerothCore GUID API
// leaks into the generic bridge.
template <class Packet, class Guid, class Result, class SpellId, class CastCount>
void ReadSpellFailure(Packet& packet, Guid& casterGuid, Result& result, SpellId& spellId, CastCount& castCount)
{
    packet.ReadGuidMask(casterGuid, 7, 3, 6, 2, 1, 5, 0, 4);
    packet.ReadGuidBytes(casterGuid, 2, 6, 7, 0, 3, 1);
    packet >> result;
    packet >> spellId;
    packet >> castCount;
    packet.ReadGuidBytes(casterGuid, 4, 5);
}

template <class Packet, class Guid, class Delay>
void ReadSpellDelayed(Packet& packet, Guid& casterGuid, Delay& delay)
{
    packet.ReadGuidMask(casterGuid, 6, 7, 2, 0, 4, 3, 1, 5);
    packet.ReadGuidBytes(casterGuid, 2, 6, 1, 7, 0, 5, 3);
    packet >> delay;
    packet.ReadGuidBytes(casterGuid, 4);
}
}
}

#endif
