/****************************************************************************
**
** Copyright (C) 2014 Ford Motor Company
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtRemoteObjects module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QREMOTEOBJECTS_ABSTRACT_ITEM_REPLICA_P_H
#define QREMOTEOBJECTS_ABSTRACT_ITEM_REPLICA_P_H

#include "qremoteobjectabstractitemmodeltypes.h"
#include "qremoteobjectabstractitemreplica.h"
#include "qremoteobjectreplica.h"
#include "qremoteobjectpendingcall.h"

namespace {
const int BufferSize = 1000;
const int LookAhead = 100;
const int HalfLookAhead = LookAhead/2;
}

typedef QVector<QPair<int,QVariant> > CacheEntry;
typedef QVector<CacheEntry> CachedRowEntry;
typedef QVector<CachedRowEntry> CacheData;

struct RequestedData
{
    IndexList start;
    IndexList end;
    QVector<int> roles;
};

struct RequestedHeaderData
{
    int role;
    int section;
    Qt::Orientation orientation;
};

class RowWatcher : public QRemoteObjectPendingCallWatcher
{
public:
    RowWatcher(IndexList _start, IndexList _end, QVector<int> _roles, const QRemoteObjectPendingReply<DataEntries> &reply)
        : QRemoteObjectPendingCallWatcher(reply),
          start(_start),
          end(_end),
          roles(_roles) {}
    IndexList start, end;
    QVector<int> roles;
};

class HeaderWatcher : public QRemoteObjectPendingCallWatcher
{
public:
    HeaderWatcher(QVector<Qt::Orientation> _orientations, QVector<int> _sections, QVector<int> _roles, const QRemoteObjectPendingReply<QVariantList> &reply)
        : QRemoteObjectPendingCallWatcher(reply),
          orientations(_orientations),
          sections(_sections),
          roles(_roles) {}
    QVector<Qt::Orientation> orientations;
    QVector<int> sections, roles;
};

class QAbstractItemReplicaPrivate : public QRemoteObjectReplica
{
    Q_OBJECT
    //TODO Use an input name for the model on the Replica side
    Q_CLASSINFO(QCLASSINFO_REMOTEOBJECT_TYPE, "ServerModelAdapter")
public:
    QAbstractItemReplicaPrivate();
    ~QAbstractItemReplicaPrivate();
    void initialize();
    void registerTypes();
    Q_PROPERTY(int rowCount READ rowCount NOTIFY rowCountChanged)
    Q_PROPERTY(int columnCount READ columnCount NOTIFY columnCountChanged)
    Q_PROPERTY(QVector<int> availableRoles READ availableRoles NOTIFY availableRolesChanged)

    int rowCount() const
    {
        return propAsVariant(0).value<int >();
    }

    int columnCount() const
    {
        return propAsVariant(1).value<int >();
    }

    QVector<int> availableRoles() const
    {
        return propAsVariant(2).value<QVector<int> >();
    }
    void setModel(QAbstractItemReplica *model) { q = model; setParent(model); }

    void clearCache(const IndexList &start, const IndexList &end, const QVector<int> &roles);

Q_SIGNALS:
    void rowCountChanged();
    void columnCountChanged();
    void availableRolesChanged();
    void dataChanged(IndexList topLeft, IndexList bottomRight, QVector<int> roles);
    void rowsInserted(IndexList parent, int first, int last);
    void rowsRemoved(IndexList parent, int first, int last);
    void rowsMoved(IndexList parent, int start, int end, IndexList destination, int row);
    void modelReset();
    void headerDataChanged(Qt::Orientation,int,int);

public Q_SLOTS:
    QRemoteObjectPendingReply<DataEntries> replicaRowRequest(IndexList start, IndexList end, QVector<int> roles)
    {
        static int __repc_index = QAbstractItemReplicaPrivate::staticMetaObject.indexOfSlot("replicaRowRequest(IndexList,IndexList,QVector<int>)");
        QVariantList __repc_args;
        __repc_args << QVariant::fromValue(start) << QVariant::fromValue(end) << QVariant::fromValue(roles);
        return QRemoteObjectPendingReply<DataEntries>(sendWithReply(QMetaObject::InvokeMetaMethod, __repc_index, __repc_args));
    }
    QRemoteObjectPendingReply<QVariantList> replicaHeaderRequest(QVector<Qt::Orientation> orientations, QVector<int> sections, QVector<int> roles)
    {
        static int __repc_index = QAbstractItemReplicaPrivate::staticMetaObject.indexOfSlot("replicaHeaderRequest(QVector<Qt::Orientation>,QVector<int>,QVector<int>)");
        QVariantList __repc_args;
        __repc_args << QVariant::fromValue(orientations) << QVariant::fromValue(sections) << QVariant::fromValue(roles);
        return QRemoteObjectPendingReply<QVariantList>(sendWithReply(QMetaObject::InvokeMetaMethod, __repc_index, __repc_args));
    }
    void onHeaderDataChanged(Qt::Orientation orientation, int first, int last);
    void onDataChanged(const IndexList &start, const IndexList &end, const QVector<int> &roles);
    void onRowsInserted(const IndexList &parent, int start, int end);
    void onRowsRemoved(const IndexList &parent, int start, int end);
    void onRowsMoved(IndexList srcParent, int srcRow, int count, IndexList destParent, int destRow);
    void onModelReset();
    void requestedData(QRemoteObjectPendingCallWatcher *);
    void requestedHeaderData(QRemoteObjectPendingCallWatcher *);
    void init();
    void fetchPendingData();
    void fetchPendingHeaderData();

public:
    QVector<CacheEntry> m_headerData[2];
    CacheData m_rows;
    int m_lastRequested;
    QVector<RequestedData> m_requestedData;
    QVector<RequestedHeaderData> m_requestedHeaderData;
    QAbstractItemReplica *q;
};

#endif // QREMOTEOBJECTS_ABSTRACT_ITEM_REPLICA_P_H

