#!/bin/bash

# Test basic commands
echo "Testing basic commands..."
ls
pwd
echo "Hello, world!"

# Test background processes
echo "Testing background processes..."
sleep 5   # Run a sleep command in the background
echo "Background process started."

# Test foreground processes
echo "Testing foreground processes..."
date  # Run a command in the foreground
echo "Foreground process completed."

# Test input/output redirection
echo "Testing input/output redirection..."
echo "This will be redirected to a file." > output.txt
cat < input.txt  # Display contents of a file
echo "Input/output redirection test complete."

# Test built-in commands
echo "Testing built-in commands..."
cd ~  # Change directory to home directory
pwd
echo "Built-in command test complete."

# Test error handling
echo "Testing error handling..."
foobar  # Invalid command
echo "Error handling test complete."

# Test signal handling
echo "Testing signal handling..."
kill -9 $$
echo "Signal handling test complete."

# End of test script
echo "Test script complete."
