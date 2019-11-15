/****************************************************************************
** Meta object code from reading C++ file 'CProcessDialog.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "stdafx.h"
#include "../../CProcessDialog.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CProcessDialog.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CProcessDialog_t {
    QByteArrayData data[8];
    char stringdata0[79];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CProcessDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CProcessDialog_t qt_meta_stringdata_CProcessDialog = {
    {
QT_MOC_LITERAL(0, 0, 14), // "CProcessDialog"
QT_MOC_LITERAL(1, 15, 13), // "OnOpenProcess"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 7), // "checked"
QT_MOC_LITERAL(4, 38, 8), // "OnCancel"
QT_MOC_LITERAL(5, 47, 9), // "OnRefresh"
QT_MOC_LITERAL(6, 57, 15), // "OnDoubleClicked"
QT_MOC_LITERAL(7, 73, 5) // "index"

    },
    "CProcessDialog\0OnOpenProcess\0\0checked\0"
    "OnCancel\0OnRefresh\0OnDoubleClicked\0"
    "index"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CProcessDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x08 /* Private */,
       1,    0,   52,    2, 0x28 /* Private | MethodCloned */,
       4,    1,   53,    2, 0x08 /* Private */,
       4,    0,   56,    2, 0x28 /* Private | MethodCloned */,
       5,    1,   57,    2, 0x08 /* Private */,
       5,    0,   60,    2, 0x28 /* Private | MethodCloned */,
       6,    1,   61,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QModelIndex,    7,

       0        // eod
};

void CProcessDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CProcessDialog *_t = static_cast<CProcessDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->OnOpenProcess((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->OnOpenProcess(); break;
        case 2: _t->OnCancel((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->OnCancel(); break;
        case 4: _t->OnRefresh((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->OnRefresh(); break;
        case 6: _t->OnDoubleClicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject CProcessDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_CProcessDialog.data,
      qt_meta_data_CProcessDialog,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *CProcessDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CProcessDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_CProcessDialog.stringdata0))
        return static_cast<void*>(const_cast< CProcessDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int CProcessDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
