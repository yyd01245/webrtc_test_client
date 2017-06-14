/*
 *  Copyright 2012 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "webrtc/examples/peerconnection/client/conductor.h"

#include <memory>
#include <utility>
#include <vector>

#include "webrtc/api/test/fakeconstraints.h"
#include "webrtc/base/common.h"
#include "webrtc/base/json.h"
#include "webrtc/base/logging.h"
#include "webrtc/base/stringutils.h"
#include "webrtc/examples/peerconnection/client/defaults.h"
#include "webrtc/media/engine/webrtcvideocapturerfactory.h"
#include "webrtc/modules/video_capture/video_capture_factory.h"
// #include <jansson.h>
// Names used for a IceCandidate JSON object.
const char kCandidateSdpMidName[] = "sdpMid";
const char kCandidateSdpMlineIndexName[] = "sdpMLineIndex";
const char kCandidateSdpName[] = "candidate";

// Names used for a SessionDescription JSON object.
const char kSessionDescriptionTypeName[] = "type";
const char kSessionDescriptionSdpName[] = "sdp";

#define DTLS_ON  true
#define DTLS_OFF false
#if 0
/* public func */
bool GetValue(const std::string& data,
                                          size_t eoh,
                                          const char* header_pattern,
                                          size_t* value) {
  ASSERT(value != NULL);
  size_t found = data.find(header_pattern);
  if (found != std::string::npos && found < eoh) {
    *value = atoi(&data[found + strlen(header_pattern)]);
    return true;
  }
  return false;
}

bool GetValue(const std::string& data, size_t eoh,
                                          const char* header_pattern,
                                          std::string* value) {
  ASSERT(value != NULL);
  size_t found = data.find(header_pattern);
  if (found != std::string::npos && found < eoh) {
    size_t begin = found + strlen(header_pattern);
    size_t end = data.find("\r\n", begin);
    if (end == std::string::npos)
      end = eoh;
    value->assign(data.substr(begin, end - begin));
    return true;
  }
  return false;
}
#endif
using rtc::sprintfn;
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
class DummySetSessionDescriptionObserver
    : public webrtc::SetSessionDescriptionObserver {
 public:
  static DummySetSessionDescriptionObserver* Create() {
    return
        new rtc::RefCountedObject<DummySetSessionDescriptionObserver>();
  }
  virtual void OnSuccess() {
    LOG(INFO) << __FUNCTION__;
  }
  virtual void OnFailure(const std::string& error) {
    LOG(INFO) << __FUNCTION__ << " " << error;
  }

 protected:
  DummySetSessionDescriptionObserver() {}
  ~DummySetSessionDescriptionObserver() {}
};

LongPullThread::LongPullThread(int maxev, unsigned long sessionid, Conductor* conductor)
  : maxev_(maxev),
    sessionid_(sessionid),
    quit_(false),
   conductor_(conductor)
{
  printf("sessionid_ ==============%lu\n",sessionid_);
}

 LongPullThread::~LongPullThread() 
{
//Stop();
quit_ = true;
}

void LongPullThread::Run() 
{
#if 1
while(!quit_)
{
#if 1
std::string now = std::to_string(time(0));
HttpRequest.reset(new rtc::HttpRequest(GetPeerName()));
HttpRequest->set_host("10.0.2.15");
HttpRequest->set_port(8088);
HttpRequest->request().verb = rtc::HV_GET;
HttpRequest->request().path = "/janus/" + std::to_string(sessionid_) + "?rid=" + now;
if (0 != maxev_)
{
      HttpRequest->request().path += "&maxev=" + std::to_string(maxev_) /* + "&_=" + now */;
}

std::string jsondata;
HttpRequest->request().setDocumentAndLength(new rtc::StringStream(&jsondata));

//HttpRequest.request().setContent("application/json", new rtc::StringStream(&jsondata));
jsondata.clear();
HttpRequest->response().setDocumentAndLength(new rtc::StringStream(&jsondata));
HttpRequest->Send();
//sleep(10);
char buffer[2048]= {0};
HttpRequest->response().document->Read(buffer, sizeof(buffer), nullptr , nullptr);
//printf ("buffer ######################================== %s \n",buffer);
  //OnRead();
printf ("Run @@@@@@@%d~~~~  %s @######################\n",
HttpRequest->response().scode,
HttpRequest->response().message.c_str());

Json::Value responseJson;  
Json::Reader reader;
/* 1. event:registered  */

/* 2. event:list  */

/* 3.incomeing calling */

/* 4.accepted */
    std::string sdp;
    std::string data;
    std::string eventtype;
    data  += buffer;
    reader.parse(data, responseJson);
//printf("LongPullThread::Run@@@@@@@@@@@@@@@@@@@@@@@ ==== %s   bret = %d\n",data.c_str(),bret);
    
    if (rtc::GetStringFromJsonObject(responseJson["plugindata"]["data"]["result"], "event",&eventtype))
    {
        if ("accepted" == eventtype)
        {
            rtc::GetStringFromJsonObject(responseJson["jsep"], "sdp",&sdp);
            conductor_->OnMessageFromPeer(1,  data);
        }
    }
/* 5.huang up */
#endif
}

#endif

    
    return;
 }

/* add by wbb */
JanusClientPlugin::JanusClientPlugin()
{
    /* create socket to thread */
    printf ("111111111111111111111111111\n");
    //MyHttpServer
    MyHttpClient.reset(new rtc::HttpClientDefault(NULL, "peer_connection_client", NULL));
    rtc::Thread* thread = rtc::Thread::Current();
    socket2janus_.reset(thread->socketserver()->CreateAsyncSocket(AF_INET, SOCK_STREAM));
    MyHttpRequest.reset( new rtc::HttpRequest("peer_connection_client"));
    /* event init */
   ASSERT(socket2janus_.get() != NULL);

    socket2janus_->SignalCloseEvent.connect(this,
        &JanusClientPlugin::OnClose);

    socket2janus_->SignalConnectEvent.connect(this,
        &JanusClientPlugin::OnConnect);
    #if 0
    socket2janus_->SignalWriteEvent.connect(this,
        &JanusClientPlugin::OnConnect);
    #endif
    //socket2janus_->SignalReadEvent.connect(this,
    //&JanusClientPlugin::OnRead);
}

