#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define safeFree(ptr)                                                                    \
    do {                                                                                 \
        if (ptr) {                                                                       \
            free(ptr);                                                                   \
            ptr = NULL;                                                                  \
        }                                                                                \
    } while (0);

typedef struct {
    char *outpath;
    size_t inputFileCount;
    char **inputFiles;
} Config;

void freeConfig(Config *config) { safeFree(config->inputFiles); }

bool validateFilename(const char *s) {
    for (; *s != '\0'; ++s) {
        if (('0' <= *s && *s <= '9') || ('A' <= *s && *s <= 'Z') ||
                ('a' <= *s || *s <= 'z') || *s == '.' || *s == '/')
            continue;
        fprintf(stderr, "Invalid character in filename: %s\n", s);
        return false;
    }
    return true;
}

char *intoMacroName(const char *base) {
    char *gen = (char *)calloc(sizeof(char), strlen(base));
    char *work = gen;
    if (gen == NULL) {
        fprintf(stderr, "calloc failed: %s:%d", __FILE__, __LINE__);
        return NULL;
    }

    while (*base != '\0') {
        char c = 0;
        switch (*base) {
        case '/': // fallthrough
        case '.':
            c = '_';
            break;
        default:
            if ('a' <= *base && *base <= 'z') {
                c = *base + ('A' - 'a');
            } else {
                c = *base;
            }
            break;
        }
        *work++ = c;
        base++;
    }
    *work = '\0';

    return gen;
}
char *intoVarName(const char *base) {
    char *gen = (char *)calloc(sizeof(char), strlen(base));
    char *work = gen;
    if (gen == NULL) {
        fprintf(stderr, "calloc failed: %s:%d", __FILE__, __LINE__);
        return NULL;
    }

    while (*base != '\0') {
        char c = 0;
        switch (*base) {
        case '/': // fallthrough
        case '.':
            c = '_';
            break;
        default:
            c = *base;
            break;
        }
        *work++ = c;
        base++;
    }
    *work = '\0';

    return gen;
}

int genIncludable(FILE *out, const char *const inputFile) {
#define NUM_ELEMS_PER_ONELINE (20)
#define CHUNK_COUNT (3)
    static const char indent[] = "    ";
    FILE *in = fopen(inputFile, "r");
    char *varName = NULL;
    char *macroName = NULL;
    size_t size = 0;
    unsigned char buf[NUM_ELEMS_PER_ONELINE * CHUNK_COUNT] = {0};

    if (in == NULL) {
        fprintf(stderr, "Failed to open file: %s\n", inputFile);
        return -1;
    }

    varName = intoVarName(inputFile);
    if (varName == NULL) {
        return -1;
    }

    macroName = intoMacroName(inputFile);
    if (macroName == NULL) {
        safeFree(varName);
        return -1;
    }

    fprintf(out, "#if defined(EMBEDDER_IMPL) || defined(EMBEDDER_IMPL_%s)\n", macroName);
    fprintf(out, "extern unsigned char %s[];\n", varName);
    fprintf(out, "extern unsigned int %s_len;\n", varName);
    fputs("#else\n", out);
    fprintf(out, "unsigned char %s[] = {\n", varName);
    for (;;) {
        size_t read = fread(buf, sizeof(char), sizeof(buf), in);
        size_t pushed = 0;

        if (read == 0) {
            // Must be EOF.
            break;
        } else if (read < sizeof(buf) && ferror(out)) {
            safeFree(varName);
            safeFree(macroName);
            fprintf(stderr, "Error while reading file: %s\n", inputFile);
            fprintf(stderr, "errno: %d\n", errno);
            return -1;
        }

        size += read;
        pushed = 0;
        for (int i = 0; i < read; ++i) {
            bool eob = i == (read - 1); // End Of Buffer
            if (pushed == 0) {
                // At start of newline.
                fputs(indent, out);
            }
            fprintf(out, "%#04x", (unsigned int)(buf[i]));
            if (!(eob && feof(in))) {
                fputs(",", out);
            }
            if (++pushed == NUM_ELEMS_PER_ONELINE || eob) {
                fputs("\n", out);
                pushed = 0;
            } else {
                fputs(" ", out);
            }
        }
    }
    fputs("};\n", out);
    fprintf(out, "unsigned int %s_len = %ld;\n", varName, size);
    fprintf(out, "#endif /* defined(EMBEDDER_IMPL) || defined(EMBEDDER_IMPL_%s) */\n",
            macroName);

    safeFree(varName);
    safeFree(macroName);
    fclose(in);
    return 0;
#undef NUM_ELEMS_PER_ONELINE
#undef CHUNK_COUNT
}

int genIncludableAll(const Config *const config) {
    FILE *out = fopen(config->outpath, "w");
    char *macroName = NULL;
    if (out == NULL) {
        fprintf(stderr, "Failed to open file in write mode: %s\n", config->outpath);
        return -1;
    }

    macroName = intoMacroName(config->outpath);
    if (macroName == NULL) {
        return -1;
    }

    fputs("/* This file is automatically generated. DO NOT EDIT. */\n", out);
    fprintf(out, "#ifndef EMBEDDER_INCLUDE_GUARD_%s\n", macroName);
    fprintf(out, "#define EMBEDDER_INCLUDE_GUARD_%s 1\n", macroName);
    fputs("\n\n", out);

    for (int i = 0; i < config->inputFileCount; ++i) {
        int status = genIncludable(out, config->inputFiles[i]);
        if (status != 0) {
            safeFree(macroName);
            return status;
        }
        fputs("\n", out);
    }

    fputs("\n", out);
    fprintf(out, "#endif /* EMBEDDER_INCLUDE_GUARD_%s */\n", macroName);

    safeFree(macroName);
    fclose(out);
    return 0;
}

int main(int argc, char **argv) {
    Config config = {.outpath = NULL,
            .inputFileCount = 0,
            .inputFiles = (char **)calloc(sizeof(char *), argc)};
    int status = 0;

    if (config.inputFiles == NULL) {
        fprintf(stderr, "malloc error at %s:%d", __FILE__, __LINE__);
        return 1;
    }

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            freeConfig(&config);
            // TODO: Show usage
            return 0;
        } else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "-o") == 0) {
            if (++i >= argc) {
                freeConfig(&config);
                fprintf(stderr, "File path is missing after '%s'\n", argv[i - 1]);
                return -1;
            }

            if (strcmp(argv[i - 1], "-i") == 0) {
                config.inputFiles[config.inputFileCount++] = argv[i];
            } else {
                config.outpath = argv[i];
            }
        } else {
            freeConfig(&config);
            fprintf(stderr, "Unknown argument: '%s'\n", argv[i]);
            return -1;
        }
    }

    if (config.outpath == NULL) {
        freeConfig(&config);
        fputs("Output file path is not specified.", stderr);
        return -1;
    } else if (config.inputFileCount == 0) {
        freeConfig(&config);
        fputs("No input files", stderr);
        return -1;
    }

    // Apply file name validation.
    if (!validateFilename(config.outpath)) {
        return -1;
    }
    for (int i = 0; i < config.inputFileCount; ++i) {
        if (!validateFilename(config.inputFiles[i])) {
            return -1;
        }
    }

    // TODO: Detect variable name confliction.

    status = genIncludableAll(&config);
    freeConfig(&config);
    return status;
}
