#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

class utUtils {
public:
  void get_dir_exec(char *dir, char *exec) {
#ifdef _WIN32
    char BufferFileName[MAX_PATH];
    memset(BufferFileName, 0, MAX_PATH);
    if (GetModuleFileName(NULL, BufferFileName, MAX_PATH)) {
      if (exec) {
        snprintf(exec, strlen(BufferFileName), "%s", BufferFileName);
      }

      std::string ss(BufferFileName);
      int idx = ss.find_last_of("\\");
      BufferFileName[idx] = '\0';
      snprintf(dir, strlen(BufferFileName), "%s", BufferFileName);
    }
    return;
#else
    char *filename = nullptr;
    if (readlink("/proc/self/exe", dir, 1024) < 0) {
      dir[0] = '\0';
      return;
    }
    filename = strrchr(dir, '/');
    if (filename == nullptr) {
      dir[0] = '\0';
      return;
    }
    ++filename;
    if (exec) {
      sprintf(exec, "%s", filename);
    }

    *filename = '\0';
    return;
#endif
  }

  std::string get_env(const std::string env) {

#ifdef _WIN32
    char *buf = nullptr;
    size_t sz = 0;
    if (_dupenv_s(&buf, &sz, env.c_str()) == 0 && buf != nullptr) {
      std::string var(buf);
      free(buf);
      return var;
    } else {
      return std::string();
    }

#else
    char *value = getenv(env.c_str());
    if (value == nullptr) {
      return std::string();
    }
    return std::string(value);
#endif
  }
};