JanusClientPlugin::~JanusClientPlugin()
{

}

void JanusClientPlugin::ConnectJanus(const std::string& server, int port,
                                                            const std::string& client_name)
{
    ASSERT(!server.empty());
    ASSERT(!client_name.empty());

    //if (state_ != NOT_CONNECTED) 
    {
        //LOG(WARNING)<< "The client must not be connected before you can call Connect()";
        //callback_->OnServerConnectionFailure();
        //return;
    }

    if (server.empty() || client_name.empty()) {
    //callback_->OnServerConnectionFailure();
    return;
    }

    if (port <= 0)
    port = 8088;

    server_address_.SetIP(server);
    server_address_.SetPort(port);

    MyHttpClient->set_server(server_address_);
    MyHttpRequest->set_host(server);
    MyHttpRequest->set_port(port);
//    MyHttpServer->set_server(server_address_);
    //this->myname_ = client_name;
    #if 0
    if (server_address_.IsUnresolvedIP()) {
        state_ = RESOLVING;
        resolver_ = new rtc::AsyncResolver();
        resolver_->SignalDone.connect(this, &PeerConnectionClient::OnResolveResult);
        resolver_->Start(server_address_);
    } else {
        DoConnect();
    }
    #endif

}
void JanusClientPlugin::SendMessageToJanus() 
{
    /* connect */
    ASSERT(socket2janus_->GetState() == rtc::Socket::CS_CLOSED);
    int err = socket2janus_->Connect(server_address_);
    printf ("JanusClientPlugin::ConnectControlSocket\n");
    if (err == SOCKET_ERROR)
     {
        socket2janus_->Close();
        exit(-1);
    }
}

void JanusClientPlugin::RequestCreate(Conductor* conductor)
{
    Json::StyledWriter writer;
    Json::Reader reader;
    Json::Value requestinfo;  
    Json::Value responseJson;  
    char  buffer[2048]= {0};
    std::string data;
    transaction_ = _randomString(12);
    requestinfo["janus"] = Json::Value("create");
    requestinfo["transaction"] = Json::Value(transaction_);

    std::string message(writer.write(requestinfo));
    #if 0
    char headers[1024];

    sprintfn(headers, sizeof(headers),
        "POST /janus HTTP/1.1\r\n"
        "Content-Length: %i\r\n"
        "Content-Type: application/json\r\n"
        "\r\n", message.length());
    
    send_data_ = headers;
    #endif
    //send_data_ +=message;
    printf ("message =============%s\n",message.c_str());

    /* 1.send adn recv create */
    MyHttpRequest->request().verb = rtc::HV_POST;
    MyHttpRequest->request().path = "/janus";// + GetPeerName();
    MyHttpRequest->request().setContent("application/json", new rtc::StringStream(&message));

    std::string str;
    MyHttpRequest->response().setDocumentAndLength(new rtc::StringStream(&str));
    MyHttpRequest->Send();
    OnRead();
    printf ("verb = %d~~~~  %s @######################\n",
    MyHttpRequest->response().scode,
    MyHttpRequest->response().message.c_str());
    
    /* 2.crate long pull */
    longpull_.reset(new LongPullThread(1, sessionid_, conductor));
    longpull_->Start();

  /* 3. attach*/
    requestinfo.clear();
    message.clear();
    data.clear();
    str.clear();
    memset(buffer,0,sizeof(buffer));

    requestinfo["janus"] = Json::Value("attach");
    requestinfo["plugin"] = Json::Value("janus.plugin.videocall");
    requestinfo["transaction"] = Json::Value(_randomString(12));

    message += (writer.write(requestinfo));
    printf("message==============%s",message.c_str());
    std::unique_ptr<rtc::HttpRequest> httpAttach;
    httpAttach.reset(new rtc::HttpRequest(GetPeerName()));  
    httpAttach->set_host("10.0.2.15");
    httpAttach->set_port(8088);
    httpAttach->request().verb = rtc::HV_POST;
    httpAttach->request().path = "/janus/" + std::to_string(sessionid_);
    httpAttach->request().setContent("application/json", new rtc::StringStream(&message));

    double pluginid;

    httpAttach->response().setDocumentAndLength(new rtc::StringStream(&str));
    httpAttach->Send();

    httpAttach->response().document->Read(buffer, sizeof(buffer), nullptr , nullptr);
    data += buffer;
    responseJson.clear();
    reader.parse(data, responseJson);
    rtc::GetDoubleFromJsonObject(responseJson["data"], "id",&pluginid);
    pluginid_ = (unsigned long)pluginid;
    printf("pluginid ==== %f\n",pluginid);
  /* 4. post message:register  {"janus":"message","body":{"request":"register","username":"112"},"transaction":"qIZRrvBkrLlE"}*/
    requestinfo.clear();
    message.clear();
    data.clear();
    str.clear();
    memset(buffer,0,sizeof(buffer));

    requestinfo["janus"] = Json::Value("message");

    Json::Value body; 
    body["request"] = Json::Value("register");
    body["username"] = Json::Value("wbb111");

    requestinfo["body"] = Json::Value(body);
    requestinfo["transaction"] = Json::Value(_randomString(12));

    message += (writer.write(requestinfo));
    printf("message==============%s",message.c_str());
    std::unique_ptr<rtc::HttpRequest> httpRegister;
    httpRegister.reset(new rtc::HttpRequest(GetPeerName()));  
    httpRegister->set_host("10.0.2.15");
    httpRegister->set_port(8088);
    httpRegister->request().verb = rtc::HV_POST;
    httpRegister->request().path = "/janus/" + std::to_string(sessionid_) + "/" + std::to_string((unsigned long)pluginid);
    httpRegister->request().setContent("application/json", new rtc::StringStream(&message));

    double session_id;
    str.clear();
    printf("str ============= %s\n",str.c_str());
    httpRegister->response().setDocumentAndLength(new rtc::StringStream(&str));
    httpRegister->Send();

    httpRegister->response().document->Read(buffer, sizeof(buffer), nullptr , nullptr);
    data += buffer;
    printf("buffer ===========%s\n", buffer);
    reader.parse(data, responseJson);
    rtc::GetDoubleFromJsonObject(responseJson, "session_id",&session_id);
    printf("session_id ==== %f\n",session_id);
  /* 5. post message:list  */
    requestinfo.clear();
    message.clear();
    data.clear();
    str.clear();
    memset(buffer,0,sizeof(buffer));

    requestinfo["janus"] = Json::Value("message");

    body.clear(); 
    body["request"] = Json::Value("list");

    requestinfo["body"] = Json::Value(body);
    requestinfo["transaction"] = Json::Value(_randomString(12));

    message += (writer.write(requestinfo));
    printf("message==============%s",message.c_str());
    std::unique_ptr<rtc::HttpRequest> httpList;
    httpList.reset(new rtc::HttpRequest(GetPeerName()));  
    httpList->set_host("10.0.2.15");
    httpList->set_port(8088);
    httpList->request().verb = rtc::HV_POST;
    httpList->request().path = "/janus/" + std::to_string(sessionid_) + "/" + std::to_string((unsigned long)pluginid);
    httpList->request().setContent("application/json", new rtc::StringStream(&message));

    session_id = 0;
    str.clear();
    printf("str ============= %s\n",str.c_str());
    httpList->response().setDocumentAndLength(new rtc::StringStream(&str));
    httpList->Send();

    httpList->response().document->Read(buffer, sizeof(buffer), nullptr , nullptr);
    data += buffer;
    printf("httpList!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ===========%s\n", buffer);
    reader.parse(data, responseJson);
    rtc::GetDoubleFromJsonObject(responseJson, "session_id",&session_id);
    printf("session_id ==== %f\n",session_id);

    /* call someone */
    //SendMessageToJanus();
}

