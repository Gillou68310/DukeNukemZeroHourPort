// configfile.c - handles loading and saving the configuration options
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "configfile.h"

#define ARRAY_LEN(arr) (sizeof(arr) / sizeof(arr[0]))

enum ConfigOptionType {
    CONFIG_TYPE_BOOL,
    CONFIG_TYPE_UINT,
    CONFIG_TYPE_FLOAT,
};

struct ConfigOption {
    const char *name;
    enum ConfigOptionType type;
    union {
        bool *boolValue;
        unsigned int *uintValue;
        float *floatValue;
    };
};

/*
 *Config options and default values
 */
bool configFullscreen = false;
bool configDebug = false;
bool configSkipIntro = false;
// Keyboard mappings (scancode values)
unsigned int configKeyA[4] = {16,75,0,0};
unsigned int configKeyB[4] = {18,77,0,0};
unsigned int configKeyStart[4] = {28,284,0,0};
unsigned int configKeyR[4] = {0x39,83,0,0};
unsigned int configKeyL[4] = {15,82,0,0};
unsigned int configKeyZ[4] = {42,78,0,0};
unsigned int configKeyCUp[4] = {0x11,76,0,0};
unsigned int configKeyCDown[4] = {0x1F,80,0,0};
unsigned int configKeyCLeft[4] = {0x1E,79,0,0};
unsigned int configKeyCRight[4] = {0x20,81,0,0};
unsigned int configKeyStickUp[4] = {0x148,0,0,0};
unsigned int configKeyStickDown[4] = {0x150,0,0,0};
unsigned int configKeyStickLeft[4] = {0x14B,0,0,0};
unsigned int configKeyStickRight[4] = {0x14D,0,0,0};
unsigned int configKeyJoyUp[4] = {0,309,0,0};
unsigned int configKeyJoyDown[4] = {0,72,0,0};
unsigned int configKeyJoyLeft[4] = {0,71,0,0};
unsigned int configKeyJoyRight[4] = {0,73,0,0};


