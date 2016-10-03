#include "kalmanfilter.h"

#include <ctype.h>
#include <stdexcept>
#include <iostream>

#include <AnnotatorLib/Annotation.h>
#include <AnnotatorLib/Session.h>
#include <AnnotatorLib/Commands/NewAnnotation.h>

#include <QDebug>
#include <QtGui/QPainter>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "widget.h"

using namespace Annotator::Plugins;
using cv::Mat;
using cv::Scalar;

Annotator::Plugins::KalmanFilter::KalmanFilter() {}

KalmanFilter::~KalmanFilter() {}

void KalmanFilter::init() {
    int dynamParams = 6;   //x,y,dx,dy,w,h
    int measureParams = 4; //x,y,w,h
    int controlParams = 0;
    KF.init(dynamParams, measureParams, controlParams, CV_32F);
    Mat state(dynamParams, measureParams, CV_32F); /* (phi, delta_phi) */
    Mat processNoise(dynamParams, measureParams, CV_32F);
    measurement = Mat::zeros(measureParams, measureParams, CV_32F);
    out = Mat::zeros(measureParams, measureParams, CV_32F);

    // Transition State Matrix A
       // Note: set dT at each processing step!
       // [ 1 0 dT 0  0 0 ]
       // [ 0 1 0  dT 0 0 ]
       // [ 0 0 1  0  0 0 ]
       // [ 0 0 0  1  0 0 ]
       // [ 0 0 0  0  1 0 ]
       // [ 0 0 0  0  0 1 ]
    setIdentity(KF.transitionMatrix);
    KF.transitionMatrix.at<float>(2) = 1.f;
    KF.transitionMatrix.at<float>(9) = 1.f;

    setIdentity(KF.measurementMatrix);
    setIdentity(KF.processNoiseCov, Scalar::all(1e-5));
    setIdentity(KF.measurementNoiseCov, Scalar::all(1e-1));
    setIdentity(KF.errorCovPost, Scalar::all(1));
    initialized = true;
}

QString KalmanFilter::getName() { return "KalmanFilter"; }

QWidget *KalmanFilter::getWidget() { return &widget; }

bool KalmanFilter::setFrame(shared_ptr<AnnotatorLib::Frame> frame, cv::Mat image)
{
    this->frame = frame;
    out = KF.predict();
    shared_ptr<AnnotatorLib::Annotation> a = object->getAnnotation(frame);
    if (a) {
        measurement.at<float>(0) = (float)a->getX();
        measurement.at<float>(1) = (float)a->getY();
        out = KF.correct(measurement);
    }
    return true;
}

void KalmanFilter::setObject(shared_ptr<AnnotatorLib::Object> object)
{
    if (object && (!this->object || object != this->object)) {
        this->object = object;
        init();
    }
}

shared_ptr<AnnotatorLib::Object> KalmanFilter::getObject() const
{
    return object;
}

void KalmanFilter::setLastAnnotation(shared_ptr<AnnotatorLib::Annotation> annotation)
{

}

std::vector<shared_ptr<AnnotatorLib::Commands::Command> > KalmanFilter::getCommands()
{
    std::vector<shared_ptr<AnnotatorLib::Commands::Command> > commands;
    if (initialized) {
        shared_ptr<AnnotatorLib::Commands::NewAnnotation> nA =
            std::make_shared<AnnotatorLib::Commands::NewAnnotation>(getProject()->getSession(), object,
                                                      frame, (int)out.at<float>(0), (int)out.at<float>(1), (int)out.at<float>(2), (int)out.at<float>(3));
        commands.push_back(nA);
    }
}

