#ifndef GASDK_H_
#define GASDK_H_

#include <WiFi.h>
#include <HTTPClient.h>

void exec_com_assistant(String apiKey, String deviceId, String modelId, String phrase);
#endif