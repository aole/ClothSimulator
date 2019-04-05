#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <QMainWindow>

namespace Ui {
class MainFrame;
}

class MainFrame : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainFrame(QWidget *parent = nullptr);
    ~MainFrame();

private slots:
    void on_actionExit_triggered();

    void on_btnSimulate_clicked();

    void on_actionSimulate_triggered();

private:
    Ui::MainFrame *ui;
};

#endif // MAINFRAME_H
