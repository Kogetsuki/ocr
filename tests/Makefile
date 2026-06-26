# =============================================================================
# tests/Makefile
# =============================================================================

CC      = gcc
CFLAGS  = -Wall -Wextra -g
LDFLAGS = -lm

TREATMENT_INC = -I../Treatment
TREATMENT_LIB = ../Treatment/libtreatment.a
TREATMENT_PKG = $(shell pkg-config --cflags --libs sdl) -lSDL_image

XOR_INC = -I../XOR
XOR_LIB = ../XOR/libxor.a

.PHONY: all clean unit e2e

all: unit e2e

# ── Unit test binaries ────────────────────────────────────────────────────────

test_treatment: test_treatment.c $(TREATMENT_LIB)
	$(CC) $(CFLAGS) $< -o $@ $(TREATMENT_INC) $(TREATMENT_LIB) $(TREATMENT_PKG) $(LDFLAGS)

test_xor: test_xor.c $(XOR_LIB)
	$(CC) $(CFLAGS) $< -o $@ $(XOR_INC) $(XOR_LIB) $(LDFLAGS)

# ── Run targets ───────────────────────────────────────────────────────────────

unit: test_treatment test_xor
	@echo ""
	@echo "Running Treatment unit tests..."
	@./test_treatment
	@echo "Running XOR unit tests..."
	@./test_xor

e2e:
	@echo ""
	@echo "Running end-to-end test suite..."
	@chmod +x test_suite.sh
	@./test_suite.sh

clean:
	rm -f test_treatment test_xor
