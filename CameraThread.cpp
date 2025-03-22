#include "CameraThread.h"
#include <QDebug>
#include <QElapsedTimer>

CameraThread::CameraThread(int cameraIndex, QObject *parent)
    : QThread(parent), cameraOpened(false), cameraIndex(cameraIndex), targetWidth(640), targetHeight(480), targetFrameRate(15), shouldStop(false) {
    tryOpenCamera();
}

void CameraThread::setTargetResolution(int width, int height) {
    targetWidth = width;
    targetHeight = height;
    if (cameraOpened) {
        cap.set(cv::CAP_PROP_FRAME_WIDTH, targetWidth);
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, targetHeight);
        qDebug() << "Set camera resolution to" << targetWidth << "x" << targetHeight;
    }
}

void CameraThread::setTargetFrameRate(int fps) {
    targetFrameRate = fps;
    if (cameraOpened) {
        cap.set(cv::CAP_PROP_FPS, targetFrameRate);
        qDebug() << "Set camera frame rate to" << targetFrameRate << "FPS";
    }
}

void CameraThread::stop() {
    shouldStop = true;
    wait();
}

bool CameraThread::tryOpenCamera() {
    if (cap.isOpened()) {
        cap.release();
    }
    cap.open(cameraIndex);
    if (!cap.isOpened()) {
        qWarning() << "Failed to open camera at index" << cameraIndex << "! Check permissions or camera availability.";
        return false;
    }
    cameraOpened = true;
    qDebug() << "Camera at index" << cameraIndex << "opened successfully.";
    cap.set(cv::CAP_PROP_FRAME_WIDTH, targetWidth);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, targetHeight);
    cap.set(cv::CAP_PROP_FPS, targetFrameRate);
    qDebug() << "Set camera resolution to" << targetWidth << "x" << targetHeight << "and frame rate to" << targetFrameRate << "FPS";
    return true;
}

void CameraThread::run() {
    QElapsedTimer timer;
    timer.start();
    qint64 frameInterval = 1000 / targetFrameRate;

    while (!shouldStop) {
        if (!cameraOpened) {
            qWarning() << "Camera not opened. Retrying...";
            if (!tryOpenCamera()) {
                QImage fallbackImage(targetWidth, targetHeight, QImage::Format_RGB888);
                fallbackImage.fill(Qt::red);
                qDebug() << "Emitting fallback image (red square).";
                emit frameReady(fallbackImage);
                msleep(1000);
                continue;
            }
        }

        cv::Mat frame;
        if (cap.read(frame)) {
            if (frame.empty()) {
                qWarning() << "Captured an empty frame. Camera may be disconnected.";
                cameraOpened = false;
                continue;
            }
            QImage qimg(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
            QImage frameCopy = qimg.rgbSwapped().copy();
            qDebug() << "Captured frame:" << frame.cols << "x" << frame.rows;
            emit frameReady(frameCopy);
        } else {
            qWarning() << "Failed to capture frame. Camera may have been disconnected.";
            cameraOpened = false;
            msleep(100);
            continue;
        }

        qint64 elapsed = timer.elapsed();
        qint64 sleepTime = frameInterval - elapsed;
        if (sleepTime > 0) {
            msleep(sleepTime);
        }
        timer.restart();
    }
    if (cap.isOpened()) {
        cap.release();
    }
}
