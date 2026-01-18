{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";

    nixgl = {
      url = "github:guibou/nixGL";
      inputs.nixpkgs.follows = "nixpkgs";
    };

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
    nixgl,
    shuvlog,
    yml-parser,
    ...
  }: let
    system = "x86_64-linux";
    pkgs = nixpkgs.legacyPackages.${system};

    # SELECT YOUR DRIVER HERE IF "Default" FAILS:
    # Use `nixGLIntel` for Intel integrated graphics
    # Use `nixGLNvidia` for proprietary Nvidia drivers
    # Use `nixGLDefault` for Mesa/AMD (Standard)
    nixGL = nixgl.packages.${system}.nixGLDefault;

    nativeBuildInputs = with pkgs; [cmake pkg-config];
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

  in {
    packages.${system} = {
      r-type_client = pkgs.gcc15Stdenv.mkDerivation {
        name = "r-type_client";
        src = ./.;

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
          "-DCMAKE_INSTALL_RPATH_USE_LINK_PATH=TRUE" # nix RPATH handling
        ];

        buildPhase = ''
          cmake --build . --parallel --target r-type_client
        '';

        # Wrap the binary with nixGL to ensure proper OpenGL driver usage
        installPhase = ''
          mkdir -p $out/bin
          cp Client/r-type_client $out/bin/.r-type_client-real

          cat > $out/bin/r-type_client <<EOF
          #!/bin/sh
          exec ${nixGL}/bin/nixGL $out/bin/.r-type_client-real "\$@"
          EOF

          chmod +x $out/bin/r-type_client
        '';
      };

      r-type_server = pkgs.gcc15Stdenv.mkDerivation {
        name = "r-type_server";
        src = ./.;

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

    apps.${system} = {
      default = self.apps.${system}.r-type_client;
      r-type_client = {
        type = "app";
        program = "${self.packages.${system}.r-type_client}/bin/r-type_client";
      };
      r-type_server = {
        type = "app";
        program = "${self.packages.${system}.r-type_server}/bin/r-type_server";
      };
    };

    formatter.${system} = pkgs.alejandra;
  };
}
