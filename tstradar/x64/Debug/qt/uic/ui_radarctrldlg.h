/********************************************************************************
** Form generated from reading UI file 'radarctrldlg.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RADARCTRLDLG_H
#define UI_RADARCTRLDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RadarCtrlDlgClass
{
public:
    QVBoxLayout *verticalLayout;
    QPushButton *pushButtonRangeAdd;
    QPushButton *pushButtonRangeSub;
    QGridLayout *gridLayout;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label;
    QSlider *horizontalSliderGain;
    QLabel *label_4;
    QSlider *horizontalSliderTune;
    QSlider *horizontalSliderSea;
    QSlider *horizontalSliderRain;
    QGridLayout *gridLayout_2;
    QCheckBox *checkBoxEBL1;
    QCheckBox *checkBoxEBL2;
    QCheckBox *checkBoxVRM1;
    QCheckBox *checkBoxVRM2;
    QGridLayout *gridLayout_3;
    QLabel *label_7;
    QLabel *label_6;
    QLabel *label_5;
    QLineEdit *lineEditCursorLat;
    QLineEdit *lineEditCursorLon;
    QLabel *label_8;
    QLineEdit *lineEditBRG;
    QLineEdit *lineEditDIST;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *RadarCtrlDlgClass)
    {
        if (RadarCtrlDlgClass->objectName().isEmpty())
            RadarCtrlDlgClass->setObjectName(QString::fromUtf8("RadarCtrlDlgClass"));
        RadarCtrlDlgClass->resize(293, 390);
        verticalLayout = new QVBoxLayout(RadarCtrlDlgClass);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        pushButtonRangeAdd = new QPushButton(RadarCtrlDlgClass);
        pushButtonRangeAdd->setObjectName(QString::fromUtf8("pushButtonRangeAdd"));

        verticalLayout->addWidget(pushButtonRangeAdd);

        pushButtonRangeSub = new QPushButton(RadarCtrlDlgClass);
        pushButtonRangeSub->setObjectName(QString::fromUtf8("pushButtonRangeSub"));

        verticalLayout->addWidget(pushButtonRangeSub);

        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label_2 = new QLabel(RadarCtrlDlgClass);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        label_3 = new QLabel(RadarCtrlDlgClass);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 2, 0, 1, 1);

        label = new QLabel(RadarCtrlDlgClass);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        horizontalSliderGain = new QSlider(RadarCtrlDlgClass);
        horizontalSliderGain->setObjectName(QString::fromUtf8("horizontalSliderGain"));
        horizontalSliderGain->setMaximum(10);
        horizontalSliderGain->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(horizontalSliderGain, 0, 1, 1, 1);

        label_4 = new QLabel(RadarCtrlDlgClass);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout->addWidget(label_4, 3, 0, 1, 1);

        horizontalSliderTune = new QSlider(RadarCtrlDlgClass);
        horizontalSliderTune->setObjectName(QString::fromUtf8("horizontalSliderTune"));
        horizontalSliderTune->setMaximum(10);
        horizontalSliderTune->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(horizontalSliderTune, 1, 1, 1, 1);

        horizontalSliderSea = new QSlider(RadarCtrlDlgClass);
        horizontalSliderSea->setObjectName(QString::fromUtf8("horizontalSliderSea"));
        horizontalSliderSea->setMaximum(10);
        horizontalSliderSea->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(horizontalSliderSea, 2, 1, 1, 1);

        horizontalSliderRain = new QSlider(RadarCtrlDlgClass);
        horizontalSliderRain->setObjectName(QString::fromUtf8("horizontalSliderRain"));
        horizontalSliderRain->setMaximum(10);
        horizontalSliderRain->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(horizontalSliderRain, 3, 1, 1, 1);


        verticalLayout->addLayout(gridLayout);

        gridLayout_2 = new QGridLayout();
        gridLayout_2->setSpacing(6);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        checkBoxEBL1 = new QCheckBox(RadarCtrlDlgClass);
        checkBoxEBL1->setObjectName(QString::fromUtf8("checkBoxEBL1"));

        gridLayout_2->addWidget(checkBoxEBL1, 0, 0, 1, 1);

        checkBoxEBL2 = new QCheckBox(RadarCtrlDlgClass);
        checkBoxEBL2->setObjectName(QString::fromUtf8("checkBoxEBL2"));

        gridLayout_2->addWidget(checkBoxEBL2, 0, 1, 1, 1);

        checkBoxVRM1 = new QCheckBox(RadarCtrlDlgClass);
        checkBoxVRM1->setObjectName(QString::fromUtf8("checkBoxVRM1"));

        gridLayout_2->addWidget(checkBoxVRM1, 1, 0, 1, 1);

        checkBoxVRM2 = new QCheckBox(RadarCtrlDlgClass);
        checkBoxVRM2->setObjectName(QString::fromUtf8("checkBoxVRM2"));

        gridLayout_2->addWidget(checkBoxVRM2, 1, 1, 1, 1);


        verticalLayout->addLayout(gridLayout_2);

        gridLayout_3 = new QGridLayout();
        gridLayout_3->setSpacing(6);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        label_7 = new QLabel(RadarCtrlDlgClass);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        gridLayout_3->addWidget(label_7, 2, 0, 1, 1);

        label_6 = new QLabel(RadarCtrlDlgClass);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        gridLayout_3->addWidget(label_6, 0, 0, 1, 1);

        label_5 = new QLabel(RadarCtrlDlgClass);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout_3->addWidget(label_5, 1, 0, 1, 1);

        lineEditCursorLat = new QLineEdit(RadarCtrlDlgClass);
        lineEditCursorLat->setObjectName(QString::fromUtf8("lineEditCursorLat"));

        gridLayout_3->addWidget(lineEditCursorLat, 0, 1, 1, 1);

        lineEditCursorLon = new QLineEdit(RadarCtrlDlgClass);
        lineEditCursorLon->setObjectName(QString::fromUtf8("lineEditCursorLon"));

        gridLayout_3->addWidget(lineEditCursorLon, 1, 1, 1, 1);

        label_8 = new QLabel(RadarCtrlDlgClass);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        gridLayout_3->addWidget(label_8, 3, 0, 1, 1);

        lineEditBRG = new QLineEdit(RadarCtrlDlgClass);
        lineEditBRG->setObjectName(QString::fromUtf8("lineEditBRG"));

        gridLayout_3->addWidget(lineEditBRG, 2, 1, 1, 1);

        lineEditDIST = new QLineEdit(RadarCtrlDlgClass);
        lineEditDIST->setObjectName(QString::fromUtf8("lineEditDIST"));

        gridLayout_3->addWidget(lineEditDIST, 3, 1, 1, 1);


        verticalLayout->addLayout(gridLayout_3);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        retranslateUi(RadarCtrlDlgClass);

        QMetaObject::connectSlotsByName(RadarCtrlDlgClass);
    } // setupUi

    void retranslateUi(QWidget *RadarCtrlDlgClass)
    {
        RadarCtrlDlgClass->setWindowTitle(QCoreApplication::translate("RadarCtrlDlgClass", "RadarCtrlDlg", nullptr));
        pushButtonRangeAdd->setText(QCoreApplication::translate("RadarCtrlDlgClass", "\351\207\217\347\250\213\345\212\240", nullptr));
        pushButtonRangeSub->setText(QCoreApplication::translate("RadarCtrlDlgClass", "\351\207\217\347\250\213\345\207\217", nullptr));
        label_2->setText(QCoreApplication::translate("RadarCtrlDlgClass", "tune", nullptr));
        label_3->setText(QCoreApplication::translate("RadarCtrlDlgClass", "sea", nullptr));
        label->setText(QCoreApplication::translate("RadarCtrlDlgClass", "gain", nullptr));
        label_4->setText(QCoreApplication::translate("RadarCtrlDlgClass", "rain", nullptr));
        checkBoxEBL1->setText(QCoreApplication::translate("RadarCtrlDlgClass", "EBL1", nullptr));
        checkBoxEBL2->setText(QCoreApplication::translate("RadarCtrlDlgClass", "EBL2", nullptr));
        checkBoxVRM1->setText(QCoreApplication::translate("RadarCtrlDlgClass", "VRM1", nullptr));
        checkBoxVRM2->setText(QCoreApplication::translate("RadarCtrlDlgClass", "VRM2", nullptr));
        label_7->setText(QCoreApplication::translate("RadarCtrlDlgClass", "\346\226\271\344\275\215", nullptr));
        label_6->setText(QCoreApplication::translate("RadarCtrlDlgClass", "\351\274\240\346\240\207\347\272\254\345\272\246", nullptr));
        label_5->setText(QCoreApplication::translate("RadarCtrlDlgClass", "\351\274\240\346\240\207\347\273\217\345\272\246", nullptr));
        label_8->setText(QCoreApplication::translate("RadarCtrlDlgClass", "\350\267\235\347\246\273", nullptr));
    } // retranslateUi

};

namespace Ui {
    class RadarCtrlDlgClass: public Ui_RadarCtrlDlgClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RADARCTRLDLG_H
