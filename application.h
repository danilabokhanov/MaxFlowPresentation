#ifndef APPLICATION_H
#define APPLICATION_H

#include "controller.h"

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
