#include <map>
#include <memory>
#include <string>

#include <fcntl.h>
#include <unistd.h>

#include <node.h>

typedef std::map<std::string, std::unique_ptr<std::string>> Environment;

int doNotCloseStreamsOnExit(int desc) {
  int flags = fcntl(desc, F_GETFD, 0);
  if (flags < 0) return flags;
  flags &= ~FD_CLOEXEC; //clear FD_CLOEXEC bit
  return fcntl(desc, F_SETFD, flags);
}

void copyArray(char* dest[], unsigned int offset, v8::Local<v8::Array> src, v8::Isolate* isolate) {
  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  unsigned int length = src->Length();
  for (unsigned int i = 0; i < length; i++) {
    v8::Local<v8::Value> arrayElem = src->Get(context, i).ToLocalChecked();
    v8::String::Utf8Value utf8Value(isolate, arrayElem);
    std::string arrayElemStr(*utf8Value);
    char* tmp = new char[arrayElemStr.length() + 1];
    strcpy(tmp, arrayElemStr.c_str());
    dest[i + offset] = tmp;
  }
}

Environment updateEnvironment(v8::Local<v8::Array> src, v8::Isolate* isolate) {
  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  Environment envBackup;

  unsigned int length = src->Length();
  v8::Local<v8::String> keyProp = v8::String::NewFromUtf8(isolate, "key").ToLocalChecked();
  v8::Local<v8::String> valueProp = v8::String::NewFromUtf8(isolate, "value").ToLocalChecked();
  for (unsigned int i = 0; i < length; i++) {
    v8::Local<v8::Object> obj = src->Get(context, i).ToLocalChecked()->ToObject(context).ToLocalChecked();

    v8::String::Utf8Value objKey(isolate, obj->Get(context, keyProp).ToLocalChecked());
    v8::String::Utf8Value objValue(isolate, obj->Get(context, valueProp).ToLocalChecked());

    std::string objKeyStr (*objKey);
    char *key = const_cast<char*> ( objKeyStr.c_str() );
    std::string objValueStr (*objValue);
    char *value = const_cast<char*> ( objValueStr.c_str() );

    char *currentValue = getenv(key);
    if (currentValue == nullptr) {
      envBackup[objKeyStr] = nullptr;
    } else {
      envBackup[objKeyStr] = std::unique_ptr<std::string>(new std::string(currentValue));
    }
    setenv(key, value, 1);
  }

  return envBackup;
}

void Method(const v8::FunctionCallbackInfo<v8::Value>& info) {
  v8::Isolate* isolate = info.GetIsolate();
  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  if (info.Length() < 3) {
    return;
  }
  if (!info[0]->IsString()) {
    return;
  }

  // get command
  v8::String::Utf8Value val(isolate, info[0]->ToString(context).ToLocalChecked());
  std::string str (*val);
  char *command = const_cast<char*> ( str.c_str() );

  // set env on the current process
  v8::Local<v8::Array> envArr = v8::Local<v8::Array>::Cast(info[1]);
  Environment envBackup = updateEnvironment(envArr, isolate);

  // build args: command, ...args, NULL
  v8::Local<v8::Array> argsArr = v8::Local<v8::Array>::Cast(info[2]);
  char* args[argsArr->Length() + 2];
  args[0] = command;
  copyArray(args, 1, argsArr, isolate);
  args[argsArr->Length() + 1] = NULL;

  // fix stream flags
  doNotCloseStreamsOnExit(0); //stdin
  doNotCloseStreamsOnExit(1); //stdout
  doNotCloseStreamsOnExit(2); //stderr

  if(execvp(command, args) < 0) {
    int savedErrno = errno;

    // Restore environment if execution fails
    for (auto&& entry : envBackup) {
      const char *key = entry.first.c_str();
      std::unique_ptr<std::string> value(entry.second.release());

      if (value) {
        setenv(key, value->c_str(), 1);
      } else {
        unsetenv(key);
      }
    }

    isolate->ThrowException(node::ErrnoException(isolate, savedErrno, "execvp", "native-exec failed", nullptr));
  }
}

void Init(v8::Local<v8::Object> exports) {
  NODE_SET_METHOD(exports, "exec", Method);
}

NODE_MODULE(exec, Init)
