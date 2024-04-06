#ifndef STATUSMANAGER_H
#define STATUSMANAGER_H
#include <memory>
#include <QTimer>
#include <deque>
#include "Kernel/mvc_messages.h"
#include "Library/observer_pattern.h"

namespace max_flow_app {
class GeomModel: public QObject {
    Q_OBJECT
public:
    using MaxFlowData = mvc_messages::MaxFlowData;
    using GeomModelData = mvc_messages::GeomModelData;
    using NetworkObserver = observer_pattern::Observer<MaxFlowData>;
    using FlowObserver = observer_pattern::Observer<MaxFlowData>;
    using ClearSignalObserver = observer_pattern::Observer<void>;
    using UnlockObserver = observer_pattern::Observer<void>;
    using StateObservable = observer_pattern::Observable<GeomModelData>;
    using StateObserver = observer_pattern::Observer<GeomModelData>;

    GeomModel();
    void RegisterView(StateObserver* observer);
    NetworkObserver* GetNetworkObserverPtr();
    FlowObserver* GetFlowObserverPtr();
    ClearSignalObserver* GetClearSignalObserverPtr();
    UnlockObserver* GetUnlockObserverPtr();

    void SkipFrames();
    void SkipFramesRequest();
    static size_t GetFPSRate();

private slots:
   void ProcessNextState();

private:
    static const size_t kFPSRate = 60;
    static const size_t kTimerInterval = 1000 / kFPSRate;
    std::unique_ptr<QTimer> timer_;
    std::deque<GeomModelData> states_;
    NetworkObserver network_observer_ = NetworkObserver(
       [this](const MaxFlowData& data) {AddStaticState(data);},
       [this](const MaxFlowData& data) {AddStaticState(data);},
       [](const MaxFlowData&) {});
    NetworkObserver flow_observer_ = FlowObserver(
        [](const MaxFlowData&) {},
        [this](const MaxFlowData& data) {AddDynamicState(data);},
        [](const MaxFlowData&) {});
    UnlockObserver unlock_observer_ = ClearSignalObserver(
        []() {},
        [this]() {AddUnlockNotification();},
        []() {});
    ClearSignalObserver clear_signal_observer_ = ClearSignalObserver(
        []() {},
        [this]() {SkipFrames();},
        []() {});
    StateObservable geom_model_observable_ = StateObservable([this]() {
        if (states_.empty()) {
            return GeomModelData{};
        }
        auto state =  std::move(states_.front());
        states_.pop_front();
        return state;
    });

    void AddDynamicState(const MaxFlowData& data);
    void AddStaticState(const MaxFlowData& data);
    void AddUnlockNotification();
    void StartTimer();
};
}

#endif  // STATUSMANAGER_H
