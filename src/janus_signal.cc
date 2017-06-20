#include "./janus_signal.h"



using namespace uprtc;




const std::string PLUGINS_NAME = "uprtc.plugin.broadcast";

std::string _randomString(int len)
{
    std::string  charSet("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
    std::string randomString;
    std::random_device rd;

    int i = 0;
    int randomPoz = 0;
    for (i = 0; i < len; i++) 
    {
        randomPoz = rd()  % charSet.size();
        randomString += charSet[randomPoz];
    }
    return randomString;
}

void JanusSignal::Run() 
{
  //printf("send long pull get begin \n");
  bool bFirst = false;
  while(!m_bQuit){
    if(m_sessionID == 0){
      usleep(1000);
      continue;
    }
  
    std::string now = std::to_string(time(0));
    //printf("send long pull get 1\n");
    // every time reset 
    if(!bFirst){
      m_longPull.reset(new rtc::HttpRequest("longPull"));
      m_longPull->set_host(m_strDestIP);
      m_longPull->set_port(m_iDestPort);
      m_longPull->request().verb = rtc::HV_GET;
      m_longPull->set_timeout(30000);
      m_longPull->request().path = "/uprtc/" + std::to_string(m_sessionID) + "?rid=" + now;
      if (0 != m_maxev)
      {
        m_longPull->request().path += "&maxev=" + std::to_string(m_maxev) /* + "&_=" + now */;
      }
      std::string jsondata;
      m_longPull->request().setDocumentAndLength(new rtc::StringStream(&jsondata));
      jsondata.clear();
      printf("send long pull get \n");
      m_longPull->response().setDocumentAndLength(new rtc::StringStream(&jsondata));
      bFirst = true;
    }
    //HttpRequest.request().setContent("application/json", new rtc::StringStream(&jsondata));
    //jsondata.clear();
    // timeout 
    GetLongPullMessage();
  }

 }


JanusSignal::JanusSignal(const std::string destIP,int destPort){
  m_strDestIP = destIP;
  m_iDestPort = destPort;
  m_sessionID = 0;
  m_transaction = "";
  m_maxev = 1;
  m_bQuit = false;
  m_bConnect = false;
  m_index = 0;
  callback_ = NULL;
  Initialize();
  printf("%s:%d  begin \n",__FUNCTION__,__LINE__);
  std::cout<<"JanusSignal create"<<std::endl;
}

JanusSignal::~JanusSignal(){
  m_bQuit = true;
}

void JanusSignal::RegisterObserver(
    PeerConnectionClientObserver* callback) {
  ASSERT(!callback_);
  callback_ = callback;
}


int JanusSignal::id() const {
  return 1111;
}

void JanusSignal::Connect(const std::string& server, int port,
              const std::string& client_name){
  // connnect to server
  
  CreateSession();              
}

bool JanusSignal::SendToPeer(uint64_t peer_id, const std::string& message){

  return true;
}
bool JanusSignal::SendHangUp(uint64_t peer_id){

  return true;
}
bool JanusSignal::IsSendingMessage(){
  return true;
}

bool JanusSignal::SignOut(){

  return true;
}

bool JanusSignal::is_connected() const {
  return m_bConnect;
}

const Peers& JanusSignal::peers() const {
  return peers_;
}


int JanusSignal::Initialize(){
  // m_HttpRequest.reset(new rtc::HttpRequest("uprtc_client"));
  // m_HttpRequest->set_host(m_strDestIP);
  // m_HttpRequest->set_port(m_iDestPort);
  // m_HttpRequest->request().verb = rtc::HV_POST;
  // m_HttpRequest->request().path = "/uprtc";

  m_bChangeUrl = false;
  // std::string str;
  // m_HttpRequest->response().setDocumentAndLength(new rtc::StringStream(&str));
  printf("init client over \n");

  m_longPull.reset(new rtc::HttpRequest("longPull"));
  m_longPull->set_host(m_strDestIP);
  m_longPull->set_port(m_iDestPort);

  printf("init long pull over \n");
  return 0;
}

int JanusSignal::GetLongPullMessage(){

  // std::string jsondata;
  // printf("send long pull get \n");
  // m_longPull->response().setDocumentAndLength(new rtc::StringStream(&jsondata));

  m_longPull->Send();
  //sleep(10);
  char buffer[2048]= {0};
  m_longPull->response().document->Read(buffer, sizeof(buffer), nullptr , nullptr);

  printf ("Run @@@@@@@%d~~~~  %s @######################\n",
  m_longPull->response().scode,
  m_longPull->response().message.c_str());

  Json::Value responseJson;  
  Json::Reader reader;
  std::string data;
  data = buffer;

  reader.parse(data, responseJson);
  parseSignal(responseJson);

  return 0;
}

int JanusSignal::GetSessionAndHandleFromJson(Json::Value &message,uint64_t &sessionID,uint64_t &handleID){
  double sessionid = 0;
  double handleid = 0;
  if(rtc::GetDoubleFromJsonObject(message , "session_id",&sessionid) )
  {
      printf("sessionid_ @@@@@@@@@@@@==============%lu\n",(uint64_t)sessionid);
      sessionID = (uint64_t)sessionid;
  }else {
      printf ("error get session_id\n");
  } 
  if(rtc::GetDoubleFromJsonObject(message, "sender",&handleid) )
  {
      printf("sessionid_ @@@@@@@@@@@@==============%lu\n",(uint64_t)handleid);
      handleID = (uint64_t)handleid;
  }else {
      printf ("error get handle id \n");
  } 
  return 0;

}

int JanusSignal::parseSignal(Json::Value &message){
  m_bConnect= true;
  std::string type;
  LOG(INFO)<< message;
  rtc::GetStringFromJsonObject(message,"uprtc",&type);
  LOG(INFO)<< type.c_str();
  if (!type.empty()) 
  {
    if(type == "keepalive"){

    }else if(type == "ack"){

    }else if(type == "success"){
      std::string transaction;

      rtc::GetStringFromJsonObject(message, "transaction", &transaction);
      printf ("get id ~~~~~~~~~~`= local:%s,remote:%s\n",m_transaction.c_str(), transaction.c_str());
      auto iter = m_transcationMap.find(transaction);
      if(iter != m_transcationMap.end()){
        auto data = iter->second;
        if(data == CREATESESSION){
          double sessionid = 0;
          if(rtc::GetDoubleFromJsonObject(message["data"], "id",&sessionid) )
          {
              m_sessionID = (uint64_t)sessionid;
                  
              // create handle
              CreateHandle();
              /* 2.crate long pull start thread base method*/
              Start();
              printf("sessionid_ @@@@@@@@@@@@==============%lu\n",m_sessionID);
          } else {
              printf ("error\n");
          }
        }else if(data == CREATEHANDLE){
          double handleid = 0;
          if(rtc::GetDoubleFromJsonObject(message["data"], "id",&handleid) )
          {
              // auto handleID = (uint64_t)handleid;
                  /* 2.crate long pull */
              // new handle instance 
              StartBroadcast((uint64_t)handleid);
                LOG(INFO)<<__FUNCTION__ << "recv Handleid "<<(uint64_t)handleid;
              //printf("sessionid_ @@@@@@@@@@@@==============%lu\n",(uint64_t)handleid);
          }else {
              printf ("error\n");
          }          
        }
      }else {
        LOG(WARNING)<< "Can not find signal response :"<<message;
        return -1;
      }
      m_transcationMap.erase(transaction);

    }else if(type == "webrtcup"){

    }else if(type == "detached"){

    }else if(type == "media"){

    }else if(type == "slowlink"){

    }else if(type == "error"){

    }else if(type == "event"){
      // push json to plugin parse
      std::string transaction;

      rtc::GetStringFromJsonObject(message, "transaction", &transaction);
      printf ("get id ~~~~~~~~~~`= local:%s,remote:%s\n",m_transaction.c_str(), transaction.c_str());
      uint64_t sessionID = 0;
      uint64_t handleID = 0;
      GetSessionAndHandleFromJson(message,sessionID,handleID);

      Json::Value eventData;
      if(rtc::GetValueFromJsonObject(message["plugindata"],"data",&eventData)){
        if(handleID != 0){
          auto iter = m_handlePlugins.find(handleID);
          if(m_handlePlugins.end() == iter)
            return -1;
          BroadcastPlugin* br = iter->second;
          if(br == NULL){
            return -1;
          }
          Json::Value jsepData;
          rtc::GetValueFromJsonObject(message,"jsep",&jsepData);
          LOG(INFO)<< "READY jsep: "<<jsepData;
          br->parseEvent(eventData,jsepData); 
        }

      }else {
        LOG(WARNING)<< "Unkown message event data"; 
      }
      m_transcationMap.erase(transaction);

    }else {
      LOG(WARNING)<< "Unkown message /event on" << m_sessionID;
    }
  }

  return 0;
}

int JanusSignal::ResponeMessage(){
  std::string data;
  char buffer[0xfff];
  //if(rtc::HC_OK == m_HttpRequest->respone().scode)

  printf("respone ---- %d : %s \n",m_HttpRequest->response().scode,
    m_HttpRequest->response().message.c_str());
  {
    m_HttpRequest->response().document->Read(buffer,sizeof(buffer),nullptr,nullptr);
    data = buffer;
    Json::Reader jsonReader;
    Json::Value message;
    jsonReader.parse(data,message);
    // if(!jsonReader.parse(data,message)){
    //   LOG(WARNING)<< "Receive unknown message."<<message;
    //   return -1;
    // }
    LOG(INFO)<<"http recv :"<<message;
    parseSignal(message);
    data.clear();
    
  }


  return 0;
}

int JanusSignal::SendMessage(std::string message, uint64_t sessionID, uint64_t handleID){
  LOG(INFO)<<"message "<<message;

  // 
  m_HttpRequest.reset(new rtc::HttpRequest("uprtc_client"));
  m_HttpRequest->set_host(m_strDestIP);
  m_HttpRequest->set_port(m_iDestPort);
  m_HttpRequest->request().verb = rtc::HV_POST;
  m_HttpRequest->request().path = "/uprtc";
  if(sessionID != 0 ){ //&& !m_bChangeUrl

    m_HttpRequest->request().path += "/";
    m_HttpRequest->request().path += std::to_string(sessionID);

    // m_bChangeUrl = true;
  }
  if(handleID != 0){
    m_HttpRequest->request().path += "/";
    m_HttpRequest->request().path += std::to_string(handleID);
  }

  LOG(INFO)<<"http path:"<<m_HttpRequest->request().path;
  m_HttpRequest->request().setContent("appliction/json",new rtc::StringStream(&message));

  std::string str;
  m_HttpRequest->response().setDocumentAndLength(new rtc::StringStream(&str));

  m_HttpRequest->Send();

  return 0;
}

int JanusSignal::CreateSession(){
  Json::StyledWriter writer;
  Json::Value requestinfo;  

  m_transaction = _randomString(12);
  requestinfo["uprtc"] = Json::Value(CREATESESSION);
  requestinfo["transaction"] = Json::Value(m_transaction);
  m_transcationMap[m_transaction] = CREATESESSION;
  std::string message(writer.write(requestinfo));
  //printf ("message =============%s\n",message.c_str());

  SendMessage(message);

  ResponeMessage();
  requestinfo.clear();
  message.clear();

  return 0;
}
int JanusSignal::CreateHandle(){
  Json::StyledWriter writer;
  Json::Value requestinfo;  

  
  m_transaction = _randomString(12);
  requestinfo["uprtc"] = Json::Value("attach");
  requestinfo["plugin"] = Json::Value(PLUGINS_NAME);
  requestinfo["transaction"] = Json::Value(m_transaction);
  m_transcationMap[m_transaction] = CREATEHANDLE;
  std::string message(writer.write(requestinfo));
  LOG(INFO)<< message << "sessionid "<< m_sessionID;
  SendMessage(message,m_sessionID);

  ResponeMessage();
  requestinfo.clear();
  message.clear();

  return 0;
}

int JanusSignal::StartBroadcast(uint64_t handleID){
  callback_->OnSignedIn();
    LOG(INFO)<<__FUNCTION__ << " Handleid "<<handleID;
  int role = PUBLISHER;
  uint64_t clientID = 2000;
  clientInfo_t client;
  client.role = role;
  client.handleID = handleID;
  client.clientID = clientID;
  BroadcastPlugin* broadcast = new BroadcastPlugin(client,this);
  m_handlePlugins[handleID] = broadcast;


  if(role == PUBLISHER){
    Register(broadcast);
  }else if(role == LISTENER){
    Join(broadcast);
  }

  return 0;
}

int JanusSignal::Register(BroadcastPlugin* broadcast){

  std::string transaction = _randomString(12);
  Json::StyledWriter writer;
  Json::Value requestinfo;  
  broadcast->Register(transaction, requestinfo);           

  LOG(INFO)<< requestinfo;
  m_transcationMap[transaction] = REGISTER;
  std::string message(writer.write(requestinfo));
  LOG(INFO)<< message << "sessionid "<< m_sessionID;
  SendMessage(message,m_sessionID,broadcast->GetHandleID());

  ResponeMessage();
  requestinfo.clear();
  message.clear();
  return 0;
}
int JanusSignal::Join(BroadcastPlugin* broadcast){
  LOG(INFO)<< "----- BEGIN JOIN ------";
  std::string transaction = _randomString(12);
  Json::StyledWriter writer;
  Json::Value requestinfo;  
  broadcast->Join(transaction, requestinfo);           

  LOG(INFO)<< requestinfo;
  m_transcationMap[transaction] = JOIN;
  std::string message(writer.write(requestinfo));
  LOG(INFO)<< message << "sessionid "<< m_sessionID;
  SendMessage(message,m_sessionID,broadcast->GetHandleID());

  ResponeMessage();
  requestinfo.clear();
  message.clear();
  return 0;
}
int JanusSignal::SendTrickle(Json::Value& msg){
  LOG(INFO)<< "----- BEGIN "<< __FUNCTION__ <<"------";
  std::string transaction = _randomString(12);
  Json::StyledWriter writer;
  Json::Value requestinfo;         
  requestinfo["uprtc"] = Json::Value("trickle");
  requestinfo["candidate"] = msg;
  requestinfo["transaction"] = Json::Value(m_transaction);
  LOG(INFO)<< requestinfo;
  m_transcationMap[transaction] = TRICKLE;
  std::string message(writer.write(requestinfo));
  LOG(INFO)<< message << "sessionid "<< m_sessionID;
  auto iter = m_handlePlugins.begin();
  BroadcastPlugin* broadcast = iter->second;
  SendMessage(message,m_sessionID,broadcast->GetHandleID());

  ResponeMessage();
  requestinfo.clear();
  message.clear();
  return 0;  

}
int JanusSignal::SendTrickle(const std::string & msg){
  LOG(INFO)<< "----- BEGIN "<< __FUNCTION__ <<"------";
  std::string transaction = _randomString(12);
  Json::StyledWriter writer;
  Json::Value requestinfo;         
  requestinfo["uprtc"] = Json::Value("trickle");
  requestinfo["candidate"] = Json::Value(msg);
  requestinfo["transaction"] = Json::Value(m_transaction);
  LOG(INFO)<< requestinfo;
  m_transcationMap[transaction] = TRICKLE;
  std::string message(writer.write(requestinfo));
  LOG(INFO)<< message << "sessionid "<< m_sessionID;
  auto iter = m_handlePlugins.begin();
  BroadcastPlugin* broadcast = iter->second;
  SendMessage(message,m_sessionID,broadcast->GetHandleID());

  ResponeMessage();
  requestinfo.clear();
  message.clear();
  return 0;  

}

int JanusSignal::Configure(BroadcastPlugin* broadcast){
  LOG(INFO)<< "----- BEGIN Configure ------";
  if(NULL == broadcast){
    LOG(WARNING)<< "error param is nul";
  }
  uint64_t id = ++m_index;
  uint64_t handleID = broadcast->GetHandleID();
  m_peers_handle[id]= handleID;
  LOG(INFO)<<__FUNCTION__ << "configure id "<<id 
      << " handleid "<<handleID;
  std::string name = " publisher ";
    peers_[id] = name;
  LOG(INFO)<< id <<name;
  callback_->OnPeerConnected(id, name);
  // callback_->OnPeerConnected(1,"first ");
  return 0;
}
int JanusSignal::ConfigureSDP2Janus(uint64_t id,std::string type,std::string sdp){
  LOG(INFO)<< "----- BEGIN ConfigureSDP2Janus ------"<<id;
  auto find = m_peers_handle.find(id);
  uint64_t handleID = 0;
  if(m_peers_handle.end() != find){
    handleID = find->second;
  }
  std::string transaction = _randomString(12);
  Json::StyledWriter writer;
  Json::Value requestinfo;  
  LOG(INFO)<< "** BEGIN CONFIGURE find broadcast handleid**** "<<handleID;
  auto iter = m_handlePlugins.find(handleID);
  if(m_handlePlugins.end() == iter){
    LOG(WARNING)<<"Can not find handlid "<<handleID;
    return -1;
  }
  BroadcastPlugin* br = iter->second;
  if(br == NULL){
    LOG(WARNING)<<"Can not find plugin"; 
    return -1;
  }
  LOG(INFO)<< "** BEGIN CONFIGURE SDP ****";
  br->Configure(transaction, requestinfo,type,sdp);           

  // todo fix support h264 sdp
  // LOG(INFO)<< requestinfo;
  m_transcationMap[transaction] = CONFIGURE;
  std::string message(writer.write(requestinfo));
  // LOG(INFO)<< message << "sessionid "<< m_sessionID;
  SendMessage(message,m_sessionID,handleID);

  ResponeMessage();
  requestinfo.clear();
  message.clear();
  return 0;
  return 0;
}
int JanusSignal::ReplyConfigure(BroadcastPlugin* broadcast,Json::Value &jsep){
  std::string type;
  std::string sdp;
  if(rtc::GetStringFromJsonObject(jsep,"type",&type)){
    if(rtc::GetStringFromJsonObject(jsep,"sdp",&sdp)){
      Json::StyledWriter writer;
      // Json::Value jmessage;
       LOG(INFO)<<__FUNCTION__<<" reply "<< jsep;
      // get m_peers_handle
      uint64_t id = 0;
      auto iter = m_peers_handle.begin();
      while(iter != m_peers_handle.end()){
        uint64_t uhandleID = iter->second;
        if(uhandleID == broadcast->GetHandleID()){
          id = iter->first;
          LOG(INFO)<< "find id :"<<id;
          break;
        }
        ++iter;
      }
      LOG(INFO)<<__FUNCTION__<<" reply id:"<< id;
      callback_->OnMessageFromPeer(id,
        writer.write(jsep));
    }
  }
  return 0;
}

int JanusSignal::Detach(){

  return 0;
}

int JanusSignal::Destory(){

  return 0;
}

