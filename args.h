#pragma once

#include <string>
#include <unordered_map>

struct Args {
  std::unordered_map<std::string, std::string> args;

  bool active_arg = false;

  Args(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
      if (argv[i][0] == '-') {
        if (active_arg) {
          args[argv[i - 1]] = "";
        }

        active_arg = true;
      } else {
        if (active_arg) {
          args[argv[i - 1]] = argv[i];
          active_arg = false;
        } else {
          // error
        }
      }
    }

    if (active_arg) {
      args[argv[argc - 1]] = "";
    }
  }

  bool getBool(const std::string &name) {
    return args.find(name) != args.end();
  }

  int getInt(const std::string &name, int defaultValue) {
    const auto &elem = args.find(name);
    if (elem == args.end())
      return defaultValue;
    else
      return std::stoi(elem->second);
  }

  double getDouble(const std::string &name, double defaultValue) {
    const auto &elem = args.find(name);
    if (elem == args.end())
      return defaultValue;
    else
      return std::stod(elem->second);
  }

  std::string getString(const std::string &name,
                        const std::string &defaultValue) {
    const auto &elem = args.find(name);
    if (elem == args.end())
      return std::string(defaultValue);
    else
      return elem->second;
  }
};