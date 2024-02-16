#ifndef MAX_FLOW_H
#define MAX_FLOW_H
#include <vector>
#include <cstddef>
#include <deque>
#include "Library/observer_pattern.h"
#include "mvc_messages.h"

namespace max_flow_app {
class MaxFlow {
public:
    using BasicEdge = mvc_messages::BasicEdge;
    using Edge = mvc_messages::Edge;
    using Data = mvc_messages::MaxFlowData;
    using Status = mvc_messages::Status;

    MaxFlow() = default;
    MaxFlow(size_t n, size_t m, const std::vector<BasicEdge>& edges);

    void ChangeVerticesNumber(size_t new_number);
    void AddEdge(const BasicEdge& edge);
    void DeleteEdge(const BasicEdge& egde);
    bool GoNext();

    size_t GetCurrentFlow() const;

    void RegisterView(observer_pattern::Observer<Data>* observer);

    Data GetData() const;

private:
    inline static const size_t kDefaultN = 2, kDefaultM = 0;
    bool FindNetwork();
    void ExtendNetwork(size_t vertex, std::vector<bool>& used,
                       std::vector<ssize_t>& parent, std::deque<size_t>& queue);
    void ChangeNewEdgeStatus(size_t vertex, const std::vector<ssize_t>& parent);
    size_t ExtractVertice(std::deque<size_t>& queue);
    void FindingNetworkInit(std::deque<size_t>& queue, std::vector<ssize_t>& parent,
                            std::vector<bool>& used);
    bool FindPath(size_t vertex, std::vector<size_t>& path);
    void ProcessPath(std::vector<size_t>& path);

    Edge& GetEdge(size_t index);
    Edge& GetReverseEdge(size_t index);
    const Edge& GetEdge(size_t index) const;
    const Edge& GetReverseEdge(size_t index) const;
    void AddEdges(const std::vector<BasicEdge>& edges);

    void ResetState();

    size_t n_ = kDefaultN, m_ = kDefaultM;
    std::vector<std::vector<size_t>> graph_ = std::vector<std::vector<size_t>>(kDefaultN);
    std::vector<size_t> dist_ = std::vector<size_t>(kDefaultN);
    std::vector<size_t> processed_neighbors_ = std::vector<size_t>(kDefaultN);
    std::vector<Edge> edges_;
    std::vector<Status> vertices_ = std::vector<Status>(n_, Status::Basic);

    size_t flow_rate_ = 1, pushed_flow_ = 0;

    observer_pattern::Observable<Data> graph_ovservable_ =
        observer_pattern::Observable<Data> ([this] () {return GetData();});
};

}
#endif // MAX_FLOW_H
