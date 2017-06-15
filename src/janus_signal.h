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

#include "webrtc/base/nethelpers.h"
#include "webrtc/base/physicalsocketserver.h"
#include "webrtc/base/signalthread.h"
#include "webrtc/base/sigslot.h"
#include "./common.h"
#include "./janus_handle.h"


struct PeerConnectionClientObserver {
  virtual void OnSignedIn() = 0;  // Called when we're logged on. attached
  virtual void OnDisconnected() = 0;
  virtual void OnPeerConnected(int id, const std::string& name) = 0;
  virtual void OnPeerDisconnected(int peer_id) = 0;
  virtual void OnMessageFromPeer(int peer_id, const std::string& message) = 0;
  virtual void OnMessageSent(int err) = 0;
  virtual void OnServerConnectionFailure() = 0;

 protected:
  virtual ~PeerConnectionClientObserver() {}
};
typedef std::map<int, std::string> Peers;

// JanusSignal 
class JanusSignal: public rtc::Thread
{
public: 
  JanusSignal(){}
  explicit JanusSignal(const std::string destIP,int destPort);
  virtual ~JanusSignal();

  void RegisterObserver(PeerConnectionClientObserver* callback);

  int id() const;
  bool is_connected() const;
  const Peers& peers() const;

  void Connect(const std::string& server, int port,
               const std::string& client_name);

  bool SendToPeer(int peer_id, const std::string& message);
  bool SendHangUp(int peer_id);
  bool IsSendingMessage();

  bool SignOut();

  // implements the MessageHandler interface
  void OnMessage(rtc::Message* msg);


  // 

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

  Peers peers_;
  // notify to wind
  PeerConnectionClientObserver* callback_;
  bool m_bQuit;
  bool m_bConnect;
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


#endif