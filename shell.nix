{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  buildInputs = [
    pkgs.glew
    pkgs.glfw
    pkgs.stb
  ];
}
