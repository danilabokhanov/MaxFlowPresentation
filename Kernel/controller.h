#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Library/observer_pattern.h"
#include "max_flow.h"
#include "Interface/geom_model.h"

namespace max_flow_app {
class Controller {
public:
    using BasicEdge = kernel_messages::BasicEdge;
    using CommandData = interface_messages::CommandData;
    using ViewObserver = observer_pattern::Observer<CommandData>;
    using MousePosition = interface_messages::MousePosition;

    Controller(MaxFlow* model_ptr, GeomModel* geom_model_ptr);
    ViewObserver* GetSubscriberPtr();

private:
    void CallChangeVerticesNumber(size_t new_number);
    void CallAddEdge(const BasicEdge &edge);
    void CallDeleteEdge(const BasicEdge& egde);
    void CallRun();
    void CallGenRandomSample();
    void CallCancel();
    void CallSkip();
    void CallMousePressedHandler(const MousePosition& pos);
    void CallMouseMovedHandler(const MousePosition& pos);
    void CallMouseReleasedHandler(const MousePosition& pos);

    void HandleData(const CommandData& data);
    MaxFlow* model_ptr_;
    GeomModel* geom_model_ptr_;
    ViewObserver view_observer_;
};
}

#endif  // CONTROLLER_H
