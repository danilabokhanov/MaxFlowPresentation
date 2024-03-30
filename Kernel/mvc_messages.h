#ifndef MVC_MESSAGES_H
#define MVC_MESSAGES_H
#include <vector>
#include <cstddef>
#include <variant>
#include <string>

namespace max_flow_app {
namespace mvc_messages {

enum class Status { Basic, OnTheNetwork, OnThePath };

struct BasicEdge {
    size_t u, to, delta;
};

struct Edge {
    size_t u, to, delta;
    Status status = Status::Basic;
};

struct MaxFlowData {
    std::vector<Edge> edges;
    std::vector<Status> vertices;
    size_t update_edge = std::string::npos;
};

struct GeomModelData {
    std::vector<Edge> edges;
    std::vector<Status> vertices;
    size_t edge_id = std::string::npos;
    size_t frame_id;
    size_t frames_number;
};

struct CommandData {
    struct Empty {

    };

    enum {
        CHANGE_VERTICES_NUMBER,
        ADD_EDGE,
        DELETE_EDGE,
        RUN,
        GEN_RANDOM_SAMPLE,
        CANCEL,
        SKIP
    } signal_type;
    std::variant<size_t, BasicEdge, Empty> args;
};

Status GetPreviousStatus(Status status);
}
}

#endif  // MVC_MESSAGES_H
