#include <ESPAsyncWebServer.h>

class CaptiveRequestHandler : public AsyncWebHandler {
public:
  CaptiveRequestHandler(String hostname) : _hostname(hostname) {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request);

  void handleRequest(AsyncWebServerRequest *request);

private:
  String _hostname;
};