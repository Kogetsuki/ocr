/*
 * test_xor.c — Unit tests for the XOR neural network module
 * Build: gcc test_xor.c -o test_xor -I../XOR ../XOR/libxor.a -lm
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>

#include "../XOR/XOR.h"
#include "../XOR/launch.h"

/* ========================================================================
 * Harness
 * ======================================================================== */

static int tests_run    = 0;
static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) static void name(void)

#define RUN(name) do {                                  \
    tests_run++;                                        \
    printf("  %-52s", #name);                           \
    name();                                             \
    printf("PASS\n");                                   \
    tests_passed++;                                     \
} while (0)

#define EXPECT(cond) do {                                               \
    if (!(cond)) {                                                      \
        printf("FAIL\n    assertion: %s  (%s:%d)\n",                   \
               #cond, __FILE__, __LINE__);                              \
        tests_failed++;                                                 \
        return;                                                         \
    }                                                                   \
} while (0)

/* ========================================================================
 * Network constants (from XOR/main.c)
 * ======================================================================== */

#define NUM_INPUT   256
#define NUM_HIDDEN  300
#define NUM_OUTPUT   59
#define NUM_PATTERN (59 * 20)

/* ========================================================================
 * Helpers
 * ======================================================================== */

static int path_exists(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0;
}

/* Allocate a (rows x cols) double matrix */
static double **alloc_matrix(int rows, int cols)
{
    double **m = malloc(rows * sizeof(double *));
    for (int i = 0; i < rows; i++)
        m[i] = calloc(cols, sizeof(double));
    return m;
}

static void free_matrix(double **m, int rows)
{
    for (int i = 0; i < rows; i++) free(m[i]);
    free(m);
}

/* ========================================================================
 * Transform() tests
 * Maps output index 0-58 to a printable character
 * ======================================================================== */

TEST(test_transform_first_class_is_printable)
{
    char c = Transform(0);
    EXPECT(c >= 32 && c <= 126);
}

TEST(test_transform_last_class_is_printable)
{
    char c = Transform(58);
    EXPECT(c >= 32 && c <= 126);
}

TEST(test_transform_all_classes_printable)
{
    for (int k = 0; k < NUM_OUTPUT; k++)
    {
        char c = Transform(k);
        EXPECT(c >= 32 && c <= 126);
    }
}

TEST(test_transform_covers_uppercase)
{
    /* At least one index must map to an uppercase letter */
    int found = 0;
    for (int k = 0; k < NUM_OUTPUT; k++)
    {
        char c = Transform(k);
        if (c >= 'A' && c <= 'Z') { found = 1; break; }
    }
    EXPECT(found);
}

TEST(test_transform_covers_lowercase)
{
    int found = 0;
    for (int k = 0; k < NUM_OUTPUT; k++)
    {
        char c = Transform(k);
        if (c >= 'a' && c <= 'z') { found = 1; break; }
    }
    EXPECT(found);
}

TEST(test_transform_no_duplicate_mapping)
{
    /* Each index should produce a unique character
     * (59 classes, each mapped to a distinct char) */
    char seen[128] = {0};
    for (int k = 0; k < NUM_OUTPUT; k++)
    {
        char c = Transform(k);
        EXPECT((unsigned char)c < 128);
        EXPECT(seen[(unsigned char)c] == 0);
        seen[(unsigned char)c] = 1;
    }
}

/* ========================================================================
 * InitialiseTarget() tests
 * ======================================================================== */

TEST(test_target_one_hot_encoding)
{
    double **Target = alloc_matrix(NUM_PATTERN, NUM_OUTPUT);

    InitialiseTarget(Target, NUM_PATTERN, NUM_OUTPUT);

    /* For each pattern, exactly one output must be 1, rest 0 */
    for (int i = 0; i < NUM_PATTERN; i++)
    {
        int ones = 0;
        for (int j = 0; j < NUM_OUTPUT; j++)
        {
            EXPECT(Target[i][j] == 0.0 || Target[i][j] == 1.0);
            if (Target[i][j] == 1.0) ones++;
        }
        EXPECT(ones == 1);
    }

    free_matrix(Target, NUM_PATTERN);
}

