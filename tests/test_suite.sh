#!/usr/bin/env bash
# =============================================================================
# test_suite.sh — End-to-end OCR pipeline tests
#
# Usage:
#   ./test_suite.sh              run all tests
#   ./test_suite.sh -v           verbose (show actual output)
#   ./test_suite.sh -k alphabet  run only tests whose name matches "alphabet"
# =============================================================================

set -euo pipefail

# ── Config ────────────────────────────────────────────────────────────────────
ROOT="$(cd "$(dirname "$0")" && pwd)"
TREATMENT="$ROOT/Treatment"
XOR="$ROOT/XOR"
INTERFACE="$ROOT/Interface"
TESTS_DIR="$TREATMENT/tests"
RESULT_FILE="$INTERFACE/finalresult.txt"
VERBOSE=0
FILTER=""

# ── Arg parsing ───────────────────────────────────────────────────────────────
while [[ $# -gt 0 ]]; do
    case "$1" in
        -v|--verbose) VERBOSE=1 ;;
        -k|--filter)  FILTER="$2"; shift ;;
        *) echo "Unknown option: $1"; exit 1 ;;
    esac
    shift
done

# ── Colours ───────────────────────────────────────────────────────────────────
GREEN="\033[0;32m"
RED="\033[0;31m"
YELLOW="\033[0;33m"
CYAN="\033[0;36m"
RESET="\033[0m"

# ── Counters ──────────────────────────────────────────────────────────────────
TOTAL=0
PASSED=0
FAILED=0
SKIPPED=0

# ── Helpers ───────────────────────────────────────────────────────────────────
pass() { echo -e "  ${GREEN}PASS${RESET}  $1"; ((PASSED++)); ((TOTAL++)); }
fail() { echo -e "  ${RED}FAIL${RESET}  $1"; ((FAILED++)); ((TOTAL++)); }
skip() { echo -e "  ${YELLOW}SKIP${RESET}  $1"; ((SKIPPED++)); }
info() { echo -e "${CYAN}$1${RESET}"; }

should_run() {
    [[ -z "$FILTER" ]] || [[ "$1" == *"$FILTER"* ]]
}

# Run seg + launcher on an image, return 0 if result file is produced
run_pipeline() {
    local img="$1"
    cd "$INTERFACE"

    # Segmentation writes to ../Treatment/.car
    "$TREATMENT/main" "$img" 2>/dev/null || true

    # Neural network reads .car, writes finalresult.txt
    "$XOR/main" "../Treatment/.car" 2>/dev/null || true

    [[ -f "$RESULT_FILE" ]]
}

# =============================================================================
# Section 1 — Build checks
# =============================================================================
info "\n── Build checks ─────────────────────────────────────────────────────────"

NAME="libtreatment.a exists"
should_run "$NAME" && {
    if [[ -f "$TREATMENT/libtreatment.a" ]]; then
        pass "$NAME"
    else
        fail "$NAME — build first: cd Treatment && make && cd ../XOR && make" 
    fi
}

NAME="libxor.a exists"
should_run "$NAME" && {
    if [[ -f "$XOR/libxor.a" ]]; then
        pass "$NAME"
    else
        fail "$NAME — run: cd XOR && make"
    fi
}

NAME="interface binary exists"
should_run "$NAME" && {
    if [[ -f "$INTERFACE/4puterscanread" ]]; then
        pass "$NAME"
    else
        fail "$NAME — run: cd Interface && make"
    fi
}

NAME="Treatment main binary exists"
should_run "$NAME" && {
    if [[ -f "$TREATMENT/main" ]]; then
        pass "$NAME"
    else
        fail "$NAME"
    fi
}

NAME="XOR main binary exists"
should_run "$NAME" && {
    if [[ -f "$XOR/main" ]]; then
        pass "$NAME"
    else
        fail "$NAME"
    fi
}

# =============================================================================
# Section 2 — Input validation
# =============================================================================
info "\n── Input validation ─────────────────────────────────────────────────────"

