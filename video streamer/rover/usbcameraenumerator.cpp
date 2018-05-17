/*
 * Copyright 2016 The University of Oklahoma.
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

#include "usbcameraenumerator.h"
#include "soro_core/logger.h"

#define LOG_TAG "UsbCameraEnumerator"


namespace Soro {

int UsbCameraEnumerator::loadCameras() {
    clearList();

    // Search for all /dev/video* devices

    QDir dev("/dev");
    QStringList allFiles = dev.entryList(QDir::NoDotAndDotDot | QDir::System | QDir::Files);

    for (QString file : allFiles) {
        if (file.contains("video")) {

            // Found a new dev device
            UsbCamera *camera = new UsbCamera;
            camera->device = "/dev/" + file;

            // Use udevadm to get info about each /dev/video* device
            QProcess udevadm;
            udevadm.start("udevadm info -a -n " + camera->device);
            udevadm.waitForFinished();
            QString udevadmOutput = QString(udevadm.readAllStandardOutput());

            QLatin1String nameToken("{name}==\"");
            int nameIndex = udevadmOutput.indexOf(nameToken);
            if (nameIndex >= 0) {
                nameIndex += nameToken.size();
                camera->name = udevadmOutput.mid(nameIndex, udevadmOutput.indexOf("\"", nameIndex) - nameIndex);
            }

            QLatin1String vendorToken("{idVendor}==\"");
            int vendorIndex = udevadmOutput.indexOf(vendorToken);
            if (vendorIndex >= 0) {
                vendorIndex += vendorToken.size();
                camera->vendorId = udevadmOutput.mid(vendorIndex, udevadmOutput.indexOf("\"", vendorIndex) - vendorIndex);
            }

            QLatin1String productToken("{idProduct}==\"");
            int productIndex = udevadmOutput.indexOf(productToken);
            if (productIndex >= 0) {
                productIndex += productToken.size();
                camera->productId = udevadmOutput.mid(productIndex, udevadmOutput.indexOf("\"", productIndex) - productIndex);
            }

            QLatin1String serialToken("{serial}==\"");
            int serialIndex = udevadmOutput.indexOf(serialToken);
            if (serialIndex >= 0) {
                serialIndex += serialToken.size();
                camera->serial = udevadmOutput.mid(serialIndex, udevadmOutput.indexOf("\"", serialIndex) - serialIndex);
            }

            LOG_I(LOG_TAG, "Found camera " + camera->toString());
            _cameras.append(camera);
        }
    }

    return _cameras.length();
}

const QList<UsbCamera*>& UsbCameraEnumerator::listDevices() const {
    return _cameras;
}

const UsbCamera* UsbCameraEnumerator::find(QString name, QString device, QString vid, QString pid, QString serial) const {
    if (!device.isEmpty() && !device.startsWith("/dev")) {
        device = "/dev/" + device;
    }
    for (UsbCamera *camera : _cameras) {
        bool match = device.isEmpty() || (camera->device == device);
        match &= name.isEmpty() || (camera->name == name);
        match &= vid.isEmpty() || (camera->vendorId == vid);
        match &= pid.isEmpty() || (camera->productId == pid);
        match &= serial.isEmpty() || (camera->serial == serial);

        if (match) return camera;
    }
    return nullptr;
}

void UsbCameraEnumerator::clearList() {
    while (!_cameras.isEmpty()) {
        delete _cameras[0];
        _cameras.removeFirst();
    }
}

UsbCameraEnumerator::~UsbCameraEnumerator() {
    clearList();
}

} // namespace Soro
