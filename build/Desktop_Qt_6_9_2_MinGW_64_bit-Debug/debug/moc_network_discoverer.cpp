/****************************************************************************
** Meta object code from reading C++ file 'network_discoverer.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../network_discoverer.h"
#include <QtNetwork/QSslPreSharedKeyAuthenticator>
#include <QtNetwork/QSslError>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'network_discoverer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.9.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN17NetworkDiscovererE_t {};
} // unnamed namespace

template <> constexpr inline auto NetworkDiscoverer::qt_create_metaobjectdata<qt_meta_tag_ZN17NetworkDiscovererE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "NetworkDiscoverer",
        "deviceDiscovered",
        "",
        "NetworkDevice",
        "device",
        "discoveryFinished",
        "progressUpdate",
        "value"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'deviceDiscovered'
        QtMocHelpers::SignalData<void(const NetworkDevice &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'discoveryFinished'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'progressUpdate'
        QtMocHelpers::SignalData<void(int)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 7 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<NetworkDiscoverer, qt_meta_tag_ZN17NetworkDiscovererE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject NetworkDiscoverer::staticMetaObject = { {
    QMetaObject::SuperData::link<QThread::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17NetworkDiscovererE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17NetworkDiscovererE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN17NetworkDiscovererE_t>.metaTypes,
    nullptr
} };

void NetworkDiscoverer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<NetworkDiscoverer *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->deviceDiscovered((*reinterpret_cast< std::add_pointer_t<NetworkDevice>>(_a[1]))); break;
        case 1: _t->discoveryFinished(); break;
        case 2: _t->progressUpdate((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (NetworkDiscoverer::*)(const NetworkDevice & )>(_a, &NetworkDiscoverer::deviceDiscovered, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (NetworkDiscoverer::*)()>(_a, &NetworkDiscoverer::discoveryFinished, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (NetworkDiscoverer::*)(int )>(_a, &NetworkDiscoverer::progressUpdate, 2))
            return;
    }
}

const QMetaObject *NetworkDiscoverer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NetworkDiscoverer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17NetworkDiscovererE_t>.strings))
        return static_cast<void*>(this);
    return QThread::qt_metacast(_clname);
}

int NetworkDiscoverer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void NetworkDiscoverer::deviceDiscovered(const NetworkDevice & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void NetworkDiscoverer::discoveryFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void NetworkDiscoverer::progressUpdate(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}
QT_WARNING_POP
