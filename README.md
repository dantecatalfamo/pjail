# pjail
Execute a program with pledge restrictions 

```
pjail: [-hl] [-p pledge] [command] [args...]
```

## flags

- `-h` Show help
- `-l` List pledge promises
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
