#ifndef MVC_MESSAGES_H
#define MVC_MESSAGES_H
#include <vector>
#include <cstddef>
#include <variant>

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
};

struct CommandData {
    struct Empty {

    };

    enum {
        CHANGE_VERTICES_NUMBER,
        ADD_EDGE,
        DELETE_EDGE,
        RUN,
        GEN_RANDOM_SAMPLE
    } signal_type;
    std::variant<size_t, BasicEdge, Empty> args;
};

}
}

#endif  // MVC_MESSAGES_H
