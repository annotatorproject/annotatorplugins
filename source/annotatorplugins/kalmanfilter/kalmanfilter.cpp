#include "kalmanfilter.h"
#include "widget.h"

#include <ctype.h>
#include <stdexcept>
#include <iostream>

#include <AnnotatorLib/Annotation.h>
#include <AnnotatorLib/Frame.h>
#include <AnnotatorLib/Session.h>

#include <QDebug>
#include <QtGui/QPainter>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>


using namespace Annotator::Plugins;

Annotator::Plugins::KalmanFilter::KalmanFilter() {}

KalmanFilter::~KalmanFilter() {}

QString KalmanFilter::getName() { return "KalmanFilter"; }

QWidget *KalmanFilter::getWidget() { return &widget; }

bool KalmanFilter::setFrame(shared_ptr<AnnotatorLib::Frame> frame, cv::Mat image)
{
    return false;
}

void KalmanFilter::setObject(shared_ptr<AnnotatorLib::Object> object)
{
    throw std::runtime_error("not implemented yet!");
}

shared_ptr<AnnotatorLib::Object> KalmanFilter::getObject() const
{
    throw std::runtime_error("not implemented yet!");
}

void KalmanFilter::setLastAnnotation(shared_ptr<AnnotatorLib::Annotation> annotation)
{

}

std::vector<shared_ptr<AnnotatorLib::Commands::Command> > KalmanFilter::getCommands()
{
    throw std::runtime_error("not implemented yet!");
}

