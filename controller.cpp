#include "controller.h"

namespace max_flow_app {
Controller::Controller(MaxFlow* model_ptr): model_ptr_(model_ptr),
      view_observer_([] (const View::ViewSendingData*){},
                     [this] (const View::ViewSendingData* data) {
          if (data ->signal_type == View::ViewSendingData::CHANGE_VERTICES_NUMBER) {
              CallChangeVerticesNumber(std::get<size_t> (data->args));
              return;
          }
          if (data ->signal_type == View::ViewSendingData::ADD_EDGE) {
              CallAddEdge(std::get<MaxFlow::BasicEdge> (data->args));
              return;
          }
          if (data ->signal_type == View::ViewSendingData::DELETE_EDGE) {
              CallDeleteEdge(std::get<MaxFlow::BasicEdge> (data->args));
              return;
          }
          CallGoNext();
      },
                     [] (const View::ViewSendingData*){}) {
}

void Controller::CallChangeVerticesNumber(size_t new_number) {
    model_ptr_ -> ChangeVerticesNumber(new_number);
}

void Controller::CallAddEdge(const MaxFlow::BasicEdge &edge) {
    model_ptr_ -> AddEdge(edge);
}

void Controller::CallDeleteEdge(const MaxFlow::BasicEdge &edge) {
    model_ptr_ -> DeleteEdge(edge);
}

void Controller::CallGoNext() {
    model_ptr_ -> GoNext();
}

Controller::ViewObserver* Controller::GetSubscriberPtr() {
    return &view_observer_;
}
}
