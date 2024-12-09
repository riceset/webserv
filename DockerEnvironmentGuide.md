
# Docker C++ Development Environment

This document explains the basic operations for using the Docker environment to build and test C++ programs.

---

## Prerequisites

1. **Docker** installed on your system.
2. **Docker Compose** installed.

---

## Setting up the Docker Environment

1. Navigate to the project directory:
   ```bash
   cd project-root
   ```

2. Build the Docker image:
   ```bash
   docker compose -f docker/docker-compose.yml build
   ```

---

## Using the Environment

1. Start the Docker container and access the shell:
   ```bash
   docker compose -f docker/docker-compose.yml run cpp-env
   ```

2. Inside the container, build your C++ program:
   ```bash
   make
   ```

3. Run the program:
   ```bash
   ./main
   ```

4. Clean up build files:
   ```bash
   make clean
   ```

5. Exit the container:
   ```bash
   exit
   ```

---

## Destroying the Environment

If you want to stop and remove the container:
```bash
docker compose -f docker/docker-compose.yml down
```

---

## Make file commands

Build the ubunts container:
```bash
make build
```

Run the container:
```bash
make run
```

down the container:
```bash
make down 
```

---

## Notes

- Any changes to the source code on your host machine will reflect in the container due to volume mapping.
- For troubleshooting or further customization, edit the `Dockerfile` or `docker-compose.yml`.
