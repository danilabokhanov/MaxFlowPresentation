#include "view.h"
#include <iostream>
#include <QAction>
#include <QPushButton>

namespace max_flow_app {
View::View():
      graph_observer_([this](const MaxFlow::Data& data) {UpdateGraphView(data);}),
      command_observable_([this]() {return ProduceViewMessage();}),
      drawer_(main_window_.GetQwtFramePtr()) {
    connect(main_window_.GetVerticesButtonPtr(), &QPushButton::clicked, this,
            &View::ApplyButtonPressed);
    connect(main_window_.GetAddButtonPtr(), &QPushButton::clicked, this,
            &View::AddButtonPressed);
    connect(main_window_.GetDeleteButtonPtr(), &QPushButton::clicked, this,
            &View::DeleteButtonPressed);
    connect(main_window_.GetGoNextButtonPtr(), &QPushButton::clicked, this,
            &View::GoNextButtonPressed);
    main_window_.show();
}

View::GraphObserver* View::GetSubscriberPtr() {
    return &graph_observer_;
}

void View::ApplyButtonPressed() {
    std::cout << "apply\n";
    auto* spin_box = main_window_.GetVerticesSpinBoxPtr();
    message_.signal_type = Data::CHANGE_VERTICES_NUMBER;
    message_.args = static_cast<size_t>(spin_box -> value());
    command_observable_.Notify();
}

void View::AddButtonPressed() {
    std::cout << "add\n";
    auto* spin_box_u = main_window_.GetUSpinBoxPtr();
    auto* spin_box_v = main_window_.GetVSpinBoxPtr();
    auto* spin_box_weight = main_window_.GetWeightSpinBoxPtr();

    message_.signal_type = Data::ADD_EDGE;
    message_.args = MaxFlow::BasicEdge{.u = static_cast<size_t>(spin_box_u -> value()),
                                       .to = static_cast<size_t>(spin_box_v -> value()),
                                       .delta = static_cast<size_t>(spin_box_weight -> value())};
    command_observable_.Notify();
}

void View::DeleteButtonPressed() {
    std::cout << "delete\n";
    auto* spin_box_u = main_window_.GetUSpinBoxPtr();
    auto* spin_box_v = main_window_.GetVSpinBoxPtr();

    message_.signal_type = Data::DELETE_EDGE;
    message_.args = MaxFlow::BasicEdge{.u = static_cast<size_t>(spin_box_u -> value()),
                                       .to = static_cast<size_t>(spin_box_v -> value())};
    command_observable_.Notify();
}

void View::GoNextButtonPressed() {
    std::cout << "go next\n";
    message_.signal_type = Data::GO_NEXT;
    message_.args = Data::Empty{};
    command_observable_.Notify();
}

void View::UpdateGraphView(const MaxFlow::Data& data) {
    drawer_.DrawGraph(data);
}

View::Data View::ProduceViewMessage() const {
    return message_;
}

void View::RegisterController(observer_pattern::Observer<View::Data>* observer) {
    command_observable_.Subscribe(observer);
}
}
