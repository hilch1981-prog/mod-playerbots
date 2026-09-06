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

// Integration-facing form used by PlayerbotAI call sites. Parse the complete
// target packet first, then report whether it belongs to the expected caster.
// Keeping this comparison beside the target-native reader prevents call sites
// from reintroducing donor ReadAsPacked() parsing while preserving the existing
// "ignore other casters" behavior.
template <class Packet, class Guid, class Result, class SpellId, class CastCount>
bool ReadSpellFailureForCaster(Packet& packet, Guid const& expectedCaster, Result& result, SpellId& spellId,
                               CastCount& castCount)
{
    Guid casterGuid;
    ReadSpellFailure(packet, casterGuid, result, spellId, castCount);
    return casterGuid == expectedCaster;
}

template <class Packet, class Guid, class Delay>
void ReadSpellDelayed(Packet& packet, Guid& casterGuid, Delay& delay)
{
    packet.ReadGuidMask(casterGuid, 6, 7, 2, 0, 4, 3, 1, 5);
    packet.ReadGuidBytes(casterGuid, 2, 6, 1, 7, 0, 5, 3);
    packet >> delay;
    packet.ReadGuidBytes(casterGuid, 4);
}

template <class Packet, class Guid, class Delay>
bool ReadSpellDelayedForCaster(Packet& packet, Guid const& expectedCaster, Delay& delay)
{
    Guid casterGuid;
    ReadSpellDelayed(packet, casterGuid, delay);
    return casterGuid == expectedCaster;
}
}
}

#endif
