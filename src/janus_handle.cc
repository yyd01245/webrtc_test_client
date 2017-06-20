#include "./janus_handle.h"
#include "./janus_signal.h"

using namespace uprtc;

BroadcastPlugin::BroadcastPlugin(clientInfo_t &cl,JanusSignal* ptr){
  m_handleID = cl.handleID;
  LOG(INFO)<< "** broadcast handleid ****"<< m_handleID;
  m_role = cl.role;
  m_clientID = cl.clientID;
  m_jansuSignaler = ptr;
}
BroadcastPlugin::~BroadcastPlugin(){


}


int BroadcastPlugin::StartBroadcast(){
  // if(m_role == PUBLISHER){
  //   Register();
  // }else if(m_role == LISTENER){
  //   Join();
  // }

  return 0;
}

int BroadcastPlugin::parseEvent(Json::Value &message,Json::Value &jsep){

  std::string type;
  LOG(INFO)<< message;
  rtc::GetStringFromJsonObject(message,"broadcast",&type);
  LOG(INFO)<< type.c_str();
  if (!type.empty()) {
    if(type == "joined"){
      if(m_jansuSignaler)
        m_jansuSignaler->Configure(this);
    }else if(type == "registered"){
      if(m_jansuSignaler)
        m_jansuSignaler->Join(this);
    }else if(type == "attached"){

    }else if(type == "event"){
      std::string res;
      if(rtc::GetStringFromJsonObject(message,"configured",&res)){
        if(res == "ok"){
          // LOG(INFO)<< "confiure return ok "<< jsep;
          if(m_jansuSignaler)
            m_jansuSignaler->ReplyConfigure(this,jsep);
        }
      }      
    }else if(type == "detroyed"){
      
    }
  }
  return 0;
}


int BroadcastPlugin::Register(std::string transaction,Json::Value &requestinfo){

  Json::Value body; 
  body["request"] = Json::Value("register");
  body["client_id"] = Json::Value((Json::UInt64)m_clientID);

  requestinfo["uprtc"] = Json::Value("message");
  requestinfo["body"] = Json::Value(body);
  requestinfo["transaction"] = Json::Value(transaction);

  return 0;
}



int BroadcastPlugin::Join(std::string transaction,Json::Value &requestinfo){
  Json::Value body; 
  body["request"] = Json::Value("join");
  body["client_id"] = Json::Value((Json::UInt64)m_clientID);
  body["ptype"] = Json::Value(m_role == PUBLISHER ? "publisher":"listener");
  body["sec_key"] = Json::Value("");
  // body["datachannel"] = Json::Value(false);

  requestinfo["uprtc"] = Json::Value("message");
  requestinfo["body"] = Json::Value(body);
  requestinfo["transaction"] = Json::Value(transaction);
  return 0;
}
int BroadcastPlugin::Trick(){

  return 0;
}
int BroadcastPlugin::Configure(std::string transaction,Json::Value &requestinfo,
        std::string type,std::string sdp){
  Json::Value body;
  // Json::Value message;
  Json::Value jsep; 
  body["request"] = Json::Value("configure");
  body["audio"] = Json::Value(true);
  body["video"] = Json::Value(true);

  jsep["type"] = Json::Value(type);
  jsep["sdp"] = Json::Value(sdp);
 
  requestinfo["uprtc"] = Json::Value("message");
  requestinfo["body"] = Json::Value(body);
  requestinfo["jsep"] = Json::Value(jsep);
  requestinfo["transaction"] = Json::Value(transaction);          
  return 0;
}
int BroadcastPlugin::Detach(){

  return 0;
}
