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

// Integration-facing form for PlayerbotAI::HandleBotOutgoingPacket. SkyFire
// packet GUIDs use ByteBuffer::ObjectGuid but gameplay objects expose raw
// uint64 GUIDs. Decode into the caller-provided packet Guid and compare via the
// target conversion rather than requiring an ObjectGuid equality operator.
template <class Packet, class Guid, class ExpectedGuid>
bool ReadDismountForGuid(Packet& packet, Guid& guid, ExpectedGuid expectedGuid)
{
    ReadDismount(packet, guid);
    return static_cast<ExpectedGuid>(guid) == expectedGuid;
}

// MoP 5.4.8 SMSG_MOVE_KNOCK_BACK is not a leading packed GUID. Unit.cpp
// writes the movement scalars first, then a MoP bit/byte GUID sequence:
//   speedXY -> vsin -> speedZ -> counter -> vcos -> GUID mask -> GUID bytes.
// Keep the wire order isolated here so PlayerbotAI does not retain the donor's
// modern ReadAsPacked() assumption.
template <class Packet, class Guid, class Counter>
void ReadMoveKnockBack(Packet& packet, Guid& guid, Counter& counter, float& vcos, float& vsin,
                       float& horizontalSpeed, float& verticalSpeed)
{
    packet >> horizontalSpeed;
    packet >> vsin;
    packet >> verticalSpeed;
    packet >> counter;
    packet >> vcos;
    packet.ReadGuidMask(guid, 2, 0, 7, 1, 4, 6, 5, 3);
    packet.ReadGuidBytes(guid, 6, 0, 7, 5, 4, 3, 1, 2);
}

template <class Packet, class Guid, class ExpectedGuid, class Counter>
bool ReadMoveKnockBackForGuid(Packet& packet, Guid& guid, ExpectedGuid expectedGuid, Counter& counter,
                              float& vcos, float& vsin, float& horizontalSpeed, float& verticalSpeed)
{
    ReadMoveKnockBack(packet, guid, counter, vcos, vsin, horizontalSpeed, verticalSpeed);
    return static_cast<ExpectedGuid>(guid) == expectedGuid;
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
