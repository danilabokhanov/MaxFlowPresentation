#include "controller.h"

namespace max_flow_app {
Controller::Controller(MaxFlow* model_ptr, GeomModel* geom_model_ptr): model_ptr_(model_ptr),
      geom_model_ptr_(geom_model_ptr), view_observer_([] (const CommandData&){},
                     [this] (const CommandData& data) {HandleData(data);},
                     [] (const CommandData&){}) {
}

void Controller::CallChangeVerticesNumber(size_t new_number) {
    model_ptr_ -> ChangeVerticesNumberRequest(new_number);
}

void Controller::CallAddEdge(const MaxFlow::BasicEdge &edge) {
    model_ptr_ -> AddEdgeRequest(edge);
}

void Controller::CallDeleteEdge(const MaxFlow::BasicEdge &edge) {
    model_ptr_ -> DeleteEdgeRequest(edge);
}

void Controller::CallRun() {
    model_ptr_ -> RunRequest();
}

void Controller::CallGenRandomSample() {
    model_ptr_ -> GenRandomSampleRequest();
}

void Controller::CallCancel() {
    model_ptr_ -> RecoverPrevStateRequest();
}

void Controller::CallSkip() {
    geom_model_ptr_ -> SkipFramesRequest();
}

void Controller::CallMousePressedHandler(const MousePosition& pos) {
    geom_model_ptr_ -> HandleMousePressedAction(pos);
}

void Controller::CallMouseMovedHandler(const MousePosition& pos) {
    geom_model_ptr_ -> HandleMouseMovedAction(pos);
}

void Controller::CallMouseReleasedHandler(const MousePosition& pos) {
    geom_model_ptr_ -> HandleMouseReleasedAction(pos);
}

Controller::ViewObserver* Controller::GetSubscriberPtr() {
    return &view_observer_;
}

void Controller::HandleData(const CommandData& data) {
    switch (data.signal_type) {
    case CommandData::SignalType::CHANGE_VERTICES_NUMBER:
            CallChangeVerticesNumber(std::get<size_t> (data.args));
            break;
        case CommandData::SignalType::ADD_EDGE:
            CallAddEdge(std::get<BasicEdge> (data.args));
            break;
        case CommandData::SignalType::DELETE_EDGE:
            CallDeleteEdge(std::get<BasicEdge> (data.args));
            break;
        case CommandData::SignalType::RUN:
            CallRun();
            break;
        case CommandData::SignalType::GEN_RANDOM_SAMPLE:
            CallGenRandomSample();
            break;
        case CommandData::SignalType::CANCEL:
            CallCancel();
            break;
        case CommandData::SignalType::SKIP:
            CallSkip();
            break;
        case CommandData::SignalType::MOUSE_PRESSED:
            CallMousePressedHandler(std::get<MousePosition>(data.args));
            break;
        case CommandData::SignalType::MOUSE_MOVED:
            CallMouseMovedHandler(std::get<MousePosition>(data.args));
            break;
        case CommandData::SignalType::MOUSE_RELEASED:
            CallMouseReleasedHandler(std::get<MousePosition>(data.args));
            break;
        default:
            assert(0);
    }
}

}
