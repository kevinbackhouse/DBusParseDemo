// Copyright 2021 Kevin Backhouse.
//
// This file is part of DBusParseDemo.
//
// DBusParseDemo is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// DBusParseDemo is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with DBusParseDemo.  If not, see <https://www.gnu.org/licenses/>.


#include <string.h>
#include <unistd.h>
#include <dbus_utils.hpp>
#include <EPollLoopDBusHandler.hpp>

class IntrospectHandler : public DBusHandler {
  const std::string destination_;
  const std::string objectpath_;

public:
  IntrospectHandler(
    const char* socketpath, const char* destination, const char* objectpath
  ) :
    DBusHandler(socketpath),
    destination_(destination),
    objectpath_(objectpath)
  {}

  void accept() override final {
    send_hello(
      [this](const std::string& busname) -> int {
        fprintf(stdout, "Unique bus name: %s\n", busname.c_str());
        fflush(stdout);

        send_call(
          DBusMessageBody::mk0(),
          _s(objectpath_),
          _s("org.freedesktop.DBus.Introspectable"),
          _s(destination_),
          _s("Introspect"),
          [this](const DBusMessage& message, bool) -> int {
            print_dbus_message(STDOUT_FILENO, message);
            return -1;
          }
        );

        return 0;
      }
    );
  }

  // We don't expect to receive any calls.
  void receive_call(const DBusMessage&) override final {
    logerror("Unexpected incoming call.");
  }

  // We don't expect to receive any calls.
  void receive_signal(const DBusMessage&) override final {
    logerror("Received a signal.");
  }

  void receive_error(const DBusMessage&) override final {
    logerror("Received an error.");
  }

  void disconnect() noexcept override final {}

  void logerror(const char* errmsg) noexcept override final {
    fprintf(stderr, "%s\n", errmsg);
  }
};

void run(
  const uid_t uid,
  const char* socketpath,
  const char* destination,
  const char* objectpath
) {
  EPollLoop loop;
  DBusAuthHandler* handler =
    new DBusAuthHandler(
      loop, uid,
      new IntrospectHandler(socketpath, destination, objectpath)
    );
  if (loop.add_handler(handler) < 0) {
    throw Error(_s("Failed to add IntrospectHandler"));
  }

  loop.run();
}

int main(int argc, char* argv[]) {
  const char* progname = argc > 0 ? argv[0] : "a.out";
  if (argc < 4) {
    fprintf(
      stderr,
      "usage:   %s <unix socket path> <destination> <object path>\n"
      "example: %s /var/run/dbus/system_bus_socket org.freedesktop.PolicyKit1 /org/freedesktop/PolicyKit1/Authority\n",
      progname,
      progname
    );
    return 1;
  }

  uid_t uid = getuid();
  const char* socketpath = argv[1];
  const char* destination = argv[2];
  const char* objectpath = argv[3];

  try {
    run(uid, socketpath, destination, objectpath);
    return EXIT_SUCCESS;
  } catch (ErrorWithErrno& e) {
    const int err = e.getErrno();
    fprintf(stderr, "%s\n%s\n", e.what(), strerror(err));
    return EXIT_FAILURE;
  } catch (std::exception& e) {
    fprintf(stderr, "%s\n", e.what());
    return EXIT_FAILURE;
  }
}
