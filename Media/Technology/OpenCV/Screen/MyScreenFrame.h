#ifndef MYSCREENFRAME_H
#define MYSCREENFRAME_H

#include <QImage>

#include <memory>

#include <opencv2/opencv.hpp>
//#include <opencv2/videoio.hpp>

#include "../Primitives/ConverterVariablesCV.h"

#include "../../../Interface/Screen/IMyScreenFrame.h"

class MyScreenFrame : public IMyScreenFrame
{
public:
    MyScreenFrame() = default;
    virtual ~MyScreenFrame() = default;

    MyPtrImage GetScreenFrame(const QString& path) override;
};

#endif // MYSCREENFRAME_H
