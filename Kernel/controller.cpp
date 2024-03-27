#include "controller.h"

namespace max_flow_app {
Controller::Controller(MaxFlow* model_ptr): model_ptr_(model_ptr),
      view_observer_([] (const CommandData&){},
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

Controller::ViewObserver* Controller::GetSubscriberPtr() {
    return &view_observer_;
}

void Controller::HandleData(const CommandData& data) {
    switch (data.signal_type) {
        case CommandData::CHANGE_VERTICES_NUMBER:
            CallChangeVerticesNumber(std::get<size_t> (data.args));
            break;
        case CommandData::ADD_EDGE:
            CallAddEdge(std::get<MaxFlow::BasicEdge> (data.args));
            break;
        case CommandData::DELETE_EDGE:
            CallDeleteEdge(std::get<MaxFlow::BasicEdge> (data.args));
            break;
        case CommandData::RUN:
            CallRun();
            break;
        default:
            CallGenRandomSample();
            break;
    }
}

}
