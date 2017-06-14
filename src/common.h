#ifndef __COMMON_H_H_
#define __COMMON_H_H_

namespace uprtc{


const int PUBLISHER = 1;
const int LISTENER = 2;


const std::string CREATESESSION = "create";
const std::string CREATEHANDLE = "attach";
const std::string REGISTER = "register";
const std::string JOIN = "join";
const std::string CONFIGURE = "configure";
const std::string TRICKLE = "trickle";

const std::string SUCCESS_SESSION  = "success";
const std::string ERROR_SESSION = "error";
const std::string ERROR_CREATEHANDLE = "errorHandle";
const std::string SUCCESS_HANDLE  = "handleSuccess";
const std::string MESSAGE = "message";
const std::string WEBRTCSTATE = "webrtcState";
const std::string MEDIASTATE = "mediaState";
const std::string SLOWLINK = "slowLink";
const std::string DETACHED = "detached";

}

#endif