static const struct ConfigOption options[] = {
    {.name = "fullscreen",           .type = CONFIG_TYPE_BOOL, .boolValue = &configFullscreen},
    {.name = "debug",                .type = CONFIG_TYPE_BOOL, .boolValue = &configDebug},
    {.name = "skipintro",            .type = CONFIG_TYPE_BOOL, .boolValue = &configSkipIntro},
    {.name = "cont1_key_a",          .type = CONFIG_TYPE_UINT, .uintValue = &configKeyA[0]},
    {.name = "cont1_key_b",          .type = CONFIG_TYPE_UINT, .uintValue = &configKeyB[0]},
    {.name = "cont1_key_start",      .type = CONFIG_TYPE_UINT, .uintValue = &configKeyStart[0]},
    {.name = "cont1_key_r",          .type = CONFIG_TYPE_UINT, .uintValue = &configKeyR[0]},
    {.name = "cont1_key_l",          .type = CONFIG_TYPE_UINT, .uintValue = &configKeyL[0]},
    {.name = "cont1_key_z",          .type = CONFIG_TYPE_UINT, .uintValue = &configKeyZ[0]},
    {.name = "cont1_key_cup",        .type = CONFIG_TYPE_UINT, .uintValue = &configKeyCUp[0]},
    {.name = "cont1_key_cdown",      .type = CONFIG_TYPE_UINT, .uintValue = &configKeyCDown[0]},
    {.name = "cont1_key_cleft",      .type = CONFIG_TYPE_UINT, .uintValue = &configKeyCLeft[0]},
    {.name = "cont1_key_cright",     .type = CONFIG_TYPE_UINT, .uintValue = &configKeyCRight[0]},
    {.name = "cont1_key_stickup",    .type = CONFIG_TYPE_UINT, .uintValue = &configKeyStickUp[0]},
    {.name = "cont1_key_stickdown",  .type = CONFIG_TYPE_UINT, .uintValue = &configKeyStickDown[0]},
    {.name = "cont1_key_stickleft",  .type = CONFIG_TYPE_UINT, .uintValue = &configKeyStickLeft[0]},
    {.name = "cont1_key_stickright", .type = CONFIG_TYPE_UINT, .uintValue = &configKeyStickRight[0]},
    {.name = "cont1_key_joyup",      .type = CONFIG_TYPE_UINT, .uintValue = &configKeyJoyUp[0]},
    {.name = "cont1_key_joydown",    .type = CONFIG_TYPE_UINT, .uintValue = &configKeyJoyDown[0]},
    {.name = "cont1_key_joyleft",    .type = CONFIG_TYPE_UINT, .uintValue = &configKeyJoyLeft[0]},
    {.name = "cont1_key_joyright",   .type = CONFIG_TYPE_UINT, .uintValue = &configKeyJoyRight[0]},
    {.name = "cont2_key_a",          .type = CONFIG_TYPE_UINT, .uintValue = &configKeyA[1]},
    {.name = "cont2_key_b",          .type = CONFIG_TYPE_UINT, .uintValue = &configKeyB[1]},
    {.name = "cont2_key_start",      .type = CONFIG_TYPE_UINT, .uintValue = &configKeyStart[1]},
    {.name = "cont2_key_r",          .type = CONFIG_TYPE_UINT, .uintValue = &configKeyR[1]},
    {.name = "cont2_key_l",          .type = CONFIG_TYPE_UINT, .uintValue = &configKeyL[1]},
    {.name = "cont2_key_z",          .type = CONFIG_TYPE_UINT, .uintValue = &configKeyZ[1]},
    {.name = "cont2_key_cup",        .type = CONFIG_TYPE_UINT, .uintValue = &configKeyCUp[1]},
    {.name = "cont2_key_cdown",      .type = CONFIG_TYPE_UINT, .uintValue = &configKeyCDown[1]},
    {.name = "cont2_key_cleft",      .type = CONFIG_TYPE_UINT, .uintValue = &configKeyCLeft[1]},
    {.name = "cont2_key_cright",     .type = CONFIG_TYPE_UINT, .uintValue = &configKeyCRight[1]},
    {.name = "cont2_key_stickup",    .type = CONFIG_TYPE_UINT, .uintValue = &configKeyStickUp[1]},
    {.name = "cont2_key_stickdown",  .type = CONFIG_TYPE_UINT, .uintValue = &configKeyStickDown[1]},
    {.name = "cont2_key_stickleft",  .type = CONFIG_TYPE_UINT, .uintValue = &configKeyStickLeft[1]},
    {.name = "cont2_key_stickright", .type = CONFIG_TYPE_UINT, .uintValue = &configKeyStickRight[1]},
    {.name = "cont2_key_joyup",      .type = CONFIG_TYPE_UINT, .uintValue = &configKeyJoyUp[1]},
    {.name = "cont2_key_joydown",    .type = CONFIG_TYPE_UINT, .uintValue = &configKeyJoyDown[1]},
    {.name = "cont2_key_joyleft",    .type = CONFIG_TYPE_UINT, .uintValue = &configKeyJoyLeft[1]},
    {.name = "cont2_key_joyright",   .type = CONFIG_TYPE_UINT, .uintValue = &configKeyJoyRight[1]},
    {.name = "cont3_key_a",          .type = CONFIG_TYPE_UINT, .uintValue = &configKeyA[2]},
    {.name = "cont3_key_b",          .type = CONFIG_TYPE_UINT, .uintValue = &configKeyB[2]},
    {.name = "cont3_key_start",      .type = CONFIG_TYPE_UINT, .uintValue = &configKeyStart[2]},
    {.name = "cont3_key_r",          .type = CONFIG_TYPE_UINT, .uintValue = &configKeyR[2]},
    {.name = "cont3_key_l",          .type = CONFIG_TYPE_UINT, .uintValue = &configKeyL[2]},
    {.name = "cont3_key_z",          .type = CONFIG_TYPE_UINT, .uintValue = &configKeyZ[2]},
    {.name = "cont3_key_cup",        .type = CONFIG_TYPE_UINT, .uintValue = &configKeyCUp[2]},
    {.name = "cont3_key_cdown",      .type = CONFIG_TYPE_UINT, .uintValue = &configKeyCDown[2]},
    {.name = "cont3_key_cleft",      .type = CONFIG_TYPE_UINT, .uintValue = &configKeyCLeft[2]},
    {.name = "cont3_key_cright",     .type = CONFIG_TYPE_UINT, .uintValue = &configKeyCRight[2]},
    {.name = "cont3_key_stickup",    .type = CONFIG_TYPE_UINT, .uintValue = &configKeyStickUp[2]},
    {.name = "cont3_key_stickdown",  .type = CONFIG_TYPE_UINT, .uintValue = &configKeyStickDown[2]},
    {.name = "cont3_key_stickleft",  .type = CONFIG_TYPE_UINT, .uintValue = &configKeyStickLeft[2]},
    {.name = "cont3_key_stickright", .type = CONFIG_TYPE_UINT, .uintValue = &configKeyStickRight[2]},
    {.name = "cont3_key_joyup",      .type = CONFIG_TYPE_UINT, .uintValue = &configKeyJoyUp[2]},
    {.name = "cont3_key_joydown",    .type = CONFIG_TYPE_UINT, .uintValue = &configKeyJoyDown[2]},
    {.name = "cont3_key_joyleft",    .type = CONFIG_TYPE_UINT, .uintValue = &configKeyJoyLeft[2]},
    {.name = "cont3_key_joyright",   .type = CONFIG_TYPE_UINT, .uintValue = &configKeyJoyRight[2]},
    {.name = "cont4_key_a",          .type = CONFIG_TYPE_UINT, .uintValue = &configKeyA[3]},
    {.name = "cont4_key_b",          .type = CONFIG_TYPE_UINT, .uintValue = &configKeyB[3]},
    {.name = "cont4_key_start",      .type = CONFIG_TYPE_UINT, .uintValue = &configKeyStart[3]},
    {.name = "cont4_key_r",          .type = CONFIG_TYPE_UINT, .uintValue = &configKeyR[3]},
    {.name = "cont4_key_l",          .type = CONFIG_TYPE_UINT, .uintValue = &configKeyL[3]},
    {.name = "cont4_key_z",          .type = CONFIG_TYPE_UINT, .uintValue = &configKeyZ[3]},
    {.name = "cont4_key_cup",        .type = CONFIG_TYPE_UINT, .uintValue = &configKeyCUp[3]},
    {.name = "cont4_key_cdown",      .type = CONFIG_TYPE_UINT, .uintValue = &configKeyCDown[3]},
    {.name = "cont4_key_cleft",      .type = CONFIG_TYPE_UINT, .uintValue = &configKeyCLeft[3]},
    {.name = "cont4_key_cright",     .type = CONFIG_TYPE_UINT, .uintValue = &configKeyCRight[3]},
    {.name = "cont4_key_stickup",    .type = CONFIG_TYPE_UINT, .uintValue = &configKeyStickUp[3]},
    {.name = "cont4_key_stickdown",  .type = CONFIG_TYPE_UINT, .uintValue = &configKeyStickDown[3]},
    {.name = "cont4_key_stickleft",  .type = CONFIG_TYPE_UINT, .uintValue = &configKeyStickLeft[3]},
    {.name = "cont4_key_stickright", .type = CONFIG_TYPE_UINT, .uintValue = &configKeyStickRight[3]},
    {.name = "cont4_key_joyup",      .type = CONFIG_TYPE_UINT, .uintValue = &configKeyJoyUp[3]},
    {.name = "cont4_key_joydown",    .type = CONFIG_TYPE_UINT, .uintValue = &configKeyJoyDown[3]},
    {.name = "cont4_key_joyleft",    .type = CONFIG_TYPE_UINT, .uintValue = &configKeyJoyLeft[3]},
    {.name = "cont4_key_joyright",   .type = CONFIG_TYPE_UINT, .uintValue = &configKeyJoyRight[3]},
};

