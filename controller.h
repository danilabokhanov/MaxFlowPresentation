#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "view.h"

namespace max_flow_app {
class Controller {
public:
    using ViewObserver = observer_pattern::Observer<View::ViewSendingData>;

    Controller(MaxFlow* model_ptr);

    void CallChangeVerticesNumber(size_t new_number);
    void CallAddEdge(const MaxFlow::BasicEdge &edge);
    void CallDeleteEdge(const MaxFlow::BasicEdge& egde);
    void CallGoNext();

    ViewObserver* GetSubscriberPtr();

private:
    MaxFlow* model_ptr_;
    ViewObserver view_observer_;
};
}

#endif  // CONTROLLER_H
