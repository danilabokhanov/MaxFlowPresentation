#include "drawer_helper.h"
#include <math.h>

namespace max_flow_app {
QPointF DrawerHelper::RotateVector(const QPointF& begin, const QPointF& end, double angle,
                                   double len) {
    QPointF direction(end - begin);
    double polar = atan2(direction.y(), direction.x());
    QPointF res = begin + QPointF(cos(polar + angle) * len, sin(polar + angle) * len);
    return res;
}

double DrawerHelper::GetVectorLength(const QPointF& vec) {
    return sqrt(vec.x() * vec.x() + vec.y() * vec.y());
}

QPointF DrawerHelper::SetVectorLength(const QPointF& vec, double len) {
    double cur_len = GetVectorLength(vec);
    return QPointF(vec.x() / cur_len * len, vec.y() / cur_len * len);
}
}  // namespace max_flow_app
