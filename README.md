[![✗](https://img.shields.io/badge/Release-v1.0.0-ffb600.svg?style=for-the-badge)](https://github.com/fa-ponce/tp-tla-2026/releases)

[![✗](https://github.com/fa-ponce/tp-tla-2026/actions/workflows/ci.yaml/badge.svg?branch=development)](https://github.com/fa-ponce/tp-tla-2026/actions/workflows/ci.yaml)

# TabularDSL

Compilador de TabularDSL, un lenguaje declarativo para describir y reutilizar transformaciones sobre archivos CSV (carga, filtrado, nuevas columnas, proyección, ordenamiento, truncado y guardado). El compilador está escrito en C con Flex y Bison, y genera un programa Python que usa únicamente la biblioteca estándar.

Proyecto Especial de Autómatas, Teoría de Lenguajes y Compiladores (ITBA, 2026).

* [Requisitos](#requisitos)
* [Configuración](#configuración)
* [Comandos](#comandos)
* [Ejemplo](#ejemplo)
* [El lenguaje](#el-lenguaje)
* [Ejemplos completos](#ejemplos-completos)
* [CI/CD](#cicd)

## Requisitos

* [Docker v29.7.2](https://www.docker.com/), para construir y probar el compilador.
* [Python 3](https://www.python.org/) (3.8 o superior), para ejecutar el programa generado. No se requiere ninguna librería externa.

La imagen de Docker no incluye Python: el programa `.py` generado (y la opción `--run`) se ejecutan fuera del contenedor, en una máquina con `python3` instalado.

## Configuración

Variables de entorno que controlan el comportamiento del compilador:

| Nombre                | Default | Descripción                                                                                                                         |
| :-------------------- | :-----: | :---------------------------------------------------------------------------------------------------------------------------------- |
| `ENVIRONMENT`         | `Local` | Nombre del entorno activo: `Local`, `Development` o `Production`.                                                                   |
| `LOG_IGNORED_LEXEMES` | `true`  | Si es `true`, loguea los lexemas ignorados por Flex en nivel `DEBUGGING`.                                                           |
| `LOGGING_LEVEL`       | `ALL`   | Nivel mínimo de log. De menor a mayor: `ALL`, `DEBUGGING`, `INFORMATION`, `WARNING`, `ERROR` y `CRITICAL`. Con `ERROR` solo se ven los errores. |

_Docker Compose_ también las lee desde un archivo `.env` (ver `compose.yaml`).

## Comandos

### Iniciar

Levanta un contenedor efímero listo para desarrollar:

```bash
docker compose run --rm compiler
```

### Construir

Construye (o reconstruye) el compilador completo:

```bash
src/main/bash/build.sh
```

### Compilar un programa

```bash
src/main/bash/run.sh <programa.itba> [<salida.py>]
```

Compila `<programa.itba>` y escribe el programa Python en `<salida.py>` (por defecto `output.py`). Las rutas son relativas a la raíz del repositorio. Si el programa tiene errores léxicos, sintácticos o semánticos, el compilador los reporta y termina con un código distinto de 0.

Luego se ejecuta el programa generado, pasando los parámetros externos (`$nombre`) como `nombre=valor`:

```bash
python3 <salida.py> [nombre=valor ...]
```

### Compilar y ejecutar en un solo paso

```bash
src/main/bash/run.sh <programa.itba> <salida.py> --run [nombre=valor ...]
```

Con `--run` el compilador ejecuta el `.py` generado inmediatamente, pasándole los parámetros que siguen. Requiere `python3` en el `PATH`, así que debe usarse fuera del contenedor.

### Tests

Ejecuta todos los casos de prueba de `src/test/c` (`accept/` deben compilar, `reject/` deben fallar, `ignore/` se omiten):

```bash
src/main/bash/test.sh
```

### Detener

```bash
exit
docker compose down
```

## Ejemplo

`usuarios.itba`:

```
LOAD "users.csv"
FILTER @age >= 18 AND @status = "active"
SELECT email
SAVE "active_adults.csv"
```

```bash
src/main/bash/run.sh usuarios.itba usuarios.py
python3 usuarios.py
```

## El lenguaje

Un programa es una lista opcional de declaraciones (`let`, `function`, `pipeline`) seguida de **un** script o de **un** `for`. Un script empieza con `LOAD`, aplica una secuencia de *stages* y termina con `SAVE` (o con bloques de ramificación, cada uno con su propio `SAVE`).

| Construcción | Sintaxis |
| :--- | :--- |
| Carga / guardado | `LOAD "in.csv"` ... `SAVE "out.csv"` |
| Filtrado | `FILTER @edad >= 18 AND NOT (@estado = "baja")` |
| Nueva columna | `MUTATE total = @precio * @cantidad` |
| Nueva columna condicional | `MUTATE envio = IF @precio > 1000 THEN 0 ELSE 250` (el `ELSE` es obligatorio) |
| Proyección | `SELECT nombre, total` |
| Ordenamiento | `SORT BY total DESC` (o `ASC`) |
| Truncado | `LIMIT 10` |
| Constante | `let iva = 0.21;` |
| Función | `function doble(x) { return x * 2; }` |
| Pipeline | `pipeline p(tasa) require tasa >= 0 { FILTER @stock > 0  MUTATE f = @precio * tasa }` |
| Aplicar pipeline | `APPLY p(0.21)` |
| Ramificación | `{ FILTER @x > 0  SAVE "a.csv" }  { SAVE "b.csv" }` |
| Iteración | `for tasa in [0.1, 0.2] { ... }` o `for (tasa, ruta) in [[0.1, "a.csv"], [0.2, "b.csv"]] { ... }` |
| Parámetro externo | `LOAD $entrada` (se pasa como `entrada=archivo.csv` al ejecutar) |
| Comentario | `// hasta el fin de línea` |

* Las palabras reservadas son *case-insensitive* (`LOAD`, `load` y `Load` son equivalentes).
* Las columnas se referencian con `@` dentro de expresiones (`FILTER` y `MUTATE`); en `SELECT`, `SORT BY` y como destino de `MUTATE` se escriben sin `@`.
* Tipos: `number`, `string` y `boolean`. El `+` con algún operando string concatena (`"precios_" + sufijo + ".csv"`).
* Operadores relacionales: `=`, `!=`, `<`, `<=`, `>`, `>=`. Lógicos: `AND`, `OR`, `NOT`.
* Los `require` de un pipeline se verifican al ejecutar el programa generado.
* Las columnas del CSV se conocen recién al ejecutar: una columna inexistente o un tipo incompatible en una columna producen un error al ejecutar el `.py`, no al compilar.

## Ejemplos completos

La carpeta `ejemplos/` contiene programas que combinan pipelines, `require`, `for` con tuplas y bloques de ramificación, junto con sus CSV de entrada. Las salidas se escriben en `ejemplos/salida/`. Desde la raíz del repositorio:

```bash
src/main/bash/run.sh ejemplos/01-ventas.itba ejemplos/01-ventas.py
python3 ejemplos/01-ventas.py entrada=ejemplos/ventas.csv

src/main/bash/run.sh ejemplos/02-precios.itba ejemplos/02-precios.py
python3 ejemplos/02-precios.py
```

## CI/CD

Para ejecutar la integración automática en cada *push*, activar _GitHub Actions_ en la pestaña _Settings_ con la siguiente configuración:

| Key                                                        | Value                                               |
| :--------------------------------------------------------- | :-------------------------------------------------- |
| `Actions permissions`                                      | `Allow all actions and reusable workflows`          |
| `Allow GitHub Actions to create and approve pull requests` | `false`                                             |
| `Artifact and log retention`                               | `30 days`                                           |
| `Fork pull request workflows from outside collaborators`   | `Require approval for all outside collaborators`    |
| `Workflow permissions`                                     | `Read repository contents and packages permissions` |
