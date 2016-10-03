#ifndef KALMANFILTER_H
#define KALMANFILTER_H

#include <memory>

#include <QtCore/QtPlugin>

#include <opencv2/video/tracking.hpp>

#include <annotator/plugins/plugin.h>
#include <AnnotatorLib/Object.h>
#include <AnnotatorLib/Frame.h>

#include "widget.h"

namespace AnnotatorLib {
class Session;
}

namespace Annotator {
namespace Plugins {

class KalmanFilter : public Plugin {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "annotator.kalmanfilter" FILE "kalmanfilter.json")
  Q_INTERFACES(Annotator::Plugin)

 public:
  KalmanFilter();
  ~KalmanFilter();
  QString getName() override;
  QWidget *getWidget() override;

  bool setFrame(shared_ptr<AnnotatorLib::Frame> frame, cv::Mat image) override;
  void setObject(shared_ptr<AnnotatorLib::Object> object) override;
  shared_ptr<AnnotatorLib::Object> getObject() const override;
  void setLastAnnotation(shared_ptr<AnnotatorLib::Annotation> annotation) override;
  std::vector<shared_ptr<AnnotatorLib::Commands::Command>> getCommands() override;

 protected:

  Widget widget;
  shared_ptr<AnnotatorLib::Object> object;
  shared_ptr<AnnotatorLib::Frame> frame;
  cv::KalmanFilter KF;
  cv::Mat measurement;
  std::pair<cv::Rect2f, bool> out;
  bool initialized = false;


  void init();
};
}
}

#endif  // KALMANFILTER_H
