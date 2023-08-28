#!/bin/bash

# Exit on any error
set -e

# Update package lists
sudo apt update

# Install prerequisites
sudo apt install -y git cmake build-essential

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
sudo ln -s "$(pwd)/path_to_your_executable" /usr/local/bin/ejp-t

# Add EJP-T as an environment variable
echo 'export EJP_T="/usr/local/bin/ejp-t"' >> ~/.bashrc
source ~/.bashrc

echo "Installation completed successfully."

