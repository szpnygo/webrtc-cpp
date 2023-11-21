#pragma once
#include "base/scoped_ref_ptr.h"
#include "rtc_peerconnection.h"
#include "signal.h"
#include "spdlog/spdlog.h"
#include "webrtc_app.h"
#include <functional>

namespace webrtc {
using namespace libwebrtc;

typedef std::function<void()> OnConnected;
typedef std::function<void()> OnDisconnected;

class Connection : public RTCPeerConnectionObserver {
public:
  Connection(const std::string clientName,
             std::shared_ptr<ISignalServer> signalServer,
             scoped_refptr<RTCPeerConnection> pc);

  void setAnswer(const std::string type, const std::string sdp);

  void addCandidate(const std::string candidate, const std::string mid,
                    const int sdpMLineIndex);

  RTCPeerConnectionState getState() { return _state; }

  // start the connection, create the peer connection and send the offer
  // to the signal server
  void start();

  bool isConnected() {
    return _state == libwebrtc::RTCPeerConnectionStateConnected;
  }

  virtual void OnPeerConnectionState(RTCPeerConnectionState state) override {
    _state = state;
    if (state == RTCPeerConnectionStateConnected) {
      spdlog::info("Connection established {}", _clientName);
      if (_onConnected) {
        _onConnected();
      }
    } else if (state == RTCPeerConnectionStateDisconnected) {
      spdlog::info("Connection disconnected {}", _clientName);
      close();
    } else if (state == RTCPeerConnectionStateFailed) {
      spdlog::info("Connection failed {}", _clientName);
      close();
    } else if (state == RTCPeerConnectionStateClosed) {
      spdlog::info("Connection closed {}", _clientName);
      close();
    } else if (state == RTCPeerConnectionStateNew) {
      spdlog::info("Connection new {}", _clientName);
    } else if (state == RTCPeerConnectionStateConnecting) {
      spdlog::info("Connection connecting {}", _clientName);
    }
  };

  void close() {
    _pc->DeRegisterRTCPeerConnectionObserver();
    _pc->Close();
    if (_onDisconnected) {
      _onDisconnected();
    }
  }

  virtual void
  OnIceCandidate(scoped_refptr<RTCIceCandidate> candidate) override {
    _signalServer->sendCandidate(_clientName,
                                 candidate.get()->candidate().std_string(),
                                 candidate.get()->sdp_mid().std_string(),
                                 candidate.get()->sdp_mline_index());
  };

  virtual void OnIceConnectionState(RTCIceConnectionState state) override{};

  virtual void OnSignalingState(RTCSignalingState state) override{};

  virtual void OnIceGatheringState(RTCIceGatheringState state) override{};

  virtual void OnAddStream(scoped_refptr<RTCMediaStream> stream) override{};

  virtual void OnRemoveStream(scoped_refptr<RTCMediaStream> stream) override{};

  virtual void
  OnDataChannel(scoped_refptr<RTCDataChannel> data_channel) override{};

  virtual void OnRenegotiationNeeded() override{};

  virtual void OnTrack(scoped_refptr<RTCRtpTransceiver> transceiver) override{};

  virtual void OnAddTrack(vector<scoped_refptr<RTCMediaStream>> streams,
                          scoped_refptr<RTCRtpReceiver> receiver) override{};

  virtual void OnRemoveTrack(scoped_refptr<RTCRtpReceiver> receiver) override{};

  void setOnConnected(OnConnected onConnected) { _onConnected = onConnected; }

  void setOnDisconnected(OnDisconnected onDisconnected) {
    _onDisconnected = onDisconnected;
  }

  scoped_refptr<RTCPeerConnection> getPeerConnection() { return _pc; }

private:
  scoped_refptr<RTCPeerConnection> _pc;
  scoped_refptr<RTCPeerConnectionFactory> _peerConnectionFactory;
  // the name of the client who is connected to the server
  std::string _clientName = "";
  // the state of the connection
  RTCPeerConnectionState _state;
  // the signal server
  std::shared_ptr<ISignalServer> _signalServer;

  OnConnected _onConnected;
  OnDisconnected _onDisconnected;

  // create the offer
  void createOffer();
  // create the offer success callback
  void onCreateOfferSuccess(const string sdp, const string type);
};

} // namespace webrtc