# GNU Neural Network (gneural_network)

Version 0.9.1

## Overview

GNU Neural Network (gneural_network) is a free software package that implements a programmable neural network. It provides various training algorithms and a flexible architecture for building, training, and deploying neural networks.

## Features

- **Flexible Network Architecture**: Create neural networks with customizable layers and connections
- **Multiple Training Algorithms**:
  - Simulated Annealing
  - Genetic Algorithm
  - Gradient Descent
  - Random Search
  - Multi-scale Monte Carlo Optimization (MSMCO)
- **Custom Activation Functions**: TANH, EXP, ID, POL1, POL2
- **Custom Discriminant Functions**: LINEAR, LEGENDRE, LAGUERRE, FOURIER
- **Error/Cost Functions**: L1 norm, L2 norm
- **Scriptable Interface**: Define neural networks through a simple scripting language
- **Save/Load Capability**: Save and load trained network configurations

## Installation

See the file `INSTALL` for detailed building and installation instructions. Basic steps:

```bash
./configure
make
make install
```

## Usage

gneural_network uses a scripting language to define networks. Example:

```
# Total number of neurons
NUMBER_OF_NEURONS 6

# Define network structure...
NETWORK NUMBER_OF_LAYERS 3
NETWORK LAYER 0 NUMBER_OF_NEURONS 1
NETWORK LAYER 1 NUMBER_OF_NEURONS 4
NETWORK LAYER 2 NUMBER_OF_NEURONS 1

# Training method
TRAINING_METHOD SIMULATED_ANNEALING ON 25 25000 1.e-4 8.0 1.e-2

# Save output
SAVE_OUTPUT ON
OUTPUT_FILE_NAME final_results.dat
```

Run with:

```bash
gneural_network your_script.input
```

## Examples

See the `tests/` directory for example scripts:

- `curve_fitting_quadratic_function.input`: Fits a quadratic function
- `curve_fitting_square_root.input`: Fits a square root function
- `invert_order.input`: Trains a network to invert the order of numbers
- `modulus_of_two_dimensional_vectors.input`: Computes vector magnitudes

## Contributing

Even though the author has worked diligently on this project, some bugs might still be present. Contributions, bug reports, and feature requests are welcome.

## License

gneural_network is free software. See the file `COPYING` for copying conditions.

## Contact

Please send all bug reports and inquiries by electronic mail to:
jeanmichel.sellier@gmail.com