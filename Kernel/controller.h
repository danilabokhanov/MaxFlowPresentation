#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Library/observer_pattern.h"
#include "mvc_messages.h"
#include "max_flow.h"

namespace max_flow_app {
class Controller {
public:
    using BasicEdge = mvc_messages::BasicEdge;
    using CommandData = mvc_messages::CommandData;
    using ViewObserver = observer_pattern::Observer<CommandData>;

    Controller(MaxFlow* model_ptr);
    ViewObserver* GetSubscriberPtr();

private:
    void CallChangeVerticesNumber(size_t new_number);
    void CallAddEdge(const BasicEdge &edge);
    void CallDeleteEdge(const BasicEdge& egde);
    void CallGoNext();

    void HandleData(const CommandData& data);
    MaxFlow* model_ptr_;
    ViewObserver view_observer_;
};
}

#endif  // CONTROLLER_H
