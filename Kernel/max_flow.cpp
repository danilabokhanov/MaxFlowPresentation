#include "max_flow.h"
#include <deque>
#include <sys/types.h>

namespace max_flow_app {
MaxFlow::MaxFlow() = default;

MaxFlow::MaxFlow(size_t n, size_t m, const std::vector<BasicEdge> &edges)
    : n_(n), m_(m), graph_(n), dist_(n_), processed_neighbors_(n),
      vertices_(n_, Status::Basic) {
    AddEdges(edges);
}

bool MaxFlow::GoNext() {
    std::vector<size_t> path;
    if (FindPath(0, path)) {
        ProcessPath(path);
        return true;
    }
    if (flow_rate_ == 1) {
        SetGraphToBasicStatus();
        return false;
    }
    flow_rate_ >>= 1;
    SetGraphToBasicStatus();
    FindNetwork();
    return true;
}

void MaxFlow::AddEdge(const MaxFlow::BasicEdge &edge) {
    ResetState();
    SetGraphToBasicStatus();

    for (Edge &e : edges_) {
        if (e.u == edge.u && e.to == edge.to) {
            e.delta += edge.delta;
            return;
        }
    }

    edges_.push_back({.u = edge.u, .to = edge.to, .delta = edge.delta});
    edges_.push_back({.u = edge.to, .to = edge.u, .delta = 0});
    graph_[edge.u].push_back(m_ << 1);
    graph_[edge.to].push_back((m_ << 1) + 1);
    m_++;
    graph_ovservable_.Notify();
}

void MaxFlow::DeleteEdge(const MaxFlow::BasicEdge &edge) {
    size_t index = 0;
    for (Edge &e : edges_) {
        if (e.u == edge.u && e.to == edge.to) {
            break;
        }
        index++;
    }
    if (index == edges_.size()) {
        return;
    }

    ResetState();
    SetGraphToBasicStatus();

    for (auto edges : graph_) {
        size_t deleted_index = edges.size();
        for (size_t i = 0; i < edges.size(); i++) {
            if (edges[i] == index) {
                deleted_index = i;
                break;
            }
        }
        if (deleted_index != edges.size()) {
            for (size_t i = 0; i < edges.size() - 1 - deleted_index; i++) {
                std::swap(edges[i + deleted_index], edges[i + deleted_index + 1]);
            }
            edges.pop_back();
        }
        for (auto &e : edges) {
            if (e > index) {
                e--;
            }
        }
    }
    m_--;
    graph_ovservable_.Notify();
}

void MaxFlow::ExtendNetwork(size_t vertex, std::vector<bool>& used,
                            std::vector<ssize_t>& parent, std::deque<size_t>& queue) {
    for (auto edge_id : graph_[vertex]) {
        auto [u, to, delta, _] = GetEdge(edge_id);
        if (delta < flow_rate_) {
            continue;
        }
        if (!used[to]) {
            parent[to] = edge_id;
            dist_[to] = dist_[u] + 1;
            used[to] = 1;
            queue.push_back(to);
        }
    }
}

void MaxFlow::ChangeNewEdgeStatus(size_t vertex, const std::vector<ssize_t>& parent) {
    if (vertex) {
        vertices_[vertex] = Status::OnTheNetwork;
        edges_[parent[vertex]].status = Status::OnTheNetwork;
        graph_ovservable_.Notify();
    }

}

size_t MaxFlow::ExtractVertice(std::deque<size_t>& queue) {
    size_t i = queue.front();
    queue.pop_front();
    return i;
}

void MaxFlow::FindingNetworkInit(std::deque<size_t>& queue, std::vector<ssize_t>& parent,
                                 std::vector<bool>& used) {
    queue = {0};
    parent.assign(n_, -1);
    dist_.assign(n_, n_ + 1);
    used.assign(n_, 0);
    used[0] = 1;
    dist_[0] = 0;
    vertices_[0] = Status::OnTheNetwork;
    graph_ovservable_.Notify();
}

bool MaxFlow::FindNetwork() {
    std::deque<size_t> queue;
    std::vector<ssize_t> parent(n_);
    std::vector<bool> used(n_);

    FindingNetworkInit(queue, parent, used);
    while (!queue.empty()) {
        size_t i = ExtractVertice(queue);
        ChangeNewEdgeStatus(i, parent);
        ExtendNetwork(i, used, parent, queue);
    }
    return used[n_ - 1];
}

bool MaxFlow::FindPath(size_t vertex, std::vector<size_t>& path) {
    if (vertex == n_ - 1) {
        return true;
    }
    processed_neighbors_.assign(n_, 0);
    while (processed_neighbors_[vertex] < graph_[vertex].size()) {
        size_t edge_id = graph_[vertex][processed_neighbors_[vertex]++];
        auto [u, to, delta, _] = GetEdge(edge_id);
        if (delta < flow_rate_ || dist_[u] + 1 != dist_[to]) {
            continue;
        }
        if (FindPath(to, path)) {
            path.push_back(edge_id);
            if (!to) {
                reverse(path.begin(), path.end());
            }
            return true;
        }
    }
    return false;
}

void MaxFlow::ProcessPath(std::vector<size_t>& path) {
    vertices_[0] = Status::OnThePath;
    graph_ovservable_.Notify();

    for (size_t edge_id : path) {
        vertices_[GetEdge(edge_id).to] = Status::OnThePath;
        edges_[edge_id].status = Status::OnThePath;
        graph_ovservable_.Notify();

        GetEdge(edge_id).delta -= flow_rate_;
        GetReverseEdge(edge_id).delta += flow_rate_;
    }
    pushed_flow_ += flow_rate_;
}

MaxFlow::Edge &MaxFlow::GetEdge(size_t index) {
    return edges_[index];
}

MaxFlow::Edge &MaxFlow::GetReverseEdge(size_t index) {
    return edges_[index ^ 1];
}

const MaxFlow::Edge &MaxFlow::GetEdge(size_t index) const {
    return edges_[index];
}

const MaxFlow::Edge &MaxFlow::GetReverseEdge(size_t index) const {
    return edges_[index ^ 1];
}

void MaxFlow::ResetState() {
    flow_rate_ = kMaxCost;
}

void MaxFlow::AddEdges(const std::vector<BasicEdge> &edges) {
    edges_.reserve(m_ << 1);
    for (auto [u, to, delta] : edges) {
        edges_.push_back({.u = u, .to = to, .delta = delta});
        edges_.push_back({.u = to, .to = u, .delta = 0});
        graph_[u].push_back(edges_.size() - 2);
        graph_[to].push_back(edges_.size() - 1);
    }
}

void MaxFlow::SetGraphToBasicStatus() {
    for (auto &vertex_status : vertices_) {
        vertex_status = Status::Basic;
    }
    for (auto &edge : edges_) {
        edge.status = Status::Basic;
    }
    graph_ovservable_.Notify();
}

size_t MaxFlow::GetCurrentFlow() const {
    return pushed_flow_;
}

MaxFlow::Data MaxFlow::GetData() const {
    return {.edges_ = edges_, .vertices_ = vertices_};
}

void MaxFlow::RegisterView(observer_pattern::Observer<Data>* observer) {
    graph_ovservable_.Subscribe(observer);
}

void MaxFlow::ChangeVerticesNumber(size_t new_number) {
    vertices_.resize(new_number, Status::Basic);
    std::vector<Edge> new_edges;
    std::vector<std::vector<size_t>> new_graph(new_number);
    for (const auto& edge : edges_) {
        if (edge.u < new_number && edge.to < new_number) {
            new_graph[edge.u].push_back(new_edges.size());
            new_edges.push_back(edge);
        }
    }
    n_ = new_number;
    m_ = (new_edges.size() >> 1);
    graph_ = std::move(new_graph);
    edges_ = std::move(new_edges);
    ResetState();
    SetGraphToBasicStatus();
}
}
