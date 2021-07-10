{ pkgs ? import <nixpkgs> { } }:
pkgs.stdenv.mkDerivation rec {
  pname = "dbuild";
  version = "0.1.0";

  src = ./.;

  buildInputs = [
    pkgs.cmake
    pkgs.gdb
  ];

  configurePhase = ''
    cmake .
  '';

  buildPhase = ''
    make
  '';

  installPhase = ''
    mkdir -p $out/bin
    mv dbuild $out/bin
  '';
}
