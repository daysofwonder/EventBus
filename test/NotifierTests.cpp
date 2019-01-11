#include <eventbus/EventBus.h>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace testing;

namespace Scope1
{
    struct SimpleEvent
    {
        int value;
    };
} // namespace Scope1
namespace Scope2
{
    struct SimpleEvent
    {
        int value;
    };
} // namespace Scope2

TEST(EventBus, SimpleTest)
{
    Dexode::EventBus bus;
    struct SimpleEvent
    {
        int value;
    };

    const auto token = bus.listen<SimpleEvent>([](const SimpleEvent& event) { EXPECT_EQ(event.value, 3); });

    bus.notify(SimpleEvent{ 3 });
    bus.unlistenAll(token);
    bus.notify(SimpleEvent{ 2 });

    bus.listen<SimpleEvent>([](const SimpleEvent& event) { EXPECT_EQ(event.value, 1); });
    bus.notify(SimpleEvent{ 1 });
}

TEST(EventBus, SimpleTest2)
{
    Dexode::EventBus bus;
    struct SimpleEvent
    {
        int value;
    };

    const auto token = bus.listen<SimpleEvent>([](const SimpleEvent& event) { EXPECT_EQ(event.value, 3); });

    bus.notify<SimpleEvent>({ 3 });
    bus.unlistenAll(token);
    bus.notify(SimpleEvent{ 2 });

    bus.listen<SimpleEvent>([](const SimpleEvent& event) { EXPECT_EQ(event.value, 1); });
    bus.notify(SimpleEvent{ 1 });
}

TEST(EventBus, ListenAndNotify)
{
    int isCalled = 0;
    Dexode::EventBus bus;
    struct SimpleEvent
    {
        int value;
    };

    const auto token = bus.listen<SimpleEvent>([&](const SimpleEvent& event) {
        ++isCalled;
        EXPECT_EQ(event.value, 3);
    });
    EXPECT_EQ(isCalled, 0);
    bus.notify(SimpleEvent{ 3 });
    EXPECT_EQ(isCalled, 1);
    bus.unlistenAll(token);
    bus.notify(SimpleEvent{ 2 });
    EXPECT_EQ(isCalled, 1);

    bus.listen<SimpleEvent>([&](const SimpleEvent& event) {
        ++isCalled;
        EXPECT_EQ(event.value, 1);
    });
    bus.notify(SimpleEvent{ 1 });
    EXPECT_EQ(isCalled, 2);
}

TEST(EventBus, DifferentNotifications)
{
    Dexode::EventBus bus;
    struct SimpleEvent1
    {
        int value;
    };
    struct SimpleEvent2
    {
        int value;
    };

    bool called1 = false;
    bool called2 = false;

    bus.listen<SimpleEvent1>([&called1](const SimpleEvent1& event) {
        called1 = true;
        EXPECT_EQ(event.value, 1);
    });

    bus.listen<SimpleEvent2>([&called2](const SimpleEvent2& event) {
        called2 = true;
        EXPECT_EQ(event.value, 2);
    });

    EXPECT_FALSE(called1);

    bus.notify(SimpleEvent1{ 1 });

    EXPECT_TRUE(called1);
    EXPECT_FALSE(called2);
    called1 = false;

    bus.notify(SimpleEvent2{ 2 });

    EXPECT_FALSE(called1);
    EXPECT_TRUE(called2);
}

TEST(EventBus, DifferentEvents)
{
    int isCalled = 0;
    Dexode::EventBus bus;

    bus.listen<Scope1::SimpleEvent>([&](const Scope1::SimpleEvent& event) {
        ++isCalled;
        EXPECT_EQ(event.value, 1);
    });
    bus.listen<Scope2::SimpleEvent>([&](const Scope2::SimpleEvent& event) {
        ++isCalled;
        EXPECT_EQ(event.value, 2);
    });
    EXPECT_EQ(isCalled, 0);

    bus.notify(Scope1::SimpleEvent{ 1 });
    EXPECT_EQ(isCalled, 1);

    bus.notify(Scope2::SimpleEvent{ 2 });
    EXPECT_EQ(isCalled, 2);
}

TEST(EventBus, ModificationDuringNotify)
{
    Dexode::EventBus bus;
    struct TestEvent
    {
    };

    int token1 = 1;
    int token2 = 2;

    int calls = 0;

    bus.listen<TestEvent>(token1, [&](const TestEvent& event) {
        ++calls;
        bus.unlistenAll(token1);
        bus.unlistenAll(token2);
    });
    bus.listen<TestEvent>(token2, [&](const TestEvent& event) {
        ++calls;
        bus.unlistenAll(token1);
        bus.unlistenAll(token2);
    });

    EXPECT_NO_THROW(bus.notify(TestEvent{}));
    EXPECT_EQ(calls, 2);

    EXPECT_NO_THROW(bus.notify(TestEvent{}));
    EXPECT_EQ(calls, 2);
}

