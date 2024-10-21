{
  description = "stk-editor flake";

  inputs = {
    nixpkgs.url = github:nixos/nixpkgs?ref=nixos-24.05;
  };

  outputs =
    { nixpkgs
    , self
    } @ inputs:
    let
      system = "x86_64-linux";
      pkgs = nixpkgs.legacyPackages.${system};
    in
    {
      packages.${system}.default = pkgs.stdenv.mkDerivation {
        pname = "supertuxkart-editor";
        version = "git";
        src = ./.;
        nativeBuildInputs = with pkgs; [
          cmake
          glew
          makeWrapper
          physfs
          xorg.libX11
          xorg.libXxf86vm
          zlib
        ];
        postInstall = ''
          cp -r $src/src/font -t $out/bin
        '';
        preFixup = ''
          wrapProgram $out/bin/supertuxkart-editor --run "cd $out/bin"
        '';
      };
    };
}
