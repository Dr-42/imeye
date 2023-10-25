# Imeye

Simple image viewer

## Building

Make sure to install builder_cpp

```console
cargo install builder_cpp
```

## Windows
```
windres icon.rc -O coff -o icon.res
builder_cpp -b
```

## Linux
```console
builder_cpp -b
```
