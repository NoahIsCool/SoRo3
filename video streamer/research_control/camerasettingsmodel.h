#ifndef CAMERASETTINGSMODEL_H
#define CAMERASETTINGSMODEL_H

#include <QString>
#include <QMap>
#include <QList>

namespace Soro {

/* Settings loader for the camera definition file. This file is a JSON formatted array
 * specifying information about the types of cameras the rover and mission control
 * should be expecting.
 */
class CameraSettingsModel
{
    Q_GADGET
public:
    /* This struct represents a single camera definition
     */
    struct Camera
    {
        QString name;
        QString serial;
        QString vendorId;
        QString productId;
    };

    /* Loads the camera definitions from the camera settings file. This will
     * throw an exception of type QString if an error occurrs
     */
    void load();

    /* Gets the camera at the specified index
     */
    Camera getCamera(uint index) const;
    /* Gets the number of cameras defined
     */
    int getCameraCount() const;

private:
    QList<CameraSettingsModel::Camera> _cameras;
};

} // namespace Soro

#endif // CAMERASETTINGSMODEL_H
