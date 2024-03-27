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
    using NetworkObserver = observer_pattern::Observer<MaxFlowData>;
    using FlowObserver = observer_pattern::Observer<MaxFlowData>;
    using ClearSignalObserver = observer_pattern::Observer<void>;
    using StateObservable = observer_pattern::Observable<MaxFlowData>;
    using StateObserver = observer_pattern::Observer<MaxFlowData>;

    GeomModel();
    void RegisterView(StateObserver* observer);
    NetworkObserver* GetNetworkObserverPtr();
    FlowObserver* GetFlowObserverPtr();
    ClearSignalObserver* GetClearSignalObserver();

private slots:
   void ProcessNextState();

private:
    static const size_t kFPSRate = 20;
    static const size_t kTimerInterval = 1000 / kFPSRate;
    std::unique_ptr<QTimer> timer_;
    std::deque<MaxFlowData> states_;
    NetworkObserver network_observer_ = NetworkObserver(
       [this](const MaxFlowData& data) {AddStaticState(data);},
       [this](const MaxFlowData& data) {AddStaticState(data);},
       [](const MaxFlowData&) {});
    NetworkObserver flow_observer_ = FlowObserver(
        [](const MaxFlowData&) {},
        [this](const MaxFlowData& data) {AddDynamicState(data);},
        [](const MaxFlowData&) {});
    ClearSignalObserver clear_signal_observer_ = ClearSignalObserver(
        []() {},
        [this]() {ClearStateQueue();},
        []() {});
    StateObservable geom_model_observable_ = StateObservable([this]() {
        if (states_.empty()) {
            return MaxFlowData{};
        }
        auto state =  std::move(states_.front());
        states_.pop_front();
        return state;
    });

    void AddDynamicState(const MaxFlowData& data);
    void AddStaticState(const MaxFlowData& data);
    void ClearStateQueue();
    void StartTimer();
};
}

#endif  // STATUSMANAGER_H
