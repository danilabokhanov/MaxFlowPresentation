#ifndef VIEW_H
#define VIEW_H

#include "Interface/mainwindow.h"
#include "Interface/drawer.h"
#include "Library/observer_pattern.h"
#include "mvc_messages.h"

namespace max_flow_app {
class View : public QObject {
    Q_OBJECT
public:
    View();

    using ReceivingData = mvc_messages::MaxFlowData;
    using SendingData = mvc_messages::CommandData;
    using GeomModelObserver = observer_pattern::Observer<ReceivingData>;
    using CommandObservable = observer_pattern::Observable<SendingData>;
    using CommandObserver = observer_pattern::Observer<SendingData>;
    using BasicEdge = mvc_messages::BasicEdge;

    GeomModelObserver* GetSubscriberPtr();

    SendingData message_;

    SendingData ProduceViewMessage() const;
    void RegisterController(CommandObserver* observer);

public slots:
    void ApplyButtonPressed();
    void AddButtonPressed();
    void DeleteButtonPressed();
    void RunButtonPressed();
    void GenRandomSampleButtonPressed();

private:

    void UpdateGraphView(const ReceivingData& data);

    MainWindow main_window_;
    Drawer drawer_;
    GeomModelObserver geom_model_observer_;
    CommandObservable command_observable_;
};
}
#endif  // VIEW_H
