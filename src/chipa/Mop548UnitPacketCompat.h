#ifndef CHIPA_MOP548_UNIT_PACKET_COMPAT_H
#define CHIPA_MOP548_UNIT_PACKET_COMPAT_H

namespace chipa
{
namespace mop548
{
// MoP 5.4.8 target-native reader for SMSG_DISMOUNT as produced by
// MOP_V2_Repack Unit::Dismount(). The donor used modern
// ObjectGuid::ReadAsPacked(); this keeps the target WorldPacket/ObjectGuid
// representation and mirrors the live SkyFire mask/byte order instead.
template <class Packet, class Guid>
void ReadDismount(Packet& packet, Guid& guid)
{
    packet.ReadGuidMask(guid, 6, 3, 0, 7, 1, 2, 5, 4);
    packet.ReadGuidBytes(guid, 3, 6, 7, 5, 1, 4, 2, 0);
}

// MoP 5.4.8 SMSG_EMOTE is not packed-GUID encoded in this runtime. Unit.cpp
// writes uint32 emote id followed by the raw uint64 object GUID. Keep parsing
// isolated here so the donor's modern ObjectGuid::IsPlayer() assumption can be
// replaced at integration time with the target-native IS_PLAYER_GUID(uint64)
// predicate from ObjectDefines.h.
template <class Packet, class EmoteId, class Guid>
void ReadEmote(Packet& packet, EmoteId& emoteId, Guid& guid)
{
    packet >> emoteId >> guid;
}
}
}

#endif
