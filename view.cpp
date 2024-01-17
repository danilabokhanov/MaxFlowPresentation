#include "view.h"

namespace max_flow_app {
View::View(): main_window_(),
      graph_observer_([this](const MaxFlow::Data* data) {return UpdateGraphView(data);}),
      view_observable_([this]() {return ProduceViewMessage();}),
      drawer_(main_window_.GetQwtFramePtr()) {
    connect(main_window_.GetVerticesButtonPtr(), SIGNAL (clicked()), this,
            SLOT (ApplyButtonPressed()));
    connect(main_window_.GetAddButtonPtr(), SIGNAL (clicked()), this,
            SLOT (AddButtonPressed()));
    connect(main_window_.GetDeleteButtonPtr(), SIGNAL (clicked()), this,
            SLOT (DeleteButtonPressed()));
    connect(main_window_.GetGoNextButtonPtr(), SIGNAL (clicked()), this,
            SLOT (GoNextButtonPressed()));
    main_window_.show();
}

View::GraphObserver* View::GetSubscriberPtr() {
    return &graph_observer_;
}

void View::ApplyButtonPressed() {
    auto* spin_box = main_window_.GetVerticesSpinBoxPtr();
    message_.signal_type = ViewSendingData::CHANGE_VERTICES_NUMBER;
    message_.args = static_cast<size_t>(spin_box -> value());
    view_observable_.Notify();
}

void View::AddButtonPressed() {
    auto* spin_box_u = main_window_.GetUSpinBoxPtr();
    auto* spin_box_v = main_window_.GetVSpinBoxPtr();
    auto* spin_box_weight = main_window_.GetWeightSpinBoxPtr();

    message_.signal_type = ViewSendingData::ADD_EDGE;
    message_.args = MaxFlow::BasicEdge{.u = static_cast<size_t>(spin_box_u -> value()),
                                       .to = static_cast<size_t>(spin_box_v -> value()),
                                       .delta = static_cast<size_t>(spin_box_weight -> value())};
    view_observable_.Notify();
}

void View::DeleteButtonPressed() {
    auto* spin_box_u = main_window_.GetUSpinBoxPtr();
    auto* spin_box_v = main_window_.GetVSpinBoxPtr();

    message_.signal_type = ViewSendingData::DELETE_EDGE;
    message_.args = MaxFlow::BasicEdge{.u = static_cast<size_t>(spin_box_u -> value()),
                                       .to = static_cast<size_t>(spin_box_v -> value())};
    view_observable_.Notify();
}

void View::GoNextButtonPressed() {
    message_.signal_type = ViewSendingData::GO_NEXT;
    message_.args = ViewSendingData::Empty{};
    view_observable_.Notify();
}

void View::UpdateGraphView(const MaxFlow::Data* data) {
    drawer_.DrawGraph(data);
}

const View::ViewSendingData* View::ProduceViewMessage() const {
    return &message_;
}

void View::RegisterController(observer_pattern::Observer<View::ViewSendingData>* observer) {
    view_observable_.Subscribe(observer);
}
}
