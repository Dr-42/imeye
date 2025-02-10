# Imeye

Simple image viewer

## Building

Make sure to install builder_cpp

```console
cargo install builder_cpp
```

### Windows

```
windres icon.rc -O coff -o icon.res
builder_cpp -b
```

### Linux

```console
builder_cpp -b
```

## Controls

| Key   | Action               |
| ----- | -------------------- |
| Esc   | Close                |
| Left  | Previous image       |
| Right | Next image           |
| Up    | Zoom in              |
| Down  | Zoom out             |
| F     | Toggle fullscreen    |
| W     | Move image up        |
| S     | Move image down      |
| A     | Move image left      |
| D     | Move image right     |
| Q     | Rotate anticlockwise |
| E     | Rotate clockwise     |
| R     | Reset view           |
