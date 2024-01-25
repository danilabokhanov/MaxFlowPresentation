#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include <QFrame>
#include <QSpinBox>
#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui { // NOLINT
class MainWindow;
}
QT_END_NAMESPACE

namespace max_flow_app {
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QFrame* GetQwtFramePtr() const;
    QSpinBox* GetVerticesSpinBoxPtr() const;
    QPushButton* GetVerticesButtonPtr() const;
    QSpinBox* GetUSpinBoxPtr() const;
    QSpinBox* GetVSpinBoxPtr() const;
    QSpinBox* GetWeightSpinBoxPtr() const;
    QPushButton* GetAddButtonPtr() const;
    QPushButton* GetDeleteButtonPtr() const;
    QPushButton* GetGoNextButtonPtr() const;

private:

    std::unique_ptr<Ui::MainWindow> ui_;

};
}
#endif // MAINWINDOW_H
