#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QPermission>
#include <QEventLoop>
#include <QQmlContext>
#include "CameraThread.h"
#include "CameraProvider.h"
#include "MLProcessor.h"

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
    camera->setTargetResolution(320, 240);  // Reduce resolution to free up resources
    camera->setTargetFrameRate(5);  // Reduce to 5 FPS to lower resource usage

    MLProcessor *mlProcessor = new MLProcessor;
    engine.rootContext()->setContextProperty("mlProcessor", mlProcessor);
    qDebug() << "MLProcessor initialized.";

    QObject::connect(camera, &CameraThread::frameReady, provider, &CameraProvider::updateImage, Qt::QueuedConnection);
    QObject::connect(camera, &CameraThread::frameReady, mlProcessor, &MLProcessor::processFrame, Qt::QueuedConnection);
    QObject::connect(&app, &QGuiApplication::aboutToQuit, camera, &CameraThread::stop);
    QObject::connect(&app, &QGuiApplication::aboutToQuit, mlProcessor, &MLProcessor::stop);

    QObject::connect(camera, &CameraThread::frameReady, [](const QImage &frame) {
        if (frame.isNull()) {
            qWarning() << "CameraThread emitted null frame";
        } else {
            qDebug() << "CameraThread emitted frame with size:" << frame.size();
        }
    });

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
