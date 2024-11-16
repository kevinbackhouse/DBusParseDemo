// Copyright 2020-2024 Kevin Backhouse.
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

#include "dbus_auth.hpp"
#include "dbus_utils.hpp"
#include "utils.hpp"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

void send_introspect(const int fd, const char *destination,
                     const char *objectpath) {
  const uint32_t serialNumber = 1000;

  dbus_method_call(fd, serialNumber, DBusMessageBody::mk0(), _s(objectpath),
                   _s("org.freedesktop.DBus.Introspectable"), _s(destination),
                   _s("Introspect"));
}

void send_test(const int fd, const char *destination, const char *objectpath) {
  const uint32_t serialNumber = 1000;

  dbus_method_call(fd, serialNumber,
                   DBusMessageBody::mk(_vec<std::unique_ptr<DBusObject>>(
                       DBusObjectArray::mk1(_vec<std::unique_ptr<DBusObject>>(
                           DBusObjectString::mk(_s("kevwozere")))))),
                   _s(objectpath), _s("org.freedesktop.Application"),
                   _s(destination), _s("Open"));
}

int run(const uid_t uid, const char *filename, const char *destination,
        const char *objectpath) {
  // Use a unix socket to connect to dbus-daemon.
  sockaddr_un address;
  memset(&address, 0, sizeof(address));
  address.sun_family = AF_UNIX;
  snprintf(address.sun_path, sizeof(address.sun_path), "%s", filename);

  const AutoCloseFD fd(socket(AF_UNIX, SOCK_STREAM, 0));
  if (fd.get() < 0) {
    fprintf(stderr, "could not create socket\n");
    return -1;
  }

  if (connect(fd.get(), (sockaddr *)(&address), sizeof(address)) < 0) {
    int err = errno;
    fprintf(stderr, "could not connect to socket: %s\n", strerror(err));
    return -1;
  }

  // Authenticate with dbus-daemon.
  dbus_sendauth(uid, fd.get());

  // Send hello message to dbus-daemon.
  dbus_send_hello(fd.get());
  std::unique_ptr<DBusMessage> hello_reply1 = receive_dbus_message(fd.get());
  print_dbus_message(STDOUT_FILENO, *hello_reply1);
  std::unique_ptr<DBusMessage> hello_reply2 = receive_dbus_message(fd.get());
  print_dbus_message(STDOUT_FILENO, *hello_reply2);

  // Call the Introspect method.
  send_introspect(fd.get(), destination, objectpath);

  std::unique_ptr<DBusMessage> introspect_reply =
      receive_dbus_message(fd.get());
  print_dbus_message(STDOUT_FILENO, *introspect_reply);

  return 0;
}

int main(int argc, char *argv[]) {
  const char *progname = argc > 0 ? argv[0] : "a.out";
  if (argc < 4) {
    fprintf(
        stderr,
        "usage:   %s <unix socket path> <destination> <object path>\n"
        "example: %s /var/run/dbus/system_bus_socket "
        "org.freedesktop.PolicyKit1 /org/freedesktop/PolicyKit1/Authority\n",
        progname, progname);
    return 1;
  }

  uid_t uid = getuid();
  const char *filename = argv[1];
  const char *destination = argv[2];
  const char *objectpath = argv[3];

  if (run(uid, filename, destination, objectpath) < 0) {
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
