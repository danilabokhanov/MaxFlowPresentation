#ifndef APPLICATION_H
#define APPLICATION_H

#include "Interface/view.h"
#include "Interface/geom_model.h"
#include "Kernel/controller.h"

namespace max_flow_app {
class Application {
public:
    Application();
private:
    MaxFlow model_;
    GeomModel geom_model_;
    View view_;
    Controller controller_;
};

}
#endif  // APPLICATION_H