TEST(test_target_hot_index_matches_pattern_mod_output)
{
    double **Target = alloc_matrix(NUM_PATTERN, NUM_OUTPUT);

    InitialiseTarget(Target, NUM_PATTERN, NUM_OUTPUT);

    for (int i = 0; i < NUM_PATTERN; i++)
    {
        int expected_hot = i % NUM_OUTPUT;
        EXPECT(Target[i][expected_hot] == 1.0);
    }

    free_matrix(Target, NUM_PATTERN);
}

/* ========================================================================
 * RandomInit() tests
 * ======================================================================== */

TEST(test_random_init_weights_not_all_zero)
{
    double **WeightIH = alloc_matrix(NUM_INPUT + 1, NUM_HIDDEN);
    double **WeightHO = alloc_matrix(NUM_HIDDEN + 1, NUM_OUTPUT);

    RandomInit(WeightIH, WeightHO, NUM_INPUT, NUM_HIDDEN, NUM_OUTPUT);

    int any_nonzero = 0;
    for (int i = 0; i <= NUM_INPUT && !any_nonzero; i++)
        for (int j = 0; j < NUM_HIDDEN; j++)
            if (WeightIH[i][j] != 0.0) { any_nonzero = 1; break; }

    free_matrix(WeightIH, NUM_INPUT + 1);
    free_matrix(WeightHO, NUM_HIDDEN + 1);

    EXPECT(any_nonzero);
}

TEST(test_random_init_weights_in_range)
{
    double **WeightIH = alloc_matrix(NUM_INPUT + 1, NUM_HIDDEN);
    double **WeightHO = alloc_matrix(NUM_HIDDEN + 1, NUM_OUTPUT);

    RandomInit(WeightIH, WeightHO, NUM_INPUT, NUM_HIDDEN, NUM_OUTPUT);

    /* Weights should be initialised in [-0.5, 0.5] as per the docstring */
    for (int i = 0; i <= NUM_INPUT; i++)
        for (int j = 0; j < NUM_HIDDEN; j++)
            EXPECT(WeightIH[i][j] >= -0.5 && WeightIH[i][j] <= 0.5);

    for (int j = 0; j <= NUM_HIDDEN; j++)
        for (int k = 0; k < NUM_OUTPUT; k++)
            EXPECT(WeightHO[j][k] >= -0.5 && WeightHO[j][k] <= 0.5);

    free_matrix(WeightIH, NUM_INPUT + 1);
    free_matrix(WeightHO, NUM_HIDDEN + 1);
}

TEST(test_random_init_two_calls_differ)
{
    double **WIH1 = alloc_matrix(NUM_INPUT + 1, NUM_HIDDEN);
    double **WHO1 = alloc_matrix(NUM_HIDDEN + 1, NUM_OUTPUT);
    double **WIH2 = alloc_matrix(NUM_INPUT + 1, NUM_HIDDEN);
    double **WHO2 = alloc_matrix(NUM_HIDDEN + 1, NUM_OUTPUT);

    RandomInit(WIH1, WHO1, NUM_INPUT, NUM_HIDDEN, NUM_OUTPUT);
    RandomInit(WIH2, WHO2, NUM_INPUT, NUM_HIDDEN, NUM_OUTPUT);

    int differ = 0;
    for (int i = 0; i <= NUM_INPUT && !differ; i++)
        for (int j = 0; j < NUM_HIDDEN; j++)
            if (WIH1[i][j] != WIH2[i][j]) { differ = 1; break; }

    free_matrix(WIH1, NUM_INPUT + 1); free_matrix(WHO1, NUM_HIDDEN + 1);
    free_matrix(WIH2, NUM_INPUT + 1); free_matrix(WHO2, NUM_HIDDEN + 1);

    EXPECT(differ);
}

