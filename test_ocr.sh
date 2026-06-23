#!/bin/bash

# Automated test automatisé
# Usage : ./test_ocr.sh [option]
# Options : all, deps, compile, run, clean, test

set -e

# Colors for display
RED=$'\033[0;31m'
GREEN=$'\033[0;32m'
YELLOW=$'\033[1;33m'
BLUE=$'\033[0;34m'
NC=$'\033[0m' # No Color

# Variables
PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
INTERFACE_DIR="$PROJECT_DIR/Interface"
TREATMENT_DIR="$PROJECT_DIR/Treatment"
XOR_DIR="$PROJECT_DIR/XOR"

# Display functions
print_header() {
  echo -e "${BLUE}===================================================${NC}"
  echo -e "${BLUE}$1${NC}"
  echo -e "${BLUE}===================================================${NC}"
}

print_success() {
  echo -e "${GREEN}✓ $1${NC}"
}

print_error() {
  echo -e "${RED}✗ $1${NC}"
}

print_warning() {
  echo -e "${YELLOW}⚠ $1${NC}"
}

# 1. Check dependencies
check_dependencies() {
  print_header "Checking dependencies"
  
  local missing_deps=0
  
  # Check compilation tools
  for tool in gcc make pkg-config; do
    if command -v $tool &> /dev/null; then
      print_success "$tool installed"
    else
      print_error "$tool missing"
      missing_deps=1
    fi
  done
  
  # Check GTK+ 3.0
  if pkg-config --exists gtk+-3.0; then
    print_success "GTK+ 3.0 installed"
  else
    print_error "GTK+ 3.0 missing"
    missing_deps=1
  fi
  
  # Check SDL
  if pkg-config --exists sdl; then
    print_success "SDL installed"
  else
    print_error "SDL missing"
    missing_deps=1
  fi
  
  # Check SDL_image
  if ldconfig -p | grep -q libSDL_image; then
    print_success "SDL_image installed"
  else
    print_warning "SDL_image might be missing"
  fi
  
  if [ $missing_deps -eq 1 ]; then
    echo ""
    print_warning "Installing missing dependencies..."
    sudo apt update
    sudo apt install -y \
      build-essential make gcc pkg-config \
      libgtk-3-dev libglib2.0-dev \
      libsdl1.2-dev libsdl-image1.2-dev
    print_success "Dependencies installed"
  fi
  
  echo ""
}

# 2. Clean builds
clean_all() {
  print_header "Cleaning compiled files"
  
  echo "Cleaning Treatment..."
  cd "$TREATMENT_DIR"
  make clean || true
  
  echo "Cleaning XOR..."
  cd "$XOR_DIR"
  make clean || true
  
  echo "Cleaning Interface..."
  cd "$INTERFACE_DIR"
  make fclean || true
  
  print_success "Cleaning completed"
  echo ""
}

# 3. Compile project
compile_project() {
  print_header "Compiling project"
  
  # Compile base modules
  echo "Compiling Treatment module..."
  cd "$TREATMENT_DIR"
  if make; then
    print_success "Treatment compiled"
  else
    print_error "Error while compiling Treatment"
    return 1
  fi
  
  echo ""
  echo "Compiling XOR module..."
  cd "$XOR_DIR"
  if make; then
    print_success "XOR compiled"
  else
    print_error "Error while compiling XOR"
    return 1
  fi
  
  echo ""
  echo "Compiling Interface module..."
  cd "$INTERFACE_DIR"
  if make; then
    print_success "Interface compiled"
  else
    print_error "Error while compiling Interface"
    return 1
  fi
  
  echo ""
  
  # Check the exe was created
  if [ -f "$INTERFACE_DIR/4puterscanread" ]; then
    print_success "$INTERFACE_DIR/4puterscanread executable created"
  else
    print_error "Executable not found"
    return 1
  fi
  
  echo ""
}

# 4. Configuer WSL display
setup_display() {
  print_header "Configuring WSL display"
  
  if [ -z "$DISPLAY" ]; then
    print_warning "DISPLAY not defined"
    echo "Configurating for WSL2..."
    export DISPLAY=$(cat /etc/hostname):0
    print_success "DISPLAY defined here : $DISPLAY"
  else
    print_success "DISPLAY alreday defined : $DISPLAY"
  fi
  
  echo ""
}

# 5. Launch UI
run_interface() {
  print_header "Launching UI"
  
  setup_display
  
  if [ ! -f "$INTERFACE_DIR/4puterscanread" ]; then
    print_error "Executable not found. Compile first with : $0 compile"
    return 1
  fi
  
  echo "Launching from : $INTERFACE_DIR"
  cd "$INTERFACE_DIR"
  
  if [ ! -f "4puterscanread.glade" ]; then
    print_error "4puterscanread.glade file not found"
    return 1
  fi
  
  print_success "Fichier .glade found"
  echo ""
  echo "Starting app..."
  echo "(UI should open in a new window)"
  echo ""
  
  ./4puterscanread
}

# 6. Help menu
show_help() {
  cat << EOF
${BLUE}OCR 4puterscanread - WSL Testing Script${NC}

${YELLOW}Usage:${NC}
  $0 [option]

${YELLOW}Options:${NC}
  all       - Execute all steps (deps, compile, run)
  deps      - Check and install dependencies
  compile   - Compile all modules
  run       - Launch UI
  clean     - Clean compiled files
  help      - Display this help

${YELLOW}Important:${NC}
  • Executer from root folder
  • WSL2 with WSLg or X11 config needed
  • Use 'sudo' to install dependencies

EOF
}

# Main
main() {
  if [ $# -eq 0 ]; then
    show_help
    return 0
  fi
  
  case "$1" in
    deps)
      check_dependencies
      ;;
    compile)
      check_dependencies
      clean_all
      compile_project
      ;;
    run)
      run_interface
      ;;
    clean)
      clean_all
      ;;
    all)
      check_dependencies
      clean_all
      compile_project
      run_interface
      ;;
    help|-h|--help)
      show_help
      ;;
    *)
      print_error "Unknown option : $1"
      echo ""
      show_help
      return 1
      ;;
  esac
}

# Execute main with all args
main "$@"
