#include "application.h"

namespace max_flow_app {
Application::Application(): controller_(&model_) {
    model_.RegisterView(view_.GetSubscriberPtr());
    view_.RegisterController(controller_.GetSubscriberPtr());
}
}