void JanusClientPlugin::OnRead()
{
      //LOG(INFO) << __FUNCTION__ ;
    std::string data;
    char buffer[0xffff];
    #if 0
    do 
    {
        int bytes = socket->Recv(buffer, sizeof(buffer), nullptr);
        if (bytes <= 0)
        break;
        data.append(buffer, bytes);
    } while (true);
    #endif

    if (rtc::HC_OK == MyHttpRequest->response().scode)
    {
        //printf("%x!!!!!!!!!!!!!!!!\n",MyHttpRequest->response().document);
        MyHttpRequest->response().document->Read(buffer, sizeof(buffer), nullptr , nullptr);
        //printf ("buffer ================== %s \n",buffer);
        data+= buffer;
        Json::Reader reader;
        Json::Value message;
        if (!reader.parse(data, message)) {
        LOG(WARNING) << "Received unknown message. " << message;
        return;
        }
        std::string type;
        double sessionid = 0;
        rtc::GetStringFromJsonObject(message, "janus", &type);
        LOG(INFO) << type.c_str();
        if (!type.empty()) 
        {
            if (type == "success")
            {
                std::string wbb ;
                //LOG(INFO) <<wbb.c_str();
                std::string transaction;
                rtc::GetStringFromJsonObject(message, "transaction", &transaction);
                printf ("get id ~~~~~~~~~~`= %s,%s\n",transaction_.c_str(), transaction.c_str());
                if(/*( transaction_ == transaction) && */rtc::GetDoubleFromJsonObject(message["data"], "id",&sessionid) )
                {
                    sessionid_ = (unsigned long) sessionid;
                    printf("sessionid_ @@@@@@@@@@@@==============%lu\n",sessionid_);
                }
                else
                {
                    printf ("error\n");
                }
            }
            else
            {


            }
        }
    }

    LOG(INFO) <<data.c_str();
}

void JanusClientPlugin::OnConnect(rtc::AsyncSocket* socket) 
{
    ASSERT(!send_data_.empty());
    //std::string wbb("GET /sign_in?wbb HTTP/1.0\r\n\r\n");
    printf ("send_data_.c_str() = %s\n",send_data_.c_str());
    size_t sent = socket->Send(send_data_.c_str(), send_data_.length());
    printf ("JanusClientPlugin::OnConnect\n");
    ASSERT(sent == send_data_.length());
    RTC_UNUSED(sent);
    send_data_.clear();
}

