#ifndef KERNEL_MESSAGES_H
#define KERNEL_MESSAGES_H
#include <cstddef>
#include <vector>
#include <string>

namespace max_flow_app {
namespace kernel_messages {
enum class Status { Basic, OnTheNetwork, OnThePath };

struct BasicEdge {
    size_t u, to, delta = 0;
};

struct Edge {
    size_t u, to, delta;
    Status status = Status::Basic;
};

struct MaxFlowData {
    std::vector<Edge> edges;
    std::vector<Status> vertices;
    size_t update_edge = std::string::npos;
    size_t flow_rate = 0, pushed_flow = 0;
};

Status GetPreviousStatus(Status status);
}  // namespace kernel_messages
}  // namespace max_flow_app
#endif  // KERNEL_MESSAGES_H
