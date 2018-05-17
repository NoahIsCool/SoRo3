#ifndef SORO_USBCAMERAENUMERATOR_H
#define SORO_USBCAMERAENUMERATOR_H

#include <QList>

namespace Soro {

/* Struct containing the details of a discovered USB camera
 */
struct UsbCamera {
    /* Human-readable name of the camera
     */
    QString name;
    /* USB vendor ID
     */
    QString vendorId;
    /* USB product ID
     */
    QString productId;
    /* Camera serial number. Not all cameras return one.
     */
    QString serial;
    /* Camera device file (/dev/video*)
     */
    QString device;

    QString toString() const {
        QString str = "{";
        if (!name.isEmpty()) {
            str += name + ",";
        }
        if (!vendorId.isEmpty()) {
            str += "v" + vendorId + ",";
        }
        if (!productId.isEmpty()) {
            str += "d" + productId + ",";
        }
        if (!serial.isEmpty()) {
            str += "s" + serial + ",";
        }
        str += device + "}";
        return str;
    }
};

/* Class used for discovering the USB cameras connected to a Linux computer.
 */
class UsbCameraEnumerator {

public:
    /* Enumerates all USB cameras connected, and returns the number of cameras detected.
     */
    int loadCameras();

    /* Gets a list of all the cameras that were found the last time loadCameras() was called
     */
    const QList<UsbCamera*>& listDevices() const;

    /* Returns the first camera matching all of the specified properties. If no cameras matched, this will
     * return null. To omit a property from the search, simply leave it as an empty string.
     */
    const UsbCamera* find(QString name="", QString device="", QString vid="", QString pid="", QString serial="") const;

    ~UsbCameraEnumerator();

protected:
    void clearList();

private:
    QList<UsbCamera*> _cameras;
};

} // namespace Soro

#endif // SORO_USBCAMERAENUMERATOR_H
