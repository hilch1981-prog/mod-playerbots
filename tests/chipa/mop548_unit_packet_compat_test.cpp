#include "Mop548UnitPacketCompat.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <vector>

struct MockGuid
{
    std::uint8_t bytes[8] = {};
    std::uint8_t& operator[](std::size_t i) { return bytes[i]; }
    std::uint8_t const& operator[](std::size_t i) const { return bytes[i]; }
};

static void AssertOrder(std::initializer_list<int> actual, std::initializer_list<int> expected)
{
    assert(std::vector<int>(actual) == std::vector<int>(expected));
}

struct DismountPacket
{
    int maskCalls = 0;
    int byteCalls = 0;

    void ReadGuidMask(MockGuid& guid, int a, int b, int c, int d, int e, int f, int g, int h)
    {
        ++maskCalls;
        AssertOrder({a, b, c, d, e, f, g, h}, {6, 3, 0, 7, 1, 2, 5, 4});
        for (int i = 0; i < 8; ++i)
            guid[i] = 1;
    }

    void ReadGuidBytes(MockGuid& guid, int a, int b, int c, int d, int e, int f, int g, int h)
    {
        ++byteCalls;
        AssertOrder({a, b, c, d, e, f, g, h}, {3, 6, 7, 5, 1, 4, 2, 0});
        for (int i : {a, b, c, d, e, f, g, h})
            guid[i] = static_cast<std::uint8_t>(0x30 + i);
    }
};

int main()
{
    MockGuid guid;
    DismountPacket packet;
    chipa::mop548::ReadDismount(packet, guid);

    assert(packet.maskCalls == 1);
    assert(packet.byteCalls == 1);
    for (int i = 0; i < 8; ++i)
        assert(guid[i] == static_cast<std::uint8_t>(0x30 + i));
}
