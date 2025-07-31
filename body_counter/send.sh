#!/bin/bash

# The URL where you want to send the request
url="http://localhost:8000"

# Create a temporary file and a named pipe
tempfile=$(mktemp)
mkfifo "$tempfile-pipe"
trap "rm -f $tempfile $tempfile-pipe" EXIT

# Function to generate 1KB of data
generate_1kb_data() {
    head -c 1024 < /dev/urandom
}

# Function to write data to the pipe
write_data_to_pipe() {
    while true; do
        generate_1kb_data > "$tempfile-pipe"
        sleep 1
    done
}

# Start the data writing in the background
write_data_to_pipe &

# Use curl to send the request with the pipe as the data source
curl -T "$tempfile-pipe" "$url" -v

# Cleanup: kill the background job and remove the temporary files
kill $!
wait $! 2>/dev/null
