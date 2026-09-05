#include "Mop548SpellPacketCompat.h"

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

struct FailurePacket
{
    int maskCalls = 0;
    int byteCalls = 0;
    int scalarStage = 0;

    void ReadGuidMask(MockGuid& guid, int a, int b, int c, int d, int e, int f, int g, int h)
    {
        ++maskCalls;
        AssertOrder({a, b, c, d, e, f, g, h}, {7, 3, 6, 2, 1, 5, 0, 4});
        for (int i = 0; i < 8; ++i)
            guid[i] = 1;
    }

    void ReadGuidBytes(MockGuid& guid, int a, int b, int c, int d, int e, int f)
    {
        assert(byteCalls++ == 0);
        AssertOrder({a, b, c, d, e, f}, {2, 6, 7, 0, 3, 1});
        for (int i : {a, b, c, d, e, f})
            guid[i] = static_cast<std::uint8_t>(0x10 + i);
    }

    void ReadGuidBytes(MockGuid& guid, int a, int b)
    {
        assert(byteCalls++ == 1);
        AssertOrder({a, b}, {4, 5});
        for (int i : {a, b})
            guid[i] = static_cast<std::uint8_t>(0x10 + i);
    }

    FailurePacket& operator>>(std::uint8_t& value)
    {
        if (scalarStage == 0)
            value = 0x33;
        else
        {
            assert(scalarStage == 2);
            value = 0x44;
        }
        ++scalarStage;
        return *this;
    }

    FailurePacket& operator>>(std::uint32_t& value)
    {
        assert(scalarStage == 1);
        value = 0xAABBCCDDu;
        ++scalarStage;
        return *this;
    }
};

struct DelayedPacket
{
    int maskCalls = 0;
    int byteCalls = 0;
    int scalarStage = 0;

    void ReadGuidMask(MockGuid& guid, int a, int b, int c, int d, int e, int f, int g, int h)
    {
        ++maskCalls;
        AssertOrder({a, b, c, d, e, f, g, h}, {6, 7, 2, 0, 4, 3, 1, 5});
        for (int i = 0; i < 8; ++i)
            guid[i] = 1;
    }

    void ReadGuidBytes(MockGuid& guid, int a, int b, int c, int d, int e, int f, int g)
    {
        assert(byteCalls++ == 0);
        AssertOrder({a, b, c, d, e, f, g}, {2, 6, 1, 7, 0, 5, 3});
        for (int i : {a, b, c, d, e, f, g})
            guid[i] = static_cast<std::uint8_t>(0x20 + i);
    }

    void ReadGuidBytes(MockGuid& guid, int a)
    {
        assert(byteCalls++ == 1);
        assert(a == 4);
        guid[a] = static_cast<std::uint8_t>(0x20 + a);
    }

    DelayedPacket& operator>>(std::uint32_t& value)
    {
        assert(scalarStage++ == 0);
        value = 777u;
        return *this;
    }
};

int main()
{
    MockGuid failureGuid;
    std::uint8_t result = 0;
    std::uint32_t spellId = 0;
    std::uint8_t castCount = 0;
    FailurePacket failure;
    chipa::mop548::ReadSpellFailure(failure, failureGuid, result, spellId, castCount);
    assert(failure.maskCalls == 1 && failure.byteCalls == 2 && failure.scalarStage == 3);
    assert(result == 0x33 && spellId == 0xAABBCCDDu && castCount == 0x44);
    for (int i = 0; i < 8; ++i)
        assert(failureGuid[i] == static_cast<std::uint8_t>(0x10 + i));

    MockGuid delayedGuid;
    std::uint32_t delay = 0;
    DelayedPacket delayed;
    chipa::mop548::ReadSpellDelayed(delayed, delayedGuid, delay);
    assert(delayed.maskCalls == 1 && delayed.byteCalls == 2 && delayed.scalarStage == 1);
    assert(delay == 777u);
    for (int i = 0; i < 8; ++i)
        assert(delayedGuid[i] == static_cast<std::uint8_t>(0x20 + i));
}
