# pjail
Execute a program with pledge restrictions 

```
pjail: [-hvli] [-p pledge] [command] [args...]
```

## flags

- `-h` Show help
- `-v` Verbose (print pledge string to `stderr` before executing)
- `-l` List pledge promises
- `-i` Invert (pledge everything except promises in flags)
- `-p` Make pledge promise

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
