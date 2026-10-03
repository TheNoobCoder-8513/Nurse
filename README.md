# Nurse

`A lightweight C++ command-line tool that logs and executes your terminal commands with tags. It helps me save useful terminal commands into a local database so that I can easily reference them days later without forgetting them.

---

##  Prerequisites & Dependencies

To build `nurse`, make sure you have:

- A C++23 capable compiler (e.g., `g++` or `clang++`).
- **SQLiteCpp** library (`libsqlitecpp-dev`).
- **SQLite3** library (`libsqlite3-dev`).

### On Ubuntu / Debian:
```bash
sudo apt update
sudo apt install build-essential libsqlitecpp-dev libsqlite3-dev
```

---

##  Usage

### Upon compilation, put the compiled executable in `{home}/.local/share/bin`.

### 1. Log and Execute a Command
Wrap your category name with colons (`:tag:`) followed by the command you want to run. `nurse` will store the entry and immediately execute the command.

```bash
nurse :node: pip install nodejs-24
```
```bash
nurse :git: git commit -m "feat: initial commit"
```

### 2. Show Command History
Display all stored logs with their ID, timestamp, user, tag, and command:

```bash
nurse --show
```

**Example Output:**
```text
ID   | Date         | Time       | User       | Tag             | Command
-----+--------------+------------+------------+-----------------+-----------------------------------
1    | 2026-10-03   | 16:40:12   | alex       | node            | pip install nodejs-24
2    | 2026-10-03   | 16:42:05   | alex       | git             | git commit -m "feat: initial commit"
```

### 3. Clear History
Wipe all stored command logs:

```bash
nurse --clear
```

### 4. Display Help
```bash
nurse --help
```

---

## Storage Location

Log history is safely stored in an SQLite database located at:
`~/.local/share/nurse/nurse.db`
