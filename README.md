
# HTTP Server Implementation in C

## Overview

This project implements a simple HTTP server in C that handles client requests and provides basic functionalities, such as serving files, listing directory contents, running CGI scripts, and returning appropriate HTTP error messages. The server is designed to run on a specified IP address and port and supports concurrent client connections.

---

## Features

1. **Directory Listing**:
   - If the requested resource is a directory, the server lists its contents in the response.

2. **File Serving**:
   - If the requested resource is a regular file, the server returns its contents to the client.

3. **CGI Script Execution**:
   - If the requested resource is a CGI script, the server executes the script and sends the output back to the client.

4. **Error Handling**:
   - Returns proper HTTP error responses (e.g., 404 Not Found) for invalid or non-existent resources.

5. **Default Favicon Handling**:
   - Handles `/favicon.ico` requests to prevent unnecessary 404 errors in logs.

---

## How It Works

The server performs the following steps:

1. Creates a socket to listen for client connections.
2. Binds the socket to a specified IP address and port (default: `8080`).
3. Listens for incoming connections.
4. Accepts client connections in a loop and processes requests using `Handle_Client_Request`.

The request processing function:
- Identifies the requested resource.
- Serves files or directories, executes scripts, or returns error responses.

---

## Setup and Installation

### Prerequisites
- A Linux-based system or any system with POSIX socket support.
- GCC (GNU Compiler Collection) or any compatible C compiler.

### Compilation
Compile the server code using `gcc`:
```bash
gcc -o http_server server.c
```

### Usage
Run the server with the following command:
```bash
./http_server <IP Address>
```
- Replace `<IP Address>` with the IP address of the host machine.

### Example
```bash
./http_server 127.0.0.1
```
This starts the server on localhost (127.0.0.1) and listens on port 8080.

---

## Testing the Server

1. **Accessing in a Browser**:
   - Open a web browser and navigate to `http://<IP Address>:8080/`.

2. **Using `curl`**:
   - Fetch a file: 
     ```bash
     curl http://127.0.0.1:8080/filename
     ```
   - List directory contents:
     ```bash
     curl http://127.0.0.1:8080/directory/
     ```

3. **Check Error Handling**:
   - Request a non-existent file:
     ```bash
     curl http://127.0.0.1:8080/nonexistentfile
     ```

---

## File Structure

- **`server.c`**: Core implementation of the HTTP server.
- **`README.md`**: Project documentation.
- **Test Files**: Example files and directories for testing server functionality.

---

## Known Issues and Limitations

- **Thread Safety**: The server processes requests sequentially. For concurrent handling, thread or process management needs to be implemented.
- **Security**: The server does not sanitize input paths, potentially allowing directory traversal attacks. Improvements are required for production use.
- **Limited Protocol Support**: Only basic HTTP/1.1 functionality is implemented.

---

## Future Enhancements

1. Add multithreading for concurrent request handling.
2. Implement HTTPS support.
3. Improve error handling and logging.
4. Add MIME type detection for better content-type responses.

---

## Author

- **Mina Saad**

Feel free to contribute or suggest improvements!

## Video
### Video size 300MB, it is uploading......
