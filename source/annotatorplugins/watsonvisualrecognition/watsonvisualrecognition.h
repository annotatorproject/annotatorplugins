#ifndef WATSONVISUALRECOGNITION_H
#define WATSONVISUALRECOGNITION_H

#include <annotator/plugins/plugin.h>
#include "widget.h"

#include <QtCore/QObject>
#include <QtCore/QtPlugin>
#include <QtGui/QIcon>
#include <memory>
#include <opencv2/core/mat.hpp>

#define WATSON_VR_API_URL \
  "https://gateway-a.watsonplatform.net/visual-recognition/api/v3/classify"

using std::shared_ptr;
using namespace AnnotatorLib;

namespace AnnotatorLib {
class Session;
}

namespace Annotator {
namespace Plugins {

class WatsonVisualRecognition : public Plugin {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "annotator.watsonvisualrecognition" FILE
                        "watsonvisualrecognition.json")
  Q_INTERFACES(Annotator::Plugin)

 public:
  WatsonVisualRecognition();
  ~WatsonVisualRecognition();
  QString getName() override;
  QWidget *getWidget() override;
  bool setFrame(shared_ptr<Frame> frame, cv::Mat image) override;
  void setObject(shared_ptr<Object> object) override;
  shared_ptr<Object> getObject() const override;
  void setLastAnnotation(shared_ptr<Annotation> annotation) override;
  std::vector<shared_ptr<Commands::Command>> getCommands() override;

  static std::string classify(std::string uri, const std::string &api_key,
                              cv::Mat image);

 protected:
  cv::Mat frameImg;
  shared_ptr<Annotation> lastAnnotation = nullptr;
  shared_ptr<Object> object = nullptr;

  Widget widget;

  shared_ptr<Frame> frame = nullptr;
  shared_ptr<Frame> lastFrame = nullptr;
};
}
}

#endif  // WATSONVISUALRECOGNITION_H
