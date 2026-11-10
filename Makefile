# Define architecture and toolchain
CC = arm-linux-gnueabihf-gcc
CFLAGS = -static

# Define target silicon environment
TARGET_IP = 172.32.0.93
TARGET_USER = root
TARGET_DIR = /root

# Dynamic Application Name (arguement should be the file name without .c, if no arguements it will default to 'main')
# Usage: make deploy APP=sys_led
APP ?= main
SRC = $(APP).c
BIN = $(APP)

# 1. The One-Command Login (Fixes the network and starts SSH session)
shell: network
	@echo "Opening secure shell to luckfox..."
	ssh $(TARGET_USER)@$(TARGET_IP)

# 2. Automatically finds the enx USB Interface and bring it up
network: 
	@echo "Checking USB Network interface..."
	@INTERFACE=$$(ip link | grep -o 'enx[a-f0-9]*' | head -n 1); \
	if [ -n "$$INTERFACE" ]; then \
		sudo ip addr add 172.32.0.98/24 dev $$INTERFACE; \
		sudo ip link set $$INTERFACE up; \
		echo "Brought up interface; $$INTERFACE"; \
	else \
		echo "No USB network interface found."; \
	fi

# 3. Cross-compile dynamically based on the APP argument
build: $(SRC)
	@echo "Cross-compiling $(SRC) for ARMv7..."
	$(CC) $(CFLAGS) $(SRC) -o $(BIN)

# 4. Transfer the file and set target execution permission via remote ssh
deploy: build network
	@echo "Deploying $(BIN) to luckfox..."
	scp $(BIN) $(TARGET_USER)@$(TARGET_IP):$(TARGET_DIR)/
	@echo "Setting execution permission on the target..."
	ssh $(TARGET_USER)@$(TARGET_IP) "chmod +x $(TARGET_DIR)/$(BIN)"

# Clean up the specific binary on the host
clean: 
	rm -f $(BIN)
