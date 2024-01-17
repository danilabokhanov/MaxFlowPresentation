#ifndef DRAWER_H
#define DRAWER_H

#include <QFrame>
#include "max_flow.h"
#include "qwt_plot.h"

namespace max_flow_app {
class Drawer {
public:
    Drawer(QFrame* frame);
    void DrawGraph(const MaxFlow::Data* data);
private:
    QwtPlot plot_;
};

}
#endif  // DRAWER_H
