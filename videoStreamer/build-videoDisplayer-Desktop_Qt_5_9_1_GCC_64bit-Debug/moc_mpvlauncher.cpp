/****************************************************************************
** Meta object code from reading C++ file 'mpvlauncher.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../videoDisplayer/mpvlauncher.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mpvlauncher.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MPVLauncher_t {
    QByteArrayData data[8];
    char stringdata0[84];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MPVLauncher_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MPVLauncher_t qt_meta_stringdata_MPVLauncher = {
    {
QT_MOC_LITERAL(0, 0, 11), // "MPVLauncher"
QT_MOC_LITERAL(1, 12, 9), // "beatHeart"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 14), // "checkHeartBeat"
QT_MOC_LITERAL(4, 38, 10), // "DataPacket"
QT_MOC_LITERAL(5, 49, 6), // "packet"
QT_MOC_LITERAL(6, 56, 9), // "onMessage"
QT_MOC_LITERAL(7, 66, 17) // "attemptConnection"

    },
    "MPVLauncher\0beatHeart\0\0checkHeartBeat\0"
    "DataPacket\0packet\0onMessage\0"
    "attemptConnection"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MPVLauncher[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x0a /* Public */,
       3,    1,   35,    2, 0x0a /* Public */,
       6,    1,   38,    2, 0x0a /* Public */,
       7,    0,   41,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void,

       0        // eod
};

void MPVLauncher::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MPVLauncher *_t = static_cast<MPVLauncher *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->beatHeart(); break;
        case 1: _t->checkHeartBeat((*reinterpret_cast< DataPacket(*)>(_a[1]))); break;
        case 2: _t->onMessage((*reinterpret_cast< DataPacket(*)>(_a[1]))); break;
        case 3: _t->attemptConnection(); break;
        default: ;
        }
    }
}

const QMetaObject MPVLauncher::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_MPVLauncher.data,
      qt_meta_data_MPVLauncher,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *MPVLauncher::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MPVLauncher::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MPVLauncher.stringdata0))
        return static_cast<void*>(const_cast< MPVLauncher*>(this));
    return QObject::qt_metacast(_clname);
}

int MPVLauncher::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
