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

    operator std::uint64_t()
    {
        std::uint64_t value = 0;
        for (std::size_t i = 0; i < 8; ++i)
            value |= static_cast<std::uint64_t>(bytes[i]) << (i * 8);
        return value;
    }
};

static void AssertOrder(std::initializer_list<int> actual, std::initializer_list<int> expected)
{
    assert(std::vector<int>(actual) == std::vector<int>(expected));
}

static std::uint64_t RawGuid(std::uint8_t base)
{
    std::uint64_t value = 0;
    for (std::size_t i = 0; i < 8; ++i)
        value |= static_cast<std::uint64_t>(base + i) << (i * 8);
    return value;
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

struct KnockbackPacket
{
    int scalarReads = 0;
    int maskCalls = 0;
    int byteCalls = 0;

    KnockbackPacket& operator>>(float& value)
    {
        if (scalarReads == 0)
            value = 4.5f;      // speedXY
        else if (scalarReads == 1)
            value = -0.25f;    // vsin
        else if (scalarReads == 2)
            value = -7.0f;     // speedZ
        else
        {
            assert(scalarReads == 4);
            value = 0.75f;     // vcos
        }
        ++scalarReads;
        return *this;
    }

    KnockbackPacket& operator>>(std::uint32_t& value)
    {
        assert(scalarReads == 3);
        value = 17u;
        ++scalarReads;
        return *this;
    }

    void ReadGuidMask(MockGuid& guid, int a, int b, int c, int d, int e, int f, int g, int h)
    {
        assert(scalarReads == 5);
        ++maskCalls;
        AssertOrder({a, b, c, d, e, f, g, h}, {2, 0, 7, 1, 4, 6, 5, 3});
        for (int i = 0; i < 8; ++i)
            guid[i] = 1;
    }

    void ReadGuidBytes(MockGuid& guid, int a, int b, int c, int d, int e, int f, int g, int h)
    {
        assert(maskCalls == 1);
        ++byteCalls;
        AssertOrder({a, b, c, d, e, f, g, h}, {6, 0, 7, 5, 4, 3, 1, 2});
        for (int i : {a, b, c, d, e, f, g, h})
            guid[i] = static_cast<std::uint8_t>(0x40 + i);
    }
};

struct EmotePacket
{
    int reads = 0;

    EmotePacket& operator>>(std::uint32_t& value)
    {
        assert(reads == 0);
        value = 77;
        ++reads;
        return *this;
    }

    EmotePacket& operator>>(std::uint64_t& value)
    {
        assert(reads == 1);
        value = UINT64_C(0x0070000000001234);
        ++reads;
        return *this;
    }
};

int main()
{
    MockGuid guid;
    DismountPacket dismountPacket;
    chipa::mop548::ReadDismount(dismountPacket, guid);

    assert(dismountPacket.maskCalls == 1);
    assert(dismountPacket.byteCalls == 1);
    assert(static_cast<std::uint64_t>(guid) == RawGuid(0x30));

    DismountPacket matchingDismount;
    MockGuid matchingGuid;
    assert(chipa::mop548::ReadDismountForGuid(matchingDismount, matchingGuid, RawGuid(0x30)));
    assert(matchingDismount.maskCalls == 1 && matchingDismount.byteCalls == 1);

    DismountPacket foreignDismount;
    MockGuid foreignGuid;
    assert(!chipa::mop548::ReadDismountForGuid(foreignDismount, foreignGuid, RawGuid(0x30) + 1));
    assert(foreignDismount.maskCalls == 1 && foreignDismount.byteCalls == 1);

    KnockbackPacket knockback;
    MockGuid knockbackGuid;
    std::uint32_t counter = 0;
    float vcos = 0.0f;
    float vsin = 0.0f;
    float horizontalSpeed = 0.0f;
    float verticalSpeed = 0.0f;
    assert(chipa::mop548::ReadMoveKnockBackForGuid(knockback, knockbackGuid, RawGuid(0x40), counter,
                                                   vcos, vsin, horizontalSpeed, verticalSpeed));
    assert(knockback.scalarReads == 5 && knockback.maskCalls == 1 && knockback.byteCalls == 1);
    assert(counter == 17u);
    assert(vcos == 0.75f && vsin == -0.25f);
    assert(horizontalSpeed == 4.5f && verticalSpeed == -7.0f);

    KnockbackPacket foreignKnockback;
    MockGuid foreignKnockbackGuid;
    assert(!chipa::mop548::ReadMoveKnockBackForGuid(foreignKnockback, foreignKnockbackGuid, RawGuid(0x40) + 1,
                                                    counter, vcos, vsin, horizontalSpeed, verticalSpeed));
    assert(foreignKnockback.scalarReads == 5 && foreignKnockback.maskCalls == 1 && foreignKnockback.byteCalls == 1);

    EmotePacket emotePacket;
    std::uint32_t emoteId = 0;
    std::uint64_t sourceGuid = 0;
    chipa::mop548::ReadEmote(emotePacket, emoteId, sourceGuid);

    assert(emotePacket.reads == 2);
    assert(emoteId == 77);
    assert(sourceGuid == UINT64_C(0x0070000000001234));
}
