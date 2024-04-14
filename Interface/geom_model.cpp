#include <QAction>
#include "geom_model.h"
#include "Interface/drawer_helper.h"
#include <numbers>

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

GeomModel::ClearSignalObserver* GeomModel::GetClearSignalObserverPtr() {
    return &clear_signal_observer_;
}

GeomModel::UnlockObserver* GeomModel::GetUnlockObserverPtr() {
    return &unlock_observer_;
}

void GeomModel::ProcessNextState() {
    if (states_.empty() && selected_vertex_ == std::string::npos) {
        return;
    }
    AddLastFrameToQueue();
    geom_model_observable_.Notify();
}

void GeomModel::AddDynamicState(const MaxFlowData& data) {
    for (size_t i = 0; i < kFPSRate; i++) {
        GeomModelData geom_model{.edges = data.edges,
            .vertices = data.vertices, .edge_id = data.update_edge,
            .frame_id = i, .frames_number = kFPSRate,
            .flow_rate = data.flow_rate, .pushed_flow = data.pushed_flow};
        states_.push_back({.geom_model = std::move(geom_model)});
    }
}

void GeomModel::AddStaticState(const MaxFlowData& data) {
    GeomModelData geom_model{.edges = data.edges, .vertices = data.vertices,
                  .edge_id = std::string::npos, .flow_rate = data.flow_rate,
                             .pushed_flow = data.pushed_flow};
    states_.push_back({.geom_model = std::move(geom_model)});
}

void GeomModel::AddUnlockNotification() {
    states_.push_back({.is_unlock = true});
}

void GeomModel::StartTimer() {
    connect(timer_.get(), &QTimer::timeout, this, &GeomModel::ProcessNextState);
    timer_ -> setInterval(kTimerInterval);
    timer_ -> start();
}

size_t GeomModel::GetFPSRate() {
    return kFPSRate;
}

void GeomModel::SkipFrames() {
    while (states_.size() > 2u || (states_.size() == 2u &&
                                   !states_.back().is_unlock)) {
        states_.pop_front();
    }
}

void GeomModel::SkipFramesRequest() {
    SkipFrames();
    AddUnlockNotification();
}

void GeomModel::ResetPos(size_t n) {
    pos_.resize(n);
    for (size_t i = 0; i < n; i++) {
        pos_[i] = GetVertexPosById(n, i);
    }
}

QPointF GeomModel::GetVertexPosById(size_t n, size_t index) const {
    if (!index) {
        return QPointF(DrawerHelper::kMaxX * DrawerHelper::kSinkPadingRate, DrawerHelper::kMaxY / 2);
    }
    if (index + 1 == n) {
        return QPointF(DrawerHelper::kMaxX * (1 - DrawerHelper::kSinkPadingRate), DrawerHelper::kMaxY / 2);
    }
    if (index & 1u) {
        return QPointF(DrawerHelper::kMaxX / 2, DrawerHelper::kMaxY / 2) +
               DrawerHelper::RotateVector({0, 0}, {1, 0},
                        std::numbers::pi / ((n + 1) / 2) * ((index + 1) / 2), DrawerHelper::kMaxX * (0.5 - DrawerHelper::kSinkPadingRate));
    }
    return QPointF(DrawerHelper::kMaxX / 2, DrawerHelper::kMaxY / 2) +
           DrawerHelper::RotateVector({0, 0}, {1, 0},
                        -std::numbers::pi / (n / 2) * ((index + 1) / 2), DrawerHelper::kMaxX * (0.5 - DrawerHelper::kSinkPadingRate));
}

GeomModel::FrameQueueData GeomModel::SendFrameToView() {
    if (states_.empty()) {
        return FrameQueueData{};
    }
    auto state =  std::move(states_.front());
    if (!state.is_unlock) {
        last_state_ = state.geom_model;
    }
    states_.pop_front();
    if (size_t vertices_number = state.geom_model.vertices.size();
        vertices_number != pos_.size() && vertices_number) {
        ResetPos(vertices_number);
    }
    state.geom_model.pos = pos_;
    state.geom_model.selected_vertex = selected_vertex_;
    return state;
}

void GeomModel::UpdateSelectedVertex(const QPointF& pos) {
    size_t closest_vertex = 0;
    double opt_dist = DrawerHelper::GetVectorLength(pos_.front() - pos);
    for (size_t i = 1; i < pos_.size(); i++) {
        double new_dist = DrawerHelper::GetVectorLength(pos_[i] - pos);
        if (new_dist < opt_dist) {
            closest_vertex = i;
            opt_dist = new_dist;
        }
    }
    if (opt_dist < DrawerHelper::kCenterPadding) {
        selected_vertex_ = closest_vertex;
    } else {
        selected_vertex_ = std::string::npos;
    }
}


bool GeomModel::IsVertexIntersected(size_t index, const QPointF& pos, QPointF& vec) const {
    for (size_t i = 0; i < pos_.size(); i++) {
        if (i == index) {
            continue;
        }
        vec = pos - pos_[i];
        if (double dist = DrawerHelper::GetVectorLength(vec);
            dist < 2 * DrawerHelper::kCenterPadding) {
            return true;
        }
    }
    return false;
}

void GeomModel::MoveVertexToPos(QPointF pos) {
    if (selected_vertex_ == std::string::npos) {
        return;
    }
    QPointF vec;
    if (IsVertexIntersected(selected_vertex_, pos, vec)) {
        if (DrawerHelper::GetVectorLength(vec) < DrawerHelper::kDistEps) {
            pos += QPointF(2 * DrawerHelper::kCenterPadding + DrawerHelper::kDistEps, 0);
        } else {
            pos -= vec;
            pos += DrawerHelper::SetVectorLength(vec,
            2 * DrawerHelper::kCenterPadding + DrawerHelper::kDistEps);
        }
    }
    pos.rx() = std::max(pos.rx(), DrawerHelper::kCenterPadding);
    pos.rx() = std::min(pos.rx(), DrawerHelper::kMaxX - DrawerHelper::kCenterPadding);
    pos.ry() = std::max(pos.ry(), DrawerHelper::kCenterPadding);
    pos.ry() = std::min(pos.ry(), DrawerHelper::kMaxY - DrawerHelper::kCenterPadding);
    pos_[selected_vertex_] = std::move(pos);
}

void GeomModel::HandleMousePressedAction(const MousePosition& pos) {
    UpdateSelectedVertex(QPointF(pos.x, pos.y));
}

void GeomModel::HandleMouseMovedAction(const MousePosition& pos) {
    MoveVertexToPos(QPointF(pos.x, pos.y));
}

void GeomModel::HandleMouseReleasedAction(const MousePosition&) {
    selected_vertex_ = std::string::npos;
    AddLastFrameToQueue();
}

void GeomModel::AddLastFrameToQueue() {
    if (states_.empty() || (states_.size() == 1 && states_.back().is_unlock)) {
        last_state_.pos = pos_;
        states_.emplace_back(FrameQueueData{.geom_model = last_state_});
    }
}
}
