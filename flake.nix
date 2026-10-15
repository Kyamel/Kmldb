{
  description = "Kmldb: banco de dados acadêmico em C";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs = { self, nixpkgs }:
    let
      systems = [ "x86_64-linux" "aarch64-linux" ];
      forAllSystems = f:
        nixpkgs.lib.genAttrs systems (system: f nixpkgs.legacyPackages.${system});
    in
    {
      packages = forAllSystems (pkgs: {
        default = pkgs.stdenv.mkDerivation {
          pname = "kmldb";
          version = "2.0.0";
          src = self;
          nativeBuildInputs = [ pkgs.cmake ];
          doCheck = true;
          checkPhase = ''
            ctest --output-on-failure
          '';
        };
      });

      devShells = forAllSystems (pkgs: {
        default = pkgs.mkShell {
          packages = [ pkgs.cmake ];
        };
      });
    };
}
