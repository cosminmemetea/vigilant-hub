#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QPermission>
#include <QEventLoop>
#include "CameraThread.h"
#include "CameraProvider.h"

int main(int argc, char *argv[])
{
    qputenv("OPENCV_AVFOUNDATION_SKIP_AUTH", "1");

    QGuiApplication app(argc, argv);

    QCameraPermission cameraPermission;
    bool permissionGranted = false;
    switch (app.checkPermission(cameraPermission)) {
    case Qt::PermissionStatus::Undetermined: {
        QEventLoop loop;
        app.requestPermission(cameraPermission, [&permissionGranted, &loop](const QPermission &permission) {
            permissionGranted = (permission.status() == Qt::PermissionStatus::Granted);
            if (permissionGranted) {
                qDebug() << "Camera permission granted.";
            } else {
                qWarning() << "Camera permission denied. The app cannot access the camera.";
            }
            loop.quit();
        });
        loop.exec();
        break;
    }
    case Qt::PermissionStatus::Denied:
        qWarning() << "Camera permission denied. Please enable it in System Settings.";
        break;
    case Qt::PermissionStatus::Granted:
        qDebug() << "Camera permission already granted.";
        permissionGranted = true;
        break;
    }

    if (!permissionGranted) {
        qWarning() << "Exiting due to lack of camera permission.";
        return -1;
    }

    QQmlApplicationEngine engine;

    CameraProvider *provider = new CameraProvider;
    engine.addImageProvider("cameraProvider", provider);

    CameraThread *camera = new CameraThread(0);
    camera->setTargetResolution(640, 480);
    camera->setTargetFrameRate(15);
    QObject::connect(camera, &CameraThread::frameReady, provider, &CameraProvider::updateImage, Qt::QueuedConnection);
    QObject::connect(&app, &QGuiApplication::aboutToQuit, camera, &CameraThread::stop);
    camera->start();

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("VigilantHub", "Main");

    return app.exec();
}
