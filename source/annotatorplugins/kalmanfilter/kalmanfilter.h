#ifndef KALMANFILTER_H
#define KALMANFILTER_H

#include <annotator/plugins/plugin.h>
#include "widget.h"

#include <opencv2/core/mat.hpp>

#include <QtCore/QObject>
#include <QtCore/QtPlugin>
#include <QtGui/QIcon>

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
  shared_ptr<AnnotatorLib::Object> getObject() override;
  void setLastAnnotation(shared_ptr<AnnotatorLib::Annotation> annotation) override;
  std::vector<shared_ptr<AnnotatorLib::Commands::Command>> getCommands() override;

 protected:

  Widget widget;
};
}
}

#endif  // KALMANFILTER_H
