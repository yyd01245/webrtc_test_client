#include "./janus_signal.h"



using namespace uprtc;


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
  while(!m_bQuit){
    if(m_sessionID == 0){
      usleep(1000);
      continue;
    }
    std::string now = std::to_string(time(0));
    //printf("send long pull get 1\n");
    m_longPull->request().verb = rtc::HV_GET;
    m_longPull->set_timeout(30000);
    m_longPull->request().path = "/uprtc/" + std::to_string(m_sessionID) + "?rid=" + now;
    // if (0 != m_maxev)
    // {
    //   m_longPull->request().path += "&maxev=" + std::to_string(m_maxev) /* + "&_=" + now */;
    // }
    std::string jsondata;
    m_longPull->request().setDocumentAndLength(new rtc::StringStream(&jsondata));

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
  printf("%s:%d  begin \n",__FUNCTION__,__LINE__);
  std::cout<<"JanusSignal create"<<std::endl;
}

JanusSignal::~JanusSignal(){
  m_bQuit = true;
}


int JanusSignal::Initialize(){
  m_HttpRequest.reset(new rtc::HttpRequest("uprtc_client"));
  m_HttpRequest->set_host(m_strDestIP);
  m_HttpRequest->set_port(m_iDestPort);
  printf("init client over \n");

  m_longPull.reset(new rtc::HttpRequest("longPull"));
  m_longPull->set_host(m_strDestIP);
  m_longPull->set_port(m_iDestPort);
  printf("init long pull over \n");
  return 0;
}

int JanusSignal::GetLongPullMessage(){

  std::string jsondata;
  printf("send long pull get \n");
  m_longPull->response().setDocumentAndLength(new rtc::StringStream(&jsondata));

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

int JanusSignal::parseSignal(Json::Value &message){

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
      double sessionid = 0;
      rtc::GetStringFromJsonObject(message, "transaction", &transaction);
      printf ("get id ~~~~~~~~~~`= local:%s,remote:%s\n",m_transaction.c_str(), transaction.c_str());
      if(rtc::GetDoubleFromJsonObject(message["data"], "id",&sessionid) )
      {
          m_sessionID = (uint64_t)sessionid;
              /* 2.crate long pull */
          Start();
          printf("sessionid_ @@@@@@@@@@@@==============%lu\n",m_sessionID);
      }
      else
      {
          printf ("error\n");
      }
    }else if(type == "webrtcup"){

    }else if(type == "detached"){

    }else if(type == "media"){

    }else if(type == "slowlink"){

    }else if(type == "error"){

    }else if(type == "event"){

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
    parseSignal(message);
    data.clear();
    
  }


  return 0;
}

int JanusSignal::SendMessage(std::string message){
  LOG(INFO)<<"message "<<message;

  // 
  m_HttpRequest->request().verb = rtc::HV_POST;
  m_HttpRequest->request().path = "/uprtc";
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
  requestinfo["uprtc"] = Json::Value("create");
  requestinfo["transaction"] = Json::Value(m_transaction);

  std::string message(writer.write(requestinfo));
  //printf ("message =============%s\n",message.c_str());

  SendMessage(message);

  ResponeMessage();
  requestinfo.clear();
  message.clear();

  return 0;
}
int JanusSignal::CreateHandle(){

  return 0;
}
int JanusSignal::Register(){

  return 0;
}
int JanusSignal::Join(){

  return 0;
}
int JanusSignal::Trick(){

  return 0;
}

int JanusSignal::Configure(){

  return 0;
}


int JanusSignal::Detach(){

  return 0;
}

int JanusSignal::Destory(){

  return 0;
}

