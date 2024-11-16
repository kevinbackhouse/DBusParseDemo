Copyright 2020-2024 Kevin Backhouse.

# DBusParseDemo

Demo of using the
[DBusParse](https://github.com/kevinbackhouse/DBusParse) library in an
application. The demo application is called `introspect`. It uses
the D-Bus
[Introspect](https://dbus.freedesktop.org/doc/dbus-specification.html#standard-interfaces-introspectable)
method to query the API of a D-Bus service.

## Usage

On Linux, you can build DBusParseDemo as follows:

```bash
mkdir build
cd build
cmake ..
make
```

To run the `introspect` application:

```bash
./introspect /var/run/dbus/system_bus_socket org.freedesktop.PolicyKit1 /org/freedesktop/PolicyKit1/Authority
```

To run the `introspect_async` application:

```bash
./introspect_async /var/run/dbus/system_bus_socket org.freedesktop.PolicyKit1 /org/freedesktop/PolicyKit1/Authority
```

The `introspect_async` application is like `introspect`, except is uses the [`EPollLoop`](https://github.com/kevinbackhouse/EPollLoop) and [`EPollLoopDBusHandler`](https://github.com/kevinbackhouse/EPollLoopDBusHandler) libraries to send and receive D-Bus messages asynchronously.
