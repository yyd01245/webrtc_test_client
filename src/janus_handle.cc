#include "./janus_handle.h"
#include "./janus_signal.h"

using namespace uprtc;

BroadcastPlugin::BroadcastPlugin(clientInfo_t &cl,JanusSignal* ptr){
  m_handleID = cl.handleID;
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

int BroadcastPlugin::parseEvent(Json::Value &message){

  std::string type;
  LOG(INFO)<< message;
  rtc::GetStringFromJsonObject(message,"broadcast",&type);
  LOG(INFO)<< type.c_str();
  if (!type.empty()) {
    if(type == "joined"){
      m_jansuSignaler->Configure();
    }else if(type == "registered"){
      m_jansuSignaler->Join(this);
    }else if(type == "attached"){

    }else if(type == "event"){

    }else if(type == "detroyed"){
      
    }
  }
  return 0;
}


int BroadcastPlugin::Register(std::string transaction,Json::Value &requestinfo){

  Json::Value body; 
  body["request"] = Json::Value("register");
  body["client_id"] = Json::Value(m_clientID);

  requestinfo["uprtc"] = Json::Value("message");
  requestinfo["body"] = Json::Value(body);
  requestinfo["transaction"] = Json::Value(transaction);

  return 0;
}



int BroadcastPlugin::Join(std::string transaction,Json::Value &requestinfo){
  Json::Value body; 
  body["request"] = Json::Value("join");
  body["client_id"] = Json::Value(m_clientID);
  body["ptype"] = Json::Value(m_role == PUBLISHER ? "publisher":"listener");
  body["sec_key"] = Json::Value("");
  body["datachannel"] = Json::Value(false);

  requestinfo["uprtc"] = Json::Value("message");
  requestinfo["body"] = Json::Value(body);
  requestinfo["transaction"] = Json::Value(transaction);
  return 0;
}
int BroadcastPlugin::Trick(){

  return 0;
}
int BroadcastPlugin::Configure(){

  return 0;
}
int BroadcastPlugin::Detach(){

  return 0;
}