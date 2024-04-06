#include "application.h"

namespace max_flow_app {
Application::Application(): controller_(&model_, &geom_model_) {
    model_.RegisterNetworkObserver(geom_model_.GetNetworkObserverPtr());
    model_.RegisterFlowObserver(geom_model_.GetFlowObserverPtr());
    model_.RegisterCleanupObserver(geom_model_.GetClearSignalObserverPtr());
    model_.RegisterUnlockObserver(geom_model_.GetUnlockObserverPtr());
    geom_model_.RegisterView(view_.GetSubscriberPtr());
    view_.RegisterController(controller_.GetSubscriberPtr());
}
}
