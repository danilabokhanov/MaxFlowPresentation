#include "mvc_messages.h"

namespace max_flow_app {
namespace mvc_messages {
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
}
}