// Reads an entire line from a file (excluding the newline character) and returns an allocated string
// Returns NULL if no lines could be read from the file
static char *read_file_line(FILE *file)
{
    char *buffer;
    size_t bufferSize = 8;
    size_t offset = 0; // offset in buffer to write

    buffer = malloc(bufferSize);
    while (1)
    {
        // Read a line from the file
        if (fgets(buffer + offset, bufferSize - offset, file) == NULL)
        {
            free(buffer);
            return NULL; // Nothing could be read.
        }
        offset = strlen(buffer);
        assert(offset > 0);

        // If a newline was found, remove the trailing newline and exit
        if (buffer[offset - 1] == '\n')
        {
            buffer[offset - 1] = '\0';
            break;
        }

        if (feof(file)) // EOF was reached
            break;

        // If no newline or EOF was reached, then the whole line wasn't read.
        bufferSize *= 2; // Increase buffer size
        buffer = realloc(buffer, bufferSize);
        assert(buffer != NULL);
    }

    return buffer;
}

// Returns the position of the first non-whitespace character
static char *skip_whitespace(char *str)
{
    while (isspace(*str))
        str++;
    return str;
}

// NULL-terminates the current whitespace-delimited word, and returns a pointer to the next word
static char *word_split(char *str)
{
    // Precondition: str must not point to whitespace
    assert(!isspace(*str));

    // Find either the next whitespace char or end of string
    while (!isspace(*str) && *str != '\0')
        str++;
    if (*str == '\0') // End of string
        return str;

    // Terminate current word
    *(str++) = '\0';

    // Skip whitespace to next word
    return skip_whitespace(str);
}

