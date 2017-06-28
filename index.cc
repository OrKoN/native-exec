#include <nan.h>
#include <fcntl.h>
#include <unistd.h>

int doNotCloseStreamsOnExit(int desc) {
  int flags = fcntl(desc, F_GETFD, 0);
  if (flags < 0) return flags;
  flags &= ~FD_CLOEXEC; //clear FD_CLOEXEC bit
  return fcntl(desc, F_SETFD, flags);
}

void Method(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 1) {
    return;
  }

  if (!info[0]->IsString()) {
    return;
  }

  v8::String::Utf8Value val(info[0]->ToString());

  std::string str (*val);

  doNotCloseStreamsOnExit(0); //stdin
  doNotCloseStreamsOnExit(1); //stdout
  doNotCloseStreamsOnExit(2); //stderr
  execlp(str.c_str(), str.c_str(), NULL);
}

void Init(v8::Local<v8::Object> exports) {
  exports->Set(Nan::New("exec").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(Method)->GetFunction());
}

NODE_MODULE(exec, Init)
