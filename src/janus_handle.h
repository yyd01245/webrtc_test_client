#ifndef _JANUS_HANDLE_H_
#define _JANUS_HANDLE_H_

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

#include "./common.h"


// namespace uprtc {

class JanusSignal;

typedef struct _clientInfo{
  int role;
  uint64_t clientID;
  uint64_t handleID;

}clientInfo_t;
class BroadcastPlugin {
public:

  BroadcastPlugin(clientInfo_t &cl,JanusSignal* ptr);
  ~BroadcastPlugin();
/*! \brief role 1 register role 2 join
* @param[in] role listen udp info
* @returns  >=0 success
*/
  int StartBroadcast();
  uint64_t GetHandleID(){
    return m_handleID;
  }

  int Register(std::string transaction,Json::Value &requestinfo);
  int Join(std::string transaction,Json::Value &requestinfo);
  int Trick();
  int Configure(std::string transaction,Json::Value &requestinfo,
        std::string type,std::string sdp);
  int Detach();
  int parseEvent(Json::Value &message,Json::Value &jsep);


private:

  uint64_t m_handleID;
  uint64_t m_role;
  // 只是流主播的 ID 
  uint64_t m_clientID; 

  JanusSignal* m_jansuSignaler;
};


// }

#endif