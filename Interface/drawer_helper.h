#ifndef DRAWER_HELPER_H
#define DRAWER_HELPER_H

#include <QFrame>
#include "Kernel/kernel_messages.h"

namespace max_flow_app {
struct DrawerHelper {
    using Status = kernel_messages::Status;

    static QPointF RotateVector(const QPointF& begin, const QPointF& end, double angle, double len);
    static double GetVectorLength(const QPointF& vec);
    static QPointF SetVectorLength(const QPointF& vec, double len);

    inline static const double kCurveRate = 0.5, kEdgeHeadSide = 0.35, kVertexRadius = 30,
                               kCenterPadding = 0.3, kMaxX = 10, kMaxY = 10, kSinkPadingRate = 0.05,
                               kEdgeWidth = 4, kDistEps = 0.05;
    inline static const QFont kGraphFont = QFont("Arial", 17, QFont::Bold);
    inline static const QFont kFlowRateFont = QFont("Arial", 20, QFont::Bold);
    inline static const QFont kPushFlowFont = kFlowRateFont;
    inline static const QPointF kFlowInfoPos = QPointF(kMaxX * 0.87, kMaxY * 0.96);
    inline static const std::map<Status, QColor> kVertexColor = {
        {Status::Basic, {192, 192, 192}},
        {Status::OnTheNetwork, {255, 102, 102}},
        {Status::OnThePath, {102, 178, 255}}};
    inline static const std::map<Status, QColor> kEdgeColor = {{Status::Basic, {30, 30, 30}},
                                                               {Status::OnTheNetwork, {255, 0, 0}},
                                                               {Status::OnThePath, {0, 102, 255}}};
    inline static const std::map<Status, QColor> kBasicColor = {{Status::Basic, {0, 0, 0}},
                                                                {Status::OnTheNetwork, {102, 0, 0}},
                                                                {Status::OnThePath, {0, 0, 102}}};
};
}  // namespace max_flow_app

#endif  // DRAWER_HELPER_H
