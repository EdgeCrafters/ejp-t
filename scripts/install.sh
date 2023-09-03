#!/bin/bash

# Exit on any error
set -e

# Update package lists
sudo apt update

# Install prerequisites
sudo apt install -y git cmake g++ pkg-config libkrb5-dev libssl-dev python3

# # Clone the repository
# git clone https://github.com/EdgeCrafters/ejp-t.git
# # Navigate to the project directory
# cd ejp-t

# Create a build directory and navigate into it
mkdir build && cd build

# Run CMake to generate the Makefile
cmake ..

# Compile the project
make

# Optional: Run tests if any (Uncomment the line below if you wish to run tests)
# make test

# Optional: Install the compiled binary to a standard location (Uncomment the lines below if needed)
# sudo make install

# Create a symlink to the application in /usr/local/bin
# This will make it easier to run the application from anywhere.
# Replace "path_to_your_executable" with the actual path.

# Add EJP-T as an environment variable
if [[ $SHELL == *"bash"* ]]; then
  echo "export PATH=PATH:$(pwd)/src" >> ~/.bashrc
  source ~/.bashrc
elif [[ $SHELL == *"zsh"* ]]; then
  echo "export EJP_T=PATH:$(pwd)/src" >> ~/.zshrc
  source ~/.zshrc
else
  echo "Unsupported shell. Please add manually."
fi

echo "Installation completed successfully."

