#include "catch.hpp"
#include "../Library/observer_pattern.h"
#include <iostream>

using namespace observer_pattern;

const int kOnSubscribeDelta = 3;

int main_counter = 0, inner_counter;

void BasicOnSubscribe(const int& cnt) {
    inner_counter += kOnSubscribeDelta;
}

void BasicOnNotify(const int& cnt) {
    ++inner_counter;
}

void BasicOnUnsubscribe(const int& cnt) {
    inner_counter -= kOnSubscribeDelta;
}

const int& BasicDataProvider() {
    return main_counter;
}

void TestSubscribe(Observable<int>& observable, Observer<int>& observer) {
    main_counter += kOnSubscribeDelta;
    REQUIRE_NOTHROW(observable.Subscribe(&observer));
    REQUIRE(observer.IsConnected());
    REQUIRE(main_counter == inner_counter);
}

void TestNotify(Observable<int>& observable) {
    ++main_counter;
    observable.Notify();
    REQUIRE(main_counter == inner_counter);
}

void TestUnsubscribe(Observer<int>& observer) {
    main_counter -= kOnSubscribeDelta;
    observer.Unsubscribe();
    REQUIRE_FALSE(observer.IsConnected());
    REQUIRE(main_counter == inner_counter);
}

TEST_CASE("Default constructors") {
    Observable<int> observable;
    Observer<int> observer;
    REQUIRE_FALSE(observable.Subscribe(nullptr));
    observable.Notify();
    REQUIRE_FALSE(observer.IsConnected());
}

TEST_CASE("Capture by function") {
    main_counter = 0, inner_counter = 0;
    Observable<int> observable(BasicDataProvider);
    Observer<int> observer(BasicOnSubscribe, BasicOnNotify, BasicOnUnsubscribe);
    TestSubscribe(observable, observer);
    TestNotify(observable);
    TestNotify(observable);
    TestUnsubscribe(observer);
}

TEST_CASE("Capture by lambda") {
    main_counter = 0, inner_counter = 0;
    Observable<int> observable([]() -> const int& { return main_counter; });
    Observer<int> observer([](const int&) { inner_counter += kOnSubscribeDelta; },
                           [](const int&) { ++inner_counter; },
                           [](const int&) { inner_counter -= kOnSubscribeDelta; });
    TestSubscribe(observable, observer);
    TestNotify(observable);
    TestNotify(observable);
    TestUnsubscribe(observer);
}

TEST_CASE("Empty Observer") {
    int switcher = 0;
    Observable<void> observable;
    {
        Observer<void> observer([&]() { switcher = 1; }, [&]() { switcher = 2; },
                                [&]() { switcher = 3; });

        observable.Subscribe(&observer);
        REQUIRE(switcher == 1);
        observable.Notify();
        REQUIRE(switcher == 2);
        observable.Notify();
        REQUIRE(switcher == 2);
        observable.Notify();
    }
    REQUIRE(switcher == 3);
}
