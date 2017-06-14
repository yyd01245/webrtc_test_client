#ifndef _JANUS_SIGNAL_H_
#define _JANUS_SIGNAL_H_

#include <string>
#include <iostream>
#include <stdint.h>
#include <random>
#include "./httprequest.h"

#include "webrtc/base/common.h"
#include "webrtc/base/json.h"
#include "webrtc/base/logging.h"
#include "webrtc/base/stringutils.h"
#include "webrtc/api/mediastreaminterface.h"
#include "webrtc/api/peerconnectioninterface.h"
#include "webrtc/base/httpserver.h"
#include "webrtc/base/httpclient.h"

#include "./common.h"
#include "./janus_handle.h"
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
  int Register(BroadcastPlugin* broadcast);
  int Join(BroadcastPlugin* broadcast);
  int Trick();
  int Configure();
  int Detach();
  int Destory();
  int StartBroadcast(uint64_t handleID);
  int  GetSessionAndHandleFromJson(Json::Value &message,uint64_t &sessionID,uint64_t &handleID);

  int SendMessage(std::string message, uint64_t sessionID=0, uint64_t handleID=0);
  int ResponeMessage();

  int GetLongPullMessage();
  int parseSignal(Json::Value &message);

  virtual void Run() ;

// multiple handle 

private:
  bool m_bQuit;
  bool m_bChangeUrl;
  std::string m_strDestIP;
  int m_iDestPort;
  std::string m_transaction;
  int m_maxev;
  uint64_t m_sessionID;
  std::map<uint64_t,BroadcastPlugin* > m_handlePlugins;
  std::map<std::string,std::string> m_transcationMap;

  std::unique_ptr<rtc::HttpRequest> m_HttpRequest;
  std::unique_ptr<rtc::HttpRequest> m_longPull;
};

}

#endif