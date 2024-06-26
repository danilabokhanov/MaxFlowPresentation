#include "kernel_messages.h"

namespace max_flow_app {
namespace kernel_messages {
Status GetPreviousStatus(Status status) {
    switch (status) {
        case Status::OnTheNetwork:
            return Status::Basic;
        case Status::OnThePath:
            return Status::OnTheNetwork;
        default:
            return Status::Basic;
    }
}
}  // namespace kernel_messages
}  // namespace max_flow_app
