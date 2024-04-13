#include "view.h"
#include <QAction>
#include <QPushButton>
#include <qwt_picker_machine.h>

namespace max_flow_app {
View::View():
      main_window_(),
      drawer_(main_window_.GetQwtFramePtr()),
      geom_model_observer_([this](const FrameQueueData& data) {UpdateGraphView(data);},
                               [this](const FrameQueueData& data) {UpdateGraphView(data);},
                               [](const FrameQueueData&) {}),
    command_observable_([this]() {return ProduceViewMessage();}),
    picker_(new QwtPlotPicker(drawer_.GetQwtPlotPtr() -> canvas())) {
    SetupButtons();
    SetupPicker();
    main_window_.show();
}

View::FrameQueueObserver* View::GetSubscriberPtr() {
    return &geom_model_observer_;
}

void View::SetupButtons() {
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
}

void View::SetupPicker() {
    picker_ -> setStateMachine(new QwtPickerDragPointMachine);
    connect(picker_, &QwtPlotPicker::appended, this,
            &View::MousePressed);
    connect(picker_, &QwtPlotPicker::moved, this, &View::MouseMoved);
    connect(picker_, qOverload<const QPointF&>(&QwtPlotPicker::selected),
            this, &View::MouseReleased);
}

void View::ApplyButtonPressed() {
    auto* spin_box = main_window_.GetVerticesSpinBoxPtr();
    message_.signal_type = CommandData::SignalType::CHANGE_VERTICES_NUMBER;
    message_.args = static_cast<size_t>(spin_box -> value());
    command_observable_.Notify();
}

void View::AddButtonPressed() {
    auto* spin_box_u = main_window_.GetUSpinBoxPtr();
    auto* spin_box_v = main_window_.GetVSpinBoxPtr();
    auto* spin_box_weight = main_window_.GetWeightSpinBoxPtr();

    message_.signal_type = CommandData::SignalType::ADD_EDGE;
    message_.args = BasicEdge{.u = static_cast<size_t>(spin_box_u -> value()),
                                       .to = static_cast<size_t>(spin_box_v -> value()),
                                       .delta = static_cast<size_t>(spin_box_weight -> value())};
    command_observable_.Notify();
}

void View::DeleteButtonPressed() {
    auto* spin_box_u = main_window_.GetUSpinBoxPtr();
    auto* spin_box_v = main_window_.GetVSpinBoxPtr();

    message_.signal_type = CommandData::SignalType::DELETE_EDGE;
    message_.args = BasicEdge{.u = static_cast<size_t>(spin_box_u -> value()),
                                       .to = static_cast<size_t>(spin_box_v -> value())};
    command_observable_.Notify();
}

void View::RunButtonPressed() {
    LockInterface();
    message_.signal_type = CommandData::SignalType::RUN;
    message_.args = std::monostate{};
    command_observable_.Notify();
}

void View::GenRandomSampleButtonPressed() {
    message_.signal_type = CommandData::SignalType::GEN_RANDOM_SAMPLE;
    message_.args = std::monostate{};
    command_observable_.Notify();
}

void View::CancelButtonPressed() {
    message_.signal_type = CommandData::SignalType::CANCEL;
    message_.args = std::monostate{};
    command_observable_.Notify();
}


void View::SkipButtonPressed() {
    message_.signal_type = CommandData::SignalType::SKIP;
    message_.args = std::monostate{};
    command_observable_.Notify();
}

void View::MousePressed(const QPointF& pos) {
    message_.signal_type = CommandData::SignalType::MOUSE_PRESSED;
    message_.args = MousePosition{.x = pos.x(), .y = pos.y()};
    command_observable_.Notify();
}

void View::MouseMoved(const QPointF& pos) {
    message_.signal_type = CommandData::SignalType::MOUSE_MOVED;
    message_.args = MousePosition{.x = pos.x(), .y = pos.y()};
    command_observable_.Notify();
}

void View::MouseReleased(const QPointF& pos) {
    message_.signal_type = CommandData::SignalType::MOUSE_RELEASED;
    message_.args = MousePosition{.x = pos.x(), .y = pos.y()};
    command_observable_.Notify();
}

void View::UpdateGraphView(const FrameQueueData& data) {
    if (data.is_unlock) {
        UnlockInterface();
        return;
    }
    main_window_.GetVerticesSpinBoxPtr()->setValue(data.geom_model.vertices.size());
    drawer_.DrawGraph(data.geom_model);
}

View::CommandData View::ProduceViewMessage() const {
    return message_;
}

void View::RegisterController(CommandObserver* observer) {
    command_observable_.Subscribe(observer);
}

void View::LockInterface() {
    main_window_.GetAddButtonPtr() -> setEnabled(false);
    main_window_.GetDeleteButtonPtr() -> setEnabled(false);
    main_window_.GetRandomSampleButtonPtr() -> setEnabled(false);
    main_window_.GetRunButtonPtr() -> setEnabled(false);
    main_window_.GetVerticesButtonPtr() -> setEnabled(false);
}

void View::UnlockInterface() {
    main_window_.GetAddButtonPtr() -> setEnabled(true);
    main_window_.GetDeleteButtonPtr() -> setEnabled(true);
    main_window_.GetRandomSampleButtonPtr() -> setEnabled(true);
    main_window_.GetRunButtonPtr() -> setEnabled(true);
    main_window_.GetVerticesButtonPtr() -> setEnabled(true);
}
}
