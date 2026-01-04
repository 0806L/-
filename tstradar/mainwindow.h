#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_mainwindow.h"

class RadarCtrlDlg;
class RadarWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:

    RadarWidget* m_widget;
    RadarCtrlDlg* m_ctrlDlg;

private:
    Ui::MainWindowClass ui;
};
