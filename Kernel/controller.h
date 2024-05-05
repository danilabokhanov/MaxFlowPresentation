#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Library/observer_pattern.h"
#include "max_flow.h"
#include "Interface/geom_model.h"

namespace max_flow_app {
class Controller {
public:
    Controller(MaxFlow* model_ptr, GeomModel* geom_model_ptr);

    using CommandData = interface_messages::CommandData;
    using ViewObserver = observer_pattern::Observer<CommandData>;

    ViewObserver* GetSubscriberPtr();

private:
    using BasicEdge = kernel_messages::BasicEdge;
    using MousePosition = interface_messages::MousePosition;

    void CallChangeVerticesNumber(size_t new_number);
    void CallAddEdge(const BasicEdge& edge);
    void CallDeleteEdge(const BasicEdge& edge);
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
}  // namespace max_flow_app

#endif  // CONTROLLER_H
