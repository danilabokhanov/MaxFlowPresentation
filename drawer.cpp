#include "drawer.h"
#include <iostream>

namespace max_flow_app {
Drawer::Drawer(QFrame* frame): plot_(frame) {

}

void Drawer::DrawGraph(const MaxFlow::Data* data) {
    (void)data;
    std::cout << "! draw !\n";
    std::cout << "vertices status:\n";
    for (auto status : data->vertices_) {
        switch (status) {
            case MaxFlow::Status::Basic:
                std::cout << "Basic\n";
                break;
            case MaxFlow::Status::OnTheNetwork:
                std::cout << "Basic\n";
                break;
            case MaxFlow::Status::OnThePath:
                std::cout << "Basic\n";
                break;
            default:
                std::cout << "Unknown\n";
                break;
        }
    }
    std::cout << "edges:\n";
    for (auto& edge : data ->edges_) {
        std::cout << "u: " << edge.u << " to: " << edge.to << " delta: " << edge.delta << " ";
        switch (edge.status) {
            case MaxFlow::Status::Basic:
                std::cout << "Basic\n";
                break;
            case MaxFlow::Status::OnTheNetwork:
                std::cout << "Basic\n";
                break;
            case MaxFlow::Status::OnThePath:
                std::cout << "Basic\n";
                break;
            default:
                std::cout << "Unknown\n";
                break;
        }
    }
    }
}
