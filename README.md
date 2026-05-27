# tfos – transfer_file_over_socket

A simple program that sends files over TCP socket.

```text
transfer_file_over_socket v0.1.0
link: https://github.com/boholder/transfer_file_over_socket
Usage: tfos [OPTIONS]

OPTIONS:
  -f, --file            Served file(s) path, can be a directory or a file (default: working directory)
  -p, --port            Which TCP port to listen on (default: 18180)
  -t, --timeout         Inactive socket timeout (after connected) in seconds (default: 60)
  -c, --max-conns       Maximum number of concurrent alive connections (default: 10)
      --input-filter    Filter client message (default: [\s\n])
  -h, --help            Show this help message
  -v, --verbose         Enable debug log
```

By the way, you can achieve almost the same thing with `nc` or `ncat` command (on host/server machine):

```shell
ncat --listen --send-only port < file
```

## Build

```shell
cmake --list-presets
cmake --preset <CHOSEN_PRESET>
cmake --build cmake-build/<CHOSEN_PRESET> --target tfos
```

For example:

```shell
cmake --preset linux-x64-release
cmake --build cmake-build/linux-x64-release --target tfos
```

## Usage

When you are landed on a server that doesn't have familiar tools like `curl`, `wget`, `ftp`, but have things below:

```shell
# the old solid nc
# run `tfos --input-filter` to remove '\n' added by pipe
echo "file-name" | nc ip port > output
```

```shell
# https://nmap.org/ncat/
# run `tfos --input-filter` to remove '\n' added by pipe
echo "file-name" | ncat ip port -o output
```
