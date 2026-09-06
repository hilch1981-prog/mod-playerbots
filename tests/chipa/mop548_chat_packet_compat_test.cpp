#include "Mop548ChatPacketCompat.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <vector>

struct GuidRef
{
    int index;
    operator bool() const { return true; }
};

struct MockGuid
{
    GuidRef operator[](std::size_t index) { return GuidRef{static_cast<int>(index)}; }
};

static void AssertOrder(std::vector<int> const& actual, std::initializer_list<int> expected)
{
    assert(actual == std::vector<int>(expected));
}

struct TextEmotePacket
{
    std::vector<std::uint32_t> scalars;
    std::vector<int> bits;
    std::vector<int> bytes;

    TextEmotePacket& operator<<(std::uint32_t value)
    {
        scalars.push_back(value);
        return *this;
    }

    void WriteBit(GuidRef ref) { bits.push_back(ref.index); }
    void WriteByteSeq(GuidRef ref) { bytes.push_back(ref.index); }
};

int main()
{
    TextEmotePacket packet;
    MockGuid targetGuid;

    chipa::mop548::WriteTextEmoteCommand(
        packet,
        targetGuid,
        std::uint32_t(12),
        std::uint32_t(3));

    assert(packet.scalars.size() == 2);
    assert(packet.scalars[0] == 12);
    assert(packet.scalars[1] == 3);
    AssertOrder(packet.bits, {6, 7, 3, 2, 0, 5, 1, 4});
    AssertOrder(packet.bytes, {0, 5, 1, 4, 2, 3, 7, 6});
}
