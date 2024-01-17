#ifndef MAX_FLOW_H
#define MAX_FLOW_H
#include <vector>
#include <cstddef>
#include "observer_pattern.h"

namespace max_flow_app {
class MaxFlow {
public:

    struct BasicEdge {
        size_t u, to, delta;
    };

    enum class Status { Basic, OnTheNetwork, OnThePath };

    struct Edge {
        size_t u, to, delta;
        Status status = Status::Basic;
    };

    struct Data {
        std::vector<Edge> edges_;
        std::vector<Status> vertices_;
    };


    MaxFlow();

    MaxFlow(size_t n, size_t m, const std::vector<BasicEdge>& edges);

    void ChangeVerticesNumber(size_t new_number);
    void AddEdge(const BasicEdge& edge);
    void DeleteEdge(const BasicEdge& egde);
    bool GoNext();

    size_t GetCurrentFlow() const;

    void RegisterView(observer_pattern::Observer<Data>* observer);

    const Data* GetData() const;

private:
    inline static const size_t kMaxCost = 32, kDefaultN = 0, kDefaultM = 2;
    bool FindNetwork();

    bool FindPath(size_t vertex);

    void ProcessPath();

    Edge& GetEdge(size_t index);

    Edge& GetReverseEdge(size_t index);

    const Edge& GetEdge(size_t index) const;

    const Edge& GetReverseEdge(size_t index) const;

    void ResetState();

    void AddEdges(const std::vector<BasicEdge>& edges);

    void SetVertexStatus(size_t vertex, const Status& status, bool should_notify);
    void SetEdgeStatus(size_t edge_id, const Status& status, bool should_notify);
    void SetVertexAndEdgeStatus(size_t vertex, size_t edge_id, const Status& status);
    void SetGraphToBasicStatus();

    size_t n_ = kDefaultN, m_ = kDefaultM;
    std::vector<std::vector<size_t>> graph_, network_;
    std::vector<size_t> processed_neighbors_;
    Data data_;
    std::vector<size_t> reversed_path_;

    size_t current_flow_rate_ = kMaxCost, current_flow_ = 0;
    bool is_path_processed_successfully_ = false;

    observer_pattern::Subject<Data> graph_ovservable_;
};

}
#endif // MAX_FLOW_H
