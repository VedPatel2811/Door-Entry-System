# Door-Entry-System

**Door-Entry-System** is a real-time access control system built on the QNX Neutrino RTOS using C. It models a secure door entry mechanism with over ten states (e.g. unlock, scan, weight detection, open, close, lock) and targets sub-50ms response times under real-time constraints. The system architecture is modular: it separates display, controller, and input logic into distinct processes. Each module communicates via QNX’s synchronous message-passing API (`MsgSend`, `MsgReceive`, `MsgReply`), and uses QNX name-based IPC (`name_attach`, `name_open`) to decouple modules by service name instead of hard-coded PIDs or channel IDs.

The design was developed iteratively using Scrum practices, and has been tested to handle over 100 sequential access events reliably.

---

## 📦 Project Summary

Developed a real-time access control system using QNX Neutrino RTOS and C for Algonquin College, aimed at teaching students real-time programming. The system handled 100+ access requests with a modular design, consisting of a display, controller, and input modules. Designed a state machine with 10+ states for secure, sub-50ms response times. Leveraged IPC for efficient inter-module communication and followed Scrum practices to enhance performance and security iteratively.

---

## 🧩 Core Components

- **`des_display`**  
  - Registers the name `display` using `name_attach()`  
  - Listens for messages from the controller  
  - Displays access-related messages (e.g. ID scans, door status)  
  - Cleans up with `name_detach()` on exit

- **`des_controller`**  
  - Registers the name `controller` using `name_attach()`  
  - Connects to the display service using `name_open()`  
  - Implements the finite state machine with 10+ states  
  - Receives messages from the input module, processes transitions  
  - Sends updates to the display using `MsgSend()`  
  - Cleans up with `name_detach()` and `name_close()`

- **`des_inputs`**  
  - Connects to the controller service using `name_open()`  
  - Sends user-inputted or file-fed events to the controller  
  - Accepts commands like `ls`, `ws`, `exit`, etc.  
  - Closes connection using `name_close()` on exit

---

## ⚙️ Building the Project

You can use the **QNX Momentics IDE** to build each module:

1. **Create a QNX C Project** for each component:
    - `des_display`
    - `des_controller`
    - `des_inputs`

2. **Add the corresponding `.c` and shared `des-mva.h` files**.

3. **Configure include paths** if needed so each module can access shared headers.

4. **Build All**:
    - Go to `Project -> Build All`
    - Ensure no errors and that the binaries are generated under the `o/` or `bin/` directory.

---

## 🚀 Running the System

### 1. Start the Display
```sh
./des_display
```

### 2. Start the Controller
```sh
./des_controller
```

### 3. Start the Inputs Module
You can either run interactively:
```sh
./des_inputs
```
Or with redirected input from a file:
```sh
./des_inputs < exitLeftDoor.txt
```

Example content for `exitLeftDoor.txt`:
```
rs
54321
gru
ro
ws
100
rc
grl
glu
lo
lc
gll
exit
```

---

## 🧠 Understanding QNX Namespaces

This system uses **QNX’s name-based IPC** instead of relying on ND/PID/CHID:

- `name_attach()`  
  Registers a service name and creates a channel. Used by servers like the display and controller.

- `name_open()`  
  Opens a connection to a named server. Used by clients like the controller (connecting to display) and inputs (connecting to controller).

- `MsgSend()` / `MsgReceive()` / `MsgReply()`  
  Standard QNX message passing between threads or processes. Send blocks until a reply is received.

- `name_close()` / `name_detach()`  
  Cleanly disconnects from the namespace and destroys the associated channel or connection.

---

## 📝 Usage Guide

When running interactively, the inputs module will prompt:

```
Select event from the following list:
 ls  - Left Scan
 rs  - Right Scan
 ws  - Weight Scale
 lo  - Left Open
 ro  - Right Open
 lc  - Left Close
 rc  - Right Close
 grl - Guard Right Lock
 gru - Guard Right Unlock
 gll - Guard Left Lock
 glu - Guard Left Unlock
 exit - Exit the program
Enter choice:
```

Provide appropriate inputs (e.g. `ls`, followed by a person ID), and the system will:
- Pass the event to the controller
- Process it through the FSM
- Print updates in the display window

---

## Features

- Modular: 3 independent modules with clean separation
- Real-Time: Sub-50ms response time goal met
- Scalable: Can handle 100+ access events
- Clean Shutdown: All processes exit gracefully on `exit` command
- Uses QNX namespace for robust IPC decoupling

---

## 📂 Directory Structure (Example)

```
/Door-Entry-System/
├── des_controller/
│   ├── src/
│   │   ├── des_controller.c
│   │   └── des-mva.h
├── des_display/
│   └── src/
│       └── des_display.c
├── des_inputs/
│   └── src/
│       └── des_inputs.c
└── exitLeftDoor.txt
```

---

## License

This project is open for educational, experimental, and RTOS development purposes. No license restrictions apply unless you add one.

