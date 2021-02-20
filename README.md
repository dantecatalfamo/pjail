# pjail
Execute a program with pledge restrictions 

```
pjail [-hilv] [-p promise] [command] [args...]
```

## flags

- `-h` Show help
- `-i` Invert (pledge everything except promises in flags)
- `-l` List pledge promises
- `-p promise` Make pledge promise
- `-v` Verbose (print pledge string to `stderr` before executing)

# compiling
To compile, just run

```shell
make
```

# installing

To install, run

```shell
doas make install
```

# more
See [man pledge](https://man.openbsd.org/pledge.2) for more details.
