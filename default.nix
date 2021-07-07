{ pkgs ? import <nixpkgs> { } }:
pkgs.stdenv.mkDerivation rec {
  pname = "dpro";
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
    mv dpro $out/bin
  '';
}
