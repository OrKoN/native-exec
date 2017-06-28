#include <nan.h>
#include <fcntl.h>
#include <unistd.h>

int doNotCloseStreamsOnExit(int desc) {
  int flags = fcntl(desc, F_GETFD, 0);
  if (flags < 0) return flags;
  flags &= ~FD_CLOEXEC; //clear FD_CLOEXEC bit
  return fcntl(desc, F_SETFD, flags);
}

void copyArray(char* dest[], unsigned int offset, v8::Local<v8::Array> src) {
  unsigned int length = src->Length();
  for (unsigned int i = 0; i < length; i++) {
    v8::String::Utf8Value arrayElem(Nan::Get(src, i).ToLocalChecked()->ToString());
    std::string arrayElemStr (*arrayElem);
    char* tmp = new char[arrayElemStr.length() +1];
    strcpy(tmp, arrayElemStr.c_str());  
    dest[i + offset] = tmp;
  }
}

void Method(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 3) {
    return;
  }
  if (!info[0]->IsString()) {
    return;
  }

  // get command
  v8::String::Utf8Value val(info[0]->ToString());
  std::string str (*val);
  char *command = const_cast<char*> ( str.c_str() );

  // build env,:...env, NULL
  v8::Local<v8::Array> envArr = v8::Local<v8::Array>::Cast(info[1]);
  char* env[envArr->Length() + 1];
  copyArray(env, 0, envArr);
  env[envArr->Length()] = NULL;

  // build args: command, ...args, NULL
  v8::Local<v8::Array> argsArr = v8::Local<v8::Array>::Cast(info[2]);
  char* args[argsArr->Length() + 2];
  args[0] = command;
  copyArray(args, 1, argsArr);
  args[argsArr->Length() + 1] = NULL;

  // fix stream flags
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
