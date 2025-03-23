#include "IScreenMonitors.h"

void IScreenMonitors::SetGeometry(const QRect& geometry) {
    geometryScreen = geometry;
}

const QRect& IScreenMonitors::GetGeometry() const {
    return geometryScreen;
}

void IScreenMonitors::SetFormat(QImage::Format format) {
    targetFormat = format;
}

QImage::Format IScreenMonitors::GetFormat() const {
    return targetFormat;
}

bool IScreenMonitors::GetIsValid() const {

    if(targetFormat == QImage::Format::Format_Invalid) {
        return false;
    }

    if(geometryScreen.isNull() || geometryScreen.isEmpty()) {
        return false;
    }

    return true;
}
