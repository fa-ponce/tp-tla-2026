#include "backend/code-generation/Generator.h"
#include "frontend/Frontend.h"
#include "frontend/lexical-analysis/FlexActions.h"
#include "frontend/syntactic-analysis/BisonActions.h"
#include "support/logging/Logger.h"
#include "support/type/CompilationStatus.h"
#include "support/type/CompilerState.h"
#include "support/type/ModuleDestructor.h"
#include "backend/semantic-analysis/SemanticAnalyzer.h"
#include "backend/semantic-analysis/TypeChecker.h"
#include <stdbool.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define RUN_FLAG "--run"

static int _runPython(const char* path, const int argc, const char** argv){
	pid_t pid = fork();
	if(pid == -1){
		return -1;
	}
	if(pid == 0){
		const char* args[argc + 3];
		args[0] = "python3";
		args[1] = path;
		for (int k = 0; k < argc; k++) {
			args[k + 2] = argv[k];
		}
		args[argc + 2] = NULL;
		execvp("python3", (char * const *) args);
		_exit(127);
	}
	int status;
	waitpid(pid, &status, 0);
	return WIFEXITED(status) ? WEXITSTATUS(status) : -1;

}

/**
 * The main entry-point of the entire application. If you use "strtok" to
 * parse anything inside this project instead of using Flex and Bison, I will
 * find you, and I will kill you (Bryan Mills; "Taken", 2008).
 */
const int main(const int length, const char ** arguments) {
	LexicalAnalyzer * lexicalAnalyzer = createLexicalAnalyzer();
	Logger * logger = createLogger("EntryPoint");
	for (int k = 0; k < length; ++k) {
		logDebugging(logger, "Argument %d: \"%s\"", k, arguments[k]);
	}
	CompilerState compilerState = {
		.abstractSyntaxtTree = NULL,
		.value = 0,
		// El primer argumento es la ruta del .py generado
		.outputPath = 1 < length ? arguments[1] : "output.py"
	};
	// Si el segundo argumento es --run, el .py se ejecuta despues de generarlo
	// y todo lo que sigue se le pasa como $param
	const bool runOutput = 2 < length && strcmp(arguments[2], RUN_FLAG) == 0;
	ModuleDestructor moduleDestructors[] = {
		initializeAbstractSyntaxTreeModule(),
		initializeFlexActionsModule(lexicalAnalyzer),
		initializeBisonActionsModule(&compilerState),
		initializeFrontendModule(lexicalAnalyzer),
		initializeSemanticAnalyzerModule(),
		initializeTypeCheckerModule(),
		initializeGeneratorModule()
	};
	CompilationStatus compilationStatus = executeSyntacticAnalysis();
	Program * program = compilerState.abstractSyntaxtTree;
	if (compilationStatus == SUCCEEDED) {
		// ----------------------------------------------------------------------------------------
		// Beginning of the Backend... ------------------------------------------------------------
		logInformation(logger, "The syntactic-analysis phase accepts the input program.");
		
		/*
			Aca realizamos el llenado de la tabla de simbolos,
			y vemos si es correcto semanticamente el programa
		*/  
		compilationStatus = executeSemanticAnalysis(program);
		if (compilationStatus != SUCCEEDED) {
      		logError(logger, "The semantic-analysis phase rejects the input program.");
  		}
		else {
			compilationStatus = executeGenerator(&compilerState);
			if (compilationStatus == SUCCEEDED && runOutput) {
				logInformation(logger, "Running \"%s\"...", compilerState.outputPath);
				const int exitCode = _runPython(compilerState.outputPath, length - 3, arguments + 3);
				if (exitCode != 0) {
					logError(logger, "The generated program failed (exit code %d).", exitCode);
					compilationStatus = FAILED;
				}
			}
		}
		// ...end of the Backend. -----------------------------------------------------------------
		// ----------------------------------------------------------------------------------------
	}
	else {
		logError(logger, "The syntactic-analysis phase rejects the input program.");
		compilationStatus = FAILED;
	}
	logDebugging(logger, "Releasing AST resources...");
	destroyProgram(program);
	for (int k = (sizeof(moduleDestructors)/sizeof(ModuleDestructor)) - 1; 0 <= k; --k) {
		moduleDestructors[k]();
	}
	logDebugging(logger, "Compilation is done.");
	destroyLogger(logger);
	destroyLexicalAnalyzer(lexicalAnalyzer);
	return compilationStatus;
}
