prefix := /usr

all:
	@make -C install_files --no-print-directory
	@make -C libraries --no-print-directory
	@CPATH=$(PWD)/libraries LIBRARY_PATH=$(PWD)/libraries make -C examples --no-print-directory
	@CPATH=$(PWD)/libraries LIBRARY_PATH=$(PWD)/libraries make -C battery_monitor_service --no-print-directory

install:
	@make -C install_files -s install
	@make -C libraries -s install
	@make -C examples -s install
	@make -C battery_monitor_service -s install
	@make -C robot_service -s install

clean:
	@make -C examples -s clean
	@make -C libraries -s clean
	@make -C battery_monitor_service -s clean
	@make -C install_files -s clean
	@echo "All Directories Cleaned"

