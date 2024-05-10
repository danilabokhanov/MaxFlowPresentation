#ifndef STATUSMANAGER_H
#define STATUSMANAGER_H
#include <memory>
#include <QTimer>
#include <deque>
#include "Kernel/kernel_messages.h"
#include "interface_messages.h"
#include "Library/observer_pattern.h"

namespace max_flow_app {
class GeomModel : public QObject {
    Q_OBJECT
public:
    GeomModel();

    using MaxFlowData = kernel_messages::MaxFlowData;
    using FrameQueueData = interface_messages::FrameQueueData;
    using MousePosition = interface_messages::MousePosition;
    using NetworkObserver = observer_pattern::Observer<MaxFlowData>;
    using FlowObserver = observer_pattern::Observer<MaxFlowData>;
    using ClearSignalObserver = observer_pattern::Observer<void>;
    using UnlockObserver = observer_pattern::Observer<void>;
    using StateObserver = observer_pattern::Observer<FrameQueueData>;

    void RegisterView(StateObserver* observer);
    NetworkObserver* GetNetworkObserverPtr();
    FlowObserver* GetFlowObserverPtr();
    ClearSignalObserver* GetClearSignalObserverPtr();
    UnlockObserver* GetUnlockObserverPtr();
    void SkipFramesRequest();
    void ChangeSpeedRequest(size_t slider_pos);
    void ChangeLatencyRequest(size_t slider_pos);
    static size_t GetFPSRate();
    void HandleMousePressedAction(const MousePosition& pos);
    void HandleMouseMovedAction(const MousePosition& pos);
    void HandleMouseReleasedAction(const MousePosition& pos);

private slots:
    void ProcessNextState();

private:
    using GeomModelData = interface_messages::GeomModelData;
    using StateObservable = observer_pattern::Observable<FrameQueueData>;

    void SkipFrames();
    void AddDynamicState(const MaxFlowData& data);
    void AddStaticState(const MaxFlowData& data);
    void AddUnlockNotification();
    void StartTimer();
    void ResetPos(size_t n);
    FrameQueueData SendFrameToView();
    QPointF GetVertexPosById(size_t n, size_t index) const;
    void UpdateSelectedVertex(const QPointF& pos);
    void MoveVertexToPos(QPointF pos);
    void AddLastFrameToQueue();
    QPointF GetShiftVector(size_t index, const QPointF& pos) const;
    bool IsVertexIntersected(size_t index, const QPointF& pos, QPointF& vec) const;

    static const size_t kFPSRate = 60;
    static const size_t kTimerInterval = 1000 / kFPSRate;
    static const size_t kBasicSpeed = kFPSRate;
    static const size_t kBasicLatency = kFPSRate;
    inline static const std::vector<double> kSpeedCoef = {7.0, 3.0, 2.5, 2.0, 1.5, 1.0,
                                                   0.75, 0.5, 0.33, 0.25, 0.166};
    inline static const std::vector<double> kLatencyCoef = {3.0, 2.0, 1.5, 1.0, 0.75, 0.5, 0.3};
    size_t speed_ = kBasicSpeed;
    size_t latency_ = kBasicLatency;
    std::unique_ptr<QTimer> timer_;
    std::deque<FrameQueueData> states_;
    NetworkObserver network_observer_ = NetworkObserver(
        [this](const MaxFlowData& data) { AddStaticState(data); },
        [this](const MaxFlowData& data) { AddStaticState(data); }, [](const MaxFlowData&) {});
    NetworkObserver flow_observer_ = FlowObserver(
        [](const MaxFlowData&) {}, [this](const MaxFlowData& data) { AddDynamicState(data); },
        [](const MaxFlowData&) {});
    UnlockObserver unlock_observer_ =
        ClearSignalObserver([]() {}, [this]() { AddUnlockNotification(); }, []() {});
    ClearSignalObserver clear_signal_observer_ =
        ClearSignalObserver([]() {}, [this]() { SkipFrames(); }, []() {});
    StateObservable geom_model_observable_ =
        StateObservable([this]() { return SendFrameToView(); });
    std::vector<QPointF> pos_;
    size_t selected_vertex_ = std::string::npos;
    GeomModelData last_state_;
};
}  // namespace max_flow_app

#endif  // STATUSMANAGER_H
