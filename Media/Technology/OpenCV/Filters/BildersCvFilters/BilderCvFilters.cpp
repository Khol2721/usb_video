#include "BilderCvFilters.h"

//Фильтры низних частот (LPF):
PtrMat BilderCvFilters::GetImageBilateralFilter(const Mat& mat) const {

    PtrMat out = cv::makePtr<cv::Mat>();
    bilateralFilter(mat, *out, 5, 255, 255);
    return out;
}

PtrMat BilderCvFilters::GetImageAdaptiveBilateralFilter(const Mat& mat) const {

    //Mat out;
    //adaptiveBilateralFilter(mat, out, cv::Size(10, 10), 255, 255);//сняли из-за низкого качества с версии 2.4!
    //https://stackoverflow.com/questions/24950367/where-cvadaptivebilateralfilter-has-disappeared

    PtrMat out = cv::makePtr<cv::Mat>();
    mat.copyTo(*out);//заглушка! TODO: Лишнее копирование!

    return out;
}

PtrMat BilderCvFilters::GetImageGaussianBlur(const Mat& mat) const {

    PtrMat out = cv::makePtr<cv::Mat>();
    GaussianBlur(mat, *out, cv::Size(5, 5), 0);//Size - размер ядра (чем больше, тем сильнее размытие)
    return out;
}

PtrMat BilderCvFilters::GetImageMedianBlur(const Mat& mat) const {

    PtrMat out = cv::makePtr<cv::Mat>();
    medianBlur(mat, *out, 5);
    return out;
}

PtrMat BilderCvFilters::GetImageBlur(const Mat& mat) const {

    PtrMat out = cv::makePtr<cv::Mat>();
    blur(mat, *out, cv::Size(2, 2));//(5, 5) //(2, 2) - ядро! (чем больше, тем больше область усреднения, но выше размытие)
    return out;
}

PtrMat BilderCvFilters::GetImageBoxFilter(const Mat& mat) const {

    PtrMat out = cv::makePtr<cv::Mat>();
    boxFilter(mat, *out, -1, cv::Size(12, 12));//чем больше Size, тем более размыто!
    return out;
}

//Фильтры верхних частот (HPF):
PtrMat BilderCvFilters::GetImageCanny(const Mat& mat) const {

    PtrMat out = cv::makePtr<cv::Mat>();
    Canny(mat, *out, 0, 50);
    return out;
}

PtrMat BilderCvFilters::GetImageSobel(const Mat& mat) const {

    //https://docs.opencv.org/3.4/d2/d2c/tutorial_sobel_derivatives.html

    PtrMat out = cv::makePtr<cv::Mat>();

    Mat grad_x, grad_y;
    Mat abs_grad_x, abs_grad_y;
    //значения по умолчанию!
    Sobel(mat, grad_x, mat.type(), 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);
    Sobel(mat, grad_y, mat.type(), 0, 1, 3, 1, 0, cv::BORDER_DEFAULT);

    //преобразовываем обратно!
    convertScaleAbs (grad_x, abs_grad_x);
    convertScaleAbs (grad_y, abs_grad_y);

    //градиент!
    cv::addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, *out);
    //чем выше альфа канал, тем более отчетливо проявляются линии!

    return out;
}

PtrMat BilderCvFilters::GetImageLaplacian(const Mat& mat) const {

    PtrMat out = cv::makePtr<cv::Mat>();

    const auto ddepth = -1;//желаемая глубина изображения! (по умолчанию = -1)
    //https://docs.opencv.org/3.4/d5/db5/tutorial_laplace_operator.html
    Laplacian(mat, *out, ddepth, 1, 1, 0, cv::BORDER_DEFAULT);

    return out;
}

PtrMat BilderCvFilters::GetImageScharr(const Mat& mat) const {

    PtrMat out = cv::makePtr<cv::Mat>();
    Mat out2;

    auto edgeThreshScharr = 170;//чем выше, тем менее чувсвителен к контрасту!
    Mat dx, dy;
    Scharr(mat, dx, CV_16S, 1, 0);
    Scharr(mat, dy, CV_16S, 0, 1);

    Canny(dx, dy, out2, edgeThreshScharr, edgeThreshScharr*3);
    *out = cv::Scalar::all(0);
    mat.copyTo(*out, out2);//копируем mat -> out применяя маску out2!

    return out;
}

PtrMat BilderCvFilters::GetImageReverse(const Mat& mat) const {

    PtrMat out = cv::makePtr<cv::Mat>();
    bitwise_not(mat, *out);
    return out;
}

PtrMat BilderCvFilters::GetImageColorMask(const Mat& mat) const {

    PtrMat out = cv::makePtr<cv::Mat>();
    cvtColor(mat, *out, MY_CV_BGR2HSV);

    const unsigned char maxMaskHSV_V = 150;//80
    //150 - хорошее выделение коптера на чистом небе!

    //!Лучше подавать чистое HSV изображение (не серое, так маска будет работать намного лучше)!
    cv::inRange(*out, cv::Scalar(0, 0, 0, 0), cv::Scalar(180, 255, maxMaskHSV_V, 0), *out);//(180, 255, maxMaskHSV_V, 0)
    //чем меньше значение яркости (3 параметр слева) тем чернее изображение, соответственно оно будет выделятья белым на маске!
    //(1 - cv::Scalar - минимальные параметры маски, 2 - cv::Scalar - максимальные)

    return out;
}
