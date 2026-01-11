# CLAUDE.md - Project Guide for gneural_network

## Project Overview

GNU Neural Network (gneural_network) is a C-based neural network library implementing programmable neural networks with multiple training algorithms. Version 0.9.1.

## Build System

This project uses GNU Autotools (autoconf/automake).

### Build Commands

```bash
# Configure and build
./configure
make

# Clean build artifacts
make clean

# Install (may require sudo)
make install

# Alternative using npm scripts
npm run configure
npm run build
```

### Dependencies

- GCC compiler (`AC_PROG_CC`)
- GNU libm (math library) - required for `exp` and other math functions
- Standard C headers: memory.h, stdint.h, stdlib.h, string.h, inttypes.h

## Project Structure

```
gneural-net/
├── src/                    # Source files
│   ├── gneural_network.c   # Main entry point
│   ├── parser.c            # Script parser (largest file, ~21KB)
│   ├── network.c           # Network structure management
│   ├── feedforward.c       # Forward propagation
│   ├── activation.c        # Activation functions
│   ├── error.c             # Error/cost function calculations
│   ├── genetic_algorithm.c # GA training implementation
│   ├── simulated_annealing.c # SA training implementation
│   ├── gradient_descent.c  # Gradient descent implementation
│   ├── random_search.c     # Random search implementation
│   ├── msmco.c             # Multi-scale Monte Carlo optimization
│   ├── save.c              # Network serialization (save)
│   ├── load.c              # Network deserialization (load)
│   ├── randomize.c         # Weight randomization
│   ├── rnd.c               # Random number utilities
│   ├── binom.c             # Binomial coefficient calculation
│   └── fact.c              # Factorial calculation
├── include/                # Header files
│   ├── defines.h           # Constants and enums
│   ├── network.h           # Core data structures and API
│   ├── activation.h        # Activation function declarations
│   ├── error.h             # Error function declarations
│   └── [algorithm].h       # Training algorithm headers
├── tests/                  # Test files
│   ├── unit/               # Unit test suite (69 tests)
│   │   ├── test_framework.h
│   │   ├── test_runner.c
│   │   ├── test_math_utils.c
│   │   ├── test_activation.c
│   │   ├── test_network.c
│   │   ├── test_feedforward.c
│   │   └── Makefile
│   ├── curve_fitting_quadratic_function.input
│   ├── curve_fitting_square_root.input
│   ├── invert_order.input
│   └── modulus_of_two_dimensional_vectors.input
├── doc/                    # Documentation
├── configure.ac            # Autoconf configuration
├── Makefile.am             # Automake template
└── package.json            # npm scripts for building
```

## Key Data Structures (include/network.h)

### neuron
```c
typedef struct _neuron {
    unsigned int global_id;
    unsigned int num_input;
    struct _neuron **connection;
    enum activation_function activation;
    enum discriminant_function discriminant;
    double *w;       // weights
    double output;   // neuron output
} neuron;
```

### network
```c
typedef struct _network {
    unsigned int num_of_neurons;
    unsigned int num_of_layers;
    layer *layers;
    neuron *neurons;
} network;
```

## Constants (include/defines.h)

- `MAX_TRAINING_POINTS`: 8
- `MAX_IN`: 16 (max input connections per neuron)
- `MAX_NUM_NEURONS`: 32
- `MAX_NUM_LAYERS`: 16
- `MAX_NUM_POINTS`: 64

## Enums

### Activation Functions
- `TANH`, `EXP`, `ID`, `POL1`, `POL2`

### Discriminant Functions
- `LINEAR`, `LEGENDRE`, `LAGUERRE`, `FOURIER`

### Error Functions
- `L1`, `L2`

### Optimization Methods
- `SIMULATED_ANNEALING`, `RANDOM_SEARCH`, `GRADIENT_DESCENT`, `GENETIC_ALGORITHM`, `MSMCO`

## Running the Program

```bash
# After building
./src/gneural_network <script_file>

# Example with test file
./src/gneural_network tests/curve_fitting_quadratic_function.input
```

## Script Language Reference

Key script commands (parsed by `src/parser.c`):
- `NUMBER_OF_NEURONS <n>` - Total neurons in network
- `NEURON <id> NUMBER_OF_CONNECTIONS <n>` - Set neuron connections
- `NEURON <id> ACTIVATION <func>` - Set activation function
- `NEURON <id> DISCRIMINANT <func>` - Set discriminant function
- `NETWORK NUMBER_OF_LAYERS <n>` - Set layer count
- `NETWORK LAYER <id> NUMBER_OF_NEURONS <n>` - Set neurons per layer
- `TRAINING_METHOD <method> <params>` - Configure training
- `SAVE_OUTPUT ON/OFF` - Toggle output saving
- `OUTPUT_FILE_NAME <file>` - Set output filename
- `LOAD_NEURAL_NETWORK <file>` - Load saved network
- `SAVE_NEURAL_NETWORK <file>` - Save network

## Testing

### Unit Tests

The project includes a comprehensive unit test suite in `tests/unit/`:

```bash
# Build and run unit tests
cd tests/unit
make
./test_runner
```

**Test Coverage (69 tests):**
- Math utilities: `fact.c`, `binom.c`, `rnd.c`
- Activation functions: TANH, EXP, ID, POL1, POL2
- Network operations: allocation, neurons, layers, connections
- Feedforward propagation: single/multi-layer, all activation functions

### Integration Tests

Run example scripts in `tests/` directory to verify functionality:
```bash
./src/gneural_network tests/curve_fitting_quadratic_function.input
```

## CI/CD

GitHub Actions workflow in `.github/workflows/`:
- `c-cpp.yml` - Main CI workflow with:
  - Build project with autotools
  - Run unit tests (69 tests)
  - Run integration tests with example scripts
  - Static analysis using cppcheck
  - Build with extra compiler warnings (-Wall -Wextra -Wpedantic)

## Development Notes

### Code Quality Improvements Applied
- Replaced unsafe `strcpy()` with bounded `snprintf()` in parser.c
- Added bounds checking to `fscanf()` calls (179-char limit)
- Fixed memory leak in simulated_annealing.c error paths
- Removed obsolete `register` keywords from all source files
- Fixed bug in LOAD_NEURAL_NETWORK flag setting

### Known Limitations
- `MAX_TRAINING_POINTS`: 8 (increase in defines.h if needed)
- `MAX_NUM_NEURONS`: 32
- `MAX_NUM_LAYERS`: 16
- Filename length limited to 179 characters

## License

GPL-3.0 (see COPYING file)

## Contact

Jean Michel Sellier <jeanmichel.sellier@gmail.com>
