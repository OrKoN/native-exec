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
  if (info.Length() < 3) {
    return;
  }
  if (!info[0]->IsString()) {
    return;
  }

  v8::String::Utf8Value val(info[0]->ToString());
  std::string str (*val);
  char *command = const_cast<char*> ( str.c_str() );

  v8::Local<v8::Array> envArr = v8::Local<v8::Array>::Cast(info[1]);
  unsigned int length = envArr->Length();
  char* env[length + 1];

  for (unsigned int i = 0; i < length; i++) {
    v8::String::Utf8Value arrayElem(Nan::Get(envArr, i).ToLocalChecked()->ToString());
    std::string arrayElemStr (*arrayElem);
    char* tmp = new char[arrayElemStr.length() +1];
    strcpy(tmp, arrayElemStr.c_str());  
    env[i] = tmp;
  }

  env[length] = NULL;

  v8::Local<v8::Array> argsArr = v8::Local<v8::Array>::Cast(info[2]);
  length = argsArr->Length();
  char* args[length + 2];

  args[0] = command;

  for (unsigned int i = 0; i < length; i++) {
    v8::String::Utf8Value arrayElem(Nan::Get(argsArr, i).ToLocalChecked()->ToString());
    std::string arrayElemStr (*arrayElem);
    char* tmp = new char[arrayElemStr.length() +1];
    strcpy(tmp, arrayElemStr.c_str());  
    args[i + 1] = tmp;
  }

  args[length + 1] = NULL;

  doNotCloseStreamsOnExit(0); //stdin
  doNotCloseStreamsOnExit(1); //stdout
  doNotCloseStreamsOnExit(2); //stderr

  execvpe(command, args, env);
}

void Init(v8::Local<v8::Object> exports) {
  exports->Set(Nan::New("exec").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(Method)->GetFunction());
}

NODE_MODULE(exec, Init)
