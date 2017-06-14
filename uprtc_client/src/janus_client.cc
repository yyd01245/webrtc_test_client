
#include <string>
#include <iostream>

#include "./janus_signal.h"

#include "webrtc/base/buffer.h"
#include "webrtc/base/helpers.h"
#include "webrtc/base/ssladapter.h"
#include "webrtc/base/ssladapter.h"
#include "webrtc/base/thread.h"
#include "webrtc/base/httpclient.h"




int main(){

  std::cout << "hello janus client demo "<<std::endl;
  std::unique_ptr<uprtc::JanusSignal> janusSignal;
  janusSignal.reset(new uprtc::JanusSignal("10.0.3.115",8088));
  std::cout<< "create signal instance "<<std::endl;
  janusSignal->Initialize();
  std::cout<< "init signal instance "<<std::endl;
  janusSignal->CreateSession();
  std::cout<< "create session "<<std::endl;
 // janusSignal->Start();
  while(true){
    usleep(100);
 //   janusSignal->Start();
  }
  return 0;
}