const http = require('http'); // Use 'https' if your target URL is an HTTPS endpoint

// Configuration for the HTTP request
const options = {
  host: 'localhost',
  port: 8000,
  path: '/',
  method: 'POST',
  headers: {
    'Content-Type': 'application/octet-stream',
  }
};

// Create the request object
const req = http.request(options, (res) => {
  console.log(`STATUS: ${res.statusCode}`);
  res.setEncoding('utf8');
  res.on('data', (chunk) => {
    console.log(`BODY: ${chunk}`);
  });
  res.on('end', () => {
    console.log('No more data in response.');
  });
});

req.on('error', (e) => {
  console.error(`problem with request: ${e.message}`);
});

// Function to generate 1KB of random data
function generate1KBData() {
  return Buffer.alloc(1024, 'a'); // Generates 1KB buffer filled with the letter 'a'
}

// Write data to the request body
setInterval(() => {
  const data = generate1KBData();
  console.log('Writing 1KB of data...');
  req.write(data); // Stream the data to the request body
}, 1000); // Do this every 1000 milliseconds (1 second)

// To keep the example simple, we'll close the request after some time
setTimeout(() => {
  req.end();
  console.log('Request ended');
}, 10000); // Adjust the time as needed, 10000 milliseconds (10 seconds) for this example
