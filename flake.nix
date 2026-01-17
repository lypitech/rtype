{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";

    shuvlog = {
      url = "github:lypitech/shuvlog/0a81e259e91b9679e22714635b77e04b1d16aa1f";
      flake = false;
    };
    yml-parser = {
      url = "github:lypitech/yml-parser/f857e76a0e31676926094da4518475b7bc508ff8";
      flake = false;
    };
  };

  outputs = {
    self,
    nixpkgs,
    shuvlog,
    yml-parser,
    ...
  }: let
    system = "x86_64-linux";
    pkgs = nixpkgs.legacyPackages.${system};

    # Define common build inputs once
    nativeBuildInputs = with pkgs; [cmake pkg-config];

    # Common runtime libraries
    buildInputs = with pkgs; [
      asio
      nlohmann_json
      imgui
      raylib
      libGL
      xorg.libX11
      xorg.libXi
      xorg.libXcursor
      xorg.libXrandr
      xorg.libXinerama
    ];

    commonSrc = ./.; # Or use builtins.path if you want to filter files
  in {
    packages.${system} = {
      r-type_client = pkgs.gcc15Stdenv.mkDerivation {
        name = "r-type_client";
        src = commonSrc;

        inherit nativeBuildInputs buildInputs;

        postPatch = ''
          rm -rf lib/shuvlog lib/yml-parser
          cp -r ${shuvlog} lib/shuvlog
          cp -r ${yml-parser} lib/yml-parser
          chmod -R +w lib/shuvlog lib/yml-parser
        '';

        cmakeFlags = [
          "-DCMAKE_BUILD_TYPE=Release"
          "-DUSE_CONAN=OFF" # Pass this if you updated CMakeLists.txt
          "-DCMAKE_INSTALL_RPATH_USE_LINK_PATH=TRUE" # Let CMake keep RPATHs so Nix can fix them
        ];

        buildPhase = ''
          cmake --build . --parallel --target r-type_client
        '';

        installPhase = ''
          mkdir -p $out/bin
          cp Client/r-type_client $out/bin/
        '';

      };

      r-type_server = pkgs.gcc15Stdenv.mkDerivation {
        name = "r-type_server";
        src = commonSrc;

        inherit nativeBuildInputs buildInputs;

        postPatch = ''
          rm -rf lib/shuvlog lib/yml-parser
          cp -r ${shuvlog} lib/shuvlog
          cp -r ${yml-parser} lib/yml-parser
          chmod -R +w lib/shuvlog lib/yml-parser
        '';

        cmakeFlags = [
          "-DCMAKE_BUILD_TYPE=Release"
          "-DUSE_CONAN=OFF"
        ];

        buildPhase = ''
          cmake --build . --parallel --target r-type_server
        '';

        installPhase = ''
          mkdir -p $out/bin
          cp Server/r-type_server $out/bin/
        '';
      };
    };

    devShells.${system}.default = pkgs.mkShell {
      packages = nativeBuildInputs ++ buildInputs ++ [pkgs.gcc15];
    };

    formatter.${system} = pkgs.alejandra;
  };
}
