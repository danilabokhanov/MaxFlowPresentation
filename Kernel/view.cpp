#include "view.h"
#include <iostream>
#include <QAction>
#include <QPushButton>

namespace max_flow_app {
View::View():
      main_window_(),
      drawer_(main_window_.GetQwtFramePtr()),
      geom_model_observer_([this](const ReceivingData& data) {UpdateGraphView(data);},
                               [this](const ReceivingData& data) {UpdateGraphView(data);},
                               [](const ReceivingData&) {}),
      command_observable_([this]() {return ProduceViewMessage();}) {
    connect(main_window_.GetVerticesButtonPtr(), &QPushButton::clicked, this,
            &View::ApplyButtonPressed);
    connect(main_window_.GetAddButtonPtr(), &QPushButton::clicked, this,
            &View::AddButtonPressed);
    connect(main_window_.GetDeleteButtonPtr(), &QPushButton::clicked, this,
            &View::DeleteButtonPressed);
    connect(main_window_.GetRunButtonPtr(), &QPushButton::clicked, this,
            &View::RunButtonPressed);
    connect(main_window_.GetRandomSampleButtonPtr(), &QPushButton::clicked, this,
            &View::GenRandomSampleButtonPressed);
    connect(main_window_.GetSkipButtonPtr(), &QPushButton::clicked, this,
            &View::SkipButtonPressed);
    connect(main_window_.GetCancelButtonPtr(), &QPushButton::clicked, this,
            &View::CancelButtonPressed);
    main_window_.show();
}

View::GeomModelObserver* View::GetSubscriberPtr() {
    return &geom_model_observer_;
}

void View::ApplyButtonPressed() {
    LockInterface();
    std::cout << "apply\n";
    auto* spin_box = main_window_.GetVerticesSpinBoxPtr();
    message_.signal_type = SendingData::CHANGE_VERTICES_NUMBER;
    message_.args = static_cast<size_t>(spin_box -> value());
    command_observable_.Notify();
    UnlockInterface();
}

void View::AddButtonPressed() {
    LockInterface();
    std::cout << "add\n";
    auto* spin_box_u = main_window_.GetUSpinBoxPtr();
    auto* spin_box_v = main_window_.GetVSpinBoxPtr();
    auto* spin_box_weight = main_window_.GetWeightSpinBoxPtr();

    message_.signal_type = SendingData::ADD_EDGE;
    message_.args = BasicEdge{.u = static_cast<size_t>(spin_box_u -> value()),
                                       .to = static_cast<size_t>(spin_box_v -> value()),
                                       .delta = static_cast<size_t>(spin_box_weight -> value())};
    command_observable_.Notify();
    UnlockInterface();
}

void View::DeleteButtonPressed() {
    LockInterface();
    std::cout << "delete\n";
    auto* spin_box_u = main_window_.GetUSpinBoxPtr();
    auto* spin_box_v = main_window_.GetVSpinBoxPtr();

    message_.signal_type = SendingData::DELETE_EDGE;
    message_.args = BasicEdge{.u = static_cast<size_t>(spin_box_u -> value()),
                                       .to = static_cast<size_t>(spin_box_v -> value())};
    command_observable_.Notify();
    UnlockInterface();
}

void View::RunButtonPressed() {
    LockInterface();
    std::cout << "go next\n";
    message_.signal_type = SendingData::RUN;
    message_.args = SendingData::Empty{};
    command_observable_.Notify();
    UnlockInterface();
}

void View::GenRandomSampleButtonPressed() {
    LockInterface();
    std::cout << "random sample\n";
    message_.signal_type = SendingData::GEN_RANDOM_SAMPLE;
    message_.args = SendingData::Empty{};
    command_observable_.Notify();
    UnlockInterface();
}

void View::CancelButtonPressed() {
    LockInterface();
    std::cout << "cancel\n";
    message_.signal_type = SendingData::CANCEL;
    message_.args = SendingData::Empty{};
    command_observable_.Notify();
    UnlockInterface();
}


void View::SkipButtonPressed() {
    LockInterface();
    std::cout << "skip\n";
    message_.signal_type = SendingData::SKIP;
    message_.args = SendingData::Empty{};
    command_observable_.Notify();
    UnlockInterface();
}

void View::UpdateGraphView(const ReceivingData& data) {
    drawer_.DrawGraph(data);
}

View::SendingData View::ProduceViewMessage() const {
    return message_;
}

void View::RegisterController(CommandObserver* observer) {
    command_observable_.Subscribe(observer);
}

void View::LockInterface() {
    main_window_.GetAddButtonPtr() -> setEnabled(false);
    main_window_.GetCancelButtonPtr() -> setEnabled(false);
    main_window_.GetDeleteButtonPtr() -> setEnabled(false);
    main_window_.GetRandomSampleButtonPtr() -> setEnabled(false);
    main_window_.GetRunButtonPtr() -> setEnabled(false);
    main_window_.GetSkipButtonPtr() -> setEnabled(false);
    main_window_.GetVerticesButtonPtr() -> setEnabled(false);
}

void View::UnlockInterface() {
    main_window_.GetAddButtonPtr() -> setEnabled(true);
    main_window_.GetCancelButtonPtr() -> setEnabled(true);
    main_window_.GetDeleteButtonPtr() -> setEnabled(true);
    main_window_.GetRandomSampleButtonPtr() -> setEnabled(true);
    main_window_.GetRunButtonPtr() -> setEnabled(true);
    main_window_.GetSkipButtonPtr() -> setEnabled(true);
    main_window_.GetVerticesButtonPtr() -> setEnabled(true);
}
}
