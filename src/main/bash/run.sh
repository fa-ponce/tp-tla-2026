#! /bin/bash

set -euo pipefail

BASE_PATH="$(dirname "$0")/../../.."
cd "$BASE_PATH"

INPUT="$1"
shift 1
if [ ! -f "$INPUT" ]; then
	echo "No se encontro el archivo '$INPUT' (las rutas son relativas a la raiz del repositorio)." >&2
	exit 1
fi
cat "$INPUT" | ".build/Flex-Bison-Compiler" "$@"
