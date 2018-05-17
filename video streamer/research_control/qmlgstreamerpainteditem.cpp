/*
 * Copyright 2017 The University of Oklahoma.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "qmlgstreamerpainteditem.h"
#include "soro_core/logger.h"
#include "soro_core/constants.h"

#include <QPainter>
#include <Qt5GStreamer/QGlib/Connect>
#include <Qt5GStreamer/QGlib/Signal>
#include <Qt5GStreamer/QGst/ElementFactory>

#define LOG_TAG "QmlGStreamerPaintedSurface"

namespace Soro {

QmlGStreamerPaintedItem::QmlGStreamerPaintedItem(QQuickItem *parent): QQuickPaintedItem(parent)
{
}

QmlGStreamerPaintedItem::~QmlGStreamerPaintedItem()
{
    if (!_sink.isNull())
    {
        _sink->setState(QGst::StateNull);
        _sink.clear();
    }
}

void QmlGStreamerPaintedItem::paint(QPainter *painter)
{
    if (!_sink.isNull())
    {
        // This line will show a syntax error if QT_NO_KEYWORDS is not defined
        QGlib::emit<void>(_sink, "paint", (void *) painter, (qreal)0, (qreal)0, (qreal)width(), (qreal)height());
    }
}

QGst::ElementPtr QmlGStreamerPaintedItem::videoSink()
{
    if (_sink.isNull())
    {
        _sink = QGst::ElementFactory::make("qt5videosink");
    }

    if (!_sink.isNull())
    {
        QGlib::connect(_sink, "update", this, &QmlGStreamerPaintedItem::onUpdate);
    }
    else
    {
        LOG_E(LOG_TAG, "Failed to create qt5videosink");
    }

    return _sink;
}

void QmlGStreamerPaintedItem::onUpdate()
{
    update();
}

} // namespace Soro
