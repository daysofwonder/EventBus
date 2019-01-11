#include <eventbus/EventCollector.h>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace testing;

TEST(EventCollector, SimpleTest)
{
    struct SimpleEvent
    {
        int value;
    };

    Dexode::EventBus bus;
    int callCount = 0;
    {
        Dexode::EventCollector listener{ &bus };
        listener.listen<SimpleEvent>([&](const SimpleEvent& event) {
            EXPECT_EQ(event.value, 3);
            ++callCount;
        });
        bus.notify(SimpleEvent{ 3 });
        EXPECT_EQ(callCount, 1);
    }
    bus.notify(SimpleEvent{ 2 });
    EXPECT_EQ(callCount, 1);
}

TEST(EventCollector, UnlistenAll)
{
    struct SimpleEvent
    {
        int value;
    };
    Dexode::EventBus bus;
    Dexode::EventCollector listener{ &bus };

    int callCount = 0;
    listener.listen<SimpleEvent>([&](const SimpleEvent& event) {
        EXPECT_EQ(event.value, 3);
        ++callCount;
    });
    bus.notify(SimpleEvent{ 3 });
    listener.unlistenAll();

    bus.notify(SimpleEvent{ 2 });
    EXPECT_EQ(callCount, 1);
}

TEST(EventCollector, ResetWhenWeAssign)
{
    struct SimpleEvent
    {
        int value;
    };

    Dexode::EventBus bus;
    int callCount = 0;
    Dexode::EventCollector listener{ &bus };
    listener.listen<SimpleEvent>([&](const SimpleEvent& event) {
        EXPECT_EQ(event.value, 3);
        ++callCount;
    });
    bus.notify(SimpleEvent{ 3 });
    EXPECT_EQ(callCount, 1);
    listener = { nullptr };

    bus.notify(SimpleEvent{ 2 });
    EXPECT_EQ(callCount, 1);
}
