#ifndef INTERFACE_MESSAGES_H
#define INTERFACE_MESSAGES_H
#include <vector>
#include <cstddef>
#include <variant>
#include <string>
#include <QPointF>
#include "Kernel/kernel_messages.h"

namespace max_flow_app {
namespace interface_messages {

using Edge = kernel_messages::Edge;
using Status = kernel_messages::Status;
using BasicEdge = kernel_messages::BasicEdge;

struct GeomModelData {
    std::vector<Edge> edges;
    std::vector<Status> vertices;
    std::vector<QPointF> pos;
    size_t edge_id = std::string::npos;
    size_t frame_id, frames_number;
    size_t flow_rate, pushed_flow;
    size_t selected_vertex = std::string::npos;
};

struct FrameQueueData {
    GeomModelData geom_model;
    bool is_unlock = false;
};

struct MousePosition {
    double x, y;
};

struct CommandData {
enum class SignalType {
        CHANGE_VERTICES_NUMBER,
        ADD_EDGE,
        DELETE_EDGE,
        RUN,
        GEN_RANDOM_SAMPLE,
        CANCEL,
        SKIP,
        MOUSE_PRESSED,
        MOUSE_MOVED,
        MOUSE_RELEASED
    } signal_type;
    std::variant<size_t, BasicEdge, MousePosition, std::monostate> args;
};
}
}

#endif  // INTERFACE_MESSAGES_H