TEST(EventBus, ModificationDuringNotify2)
{
    Dexode::EventBus bus;
    struct TestEvent
    {
    };

    int token1 = 1;
    int token2 = 2;
    int token3 = 3;

    int calls = 0;

    bus.listen<TestEvent>(token1, [&](const TestEvent& event) {
        ++calls;
        bus.unlistenAll(token1);
        bus.unlistenAll(token2);
        bus.unlistenAll(token3);
    });
    bus.listen<TestEvent>(token2, [&](const TestEvent& event) {
        ++calls;
        bus.unlistenAll(token1);
        bus.unlistenAll(token2);
        bus.unlistenAll(token3);
    });
    bus.listen<TestEvent>(token3, [&](const TestEvent& event) {
        ++calls;
        bus.unlistenAll(token1);
        bus.unlistenAll(token2);
        bus.unlistenAll(token3);
    });

    EXPECT_NO_THROW(bus.notify(TestEvent{}));
    EXPECT_EQ(calls, 3);

    EXPECT_NO_THROW(bus.notify(TestEvent{}));
    EXPECT_EQ(calls, 3);
}

TEST(EventBus, ModificationDuringNotify3)
{
    Dexode::EventBus bus;
    struct TestEvent
    {
    };

    int token1 = 1;
    int token2 = 2;
    int token3 = 3;

    int calls = 0;

    bus.listen<TestEvent>(token1, [&](const TestEvent& event) {
        ++calls;
        bus.unlistenAll(token1);
    });
    bus.listen<TestEvent>(token2, [&](const TestEvent& event) {
        ++calls;
        bus.unlistenAll(token1);
        bus.unlistenAll(token3);
        bus.unlistenAll(token2);
    });
    bus.listen<TestEvent>(token3, [&](const TestEvent& event) {
        ++calls;
        bus.unlistenAll(token1);
        bus.unlistenAll(token2);
        bus.unlistenAll(token3);
    });

    EXPECT_NO_THROW(bus.notify(TestEvent{}));
    EXPECT_EQ(calls, 3);
}

TEST(EventBus, ModificationDuringNotify4)
{
    Dexode::EventBus bus;
    struct TestEvent
    {
    };

    int token1 = 1;
    int token2 = 2;
    int token3 = 3;

    int calls = 0;

    bus.listen<TestEvent>(token1, [&](const TestEvent& event) {
        ++calls;
        bus.unlistenAll(token1);

        bus.listen<TestEvent>(token2, [&](const TestEvent& event) {
            ++calls;
            bus.unlistenAll(token1);
            bus.unlistenAll(token3);
            bus.unlistenAll(token2);
        });
    });
    bus.listen<TestEvent>(token3, [&](const TestEvent& event) {
        ++calls;
        bus.unlistenAll(token1);
        bus.unlistenAll(token2);
        bus.unlistenAll(token3);
    });

    EXPECT_NO_THROW(bus.notify(TestEvent{}));
    EXPECT_EQ(calls, 2);
    EXPECT_NO_THROW(bus.notify(TestEvent{}));
    EXPECT_EQ(calls, 2);
}

TEST(EventBus, ModificationDuringNotify5)
{
    Dexode::EventBus bus;
    struct TestEvent
    {
    };

    int token1 = 1;
    int token2 = 2;
    int token3 = 3;

    int calls = 0;

    bus.listen<TestEvent>(token1, [&](const TestEvent& event) {
        ++calls;
        bus.unlistenAll(token1);
    });
    bus.listen<TestEvent>(token2, [&](const TestEvent& event) {
        ++calls;
        bus.unlistenAll(token1);
        bus.unlistenAll(token2);

        bus.listen<TestEvent>(token3, [&](const TestEvent& event) {
            ++calls;
            bus.unlistenAll(token1);
            bus.unlistenAll(token2);
            bus.unlistenAll(token3);
        });
    });

    EXPECT_NO_THROW(bus.notify(TestEvent{}));
    EXPECT_EQ(calls, 2);

    EXPECT_NO_THROW(bus.notify(TestEvent{}));
    EXPECT_EQ(calls, 3);
}

TEST(EventBus, ModificationDuringNestedNotify)
{
    Dexode::EventBus bus;
    struct TestEvent
    {
    };
    struct TestEvent2
    {
    };

    int token1 = 1;
    int token2 = 2;
    int token3 = 3;

    int calls = 0;

    bus.listen<TestEvent>(token1, [&](const TestEvent& event) {
        bus.notify(TestEvent2{});

        ++calls;
        bus.unlistenAll(token1);

        bus.listen<TestEvent>(token2, [&](const TestEvent& event) {
            ++calls;
            bus.unlistenAll(token1);
            bus.unlistenAll(token3);
            bus.unlistenAll(token2);
        });
    });
    bus.listen<TestEvent>(token3, [&](const TestEvent& event) {
        ++calls;
        bus.unlistenAll(token1);
        bus.unlistenAll(token2);
        bus.unlistenAll(token3);
    });

    EXPECT_NO_THROW(bus.notify(TestEvent{}));
    EXPECT_EQ(calls, 2);
    EXPECT_NO_THROW(bus.notify(TestEvent{}));
    EXPECT_EQ(calls, 2);
}