/* ========================================================================
 * launcher() / Run() integration tests
 * These require that seg() has already been run to produce .car
 * ======================================================================== */

TEST(test_launcher_does_not_crash)
{
    if (!path_exists("../Treatment/.car"))
    {
        printf("SKIP (run seg first) ");
        tests_run--; /* don't count as a real run */
        return;
    }
    launcher("../Treatment/.car");
    EXPECT(1);
}

TEST(test_launcher_writes_result_file)
{
    if (!path_exists("../Treatment/.car"))
    {
        printf("SKIP (run seg first) ");
        tests_run--;
        return;
    }
    launcher("../Treatment/.car");

    FILE *f = fopen("finalresult.txt", "r");
    int exists = (f != NULL);
    if (f) fclose(f);
    EXPECT(exists);
}

TEST(test_result_file_not_empty)
{
    FILE *f = fopen("finalresult.txt", "r");
    if (!f) { EXPECT(0); }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fclose(f);
    EXPECT(size > 0);
}

TEST(test_result_file_only_printable_chars)
{
    FILE *f = fopen("finalresult.txt", "r");
    if (!f) { EXPECT(0); }

    int c;
    while ((c = fgetc(f)) != EOF)
    {
        int ok = (c >= 32 && c <= 126) || c == '\n' || c == '\r';
        if (!ok) { fclose(f); EXPECT(0); }
    }
    fclose(f);
    EXPECT(1);
}

TEST(test_run_returns_printable_char)
{
    double **WeightIH = alloc_matrix(NUM_INPUT + 1, NUM_HIDDEN);
    double **WeightHO = alloc_matrix(NUM_HIDDEN + 1, NUM_OUTPUT);

    /* Load real trained weights so the output is meaningful */
    FileParser1(WeightIH, WeightHO, NUM_INPUT, NUM_HIDDEN, NUM_OUTPUT);

    double input[NUM_INPUT];
    memset(input, 0, sizeof(input));

    char result = Run(input, WeightIH, WeightHO, NUM_INPUT, NUM_HIDDEN, NUM_OUTPUT);

    free_matrix(WeightIH, NUM_INPUT + 1);
    free_matrix(WeightHO, NUM_HIDDEN + 1);

    EXPECT(result >= 32 && result <= 126);
}

/* ========================================================================
 * Entry point
 * ======================================================================== */

int main(void)
{
    printf("\n=== XOR Neural Network Unit Tests ===\n\n");

    printf("-- Transform() character mapping --\n");
    RUN(test_transform_first_class_is_printable);
    RUN(test_transform_last_class_is_printable);
    RUN(test_transform_all_classes_printable);
    RUN(test_transform_covers_uppercase);
    RUN(test_transform_covers_lowercase);
    RUN(test_transform_no_duplicate_mapping);

    printf("\n-- InitialiseTarget() --\n");
    RUN(test_target_one_hot_encoding);
    RUN(test_target_hot_index_matches_pattern_mod_output);

    printf("\n-- RandomInit() --\n");
    RUN(test_random_init_weights_not_all_zero);
    RUN(test_random_init_weights_in_range);
    RUN(test_random_init_two_calls_differ);

    printf("\n-- launcher() / Run() integration --\n");
    RUN(test_launcher_does_not_crash);
    RUN(test_launcher_writes_result_file);
    RUN(test_result_file_not_empty);
    RUN(test_result_file_only_printable_chars);
    RUN(test_run_returns_printable_char);

    printf("\n======================================\n");
    printf("Results: %d/%d passed", tests_passed, tests_run);
    if (tests_failed > 0)
        printf("  (%d FAILED)", tests_failed);
    printf("\n\n");

    return tests_failed > 0 ? 1 : 0;
}
