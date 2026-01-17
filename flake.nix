{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    pre-commit-hooks.url = "github:cachix/pre-commit-hooks.nix";
    nixgl.url = "github:nix-community/nixGL";
  };

  outputs = {
    self,
    nixpkgs,
    pre-commit-hooks,
    nixgl,
    ...
  }: let
    system = "x86_64-linux";
    pkgs = nixpkgs.legacyPackages.${system};
    nixgl-pkgs = nixgl.packages.${system};

    shuvlog = pkgs.fetchgit {
      url = "https://github.com/lypitech/shuvlog";
      rev = "0a81e259e91b9679e22714635b77e04b1d16aa1f";
      sha256 = "sha256-u9+/5XGXcLSUmqtaZTwXMJhm14uEX+4Ygftn3qxVpkg=";
    };

    yml-parser = pkgs.fetchgit {
      url = "https://github.com/lypitech/yml-parser";
      rev = "f857e76a0e31676926094da4518475b7bc508ff8";
      sha256 = "sha256-xCXSKUuBs3IKZtpzONKcqwYpnbuY3ozgHDL2/0cXZtA=";
    };
  in {
    devShells.${system} = {
      default =
        pkgs.mkShell.override {
          stdenv = pkgs.gcc15Stdenv;
        } {
          inputsFrom = [
            self.packages.${system}.client
            self.packages.${system}.server
          ];
          buildInputs = [pkgs.stdenv.cc.cc.lib];
        };
    };

    packages.${system} = let
      commonPatch = ''
        rm -rf lib/shuvlog lib/yml-parser
        cp -r ${shuvlog} lib/shuvlog
        cp -r ${yml-parser} lib/yml-parser
        chmod -R +w lib/shuvlog lib/yml-parser
        substituteInPlace Client/CMakeLists.txt \
          --replace-fail "include(\''${CMAKE_BINARY_DIR}/conan_toolchain.cmake)" ""
        substituteInPlace Server/CMakeLists.txt \
          --replace-fail "include(\''${CMAKE_BINARY_DIR}/conan_toolchain.cmake)" ""
        substituteInPlace Client/CMakeLists.txt \
          --replace-fail "find_package(asio REQUIRED)" "find_path(ASIO_INCLUDE_DIR asio.hpp REQUIRED)"
        substituteInPlace Client/CMakeLists.txt \
          --replace-fail "asio::asio" ""
        substituteInPlace lib/rtnt/CMakeLists.txt \
          --replace-fail "find_package(asio REQUIRED)" "find_path(ASIO_INCLUDE_DIR asio.hpp REQUIRED)"
        substituteInPlace lib/rtnt/CMakeLists.txt \
          --replace-fail "asio::asio" ""
        substituteInPlace Server/CMakeLists.txt \
          --replace-fail "find_package(asio REQUIRED)" "find_path(ASIO_INCLUDE_DIR asio.hpp REQUIRED)"
        substituteInPlace Server/CMakeLists.txt \
          --replace-fail "asio::asio" ""
        echo "target_include_directories(\''${PROJECT_NAME} PUBLIC \''${ASIO_INCLUDE_DIR})" >> lib/rtnt/CMakeLists.txt
        echo "target_include_directories(\''${PROJECT_NAME} PRIVATE \''${ASIO_INCLUDE_DIR})" >> Client/CMakeLists.txt
        echo "target_include_directories(\''${PROJECT_NAME} PRIVATE \''${ASIO_INCLUDE_DIR})" >> Server/CMakeLists.txt
      '';
      commonSrc = builtins.path {
        path = ./.;
        name = "rtype-src";
      };
    in {
      client = (pkgs.callPackage ({
        stdenv,
        cmake,
        pkg-config,
        asio,
        gtest,
        nlohmann_json,
        imgui,
        raylib,
        libGL,
        xorg,
      }:
        stdenv.mkDerivation {
          name = "rtype-client";
          src = commonSrc;
          nativeBuildInputs = [cmake pkg-config pkgs.patchelf];
          buildInputs = [asio nlohmann_json imgui raylib libGL xorg.libX11 xorg.libXi xorg.libXcursor xorg.libXrandr xorg.libXinerama];
          postPatch = commonPatch;
          cmakeFlags = [
            "-DCMAKE_BUILD_TYPE=Release"
            "-DBUILD_TESTING=OFF"
          ];
          buildPhase = ''
            cmake --build . --parallel --target r-type_client
          '';
          installPhase = ''
            mkdir -p $out/bin
            cp Client/r-type_client $out/bin/.r-type_client-unwrapped
            cat > $out/bin/r-type_client << EOF
            #!/bin/sh
            exec ${nixgl-pkgs.nixGLDefault}/bin/nixGL $out/bin/.r-type_client-unwrapped "\$@"
            EOF
            chmod +x $out/bin/r-type_client
          '';
          postFixup = ''
            patchelf --set-rpath "${pkgs.lib.makeLibraryPath [pkgs.libGL pkgs.xorg.libX11 pkgs.xorg.libXi pkgs.xorg.libXcursor pkgs.xorg.libXrandr pkgs.xorg.libXinerama raylib stdenv.cc.cc.lib]}" $out/bin/.r-type_client-unwrapped
          '';
        }) {})
      .override {stdenv = pkgs.gcc15Stdenv;};

      server = (pkgs.callPackage ({
        stdenv,
        cmake,
        pkg-config,
        asio,
        gtest,
        nlohmann_json,
        imgui,
        raylib,
      }:
        stdenv.mkDerivation {
          name = "rtype-server";
          src = commonSrc;
          nativeBuildInputs = [cmake pkg-config];
          buildInputs = [asio nlohmann_json imgui raylib];
          postPatch = commonPatch;
          cmakeFlags = [
            "-DCMAKE_BUILD_TYPE=Release"
            "-DBUILD_TESTING=OFF"
          ];
          buildPhase = ''
            cmake --build . --parallel --target r-type_server
          '';
          installPhase = ''
            mkdir -p $out/bin
            cp Server/r-type_server $out/bin/
          '';
        }) {})
      .override {stdenv = pkgs.gcc15Stdenv;};
    };

    apps.${system} = {
      client = {
        type = "app";
        program = "${self.packages.${system}.client}/bin/r-type_client";
      };
      server = {
        type = "app";
        program = "${self.packages.${system}.server}/bin/r-type_server";
      };
    };

    formatter.${system} = pkgs.alejandra;
  };
}
