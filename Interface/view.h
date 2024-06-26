#ifndef VIEW_H
#define VIEW_H

#include "Interface/mainwindow.h"
#include "Interface/drawer.h"
#include "Library/observer_pattern.h"
#include <qwt_plot_picker.h>

namespace max_flow_app {
class View : public QObject {
    Q_OBJECT
public:
    using FrameQueueData = interface_messages::FrameQueueData;
    using CommandData = interface_messages::CommandData;
    using FrameQueueObserver = observer_pattern::Observer<FrameQueueData>;
    using CommandObserver = observer_pattern::Observer<CommandData>;

    View();

    FrameQueueObserver* GetSubscriberPtr();
    void RegisterController(CommandObserver* observer);

private slots:
    void ApplyButtonPressed();
    void AddButtonPressed();
    void DeleteButtonPressed();
    void RunButtonPressed();
    void GenRandomSampleButtonPressed();
    void CancelButtonPressed();
    void SkipButtonPressed();
    void MousePressed(const QPointF& pos);
    void MouseMoved(const QPointF& pos);
    void MouseReleased(const QPointF& pos);
    void SpeedSliderMoved(int slider_pos);
    void LatencySliderMoved(int slider_pos);

private:
    using CommandObservable = observer_pattern::Observable<CommandData>;
    using BasicEdge = kernel_messages::BasicEdge;
    using MousePosition = interface_messages::MousePosition;

    void SetupButtons();
    void SetupPicker();
    void LockInterface();
    void UnlockInterface();
    void UpdateGraphView(const FrameQueueData& data);
    const CommandData& ProduceViewMessage() const;

    MainWindow main_window_;
    Drawer drawer_;
    CommandData message_;
    FrameQueueObserver geom_model_observer_;
    CommandObservable command_observable_;
    QwtPlotPicker* picker_;
};
}  // namespace max_flow_app
#endif  // VIEW_H
