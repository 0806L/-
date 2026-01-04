#pragma once

#include <QWidget>
#include "ui_radarctrldlg.h"

class RadarController;

class RadarCtrlDlg : public QWidget
{
    Q_OBJECT

public:
    RadarCtrlDlg(QWidget *parent = nullptr);
    ~RadarCtrlDlg();

    void setRadarController(RadarController* c);

protected slots:

    void on_pushButtonRangeAdd_clicked();

    void on_pushButtonRangeSub_clicked();

    void on_checkBoxEBL1_clicked();
    void on_checkBoxEBL2_clicked();
    void on_checkBoxVRM1_clicked();
    void on_checkBoxVRM2_clicked();

    void on_horizontalSliderGain_valueChanged(int val);
    void on_horizontalSliderTune_valueChanged(int val);
    void on_horizontalSliderSea_valueChanged(int val);
    void on_horizontalSliderRain_valueChanged(int val);

    void slotCursorPosChanged(float lat, float lon, float bearing, float distNM);

protected:

    // RadarWidget* m_widget;
    RadarController* m_controller;

private:
    Ui::RadarCtrlDlgClass ui;
};
