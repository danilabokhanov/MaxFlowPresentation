#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include <QFrame>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QSlider>

QT_BEGIN_NAMESPACE
namespace Ui {  // NOLINT
class MainWindow;
}
QT_END_NAMESPACE

namespace max_flow_app {
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    QFrame* GetQwtFramePtr() const;
    QSpinBox* GetVerticesSpinBoxPtr() const;
    QPushButton* GetVerticesButtonPtr() const;
    QSpinBox* GetUSpinBoxPtr() const;
    QSpinBox* GetVSpinBoxPtr() const;
    QSpinBox* GetWeightSpinBoxPtr() const;
    QPushButton* GetAddButtonPtr() const;
    QPushButton* GetDeleteButtonPtr() const;
    QPushButton* GetRunButtonPtr() const;
    QPushButton* GetRandomSampleButtonPtr() const;
    QPushButton* GetCancelButtonPtr() const;
    QPushButton* GetSkipButtonPtr() const;
    QSlider* GetSpeedSliderPtr() const;
    QSlider* GetLatencySliderPtr() const;
    size_t GetSpeedSliderNumber() const;
    size_t GetLatencySliderNumber() const;
    size_t GetVerticesSpinBoxNumber() const;
    size_t GetUSpinBoxNumber() const;
    size_t GetVSpinBoxNumber() const;
    size_t GetWeightBoxNumber() const;

private:
    std::unique_ptr<Ui::MainWindow> ui_;
};
}  // namespace max_flow_app
#endif  // MAINWINDOW_H