void JanusClientPlugin::OnClose(rtc::AsyncSocket* socket, int err) {
  LOG(INFO) << __FUNCTION__;
  printf ("OnClose~~~~~~~~~~~~~~~~~~~~~~\n");
  socket->Close();
#if 0
#ifdef WIN32
  if (err != WSAECONNREFUSED) {
#else
  if (err != ECONNREFUSED) {
#endif
    if (socket == hanging_get_.get()) {
      if (state_ == CONNECTED) {
        hanging_get_->Close();
        hanging_get_->Connect(server_address_);
      }
    } else {
      callback_->OnMessageSent(err);
    }
  } else {
    if (socket == control_socket_.get()) {
      LOG(WARNING) << "Connection refused; retrying in 2 seconds";
      printf ("__FUNCTION__ = %s   %s\n",__FUNCTION__, __FILE__ ":" STRINGIZE(__LINE__));
      rtc::Thread::Current()->PostDelayed(RTC_FROM_HERE, kReconnectDelay, this,
                                          0);
    } else {
      Close();
      callback_->OnDisconnected();
    }
    #endif
  }

/***********************Videocall_plugin*************/

VideoCallPlugin::VideoCallPlugin()
{
    printf ("2w2222222222222222222222222222222\n");
}
VideoCallPlugin::~VideoCallPlugin()
{

}

/*end add */
Conductor::Conductor(PeerConnectionClient* client, MainWindow* main_wnd)
  : peer_id_(-1),
    loopback_(false),
    client_(client),
    main_wnd_(main_wnd) {
  client_->RegisterObserver(this);
  main_wnd->RegisterObserver(this);
  videoall_plugin_.reset(new VideoCallPlugin());
}

Conductor::~Conductor() {
  ASSERT(peer_connection_.get() == NULL);
}

bool Conductor::connection_active() const {
  return peer_connection_.get() != NULL;
}

void Conductor::Close() {
  client_->SignOut();
  DeletePeerConnection();
}

bool Conductor::InitializePeerConnection() {
  ASSERT(peer_connection_factory_.get() == NULL);
  ASSERT(peer_connection_.get() == NULL);

  peer_connection_factory_  = webrtc::CreatePeerConnectionFactory();

  if (!peer_connection_factory_.get()) {
    main_wnd_->MessageBox("Error",
        "Failed to initialize PeerConnectionFactory", true);
    DeletePeerConnection();
    return false;
  }

  if (!CreatePeerConnection(DTLS_ON)) {
    main_wnd_->MessageBox("Error",
        "CreatePeerConnection failed", true);
    DeletePeerConnection();
  }
  AddStreams();
  return peer_connection_.get() != NULL;
}

bool Conductor::ReinitializePeerConnectionForLoopback() {
  loopback_ = true;
  rtc::scoped_refptr<webrtc::StreamCollectionInterface> streams(
      peer_connection_->local_streams());
  peer_connection_ = NULL;
  if (CreatePeerConnection(DTLS_OFF)) {
    for (size_t i = 0; i < streams->count(); ++i)
      peer_connection_->AddStream(streams->at(i));
    peer_connection_->CreateOffer(this, NULL);
  }
  return peer_connection_.get() != NULL;
}

bool Conductor::CreatePeerConnection(bool dtls) {
  ASSERT(peer_connection_factory_.get() != NULL);
  ASSERT(peer_connection_.get() == NULL);

  webrtc::PeerConnectionInterface::RTCConfiguration config;
  webrtc::PeerConnectionInterface::IceServer server;
  server.uri = GetPeerConnectionString();
  config.servers.push_back(server);

  webrtc::FakeConstraints constraints;
  if (dtls) {
    constraints.AddOptional(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp,
                            "true");
  } else {
    constraints.AddOptional(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp,
                            "false");
  }

  peer_connection_ = peer_connection_factory_->CreatePeerConnection(
      config, &constraints, NULL, NULL, this);
  return peer_connection_.get() != NULL;
}

void Conductor::DeletePeerConnection() {
  peer_connection_ = NULL;
  active_streams_.clear();
  main_wnd_->StopLocalRenderer();
  main_wnd_->StopRemoteRenderer();
  peer_connection_factory_ = NULL;
  peer_id_ = -1;
  loopback_ = false;
}

void Conductor::EnsureStreamingUI() {
  ASSERT(peer_connection_.get() != NULL);
  if (main_wnd_->IsWindow()) {
    if (main_wnd_->current_ui() != MainWindow::STREAMING)
      main_wnd_->SwitchToStreamingUI();
  }
}

//
// PeerConnectionObserver implementation.
//

// Called when a remote stream is added
void Conductor::OnAddStream(
    rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) {
  LOG(INFO) << __FUNCTION__ << " " << stream->label();
  main_wnd_->QueueUIThreadCallback(NEW_STREAM_ADDED, stream.release());
}

void Conductor::OnRemoveStream(
    rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) {
  LOG(INFO) << __FUNCTION__ << " " << stream->label();
  main_wnd_->QueueUIThreadCallback(STREAM_REMOVED, stream.release());
}

void Conductor::OnIceCandidate(const webrtc::IceCandidateInterface* candidate) {
  LOG(INFO) << __FUNCTION__ << " " << candidate->sdp_mline_index();
  // For loopback test. To save some connecting delay.
  if (loopback_) {
    if (!peer_connection_->AddIceCandidate(candidate)) {
      LOG(WARNING) << "Failed to apply the received candidate";
    }
    return;
  }

  Json::StyledWriter writer;
  Json::Value jmessage;
  jmessage["janus"] = "trickle";
  jmessage["transaction"] = Json::Value(_randomString(12));
  jmessage[kCandidateSdpMidName] = candidate->sdp_mid();
  jmessage[kCandidateSdpMlineIndexName] = candidate->sdp_mline_index();
  std::string sdp;
  if (!candidate->ToString(&sdp)) {
    LOG(LS_ERROR) << "Failed to serialize candidate";
    return;
  }
  printf("OnIceCandidate  sdp ============== %s!~~~~~~~~~~\n",sdp.c_str());
  jmessage[kCandidateSdpName] = sdp;
  //SendMessage(writer.write(jmessage));

    /* add by wbb */
    std::string message(writer.write(jmessage));

    Json::Reader reader;
    Json::Value responseJson;  
    char  buffer[2048]= {0};
    std::string data;
    std::unique_ptr<rtc::HttpRequest> httpTrickle;
    std::string str;
    httpTrickle.reset(new rtc::HttpRequest(GetPeerName()));  
    httpTrickle->set_host("10.0.4.66");
    httpTrickle->set_port(8088);
    httpTrickle->request().verb = rtc::HV_POST;
    httpTrickle->request().path = "/janus/" + std::to_string(videoall_plugin_->sessionid_) + "/" + std::to_string(videoall_plugin_->pluginid_);
    httpTrickle->request().setContent("application/json", new rtc::StringStream(&message));

    double session_id;
    str.clear();
    httpTrickle->response().setDocumentAndLength(new rtc::StringStream(&str));
    httpTrickle->Send();

    httpTrickle->response().document->Read(buffer, sizeof(buffer), nullptr , nullptr);
    data += buffer;
    responseJson.clear();
    reader.parse(data, responseJson);
    rtc::GetDoubleFromJsonObject(responseJson, "session_id",&session_id);
    printf("session_id ==== %f\n",session_id);
}

//
// PeerConnectionClientObserver implementation.
//

void Conductor::OnSignedIn() {
  LOG(INFO) << __FUNCTION__;
  main_wnd_->SwitchToPeerList(client_->peers());
}

void Conductor::OnDisconnected() {
  LOG(INFO) << __FUNCTION__;

  DeletePeerConnection();

  if (main_wnd_->IsWindow())
    main_wnd_->SwitchToConnectUI();
}

void Conductor::OnPeerConnected(int id, const std::string& name) {
  LOG(INFO) << __FUNCTION__;
  // Refresh the list if we're showing it.
  if (main_wnd_->current_ui() == MainWindow::LIST_PEERS)
    main_wnd_->SwitchToPeerList(client_->peers());
}

void Conductor::OnPeerDisconnected(int id) {
  LOG(INFO) << __FUNCTION__;
  if (id == peer_id_) {
    LOG(INFO) << "Our peer disconnected";
    main_wnd_->QueueUIThreadCallback(PEER_CONNECTION_CLOSED, NULL);
  } else {
    // Refresh the list if we're showing it.
    if (main_wnd_->current_ui() == MainWindow::LIST_PEERS)
      main_wnd_->SwitchToPeerList(client_->peers());
  }
}

void Conductor::OnMessageFromPeer(int peer_id, const std::string& message) {
  ASSERT(peer_id_ == peer_id || peer_id_ == -1);
  ASSERT(!message.empty());

  if (!peer_connection_.get()) {
    ASSERT(peer_id_ == -1);
    peer_id_ = peer_id;

    if (!InitializePeerConnection()) {
      LOG(LS_ERROR) << "Failed to initialize our PeerConnection instance";
      client_->SignOut();
      return;
    }
  } else if (peer_id != peer_id_) {
    ASSERT(peer_id_ != -1);
    LOG(WARNING) << "Received a message from unknown peer while already in a "
                    "conversation with a different peer.";
    return;
  }

  Json::Reader reader;
  Json::Value jmessage;
  if (!reader.parse(message, jmessage)) {
    LOG(WARNING) << "Received unknown message. " << message;
    return;
  }
  std::string type;
  std::string json_object;

  rtc::GetStringFromJsonObject(jmessage["jsep"], kSessionDescriptionTypeName, &type);
  if (!type.empty()) {
    if (type == "offer-loopback") {
      // This is a loopback call.
      // Recreate the peerconnection with DTLS disabled.
      if (!ReinitializePeerConnectionForLoopback()) {
        LOG(LS_ERROR) << "Failed to initialize our PeerConnection instance";
        DeletePeerConnection();
        client_->SignOut();
      }
      return;
    }

    std::string sdp;
    if (!rtc::GetStringFromJsonObject(jmessage["jsep"], kSessionDescriptionSdpName,
                                      &sdp)) {
      LOG(WARNING) << "Can't parse received session description message&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&.";
      return;
    }
    printf("sdp #############################################= %s",sdp.c_str());
    webrtc::SdpParseError error;
    webrtc::SessionDescriptionInterface* session_description(
        webrtc::CreateSessionDescription(type, sdp, &error));
    if (!session_description) {
      LOG(WARNING) << "Can't parse received session description message. "
          << "SdpParseError was: " << error.description;
      return;
    }
    LOG(INFO) << " Received session description :" << message;
    peer_connection_->SetRemoteDescription(
        DummySetSessionDescriptionObserver::Create(), session_description);
    if (session_description->type() ==
        webrtc::SessionDescriptionInterface::kOffer) {
      peer_connection_->CreateAnswer(this, NULL);
    }
    return;
  } else {
    std::string sdp_mid;
    int sdp_mlineindex = 0;
    std::string sdp;
    if (!rtc::GetStringFromJsonObject(jmessage, kCandidateSdpMidName,
                                      &sdp_mid) ||
        !rtc::GetIntFromJsonObject(jmessage, kCandidateSdpMlineIndexName,
                                   &sdp_mlineindex) ||
        !rtc::GetStringFromJsonObject(jmessage, kCandidateSdpName, &sdp)) {
      LOG(WARNING) << "Can't parse received message.";
      return;
    }
    webrtc::SdpParseError error;
    std::unique_ptr<webrtc::IceCandidateInterface> candidate(
        webrtc::CreateIceCandidate(sdp_mid, sdp_mlineindex, sdp, &error));
    if (!candidate.get()) {
      LOG(WARNING) << "Can't parse received candidate message. "
          << "SdpParseError was: " << error.description;
      return;
    }
    if (!peer_connection_->AddIceCandidate(candidate.get())) {
      LOG(WARNING) << "Failed to apply the received candidate";
      return;
    }
    LOG(INFO) << " Received candidate :" << message;
    return;
  }
}

void Conductor::OnMessageSent(int err) {
  // Process the next pending message if any.
  main_wnd_->QueueUIThreadCallback(SEND_MESSAGE_TO_PEER, NULL);
}

void Conductor::OnServerConnectionFailure() {
    main_wnd_->MessageBox("Error", ("Failed to connect to " + server_).c_str(),
                          true);
}

//
// MainWndCallback implementation.
//
 
void Conductor::StartLogin(const std::string& server, int port) {
  if (client_->is_connected())
    return;
  server_ = server;
  /* start add by wbb */
  printf ("StartLogin~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  OnSignedIn();
  videoall_plugin_->ConnectJanus(server, port, GetPeerName());
  videoall_plugin_->RequestCreate(this);
  ConnectToPeer(1);
  
  /*end add by wbb */
  //client_->Connect(server, port, GetPeerName());
}

void Conductor::DisconnectFromServer() {
  if (client_->is_connected())
    client_->SignOut();
}

void Conductor::ConnectToPeer(int peer_id) {
  ASSERT(peer_id_ == -1);
  ASSERT(peer_id != -1);

  if (peer_connection_.get()) {
    main_wnd_->MessageBox("Error",
        "We only support connecting to one peer at a time", true);
    return;
  }

  if (InitializePeerConnection()) {
    peer_id_ = peer_id;
    peer_connection_->CreateOffer(this, NULL);
  } else {
    main_wnd_->MessageBox("Error", "Failed to initialize PeerConnection", true);
  }
}

cricket::VideoCapturer* Conductor::OpenVideoCaptureDevice() {
  std::vector<std::string> device_names;
  {
    std::unique_ptr<webrtc::VideoCaptureModule::DeviceInfo> info(
        webrtc::VideoCaptureFactory::CreateDeviceInfo(0));
    if (!info) {
      return nullptr;
    }
    int num_devices = info->NumberOfDevices();
    for (int i = 0; i < num_devices; ++i) {
      const uint32_t kSize = 256;
      char name[kSize] = {0};
      char id[kSize] = {0};
      if (info->GetDeviceName(i, name, kSize, id, kSize) != -1) {
        device_names.push_back(name);
      }
    }
  }

  cricket::WebRtcVideoDeviceCapturerFactory factory;
  cricket::VideoCapturer* capturer = nullptr;
  for (const auto& name : device_names) {
    capturer = factory.Create(cricket::Device(name, 0));
    if (capturer) {
      break;
    }
  }
  return capturer;
}

void Conductor::AddStreams() {
  if (active_streams_.find(kStreamLabel) != active_streams_.end())
    return;  // Already added.

  rtc::scoped_refptr<webrtc::AudioTrackInterface> audio_track(
      peer_connection_factory_->CreateAudioTrack(
          kAudioLabel, peer_connection_factory_->CreateAudioSource(NULL)));

  rtc::scoped_refptr<webrtc::VideoTrackInterface> video_track(
      peer_connection_factory_->CreateVideoTrack(
          kVideoLabel,
          peer_connection_factory_->CreateVideoSource(OpenVideoCaptureDevice(),
                                                      NULL)));
  main_wnd_->StartLocalRenderer(video_track);

  rtc::scoped_refptr<webrtc::MediaStreamInterface> stream =
      peer_connection_factory_->CreateLocalMediaStream(kStreamLabel);

  stream->AddTrack(audio_track);
  stream->AddTrack(video_track);
  if (!peer_connection_->AddStream(stream)) {
    LOG(LS_ERROR) << "Adding stream to PeerConnection failed";
  }
  typedef std::pair<std::string,
                    rtc::scoped_refptr<webrtc::MediaStreamInterface> >
      MediaStreamPair;
  active_streams_.insert(MediaStreamPair(stream->label(), stream));
  main_wnd_->SwitchToStreamingUI();
}

void Conductor::DisconnectFromCurrentPeer() {
  LOG(INFO) << __FUNCTION__;
  if (peer_connection_.get()) {
    client_->SendHangUp(peer_id_);
    DeletePeerConnection();
  }

  if (main_wnd_->IsWindow())
    main_wnd_->SwitchToPeerList(client_->peers());
}

void Conductor::UIThreadCallback(int msg_id, void* data) {
  switch (msg_id) {
    case PEER_CONNECTION_CLOSED:
      LOG(INFO) << "PEER_CONNECTION_CLOSED";
      DeletePeerConnection();

      ASSERT(active_streams_.empty());

      if (main_wnd_->IsWindow()) {
        if (client_->is_connected()) {
          main_wnd_->SwitchToPeerList(client_->peers());
        } else {
          main_wnd_->SwitchToConnectUI();
        }
      } else {
        DisconnectFromServer();
      }
      break;

    case SEND_MESSAGE_TO_PEER: {
      LOG(INFO) << "SEND_MESSAGE_TO_PEER";
      std::string* msg = reinterpret_cast<std::string*>(data);
      if (msg) {
        // For convenience, we always run the message through the queue.
        // This way we can be sure that messages are sent to the server
        // in the same order they were signaled without much hassle.
        pending_messages_.push_back(msg);
      }

      if (!pending_messages_.empty() && !client_->IsSendingMessage()) {
        msg = pending_messages_.front();
        pending_messages_.pop_front();

        if (!client_->SendToPeer(peer_id_, *msg) && peer_id_ != -1) {
          LOG(LS_ERROR) << "SendToPeer failed";
          DisconnectFromServer();
        }
        delete msg;
      }

      if (!peer_connection_.get())
        peer_id_ = -1;

      break;
    }

    case NEW_STREAM_ADDED: {
        printf(" NEW_STREAM_ADDED NEW_STREAM_ADDED@@@@@@@@@@@@@@@@@@\n");
      webrtc::MediaStreamInterface* stream =
          reinterpret_cast<webrtc::MediaStreamInterface*>(
          data);
      webrtc::VideoTrackVector tracks = stream->GetVideoTracks();
      // Only render the first track.
      if (!tracks.empty()) {
        webrtc::VideoTrackInterface* track = tracks[0];
        main_wnd_->StartRemoteRenderer(track);
      }
      stream->Release();
      break;
    }

    case STREAM_REMOVED: {
      // Remote peer stopped sending a stream.
      webrtc::MediaStreamInterface* stream =
          reinterpret_cast<webrtc::MediaStreamInterface*>(
          data);
      stream->Release();
      break;
    }

    default:
      ASSERT(false);
      break;
  }
}

void Conductor::OnSuccess(webrtc::SessionDescriptionInterface* desc) {
  peer_connection_->SetLocalDescription(
      DummySetSessionDescriptionObserver::Create(), desc);

  std::string sdp;
  desc->ToString(&sdp);

  // For loopback test. To save some connecting delay.
  if (loopback_) {
    // Replace message type from "offer" to "answer"
    webrtc::SessionDescriptionInterface* session_description(
        webrtc::CreateSessionDescription("answer", sdp, nullptr));
    peer_connection_->SetRemoteDescription(
        DummySetSessionDescriptionObserver::Create(), session_description);
    return;
  }

  Json::StyledWriter writer;
  Json::Value jmessage;
  jmessage[kSessionDescriptionTypeName] = desc->type();
  jmessage[kSessionDescriptionSdpName] = sdp;
  //SendMessage(writer.write(jmessage));
    /* add by wbb */
    Json::Reader reader;
    Json::Value requestinfo;  
    Json::Value responseJson;  
    char  buffer[2048]= {0};
    std::string data;
    requestinfo["janus"] = Json::Value("message");
    requestinfo["transaction"] = "111111111111";

    Json::Value body; 
    body["request"] = Json::Value("call");
    body["username"] = Json::Value("wbb");
    requestinfo["body"] = Json::Value(body);

    Json::Value jsep; 
    jsep["type"] = desc->type();
    jsep["sdp"] = sdp;//"v=0\r\no=mozilla...THIS_IS_SDPARTA-47.0 7354462103758933412 0 IN IP4 10.0.1.201\r\ns=-\r\nt=0 0\r\na=group:BUNDLE audio video data\r\na=msid-semantic: WMS janus\r\nm=audio 1 RTP/SAVPF 109 9 0 8\r\nc=IN IP4 10.0.1.201\r\na=mid:audio\r\na=sendrecv\r\na=rtcp-mux\r\na=ice-ufrag:E9MU\r\na=ice-pwd:8QEwpNXgsOFf6iSUKuRq3H\r\na=ice-options:trickle\r\na=fingerprint:sha-256 D2:B9:31:8F:DF:24:D8:0E:ED:D2:EF:25:9E:AF:6F:B8:34:AE:53:9C:E6:F3:8F:F2:64:15:FA:E8:7F:53:2D:38\r\na=setup:actpass\r\na=connection:new\r\na=fmtp:109 maxplaybackrate=48000;stereo=1\r\na=rtpmap:109 opus/48000/2\r\na=rtpmap:9 G722/8000/1\r\na=rtpmap:0 PCMU/8000\r\na=rtpmap:8 PCMA/8000\r\na=ssrc:2243984433 cname:janusaudio\r\na=ssrc:2243984433 msid:janus janusa0\r\na=ssrc:2243984433 mslabel:janus\r\na=ssrc:2243984433 label:janusa0\r\na=candidate:1 1 udp 2013266431 10.0.1.201 41125 typ host\r\na=candidate:1 2 udp 2013266430 10.0.1.201 36936 typ host\r\nm=video 1 RTP/SAVPF 120 126 97\r\nc=IN IP4 10.0.1.201\r\na=mid:video\r\na=sendrecv\r\na=rtcp-mux\r\na=ice-ufrag:Khwd\r\na=ice-pwd:1sjIf4Kr2obkLBeFAalIx2\r\na=ice-options:trickle\r\na=fingerprint:sha-256 D2:B9:31:8F:DF:24:D8:0E:ED:D2:EF:25:9E:AF:6F:B8:34:AE:53:9C:E6:F3:8F:F2:64:15:FA:E8:7F:53:2D:38\r\na=setup:actpass\r\na=connection:new\r\na=fmtp:126 profile-level-id=42e01f;level-asymmetry-allowed=1;packetization-mode=1\r\na=fmtp:97 profile-level-id=42e01f;level-asymmetry-allowed=1\r\na=fmtp:120 max-fs=12288;max-fr=60\r\na=rtcp-fb:120 nack\r\na=rtcp-fb:120 nack pli\r\na=rtcp-fb:120 ccm fir\r\na=rtcp-fb:126 nack\r\na=rtcp-fb:126 nack pli\r\na=rtcp-fb:126 ccm fir\r\na=rtcp-fb:97 nack\r\na=rtcp-fb:97 nack pli\r\na=rtcp-fb:97 ccm fir\r\na=rtpmap:120 VP8/90000\r\na=rtpmap:126 H264/90000\r\na=rtpmap:97 H264/90000\r\na=ssrc:2939252605 cname:janusvideo\r\na=ssrc:2939252605 msid:janus janusv0\r\na=ssrc:2939252605 mslabel:janus\r\na=ssrc:2939252605 label:janusv0\r\na=candidate:2 1 udp 2013266431 10.0.1.201 34159 typ host\r\na=candidate:2 2 udp 2013266430 10.0.1.201 57626 typ host\r\nm=application 1 DTLS/SCTP 5000\r\nc=IN IP4 10.0.1.201\r\na=mid:data\r\na=sctpmap:5000 webrtc-datachannel 16\r\na=ice-ufrag:TG7p\r\na=ice-pwd:+tD+2zgRAmb9VhW9USp4ZZ\r\na=ice-options:trickle\r\na=fingerprint:sha-256 D2:B9:31:8F:DF:24:D8:0E:ED:D2:EF:25:9E:AF:6F:B8:34:AE:53:9C:E6:F3:8F:F2:64:15:FA:E8:7F:53:2D:38\r\na=setup:actpass\r\na=connection:new\r\na=candidate:3 1 udp 2013266431 10.0.1.201 47723 typ host\r\n";
                                    
    //jsep["sdp"] = "v=0\r\no=mozilla...THIS_IS_SDPARTA-47.0 6392083248009228638 0 IN IP4 0.0.0.0\r\ns=-\r\nt=0 0\r\na=fingerprint:sha-256 86:BE:B9:EE:01:89:A5:2D:70:5B:F6:51:52:32:FE:E3:A8:E9:80:40:9F:1B:EF:97:ED:C7:47:0E:9F:D6:38:4F\r\na=group:BUNDLE sdparta_0 sdparta_1 sdparta_2\r\na=ice-options:trickle\r\na=msid-semantic:WMS *\r\nm=audio 9 UDP/TLS/RTP/SAVPF 109 9 0 8\r\nc=IN IP4 0.0.0.0\r\na=sendrecv\r\na=extmap:1 urn:ietf:params:rtp-hdrext:ssrc-audio-level\r\na=fmtp:109 maxplaybackrate=48000;stereo=1\r\na=ice-pwd:aaf9a41ecb8b926253f7699e8e78a244\r\na=ice-ufrag:1fdf2d3d\r\na=mid:sdparta_0\r\na=msid:{36009eeb-c42a-4b35-92ef-6b0919a785dc} {ca3fce00-11d2-4e10-85fe-979c7eacc649}\r\na=rtcp-mux\r\na=rtpmap:109 opus/48000/2\r\na=rtpmap:9 G722/8000/1\r\na=rtpmap:0 PCMU/8000\r\na=rtpmap:8 PCMA/8000\r\na=setup:actpass\r\na=ssrc:2926802864 cname:{64270aa5-04d5-4cb2-8cf2-3fdac9e5dbe8}\r\nm=video 9 UDP/TLS/RTP/SAVPF 120 126 97\r\nc=IN IP4 0.0.0.0\r\na=sendrecv\r\na=fmtp:126 profile-level-id=42e01f;level-asymmetry-allowed=1;packetization-mode=1\r\na=fmtp:97 profile-level-id=42e01f;level-asymmetry-allowed=1\r\na=fmtp:120 max-fs=12288;max-fr=60\r\na=ice-pwd:aaf9a41ecb8b926253f7699e8e78a244\r\na=ice-ufrag:1fdf2d3d\r\na=mid:sdparta_1\r\na=msid:{36009eeb-c42a-4b35-92ef-6b0919a785dc} {39d55bc6-4b68-446f-bf24-9f39aca617ae}\r\na=rtcp-fb:120 nack\r\na=rtcp-fb:120 nack pli\r\na=rtcp-fb:120 ccm fir\r\na=rtcp-fb:126 nack\r\na=rtcp-fb:126 nack pli\r\na=rtcp-fb:126 ccm fir\r\na=rtcp-fb:97 nack\r\na=rtcp-fb:97 nack pli\r\na=rtcp-fb:97 ccm fir\r\na=rtcp-mux\r\na=rtpmap:120 VP8/90000\r\na=rtpmap:126 H264/90000\r\na=rtpmap:97 H264/90000\r\na=setup:actpass\r\na=ssrc:501521387 cname:{64270aa5-04d5-4cb2-8cf2-3fdac9e5dbe8}\r\nm=application 9 DTLS/SCTP 5000\r\nc=IN IP4 0.0.0.0\r\na=sendrecv\r\na=ice-pwd:aaf9a41ecb8b926253f7699e8e78a244\r\na=ice-ufrag:1fdf2d3d\r\na=mid:sdparta_2\r\na=sctpmap:5000 webrtc-datachannel 256\r\na=setup:actpass\r\na=ssrc:75978560 cname:{64270aa5-04d5-4cb2-8cf2-3fdac9e5dbe8}\r\n";
    requestinfo["jsep"] = Json::Value(jsep);

    std::string message(writer.write(requestinfo));

    printf ("sdp =============%s\n",sdp.c_str());

    std::unique_ptr<rtc::HttpRequest> httpCall;
    std::string str;
    httpCall.reset(new rtc::HttpRequest(GetPeerName()));  
    httpCall->set_host("10.0.2.15");
    httpCall->set_port(8088);
    httpCall->request().verb = rtc::HV_POST;
    httpCall->request().path = "/janus/" + std::to_string(videoall_plugin_->sessionid_) + "/" + std::to_string(videoall_plugin_->pluginid_);
    httpCall->request().setContent("application/json", new rtc::StringStream(&message));

    double session_id;
    str.clear();
    httpCall->response().setDocumentAndLength(new rtc::StringStream(&str));
    httpCall->Send();

    httpCall->response().document->Read(buffer, sizeof(buffer), nullptr , nullptr);
    data += buffer;
    responseJson.clear();
    reader.parse(data, responseJson);
    rtc::GetDoubleFromJsonObject(responseJson, "session_id",&session_id);
    printf("session_id ==== %f\n",session_id);
}

void Conductor::OnFailure(const std::string& error) {
    LOG(LERROR) << error;
}

void Conductor::SendMessage(const std::string& json_object) {
  std::string* msg = new std::string(json_object);
  main_wnd_->QueueUIThreadCallback(SEND_MESSAGE_TO_PEER, msg);
}
