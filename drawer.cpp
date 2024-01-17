#include "drawer.h"
#include <iostream>

namespace max_flow_app {
Drawer::Drawer(QFrame* frame): plot_(frame) {

}

void Drawer::DrawGraph(const MaxFlow::Data* data) {
    (void)data;
    std::cout << "test\n";
}
}
