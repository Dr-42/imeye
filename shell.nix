{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  buildInputs = [
    pkgs.cmake
    pkgs.glew
    pkgs.glfw
    pkgs.stb
  ];
}
