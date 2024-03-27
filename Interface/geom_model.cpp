#include "geom_model.h"
#include <QAction>

namespace max_flow_app {
GeomModel::GeomModel(): timer_(new QTimer(this)) {}

void GeomModel::RegisterView(StateObserver* observer) {
    geom_model_observable_.Subscribe(observer);
    StartTimer();
}

GeomModel::NetworkObserver* GeomModel::GetNetworkObserverPtr() {
    return &network_observer_;
}

GeomModel::FlowObserver* GeomModel::GetFlowObserverPtr() {
    return &flow_observer_;
}

GeomModel::ClearSignalObserver* GeomModel::GetClearSignalObserver() {
    return &clear_signal_observer_;
}

void GeomModel::ProcessNextState() {
    if (states_.empty()) {
        return;
    }
    geom_model_observable_.Notify();
}

void GeomModel::AddDynamicState(const MaxFlowData& data) {
    for (size_t i = 0; i < kFPSRate; i++) {
        states_.push_back(data);
    }
}

void GeomModel::AddStaticState(const MaxFlowData& data) {
    states_.push_back(data);
}

void GeomModel::ClearStateQueue() {
    states_.clear();
}

void GeomModel::StartTimer() {
    connect(timer_.get(), &QTimer::timeout, this, &GeomModel::ProcessNextState);
    timer_ -> setInterval(kTimerInterval);
    timer_ -> start();
}

}
