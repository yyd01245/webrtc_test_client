#ifndef _JANUS_SIGNAL_H_
#define _JANUS_SIGNAL_H_

#include <string>
#include <iostream>
#include <stdint.h>

#include "./httprequest.h"

#include "webrtc/base/common.h"
#include "webrtc/base/json.h"
#include "webrtc/base/logging.h"
#include "webrtc/base/stringutils.h"
#include "webrtc/api/mediastreaminterface.h"
#include "webrtc/api/peerconnectioninterface.h"
#include "webrtc/base/httpserver.h"
#include "webrtc/base/httpclient.h"

namespace uprtc {


// JanusSignal 
class JanusSignal: public rtc::Thread
{
public: 
  explicit JanusSignal(const std::string destIP,int destPort);
  virtual ~JanusSignal();

// uprtc signal
  int Initialize();
  int CreateSession();
  int CreateHandle();
  int Register();
  int Join();
  int Trick();
  int Configure();
  int Detach();
  int Destory();

  int SendMessage(std::string message);
  int ResponeMessage();

  int GetLongPullMessage();
  int parseSignal(Json::Value &message);

  virtual void Run() ;


private:
  bool m_bQuit;
  std::string m_strDestIP;
  int m_iDestPort;
  std::string m_transaction;
  int m_maxev;
  uint64_t m_sessionID;

  std::unique_ptr<rtc::HttpRequest> m_HttpRequest;
  std::unique_ptr<rtc::HttpRequest> m_longPull;
};

}

#endif