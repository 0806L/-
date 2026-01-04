/****************************************************************************
** Meta object code from reading C++ file 'radarctrldlg.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../radarctrldlg.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'radarctrldlg.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_RadarCtrlDlg_t {
    QByteArrayData data[18];
    char stringdata0[365];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_RadarCtrlDlg_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_RadarCtrlDlg_t qt_meta_stringdata_RadarCtrlDlg = {
    {
QT_MOC_LITERAL(0, 0, 12), // "RadarCtrlDlg"
QT_MOC_LITERAL(1, 13, 29), // "on_pushButtonRangeAdd_clicked"
QT_MOC_LITERAL(2, 43, 0), // ""
QT_MOC_LITERAL(3, 44, 29), // "on_pushButtonRangeSub_clicked"
QT_MOC_LITERAL(4, 74, 23), // "on_checkBoxEBL1_clicked"
QT_MOC_LITERAL(5, 98, 23), // "on_checkBoxEBL2_clicked"
QT_MOC_LITERAL(6, 122, 23), // "on_checkBoxVRM1_clicked"
QT_MOC_LITERAL(7, 146, 23), // "on_checkBoxVRM2_clicked"
QT_MOC_LITERAL(8, 170, 36), // "on_horizontalSliderGain_value..."
QT_MOC_LITERAL(9, 207, 3), // "val"
QT_MOC_LITERAL(10, 211, 36), // "on_horizontalSliderTune_value..."
QT_MOC_LITERAL(11, 248, 35), // "on_horizontalSliderSea_valueC..."
QT_MOC_LITERAL(12, 284, 36), // "on_horizontalSliderRain_value..."
QT_MOC_LITERAL(13, 321, 20), // "slotCursorPosChanged"
QT_MOC_LITERAL(14, 342, 3), // "lat"
QT_MOC_LITERAL(15, 346, 3), // "lon"
QT_MOC_LITERAL(16, 350, 7), // "bearing"
QT_MOC_LITERAL(17, 358, 6) // "distNM"

    },
    "RadarCtrlDlg\0on_pushButtonRangeAdd_clicked\0"
    "\0on_pushButtonRangeSub_clicked\0"
    "on_checkBoxEBL1_clicked\0on_checkBoxEBL2_clicked\0"
    "on_checkBoxVRM1_clicked\0on_checkBoxVRM2_clicked\0"
    "on_horizontalSliderGain_valueChanged\0"
    "val\0on_horizontalSliderTune_valueChanged\0"
    "on_horizontalSliderSea_valueChanged\0"
    "on_horizontalSliderRain_valueChanged\0"
    "slotCursorPosChanged\0lat\0lon\0bearing\0"
    "distNM"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RadarCtrlDlg[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   69,    2, 0x09 /* Protected */,
       3,    0,   70,    2, 0x09 /* Protected */,
       4,    0,   71,    2, 0x09 /* Protected */,
       5,    0,   72,    2, 0x09 /* Protected */,
       6,    0,   73,    2, 0x09 /* Protected */,
       7,    0,   74,    2, 0x09 /* Protected */,
       8,    1,   75,    2, 0x09 /* Protected */,
      10,    1,   78,    2, 0x09 /* Protected */,
      11,    1,   81,    2, 0x09 /* Protected */,
      12,    1,   84,    2, 0x09 /* Protected */,
      13,    4,   87,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void, QMetaType::Float, QMetaType::Float, QMetaType::Float, QMetaType::Float,   14,   15,   16,   17,

       0        // eod
};

void RadarCtrlDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<RadarCtrlDlg *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_pushButtonRangeAdd_clicked(); break;
        case 1: _t->on_pushButtonRangeSub_clicked(); break;
        case 2: _t->on_checkBoxEBL1_clicked(); break;
        case 3: _t->on_checkBoxEBL2_clicked(); break;
        case 4: _t->on_checkBoxVRM1_clicked(); break;
        case 5: _t->on_checkBoxVRM2_clicked(); break;
        case 6: _t->on_horizontalSliderGain_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->on_horizontalSliderTune_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->on_horizontalSliderSea_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->on_horizontalSliderRain_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->slotCursorPosChanged((*reinterpret_cast< float(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2])),(*reinterpret_cast< float(*)>(_a[3])),(*reinterpret_cast< float(*)>(_a[4]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject RadarCtrlDlg::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_RadarCtrlDlg.data,
    qt_meta_data_RadarCtrlDlg,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *RadarCtrlDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RadarCtrlDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_RadarCtrlDlg.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int RadarCtrlDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
