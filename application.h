#ifndef APPLICATION_H
#define APPLICATION_H

#include "Kernel/view.h"
#include "Kernel/controller.h"

namespace max_flow_app {
class Application {
public:
    Application();
private:
    MaxFlow model_;
    View view_;
    Controller controller_;
};

}
#endif  // APPLICATION_H
