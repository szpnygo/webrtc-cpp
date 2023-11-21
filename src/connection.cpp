#include "connection.h"
#include "rtc_mediaconstraints.h"
#include "signal.h"
#include "spdlog/spdlog.h"
#include <functional>

namespace webrtc {

Connection::Connection(const std::string clientName,
                       std::shared_ptr<ISignalServer> signalServer,
                       scoped_refptr<RTCPeerConnection> pc)
    : _clientName(clientName), _signalServer(signalServer), _pc(pc) {
  _pc->RegisterRTCPeerConnectionObserver(this);
};

void Connection::start() { createOffer(); };

void Connection::setAnswer(const std::string type, const std::string sdp) {
  _pc->SetRemoteDescription(
      sdp, type, [this]() {},
      [this](const char *error) {
        spdlog::error("SetRemoteDescription error {} {}", error, _clientName);
      });
};

void Connection::addCandidate(const std::string candidate,
                              const std::string mid, const int sdpMLineIndex) {
  _pc->AddCandidate(mid, sdpMLineIndex, candidate);
};

void Connection::createOffer() {
  auto constraints = RTCMediaConstraints::Create();
  constraints->AddMandatoryConstraint("OfferToReceiveAudio", "false");
  constraints->AddMandatoryConstraint("OfferToReceiveVideo", "false");

  _pc->CreateOffer(
      [this](const string sdp, const string type) {
        onCreateOfferSuccess(sdp, type);
      },
      [this](const char *error) {
        spdlog::error("CreateOffer error {} {}", error, _clientName);
      },
      constraints);
};

void Connection::onCreateOfferSuccess(const string sdp, const string type) {
  // set sender parameters

  _pc->SetLocalDescription(
      sdp, type,
      [this, sdp]() {
        _signalServer->sendOffer(_clientName, "offer", sdp.std_string());
      },
      [this](const char *error) {
        spdlog::error("SetLocalDescription error {} {}", error, _clientName);
      });
}

} // namespace webrtc
