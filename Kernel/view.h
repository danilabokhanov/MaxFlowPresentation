#ifndef VIEW_H
#define VIEW_H

#include "Interface/mainwindow.h"
#include "Interface/drawer.h"
#include "mvc_messages.h"

namespace max_flow_app {
class View : public QObject {
    Q_OBJECT
public:
    View();

    using GraphObserver = observer_pattern::Observer<mvc_messages::MaxFlowData>;
    using Data = mvc_messages::CommandData;

    GraphObserver* GetSubscriberPtr();

    Data message_;

    using CommandObservable = observer_pattern::Observable<Data>;

    Data ProduceViewMessage() const;
    void RegisterController(observer_pattern::Observer<Data>* observer);

public slots:
    void ApplyButtonPressed();
    void AddButtonPressed();
    void DeleteButtonPressed();
    void GoNextButtonPressed();

private:

    void UpdateGraphView(const MaxFlow::Data& data);

    MainWindow main_window_;
    GraphObserver graph_observer_;
    CommandObservable command_observable_;
    Drawer drawer_;
};
}
#endif  // VIEW_H
