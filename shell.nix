{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  buildInputs = [
    pkgs.cargo
    pkgs.glew
    pkgs.glfw
    pkgs.stb
  ];
}
