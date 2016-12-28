#include "watsonvisualrecognition.h"
#include "widget.h"

#include <AnnotatorLib/Annotation.h>
#include <AnnotatorLib/Commands/NewAnnotation.h>
#include <AnnotatorLib/Frame.h>
#include <AnnotatorLib/Session.h>
#include <QDebug>
#include <QtGui/QPainter>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/tracking.hpp>

#include <Poco/Net/AcceptCertificateHandler.h>
#include <Poco/Net/FilePartSource.h>
#include <Poco/Net/HTMLForm.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/InvalidCertificateHandler.h>
#include <Poco/Net/SSLManager.h>
#include <Poco/Net/StringPartSource.h>
#include <Poco/URI.h>

#include <ctype.h>
#include <iostream>

using namespace Annotator::Plugins;

Annotator::Plugins::WatsonVisualRecognition::WatsonVisualRecognition() {}

WatsonVisualRecognition::~WatsonVisualRecognition() {}

QString WatsonVisualRecognition::getName() { return "WatsonVisualRecognition"; }

QWidget *WatsonVisualRecognition::getWidget() { return &widget; }

bool WatsonVisualRecognition::setFrame(shared_ptr<Frame> frame, cv::Mat image) {
  this->lastFrame = this->frame;
  this->frame = frame;
  this->frameImg = image;
  return lastFrame != frame;
}

// first call
void WatsonVisualRecognition::setObject(shared_ptr<Object> object) {
  this->object = object;
}

shared_ptr<Object> WatsonVisualRecognition::getObject() const { return object; }

// second call
void WatsonVisualRecognition::setLastAnnotation(
    shared_ptr<Annotation> annotation) {
  if (annotation == nullptr || annotation->getObject() != object)
    return;
  if (lastAnnotation != nullptr &&
      annotation->getObject() == lastAnnotation->getObject())
    return;
}

std::vector<shared_ptr<Commands::Command>>
WatsonVisualRecognition::getCommands() {
  std::vector<shared_ptr<Commands::Command>> commands;
  std::cout << classify(std::string(WATSON_VR_API_URL), widget.getApiKey(), this->frameImg) << std::endl;
  return commands;
}

std::string WatsonVisualRecognition::classify(std::string uri,
                                              const std::string &api_key,
                                              cv::Mat image) {
  Poco::Net::HTMLForm htmlform;
  htmlform.setEncoding(Poco::Net::HTMLForm::ENCODING_MULTIPART);

  std::vector<uchar> buffer;
  cv::imencode(".jpg", image, buffer);

  std::string str(reinterpret_cast<const char *>(buffer.data()), buffer.size());

  Poco::Net::PartSource *pFileSrc = new Poco::Net::StringPartSource(
      str, "application/octet-stream", "image.jpg");

  htmlform.addPart("images_file", pFileSrc);

  Poco::URI _uri(uri);
  _uri.addQueryParameter("api_key", api_key);
  _uri.addQueryParameter("version", "2016-05-20");

  Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST,
                                 _uri.getPathAndQuery(),
                                 Poco::Net::HTTPMessage::HTTP_1_1);

  htmlform.prepareSubmit(request);

  Poco::Net::HTTPClientSession *httpSession =
      new Poco::Net::HTTPClientSession(_uri.getHost());
  if (_uri.getScheme() == "https") {
    delete httpSession;
    httpSession = new Poco::Net::HTTPSClientSession(_uri.getHost());
  }

  httpSession->setTimeout(Poco::Timespan(20, 0));
  htmlform.write(httpSession->sendRequest(request));

  Poco::Net::HTTPResponse response;
  std::istream &rs = httpSession->receiveResponse(response);

  std::string ret(std::istreambuf_iterator<char>(rs), {});
  return ret;
}
