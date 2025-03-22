#ifndef CAMERAPROVIDER_H
#define CAMERAPROVIDER_H
#include <QQuickImageProvider>
#include <QImage>

class CameraProvider : public QQuickImageProvider {
public:
    CameraProvider() : QQuickImageProvider(QQuickImageProvider::Image) {}
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override {
        Q_UNUSED(id);
        Q_UNUSED(requestedSize);
        if (size) *size = m_image.size();
        if (m_image.isNull()) {
            qWarning() << "CameraProvider: Requested image is null.";
        } else {
            qDebug() << "CameraProvider: Providing image:" << m_image.size();
        }
        return m_image;
    }
    void updateImage(const QImage &image) {
        if (image.isNull()) {
            qWarning() << "CameraProvider: Received null image.";
        } else {
            qDebug() << "CameraProvider: Updated image:" << image.size();
        }
        m_image = image;
    }
private:
    QImage m_image;
};
#endif
