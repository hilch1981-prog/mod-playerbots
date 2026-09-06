#ifndef CHIPA_MOP548_CHAT_PACKET_COMPAT_H
#define CHIPA_MOP548_CHAT_PACKET_COMPAT_H

namespace chipa
{
namespace mop548
{
// Build the exact packet body consumed by the target MoP 5.4.8
// WorldSession::HandleTextEmoteOpcode implementation. The donor PlayEmote()
// writes a modern flat ObjectGuid payload, which does not match this runtime.
// Keep this helper opcode-agnostic: callers own WorldPacket construction while
// this function owns only the target handler's body layout.
template <class Packet, class Guid, class TextEmote, class EmoteNumber>
void WriteTextEmoteCommand(Packet& packet, Guid& targetGuid, TextEmote textEmote, EmoteNumber emoteNumber)
{
    packet << textEmote;
    packet << emoteNumber;

    packet.WriteBit(targetGuid[6]);
    packet.WriteBit(targetGuid[7]);
    packet.WriteBit(targetGuid[3]);
    packet.WriteBit(targetGuid[2]);
    packet.WriteBit(targetGuid[0]);
    packet.WriteBit(targetGuid[5]);
    packet.WriteBit(targetGuid[1]);
    packet.WriteBit(targetGuid[4]);

    packet.WriteByteSeq(targetGuid[0]);
    packet.WriteByteSeq(targetGuid[5]);
    packet.WriteByteSeq(targetGuid[1]);
    packet.WriteByteSeq(targetGuid[4]);
    packet.WriteByteSeq(targetGuid[2]);
    packet.WriteByteSeq(targetGuid[3]);
    packet.WriteByteSeq(targetGuid[7]);
    packet.WriteByteSeq(targetGuid[6]);
}
}
}

#endif
