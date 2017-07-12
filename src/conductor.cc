/*
 *  Copyright 2012 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "webrtc/examples/webrtc_client/src/conductor.h"

#include <memory>
#include <utility>
#include <vector>

#include "webrtc/api/test/fakeconstraints.h"
#include "webrtc/base/checks.h"
#include "webrtc/base/common.h"
#include "webrtc/base/json.h"
#include "webrtc/base/logging.h"
#include "webrtc/examples/webrtc_client/src/defaults.h"
#include "webrtc/media/engine/webrtcvideocapturerfactory.h"
#include "webrtc/modules/video_capture/video_capture_factory.h"

// Names used for a IceCandidate JSON object.
const char kCandidateSdpMidName[] = "sdpMid";
const char kCandidateSdpMlineIndexName[] = "sdpMLineIndex";
const char kCandidateSdpName[] = "candidate";

// Names used for a SessionDescription JSON object.
const char kSessionDescriptionTypeName[] = "type";
const char kSessionDescriptionSdpName[] = "sdp";

#define DTLS_ON  true
#define DTLS_OFF false

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
Conductor::Conductor(JanusSignal* client, MainWindow* main_wnd)
  : peer_id_(0),
    loopback_(false),
    client_(client),
    main_wnd_(main_wnd) {
  client_->RegisterObserver(this);
  if(main_wnd_)
    main_wnd_->RegisterObserver(this);
  LOG(INFO) << __FUNCTION__<<" line " << __LINE__;
    // add log
  rtc::LogMessage::LogToDebug(rtc::LS_VERBOSE);
  // rtc::LogMessage::LogToDebug(rtc::LS_SENSITIVE);
  // rtc::LogMessage::LogToDebug(rtc::LS_WARNING);


}
Conductor::Conductor(JanusSignal* client)
  : peer_id_(0),
    loopback_(false),
    client_(client),
    main_wnd_(NULL) {
      //  rtc::InitializeSSL();
  client_->RegisterObserver(this);
  // if(main_wnd_)
  //   main_wnd_->RegisterObserver(this);
  LOG(INFO) << __FUNCTION__<<" line " << __LINE__;
    // add log
  rtc::LogMessage::LogToDebug(rtc::LS_VERBOSE);
    
  // rtc::LogMessage::LogToDebug(rtc::LS_SENSITIVE);
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
  LOG(INFO) << __FUNCTION__<<" line " << __LINE__;
  if (!peer_connection_factory_.get()) {
    // if(main_wnd_)
    //   main_wnd_->MessageBox("Error",
    //     "Failed to initialize PeerConnectionFactory", true);
    DeletePeerConnection();
    return false;
  }
  LOG(INFO) << __FUNCTION__<<" line " << __LINE__;
  if (!CreatePeerConnection(DTLS_ON)) {
    // if(main_wnd_)
    //   main_wnd_->MessageBox("Error",
    //     "CreatePeerConnection failed", true);
    DeletePeerConnection();
  }
  AddStreams();
  LOG(INFO)<<__FUNCTION__<<" INIT PEERCONNECTION OVER "<<__LINE__;
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
  LOG(INFO) << __FUNCTION__<<" line " << __LINE__;
  webrtc::FakeConstraints constraints;
  if (dtls) {
    constraints.AddOptional(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp,
                            "true");
  } else {
    constraints.AddOptional(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp,
                            "false");
  }
  // constraints.SetMandatoryMinWidth(720);
  // constraints.SetMandatoryMinHeight(576);
  // constraints.SetOptionalMaxWidth(720);
  // constraints.SetOptionalMaxHeight(576);
  LOG(INFO) << __FUNCTION__<<" line " << __LINE__;
  peer_connection_ = peer_connection_factory_->CreatePeerConnection(
      config, &constraints, NULL, NULL, this);
  LOG(INFO) << __FUNCTION__<<" line " << __LINE__;  

  return peer_connection_.get() != NULL;
}

void Conductor::DeletePeerConnection() {
  peer_connection_ = NULL;
  active_streams_.clear();
    // fixme
  // if(main_wnd_){
  //   main_wnd_->StopLocalRenderer();
  //   main_wnd_->StopRemoteRenderer();
  // }

  peer_connection_factory_ = NULL;
  peer_id_ = 0;
  loopback_ = false;
}

void Conductor::EnsureStreamingUI() {
  ASSERT(peer_connection_.get() != NULL);
  //fixme
  // if(main_wnd_){
  //   if (main_wnd_->IsWindow()) {
  //     if (main_wnd_->current_ui() != MainWindow::STREAMING)
  //       main_wnd_->SwitchToStreamingUI();
  //   }
  // }
}

//
// PeerConnectionObserver implementation.
//

// Called when a remote stream is added
void Conductor::OnAddStream(
    rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) {
  LOG(INFO) << __FUNCTION__ << " " << stream->label();
  // if(main_wnd_){
  //   // fixme
  //   // main_wnd_->QueueUIThreadCallback(NEW_STREAM_ADDED, stream.release());
  // }
}

void Conductor::OnRemoveStream(
    rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) {
  LOG(INFO) << __FUNCTION__ << " " << stream->label();
    // fixme
  // if(main_wnd_)
  //   main_wnd_->QueueUIThreadCallback(STREAM_REMOVED, stream.release());
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

  jmessage[kCandidateSdpMidName] = candidate->sdp_mid();
  jmessage[kCandidateSdpMlineIndexName] = candidate->sdp_mline_index();
  std::string sdp;
  if (!candidate->ToString(&sdp)) {
    LOG(LS_ERROR) << "Failed to serialize candidate";
    return;
  }
  jmessage[kCandidateSdpName] = sdp;
  // SendMessage(writer.write(jmessage));
  LOG(INFO)<< "BEGIN send trickle :"<<jmessage;
  client_->SendTrickle(jmessage);

}

//
// PeerConnectionClientObserver implementation.
//

// todo recover 
void Conductor::OnSignedIn() {
  LOG(INFO) << __FUNCTION__;
  if(main_wnd_)
    main_wnd_->SwitchToPeerList(client_->peers());
}

void Conductor::OnDisconnected() {
  LOG(INFO) << __FUNCTION__;

  DeletePeerConnection();
  // if(main_wnd_){
  //   if (main_wnd_->IsWindow())
  //     main_wnd_->SwitchToConnectUI();
  // }
}
void SimulateLastRowActivated(void* data){
  LOG(INFO) << __FUNCTION__ << "onpeerconnect peerid= ";
  Conductor* this0 = reinterpret_cast<Conductor*>(data);
  this0->ConnectToPeer(1);

}
bool Conductor::setConfig_callback(CALLBACK_CONFIG func){
  configure_callback_ = func;
  return true;
}

void Conductor::OnPeerConnected(uint64_t id, const std::string& name) {
  LOG(INFO) << __FUNCTION__ << "onpeerconnect peerid= "<<id;
  // Refresh the list if we're showing it.
  // fixme todo recover
  if(main_wnd_){
   if (main_wnd_->current_ui() == MainWindow::LIST_PEERS)
      main_wnd_->SwitchToPeerList(client_->peers());
    //       usleep(3000*1000);
    // // ConnectToPeer(id);
    // main_wnd_->ConnectCallback(id);

    //           usleep(2000*1000);
    //  ConnectToPeer(id);
  }else{
    // main_wnd_->OnRowActivated(NULL,NULL,NULL);
  //  usleep(3000*1000);

	//rtc::Thread::Current()->ProcessMessages(1);
    ConnectToPeer(id);
    // g_idle_add(SimulateLastRowActivated, this);
    // main_wnd_->ConnectCallback(id);
  }
  // todo console

    // usleep(3000*1000);
    // ConnectToPeer(id);


}

void Conductor::OnPeerDisconnected(uint64_t id) {
  LOG(INFO) << __FUNCTION__;
  if (id == peer_id_) {
    LOG(INFO) << "Our peer disconnected";
      // fixme
    // if(main_wnd_){
    //   main_wnd_->QueueUIThreadCallback(PEER_CONNECTION_CLOSED, NULL);
    // }
  } else {
    // Refresh the list if we're showing it.
    // if(main_wnd_){
    //   if (main_wnd_->current_ui() == MainWindow::LIST_PEERS)
    //     main_wnd_->SwitchToPeerList(client_->peers());
    // }
  }
}

void Conductor::OnMessageFromPeer(uint64_t peer_id, const std::string& message) {
  ASSERT(peer_id_ == peer_id || peer_id_ == 0);
  ASSERT(!message.empty());
  LOG(INFO) << __FUNCTION__<<" line " << __LINE__;
  LOG(INFO)<< "OnMessageFromPeer : "<< message;
  if (!peer_connection_.get()) {
    // ASSERT(peer_id_ == -1);
    peer_id_ = peer_id;
    LOG(INFO)<< "begin init peerconnection" << "message"<< message;
    if (!InitializePeerConnection()) {
      LOG(LS_ERROR) << "Failed to initialize our PeerConnection instance";
      client_->SignOut();
      return;
    }
  } else if (peer_id != peer_id_) {
    ASSERT(peer_id_ != 0);
    LOG(WARNING) << "Received a message from unknown peer while already in a "
                    "conversation with a different peer.";
    return;
  }
  LOG(INFO) << __FUNCTION__<<" line " << __LINE__;
  Json::Reader reader;
  Json::Value jmessage;
  if (!reader.parse(message, jmessage)) {
    LOG(WARNING) << "Received unknown message. " << message;
    return;
  }
  std::string type;
  std::string json_object;
  LOG(INFO)<< jmessage;
  rtc::GetStringFromJsonObject(jmessage, kSessionDescriptionTypeName, &type);
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
    if (!rtc::GetStringFromJsonObject(jmessage, kSessionDescriptionSdpName,
                                      &sdp)) {
      LOG(WARNING) << "Can't parse received session description message.";
      return;
    }
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
    // fixme
  // if(main_wnd_)
  //   main_wnd_->QueueUIThreadCallback(SEND_MESSAGE_TO_PEER, NULL);
}

void Conductor::OnServerConnectionFailure() {
    // if(main_wnd_)
    //   main_wnd_->MessageBox("Error", ("Failed to connect to " + server_).c_str(),
    //                       true);
}

//
// MainWndCallback implementation.
//

void Conductor::StartLogin(const std::string& server, int port) {
  if (client_->is_connected())
    return;
  server_ = server;
  client_->Connect(server, port, GetPeerName());
}

void Conductor::DisconnectFromServer() {
  if (client_->is_connected())
    client_->SignOut();
}

void Conductor::ConnectToPeer(uint64_t peer_id) {
    LOG(INFO) << __FUNCTION__<<" line " << __LINE__<<" id= "<<peer_id;
  // begin create offer and connect to peer
  ASSERT(peer_id_ == 0);
  ASSERT(peer_id != 0);
  LOG(INFO) << __FUNCTION__<<" line " << __LINE__;
  if (peer_connection_.get()) {
    // if(main_wnd_)
    //   main_wnd_->MessageBox("Error",
    //     "We only support connecting to one peer at a time", true);
    return;
  }
  LOG(INFO)<<  "BEGIN connect peer : ConnectToPeer "<< peer_id;
  if (InitializePeerConnection()) {
    peer_id_ = peer_id;
    LOG(INFO)<<__FUNCTION__<< " BEGIN CREATEOFFER "<<__LINE__;
    peer_connection_->CreateOffer(this, NULL);
    if(NULL == main_wnd_)
      rtc::Thread::Current()->ProcessMessages(1000);
    LOG(INFO)<<__FUNCTION__<<" CREATEOFFER OVER "<<__LINE__;
  } else {
    // if(main_wnd_)
    //   main_wnd_->MessageBox("Error", "Failed to initialize PeerConnection", true);
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
  LOG(INFO)<<__FUNCTION__<<" CAPTURE DEVICE NUM: "<< num_devices;	
    for (int i = 0; i < num_devices; ++i) {
      const uint32_t kSize = 256;
      char name[kSize] = {0};
      char id[kSize] = {0};
      if (info->GetDeviceName(i, name, kSize, id, kSize) != -1) {
		          LOG(INFO)<<__FUNCTION__<<" CAPTURE DEVICE name: "<< name;
        device_names.push_back(name);
      }
    }
  }
  LOG(INFO)<<__FUNCTION__<<" CAPTURE DEVICE "<<__LINE__;
  cricket::WebRtcVideoDeviceCapturerFactory factory;
  cricket::VideoCapturer* capturer = nullptr;
  for (const auto& name : device_names) {
    capturer = factory.Create(cricket::Device(name, 0));
	LOG(INFO)<<__FUNCTION__<<" CAPTURE DEVICE name: "<< name;
    if (capturer) {
     // break;
    }
  }
  return capturer;
}

void Conductor::AddStreams() {
  LOG(INFO)<<__FUNCTION__<<" ADD STREAM "<<__LINE__;
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
  // fixme
  // if(main_wnd_)
  //   main_wnd_->StartLocalRenderer(video_track);

  rtc::scoped_refptr<webrtc::MediaStreamInterface> stream =
      peer_connection_factory_->CreateLocalMediaStream(kStreamLabel);

  stream->AddTrack(audio_track);
  stream->AddTrack(video_track);
  if (!peer_connection_->AddStream(stream)) {
    LOG(LS_ERROR) << "Adding stream to PeerConnection failed";
  }
  LOG(INFO)<<__FUNCTION__<<" INSERT TO PAIR "<<__LINE__;
  typedef std::pair<std::string,
                    rtc::scoped_refptr<webrtc::MediaStreamInterface> >
      MediaStreamPair;
  active_streams_.insert(MediaStreamPair(stream->label(), stream));
  // fixme
  // if(main_wnd_)
  //   main_wnd_->SwitchToStreamingUI();
}

void Conductor::DisconnectFromCurrentPeer() {
  LOG(INFO) << __FUNCTION__;
  if (peer_connection_.get()) {
    client_->SendHangUp(peer_id_);
    DeletePeerConnection();
  }
  // if(main_wnd_){
  //   if (main_wnd_->IsWindow())
  //     main_wnd_->SwitchToPeerList(client_->peers());
  // }
}

void Conductor::UIThreadCallback(int msg_id, void* data) {
  LOG(INFO)<<__FUNCTION__<<" CALLBACK MSGID: "<<msg_id;
  // switch (msg_id) {
  //   case PEER_CONNECTION_CLOSED:
  //     LOG(INFO) << "PEER_CONNECTION_CLOSED";
  //     DeletePeerConnection();

  //     ASSERT(active_streams_.empty());
  //     if(main_wnd_){
  //       if (main_wnd_->IsWindow()) {
  //         if (client_->is_connected()) {
  //           main_wnd_->SwitchToPeerList(client_->peers());
  //         } else {
  //           main_wnd_->SwitchToConnectUI();
  //         }
  //       } else {
  //         DisconnectFromServer();
  //       }
  //     }
  //     break;

  //   case SEND_MESSAGE_TO_PEER: {
  //     LOG(INFO) << "SEND_MESSAGE_TO_PEER";
  //     std::string* msg = reinterpret_cast<std::string*>(data);
  //     if (msg) {
  //       // For convenience, we always run the message through the queue.
  //       // This way we can be sure that messages are sent to the server
  //       // in the same order they were signaled without much hassle.
  //       pending_messages_.push_back(msg);
  //     }

  //     if (!pending_messages_.empty() && !client_->IsSendingMessage()) {
  //       msg = pending_messages_.front();
  //       pending_messages_.pop_front();

  //       if (!client_->SendToPeer(peer_id_, *msg) && peer_id_ != 0) {
  //         LOG(LS_ERROR) << "SendToPeer failed";
  //         DisconnectFromServer();
  //       }
  //       delete msg;
  //     }

  //     if (!peer_connection_.get())
  //       peer_id_ = 0;

  //     break;
  //   }

  //   case NEW_STREAM_ADDED: {
  //     webrtc::MediaStreamInterface* stream =
  //         reinterpret_cast<webrtc::MediaStreamInterface*>(
  //         data);
  //     webrtc::VideoTrackVector tracks = stream->GetVideoTracks();
  //     // Only render the first track.
  //     if (!tracks.empty()) {
  //       webrtc::VideoTrackInterface* track = tracks[0];
  //       if(main_wnd_)
  //         main_wnd_->StartRemoteRenderer(track);
  //     }
  //     stream->Release();
  //     break;
  //   }

  //   case STREAM_REMOVED: {
  //     // Remote peer stopped sending a stream.
  //     webrtc::MediaStreamInterface* stream =
  //         reinterpret_cast<webrtc::MediaStreamInterface*>(
  //         data);
  //     stream->Release();
  //     break;
  //   }

  //   default:
  //     RTC_NOTREACHED();
  //     break;
  // }
}

void Conductor::OnSuccess(webrtc::SessionDescriptionInterface* desc) {
  LOG(INFO)<<__FUNCTION__<<" BEGIN SET LOCALDESCRIPTION "<<__LINE__; 
  peer_connection_->SetLocalDescription(
      DummySetSessionDescriptionObserver::Create(), desc);
  LOG(INFO)<<__FUNCTION__<<" SET LOCALDESCRIPTION SUCCESS "<<__LINE__;
  std::string sdp;
  desc->ToString(&sdp);
  LOG(INFO)<<__FUNCTION__<<" line "<<__LINE__;
  LOG(INFO)<<"recv : "<< sdp;
  // set sdp to server
  client_->ConfigureSDP2Janus(peer_id_,desc->type(),sdp);

  // For loopback test. To save some connecting delay.
  // if (loopback_) {
  //   // Replace message type from "offer" to "answer"
  //   webrtc::SessionDescriptionInterface* session_description(
  //       webrtc::CreateSessionDescription("answer", sdp, nullptr));
  //   peer_connection_->SetRemoteDescription(
  //       DummySetSessionDescriptionObserver::Create(), session_description);
  //   return;
  // }

  // Json::StyledWriter writer;
  // Json::Value jmessage;
  // jmessage[kSessionDescriptionTypeName] = desc->type();
  // jmessage[kSessionDescriptionSdpName] = sdp;
  // SendMessage(writer.write(jmessage));
}

void Conductor::OnFailure(const std::string& error) {
  LOG(INFO)<<__FUNCTION__<< " FAILED CREATE DESC "<<error;
    LOG(LERROR) << error;
}

void Conductor::SendMessage(const std::string& json_object) {
  // std::string* msg = new std::string(json_object);
  // main_wnd_->QueueUIThreadCallback(SEND_MESSAGE_TO_PEER, msg);
  client_->SendMessage(json_object);

}
