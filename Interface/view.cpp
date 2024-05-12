#include "view.h"
#include <QAction>
#include <QPushButton>
#include <qwt_picker_machine.h>

namespace max_flow_app {
View::View()
    : drawer_(main_window_.GetQwtFramePtr()),
      geom_model_observer_([this](const FrameQueueData& data) { UpdateGraphView(data); },
                           [this](const FrameQueueData& data) { UpdateGraphView(data); },
                           [](const FrameQueueData&) {}),
      command_observable_([this]() { return ProduceViewMessage(); }),
      picker_(new QwtPlotPicker(drawer_.GetQwtPlotPtr()->canvas())) {
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
    connect(main_window_.GetAddButtonPtr(), &QPushButton::clicked, this, &View::AddButtonPressed);
    connect(main_window_.GetDeleteButtonPtr(), &QPushButton::clicked, this,
            &View::DeleteButtonPressed);
    connect(main_window_.GetRunButtonPtr(), &QPushButton::clicked, this, &View::RunButtonPressed);
    connect(main_window_.GetRandomSampleButtonPtr(), &QPushButton::clicked, this,
            &View::GenRandomSampleButtonPressed);
    connect(main_window_.GetSkipButtonPtr(), &QPushButton::clicked, this, &View::SkipButtonPressed);
    connect(main_window_.GetCancelButtonPtr(), &QPushButton::clicked, this,
            &View::CancelButtonPressed);
    connect(main_window_.GetSpeedSliderPtr(), &QSlider::valueChanged, this,
            &View::SpeedSliderMoved);
    connect(main_window_.GetLatencySliderPtr(), &QSlider::valueChanged, this,
            &View::LatencySliderMoved);
}

void View::SetupPicker() {
    picker_->setStateMachine(new QwtPickerDragPointMachine);
    connect(picker_, &QwtPlotPicker::appended, this, &View::MousePressed);
    connect(picker_, &QwtPlotPicker::moved, this, &View::MouseMoved);
    connect(picker_, qOverload<const QPointF&>(&QwtPlotPicker::selected), this,
            &View::MouseReleased);
}

void View::ApplyButtonPressed() {
    message_.signal_type = CommandData::SignalType::ChangeVerticesNumber;
    message_.args = main_window_.GetVerticesSpinBoxNumber();
    command_observable_.Notify();
}

void View::AddButtonPressed() {
    message_.signal_type = CommandData::SignalType::AddEdge;
    message_.args = BasicEdge{.u = main_window_.GetUSpinBoxNumber(),
                              .to = main_window_.GetVSpinBoxNumber(),
                              .delta = main_window_.GetWeightBoxNumber()};
    command_observable_.Notify();
}

void View::DeleteButtonPressed() {
    message_.signal_type = CommandData::SignalType::DeleteEdge;
    message_.args =
        BasicEdge{.u = main_window_.GetUSpinBoxNumber(), .to = main_window_.GetVSpinBoxNumber()};
    command_observable_.Notify();
}

void View::RunButtonPressed() {
    LockInterface();
    message_.signal_type = CommandData::SignalType::Run;
    message_.args = std::monostate{};
    command_observable_.Notify();
}

void View::GenRandomSampleButtonPressed() {
    message_.signal_type = CommandData::SignalType::GenRandomSample;
    message_.args = std::monostate{};
    command_observable_.Notify();
}

void View::CancelButtonPressed() {
    message_.signal_type = CommandData::SignalType::Cancel;
    message_.args = std::monostate{};
    command_observable_.Notify();
}

void View::SkipButtonPressed() {
    message_.signal_type = CommandData::SignalType::Skip;
    message_.args = std::monostate{};
    command_observable_.Notify();
}

void View::MousePressed(const QPointF& pos) {
    message_.signal_type = CommandData::SignalType::MousePressed;
    message_.args = MousePosition{.x = pos.x(), .y = pos.y()};
    command_observable_.Notify();
}

void View::MouseMoved(const QPointF& pos) {
    message_.signal_type = CommandData::SignalType::MouseMoved;
    message_.args = MousePosition{.x = pos.x(), .y = pos.y()};
    command_observable_.Notify();
}

void View::MouseReleased(const QPointF& pos) {
    message_.signal_type = CommandData::SignalType::MouseReleased;
    message_.args = MousePosition{.x = pos.x(), .y = pos.y()};
    command_observable_.Notify();
}

void View::SpeedSliderMoved(int slider_pos) {
    message_.signal_type = CommandData::SignalType::ChangeSpeed;
    message_.args = static_cast<size_t>(slider_pos);
    command_observable_.Notify();
}

void View::LatencySliderMoved(int slider_pos) {
    message_.signal_type = CommandData::SignalType::ChangeLatency;
    message_.args = static_cast<size_t>(slider_pos);
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

const View::CommandData& View::ProduceViewMessage() const {
    return message_;
}

void View::RegisterController(CommandObserver* observer) {
    assert(command_observable_.Subscribe(observer));
}

void View::LockInterface() {
    main_window_.GetAddButtonPtr()->setEnabled(false);
    main_window_.GetDeleteButtonPtr()->setEnabled(false);
    main_window_.GetRandomSampleButtonPtr()->setEnabled(false);
    main_window_.GetRunButtonPtr()->setEnabled(false);
    main_window_.GetVerticesButtonPtr()->setEnabled(false);
}

void View::UnlockInterface() {
    main_window_.GetAddButtonPtr()->setEnabled(true);
    main_window_.GetDeleteButtonPtr()->setEnabled(true);
    main_window_.GetRandomSampleButtonPtr()->setEnabled(true);
    main_window_.GetRunButtonPtr()->setEnabled(true);
    main_window_.GetVerticesButtonPtr()->setEnabled(true);
}
}  // namespace max_flow_app