NAME="all test images are readable"
should_run "$NAME" && {
    ok=1
    for img in "$TESTS_DIR"/*.png; do
        [[ -r "$img" ]] || { echo "    missing: $img"; ok=0; }
    done
    [[ $ok -eq 1 ]] && pass "$NAME" || fail "$NAME"
}

NAME="test images are valid PNG (not truncated)"
should_run "$NAME" && {
    ok=1
    for img in "$TESTS_DIR"/*.png; do
        # PNG magic bytes: 89 50 4E 47
        header=$(xxd -p -l 4 "$img" 2>/dev/null || true)
        [[ "$header" == "89504e47" ]] || { echo "    bad header: $img"; ok=0; }
    done
    [[ $ok -eq 1 ]] && pass "$NAME" || fail "$NAME"
}

# =============================================================================
# Section 3 — Pipeline smoke tests (one per test image)
# =============================================================================
info "\n── Pipeline: per-image smoke tests ─────────────────────────────────────"

for img in "$TESTS_DIR"/*.png; do
    imgname="$(basename "$img" .png)"

    NAME="pipeline completes: $imgname"
    should_run "$NAME" || { skip "$NAME"; continue; }

    if run_pipeline "$img"; then
        pass "$NAME"
        [[ $VERBOSE -eq 1 ]] && echo "    output: $(cat "$RESULT_FILE" | head -3 | tr '\n' ' ')"
    else
        fail "$NAME — result file not produced"
    fi
done

# =============================================================================
# Section 4 — Output quality checks
# =============================================================================
info "\n── Output quality ───────────────────────────────────────────────────────"

NAME="result file is not empty after alphabet.png"
should_run "$NAME" && {
    run_pipeline "$TESTS_DIR/alphabet.png" 2>/dev/null || true
    if [[ -s "$RESULT_FILE" ]]; then
        pass "$NAME"
    else
        fail "$NAME"
    fi
}

NAME="result file contains only printable characters"
should_run "$NAME" && {
    if [[ -f "$RESULT_FILE" ]]; then
        if LC_ALL=C grep -qP '[^\x09\x0a\x0d\x20-\x7e]' "$RESULT_FILE" 2>/dev/null; then
            fail "$NAME — non-printable bytes found"
        else
            pass "$NAME"
        fi
    else
        skip "$NAME — result file missing"
    fi
}

NAME="result file does not exceed 1 MB (sanity)"
should_run "$NAME" && {
    if [[ -f "$RESULT_FILE" ]]; then
        size=$(wc -c < "$RESULT_FILE")
        [[ $size -lt 1048576 ]] && pass "$NAME" || fail "$NAME (${size} bytes)"
    else
        skip "$NAME — result file missing"
    fi
}

NAME="Lorem_2.png output contains at least 20 characters"
should_run "$NAME" && {
    run_pipeline "$TESTS_DIR/Lorem_2.png" 2>/dev/null || true
    if [[ -f "$RESULT_FILE" ]]; then
        len=$(wc -c < "$RESULT_FILE")
        [[ $len -ge 20 ]] && pass "$NAME" || fail "$NAME (got $len chars)"
    else
        fail "$NAME — result file missing"
    fi
}

NAME="alphabet.png output contains at least 10 characters"
should_run "$NAME" && {
    run_pipeline "$TESTS_DIR/alphabet.png" 2>/dev/null || true
    if [[ -f "$RESULT_FILE" ]]; then
        len=$(wc -c < "$RESULT_FILE")
        [[ $len -ge 10 ]] && pass "$NAME" || fail "$NAME (got $len chars)"
    else
        fail "$NAME — result file missing"
    fi
}

# =============================================================================
# Section 5 — Interface logic (headless, no display needed)
# =============================================================================
info "\n── Interface: file & binary checks ─────────────────────────────────────"

NAME="style.css exists"
should_run "$NAME" && {
    [[ -f "$INTERFACE/style.css" ]] && pass "$NAME" || fail "$NAME"
}

NAME="glade file exists"
should_run "$NAME" && {
    [[ -f "$INTERFACE/4puterscanread.glade" ]] && pass "$NAME" || fail "$NAME"
}

NAME="glade file is valid XML"
should_run "$NAME" && {
    if command -v xmllint &>/dev/null; then
        xmllint --noout "$INTERFACE/4puterscanread.glade" 2>/dev/null \
            && pass "$NAME" || fail "$NAME — malformed XML"
    else
        skip "$NAME — xmllint not installed"
    fi
}

NAME="finalresult.txt is writable by current user"
should_run "$NAME" && {
    touch "$RESULT_FILE" 2>/dev/null && pass "$NAME" || fail "$NAME"
}

# =============================================================================
# Summary
# =============================================================================
echo ""
echo "══════════════════════════════════════"
echo -e "  Total:   $TOTAL"
echo -e "  ${GREEN}Passed:  $PASSED${RESET}"
[[ $FAILED  -gt 0 ]] && echo -e "  ${RED}Failed:  $FAILED${RESET}"  || echo "  Failed:  $FAILED"
[[ $SKIPPED -gt 0 ]] && echo -e "  ${YELLOW}Skipped: $SKIPPED${RESET}" || echo "  Skipped: $SKIPPED"
echo "══════════════════════════════════════"
echo ""

[[ $FAILED -eq 0 ]]
