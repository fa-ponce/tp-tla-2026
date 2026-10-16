# Columnas con sintaxis propia (opción C)

## Problema

En la gramática, cualquier nombre suelto dentro de una expresión (`primary: IDENT`) se convierte en un `Expression` de tipo `IDENTIFIER`, sea una variable del programa o una columna del CSV:

```
let minimo = 18;
pipeline adultos() {
    filter edad > minimo
}
```

```
COMPARISON (>)
├── IDENTIFIER "edad"     <- columna
└── IDENTIFIER "minimo"   <- variable
```

El analizador semántico solo ve el AST, así que no puede distinguirlos. `lookupSymbol("edad")` devuelve `NULL` porque las columnas no se declaran (existen en el CSV, que se lee recién al ejecutar), y se reporta un falso "no declarado".

## Solución

Darle a las columnas su propia sintaxis (`@edad`), con su propio token y su propio nodo en el AST. Es el mismo camino que ya sigue `EXTERNAL_PARAM` (`$nombre`).

Así el parser genera nodos distintos y el AST ya trae la información:

```
COMPARISON (>)
├── COLUMN_REFERENCE "edad"   -> no se busca en la tabla
└── IDENTIFIER "minimo"       -> se busca en la tabla
```

`IDENTIFIER` pasa a ser siempre una variable, así que el error de "no declarado" es exacto en todo el programa.

Se eligió `@` porque ningún patrón del lexer lo usa (`[` ya está ocupado).

## Pasos

### 1. Lexer: patrón (`frontend/lexical-analysis/FlexPatterns.l`)

Al lado de `"$"{IDENT}`:

```
"$"{IDENT}      { return ExternalParamLexemeAction(); }
"@"{IDENT}      { return ColumnLexemeAction(); }
```

Flex elige el match más largo; `{IDENT}` no puede empezar con `@`, así que no hay conflicto.

### 2. Lexer: acción (`FlexActions.c` + prototipo en `FlexActions.h`)

```c
CompilationStatus ColumnLexemeAction(){
	Token * token = createToken(_lexicalAnalyzer, COLUMN);
	token->semanticValue->string = strdup(token->lexeme + 1); // sin el '@'
	_logTokenAction(__FUNCTION__, token);
	CompilationStatus status = pushToken(_lexicalAnalyzer, token);
	destroyToken(token);
	return status;
}
```

```c
CompilationStatus ColumnLexemeAction();
```

### 3. Parser: declarar el token (`frontend/syntactic-analysis/BisonGrammar.y`)

Al lado de `%token <string> EXTERNAL_PARAM`:

```
%token <string> COLUMN
```

Bison genera el `#define COLUMN` que usa `createToken` en el paso 2.

### 4. Parser: regla (`BisonGrammar.y`, regla `primary`)

```
| EXTERNAL_PARAM    { $$ = ExternalParameterExpressionSemanticAction($1); }
| COLUMN            { $$ = ColumnExpressionSemanticAction($1); }
```

Al estar en `primary`, una columna puede aparecer en cualquier expresión (comparaciones, cuentas, argumentos). `filter` y `mutate` no se tocan.

### 5. AST: tipo y campo (`AbstractSyntaxTree.h`)

En `enum ExpressionType`:

```c
	EXTERNAL_PARAMETER,
	COLUMN_REFERENCE,
	FUNCTION_CALL
```

En la union de `struct Expression`:

```c
		char * externalParameter;
		char * column;
```

### 6. AST: constructor (`BisonActions.c` + prototipo en `BisonActions.h`)

```c
Expression * ColumnExpressionSemanticAction(char * column) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expression = calloc(1, sizeof(Expression));
	expression->column = column;
	expression->type = COLUMN_REFERENCE;
	return expression;
}
```

```c
Expression * ColumnExpressionSemanticAction(char * column);
```

### 7. AST: destructor (`AbstractSyntaxTree.c`, `destroyExpression`)

```c
			case COLUMN_REFERENCE:
				free(expression->column);
				break;
```

Sin esto no hay error de compilación, pero se pierde la memoria del `strdup` del paso 2.

### 8. Semántico (`backend/semantic-analysis/SemanticAnalyzer.c`, `_visitExpression`)

```c
    case COLUMN_REFERENCE:
        break;
```

`case IDENTIFIER` queda igual: busca en la tabla y da error si no está.

## Pendientes

- `select`, `sort` y `mutate` usan `IDENT` directo para la columna (`colList`, `SORT BY IDENT`, `MUTATE IDENT EQUAL`), no expresiones. Siguen escribiéndose sin `@` (`select edad`), lo que queda inconsistente con `filter @edad > 18`. Para que sea uniforme, cambiar esas reglas para que usen `COLUMN`.
- Reescribir los programas de prueba que usan columnas en `filter`/`mutate`.
- Cambia la sintaxis respecto de la spec entregada; confirmar con la cátedra.
