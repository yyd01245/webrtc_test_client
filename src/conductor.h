/*
 *  Copyright 2012 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_EXAMPLES_PEERCONNECTION_CLIENT_CONDUCTOR_H_
#define WEBRTC_EXAMPLES_PEERCONNECTION_CLIENT_CONDUCTOR_H_
#pragma once

#include <deque>
#include <map>
#include <set>
#include <string>
#include <random>
#include "webrtc/api/mediastreaminterface.h"
#include "webrtc/api/peerconnectioninterface.h"
#include "webrtc/examples/peerconnection/client/main_wnd.h"
#include "webrtc/examples/peerconnection/client/peer_connection_client.h"
#include "webrtc/base/httpserver.h"
#include "webrtc/base/httpclient.h"
#include "webrtc/base/httprequest.h"

namespace webrtc {
class VideoCaptureModule;
}  // namespace webrtc

namespace cricket {
class VideoRenderer;
}  // namespace cricket
class Conductor;
class LongPullThread : public rtc::Thread
{
 public:
  explicit LongPullThread(int maxev, unsigned long sessionid, Conductor * conductor);
    
  virtual ~LongPullThread() ;
 
  virtual void Run() ;
  
 private:
  int maxev_;
  unsigned long  sessionid_;
  bool quit_;
  std::unique_ptr<rtc::HttpRequest> HttpRequest;
  std::string send_data_;
  Conductor* conductor_;
};
class JanusClientPlugin
:public sigslot::has_slots<>
{
public:
	JanusClientPlugin();
	~JanusClientPlugin();
	void RequestCreate(Conductor* conductor);
	void RequestAttach();
	void RequestDetaach();
	void CreateOffer();
	void CreateAnswer();
	void SendSDP();
	void StreamsDone();
	void PrepareWebrtcPeer();
	void ConnectJanus(const std::string& , int ,const std::string&);
	unsigned long  pluginid_;
	unsigned long  sessionid_;
protected:
 	//std::string _randomString(int len = 12);//std::random_device rd
	void _eventHandler();
	void _keepAlive();
	void _createSession();
	void _destroySession();
	void _createHandle();
	void _destroyHandle();
	void _handleEvent();
	void SendMessage();
	void sendTrickleCandidate();
	void SendMessageToJanus();
	void OnRead(/* rtc::AsyncSocket* socket */);
	void OnConnect(rtc::AsyncSocket* socket);
	void OnClose(rtc::AsyncSocket* socket, int err);
private:
	std::vector<void*> _plugin_handle;
	std::string transaction_;

	int retries_;/*  for long pull */

	/* socket */
	rtc::SocketAddress server_address_;
	std::unique_ptr<rtc::AsyncSocket> socket2janus_;

	std::string send_data_;

	std::unique_ptr<rtc::HttpServer> MyHttpServer;
	std::unique_ptr<rtc::HttpClient> MyHttpClient;
	std::unique_ptr<rtc::StringStream> stream__;
	std::unique_ptr<rtc::HttpRequest> MyHttpRequest;
	std::unique_ptr<LongPullThread> longpull_;
	int maxev_;
};

class VideoCallPlugin
:public JanusClientPlugin
{
public:
	VideoCallPlugin();
	~VideoCallPlugin();
	/* videocall send message */
	void SendRegisterMessage();
	void SendGetUserListMessage();
	void SendDoCallMessage();
	void SendAcceptMssage();
	void SendSetMessage();
	void DoHangupMessage();
	void SendDataMessage(); /* for datachannel */
	std::string myname_;
protected:


};

class Conductor
	: public webrtc::PeerConnectionObserver,
		public webrtc::CreateSessionDescriptionObserver,
		public PeerConnectionClientObserver,
		public MainWndCallback {
 public:
	enum CallbackID {
		MEDIA_CHANNELS_INITIALIZED = 1,
		PEER_CONNECTION_CLOSED,
		SEND_MESSAGE_TO_PEER,
		NEW_STREAM_ADDED,
		STREAM_REMOVED,
	};

	Conductor(PeerConnectionClient* client, MainWindow* main_wnd);

	bool connection_active() const;

	virtual void Close();
	virtual void OnMessageFromPeer(int peer_id, const std::string& message);
 protected:
	~Conductor();
	bool InitializePeerConnection();
	bool ReinitializePeerConnectionForLoopback();
	bool CreatePeerConnection(bool dtls);
	void DeletePeerConnection();
	void EnsureStreamingUI();
	void AddStreams();
	cricket::VideoCapturer* OpenVideoCaptureDevice();

	//
	// PeerConnectionObserver implementation.
	//

	void OnSignalingChange(
			webrtc::PeerConnectionInterface::SignalingState new_state) override{};
	void OnAddStream(
			rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override;
	void OnRemoveStream(
			rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override;
	void OnDataChannel(
			rtc::scoped_refptr<webrtc::DataChannelInterface> channel) override {}
	void OnRenegotiationNeeded() override {}
	void OnIceConnectionChange(
			webrtc::PeerConnectionInterface::IceConnectionState new_state) override{};
	void OnIceGatheringChange(
			webrtc::PeerConnectionInterface::IceGatheringState new_state) override{};
	void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) override;
	void OnIceConnectionReceivingChange(bool receiving) override {}

	//
	// PeerConnectionClientObserver implementation.
	//

	virtual void OnSignedIn();

	virtual void OnDisconnected();

	virtual void OnPeerConnected(int id, const std::string& name);

	virtual void OnPeerDisconnected(int id);

	virtual void OnMessageSent(int err);

	virtual void OnServerConnectionFailure();

	//
	// MainWndCallback implementation.
	//

	virtual void StartLogin(const std::string& server, int port);

	virtual void DisconnectFromServer();

	virtual void ConnectToPeer(int peer_id);

	virtual void DisconnectFromCurrentPeer();

	virtual void UIThreadCallback(int msg_id, void* data);

	// CreateSessionDescriptionObserver implementation.
	virtual void OnSuccess(webrtc::SessionDescriptionInterface* desc);
	virtual void OnFailure(const std::string& error);

 protected:
	// Send a message to the remote peer.
	void SendMessage(const std::string& json_object);

	int peer_id_;
	bool loopback_;
	rtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection_;
	rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface>
			peer_connection_factory_;
	PeerConnectionClient* client_;
	MainWindow* main_wnd_;
	std::deque<std::string*> pending_messages_;
	std::map<std::string, rtc::scoped_refptr<webrtc::MediaStreamInterface> >
			active_streams_;
	std::string server_;
	std::unique_ptr<VideoCallPlugin> videoall_plugin_;
};

#endif  // WEBRTC_EXAMPLES_PEERCONNECTION_CLIENT_CONDUCTOR_H_