// Splits a string into words, and stores the words into the 'tokens' array
// 'maxTokens' is the length of the 'tokens' array
// Returns the number of tokens parsed
static unsigned int tokenize_string(char *str, int maxTokens, char **tokens)
{
    int count = 0;

    str = skip_whitespace(str);
    while (str[0] != '\0' && count < maxTokens)
    {
        tokens[count] = str;
        str = word_split(str);
        count++;
    }
    return count;
}

// Loads the config file specified by 'filename'
void configfile_load(const char *filename)
{
    FILE *file;
    char *line;

    printf("Loading configuration from '%s'\n", filename);

    file = fopen(filename, "r");
    if (file == NULL)
    {
        // Create a new config file and save defaults
        printf("Config file '%s' not found. Creating it.\n", filename);
        configfile_save(filename);
        return;
    }

    // Go through each line in the file
    while ((line = read_file_line(file)) != NULL)
    {
        char *p = line;
        char *tokens[2];
        int numTokens;

        while (isspace(*p))
            p++;
        numTokens = tokenize_string(p, 2, tokens);
        if (numTokens != 0)
        {
            if (numTokens == 2)
            {
                const struct ConfigOption *option = NULL;

                for (unsigned int i = 0; i < ARRAY_LEN(options); i++)
                {
                    if (strcmp(tokens[0], options[i].name) == 0)
                    {
                        option = &options[i];
                        break;
                    }
                }
                if (option == NULL)
                    printf("unknown option '%s'\n", tokens[0]);
                else
                {
                    switch (option->type)
                    {
                    case CONFIG_TYPE_BOOL:
                        if (strcmp(tokens[1], "true") == 0)
                            *option->boolValue = true;
                        else if (strcmp(tokens[1], "false") == 0)
                            *option->boolValue = false;
                        break;
                    case CONFIG_TYPE_UINT:
                        sscanf(tokens[1], "%u", option->uintValue);
                        break;
                    case CONFIG_TYPE_FLOAT:
                        sscanf(tokens[1], "%f", option->floatValue);
                        break;
                    default:
                        assert(0); // bad type
                    }
                    printf("option: '%s', value: '%s'\n", tokens[0], tokens[1]);
                }
            }
            else
                puts("error: expected value");
        }
        free(line);
    }

    fclose(file);
}

// Writes the config file to 'filename'
void configfile_save(const char *filename)
{
    FILE *file;

    printf("Saving configuration to '%s'\n", filename);

    file = fopen(filename, "w");
    if (file == NULL)
    {
        // error
        return;
    }

    for (unsigned int i = 0; i < ARRAY_LEN(options); i++)
    {
        const struct ConfigOption *option = &options[i];

        switch (option->type)
        {
        case CONFIG_TYPE_BOOL:
            fprintf(file, "%s %s\n", option->name, *option->boolValue ? "true" : "false");
            break;
        case CONFIG_TYPE_UINT:
            fprintf(file, "%s %u\n", option->name, *option->uintValue);
            break;
        case CONFIG_TYPE_FLOAT:
            fprintf(file, "%s %f\n", option->name, *option->floatValue);
            break;
        default:
            assert(0); // unknown type
        }
    }

    fclose(file);
}
