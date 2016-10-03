#include "kalmanfilter.h"

#include <ctype.h>
#include <stdexcept>
#include <iostream>

#include <AnnotatorLib/Annotation.h>
#include <AnnotatorLib/Session.h>
#include <AnnotatorLib/Commands/NewAnnotation.h>
#include <AnnotatorLib/Commands/UpdateAnnotation.h>

#include <QDebug>
#include <QtGui/QPainter>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "widget.h"

using namespace Annotator::Plugins;
using cv::Mat;
using cv::Scalar;

Annotator::Plugins::KalmanFilter::KalmanFilter() {}

KalmanFilter::~KalmanFilter() { }

void KalmanFilter::init() {
    int dynamParams = 6;   //x,y,dx,dy,w,h
    int measureParams = 2; //x,y,w,h
    int controlParams = 0;
    KF.init(dynamParams, measureParams, controlParams, CV_32F);

    // Transition State Matrix A
     // Note: set dT at each processing step!
     // [ 1 0 dT 0  0 0 ]
     // [ 0 1 0  dT 0 0 ]
     // [ 0 0 1  0  0 0 ]
     // [ 0 0 0  1  0 0 ]
     // [ 0 0 0  0  1 0 ]
     // [ 0 0 0  0  0 1 ]
    setIdentity(KF.transitionMatrix);

    // Measure Matrix H
    // [ 1 0 0 0 0 0 ]
    // [ 0 1 0 0 0 0 ]
    // [ 0 0 0 0 1 0 ]
    // [ 0 0 0 0 0 1 ]
    KF.measurementMatrix = cv::Mat::zeros(measureParams, dynamParams, CV_32F);
    KF.measurementMatrix.at<float>(0) = 1.0f;
    KF.measurementMatrix.at<float>(7) = 1.0f;
    KF.measurementMatrix.at<float>(16) = 1.0f;
    KF.measurementMatrix.at<float>(23) = 1.0f;

    // Process Noise Covariance Matrix Q
    // [ Ex 0  0    0 0    0 ]
    // [ 0  Ey 0    0 0    0 ]
    // [ 0  0  Ev_x 0 0    0 ]
    // [ 0  0  0    1 Ev_y 0 ]
    // [ 0  0  0    0 1    Ew ]
    // [ 0  0  0    0 0    Eh ]
    KF.processNoiseCov.at<float>(0) = 1e-2;
    KF.processNoiseCov.at<float>(7) = 1e-2;
    KF.processNoiseCov.at<float>(14) = 2.0f;
    KF.processNoiseCov.at<float>(21) = 1.0f;
    KF.processNoiseCov.at<float>(28) = 1e-2;
    KF.processNoiseCov.at<float>(35) = 1e-2;

    // Measures Noise Covariance Matrix R
    cv::setIdentity(KF.measurementNoiseCov, cv::Scalar(1e-1));

    setIdentity(KF.errorCovPost, Scalar::all(1));

    measurement.create(measureParams, 1, CV_32F);
    measurement.setTo(Scalar(0));

    out.second = false;

    initialized = true;
}

QString KalmanFilter::getName() { return "KalmanFilter"; }

QWidget *KalmanFilter::getWidget() { return &widget; }

bool KalmanFilter::setFrame(shared_ptr<AnnotatorLib::Frame> frame, cv::Mat image)
{
    this->frame = frame;
    if (initialized) {
        Mat prediction = KF.predict();
        out.first.x =  prediction.at<float>(0);
        out.first.y =  prediction.at<float>(1);
        out.first.width =  prediction.at<float>(4);
        out.first.height =  prediction.at<float>(5);
        out.second = true; //does previously not exist

        shared_ptr<AnnotatorLib::Annotation> a = getProject()->getSession()->getAnnotation(frame, object);
        //shared_ptr<AnnotatorLib::Annotation> aAnnotatorLib::Algo::InterpolateAnnotation::getInterpolation(getProject()->getSession(), frame, object);
        if (a) {
            KF.transitionMatrix.at<float>(2) = 1.f; //dT;
            KF.transitionMatrix.at<float>(9) = 1.f; //dT

            measurement.at<float>(0) = (float)a->getX();
            measurement.at<float>(1) = (float)a->getY();
            measurement.at<float>(2) = (float)a->getWidth();
            measurement.at<float>(3) = (float)a->getHeight();

            Mat estimation = KF.correct(measurement);
            out.first.x =  estimation.at<float>(0);
            out.first.y =  estimation.at<float>(1);
            out.first.width =  estimation.at<float>(4);
            out.first.height =  estimation.at<float>(5);
            out.second = false; //does previously exist
        }

        std::cout << "Predicting kalman to:(" << out.first << ")" << std::endl;
        return true;
    }
    return false;
}

void KalmanFilter::setObject(shared_ptr<AnnotatorLib::Object> object)
{
    if (object && (!this->object || object != this->object)) {
        this->object = object;
        init();

        KF.statePre.at<float>(0) = (float)object->getFirstAnnotation()->getX();
        KF.statePre.at<float>(1) = (float)object->getFirstAnnotation()->getY();
        KF.statePre.at<float>(4) = (float)object->getFirstAnnotation()->getWidth();
        KF.statePre.at<float>(5) = (float)object->getFirstAnnotation()->getHeight();

        KF.errorCovPre.at<float>(0) = 1; // px
        KF.errorCovPre.at<float>(7) = 1; // px
        KF.errorCovPre.at<float>(14) = 1;
        KF.errorCovPre.at<float>(21) = 1;
        KF.errorCovPre.at<float>(28) = 1; // px
        KF.errorCovPre.at<float>(35) = 1; // px

        //KF.predict();

        measurement.at<float>(0) = (float)object->getFirstAnnotation()->getX();
        measurement.at<float>(1) = (float)object->getFirstAnnotation()->getY();
        measurement.at<float>(2) = (float)object->getFirstAnnotation()->getWidth();
        measurement.at<float>(3) = (float)object->getFirstAnnotation()->getHeight();

        KF.correct(measurement);
    }
}

shared_ptr<AnnotatorLib::Object> KalmanFilter::getObject() const
{
    return object;
}

void KalmanFilter::setLastAnnotation(shared_ptr<AnnotatorLib::Annotation> annotation) {}

std::vector<shared_ptr<AnnotatorLib::Commands::Command> > KalmanFilter::getCommands()
{
    std::vector<shared_ptr<AnnotatorLib::Commands::Command> > commands;
    if (initialized) {
        shared_ptr<AnnotatorLib::Commands::Command> cmd;
        if (out.second) {
          cmd = std::make_shared<AnnotatorLib::Commands::NewAnnotation>(getProject()->getSession(),
                                                                      object,
                                                                      frame,
                                                                      (int)out.first.x,
                                                                      (int)out.first.y,
                                                                      (int)out.first.width,
                                                                      (int)out.first.height);
        } else {
            cmd = std::make_shared<AnnotatorLib::Commands::UpdateAnnotation>(getProject()->getSession()->getAnnotation(
                                                                           frame, object),
                                                                           (int)out.first.x,
                                                                           (int)out.first.y,
                                                                           (int)out.first.width,
                                                                           (int)out.first.height);
        }
        commands.push_back(cmd);
    }
    return commands;
}

