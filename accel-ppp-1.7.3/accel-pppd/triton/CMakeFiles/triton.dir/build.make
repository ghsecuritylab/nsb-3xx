# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canoncical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Produce verbose output by default.
VERBOSE = 1

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /home/igarashi/cortina/staging_dir/host/bin/cmake

# The command to remove a file.
RM = /home/igarashi/cortina/staging_dir/host/bin/cmake -E remove -f

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /home/igarashi/cortina/staging_dir/host/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3

# Include any dependencies generated for this target.
include accel-pppd/triton/CMakeFiles/triton.dir/depend.make

# Include the progress variables for this target.
include accel-pppd/triton/CMakeFiles/triton.dir/progress.make

# Include the compile flags for this target's objects.
include accel-pppd/triton/CMakeFiles/triton.dir/flags.make

accel-pppd/triton/CMakeFiles/triton.dir/md.c.o: accel-pppd/triton/CMakeFiles/triton.dir/flags.make
accel-pppd/triton/CMakeFiles/triton.dir/md.c.o: accel-pppd/triton/md.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object accel-pppd/triton/CMakeFiles/triton.dir/md.c.o"
	cd /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton && /home/igarashi/cortina/staging_dir/toolchain-arm_gcc-4.5.1+l_uClibc-0.9.32_eabi/bin/arm-openwrt-linux-uclibcgnueabi-gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/triton.dir/md.c.o   -c /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton/md.c

accel-pppd/triton/CMakeFiles/triton.dir/md.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/triton.dir/md.c.i"
	cd /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton && /home/igarashi/cortina/staging_dir/toolchain-arm_gcc-4.5.1+l_uClibc-0.9.32_eabi/bin/arm-openwrt-linux-uclibcgnueabi-gcc  $(C_DEFINES) $(C_FLAGS) -E /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton/md.c > CMakeFiles/triton.dir/md.c.i

accel-pppd/triton/CMakeFiles/triton.dir/md.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/triton.dir/md.c.s"
	cd /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton && /home/igarashi/cortina/staging_dir/toolchain-arm_gcc-4.5.1+l_uClibc-0.9.32_eabi/bin/arm-openwrt-linux-uclibcgnueabi-gcc  $(C_DEFINES) $(C_FLAGS) -S /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton/md.c -o CMakeFiles/triton.dir/md.c.s

accel-pppd/triton/CMakeFiles/triton.dir/md.c.o.requires:
.PHONY : accel-pppd/triton/CMakeFiles/triton.dir/md.c.o.requires

accel-pppd/triton/CMakeFiles/triton.dir/md.c.o.provides: accel-pppd/triton/CMakeFiles/triton.dir/md.c.o.requires
	$(MAKE) -f accel-pppd/triton/CMakeFiles/triton.dir/build.make accel-pppd/triton/CMakeFiles/triton.dir/md.c.o.provides.build
.PHONY : accel-pppd/triton/CMakeFiles/triton.dir/md.c.o.provides

accel-pppd/triton/CMakeFiles/triton.dir/md.c.o.provides.build: accel-pppd/triton/CMakeFiles/triton.dir/md.c.o
.PHONY : accel-pppd/triton/CMakeFiles/triton.dir/md.c.o.provides.build

accel-pppd/triton/CMakeFiles/triton.dir/timer.c.o: accel-pppd/triton/CMakeFiles/triton.dir/flags.make
accel-pppd/triton/CMakeFiles/triton.dir/timer.c.o: accel-pppd/triton/timer.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object accel-pppd/triton/CMakeFiles/triton.dir/timer.c.o"
	cd /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton && /home/igarashi/cortina/staging_dir/toolchain-arm_gcc-4.5.1+l_uClibc-0.9.32_eabi/bin/arm-openwrt-linux-uclibcgnueabi-gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/triton.dir/timer.c.o   -c /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton/timer.c

accel-pppd/triton/CMakeFiles/triton.dir/timer.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/triton.dir/timer.c.i"
	cd /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton && /home/igarashi/cortina/staging_dir/toolchain-arm_gcc-4.5.1+l_uClibc-0.9.32_eabi/bin/arm-openwrt-linux-uclibcgnueabi-gcc  $(C_DEFINES) $(C_FLAGS) -E /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton/timer.c > CMakeFiles/triton.dir/timer.c.i

accel-pppd/triton/CMakeFiles/triton.dir/timer.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/triton.dir/timer.c.s"
	cd /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton && /home/igarashi/cortina/staging_dir/toolchain-arm_gcc-4.5.1+l_uClibc-0.9.32_eabi/bin/arm-openwrt-linux-uclibcgnueabi-gcc  $(C_DEFINES) $(C_FLAGS) -S /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton/timer.c -o CMakeFiles/triton.dir/timer.c.s

accel-pppd/triton/CMakeFiles/triton.dir/timer.c.o.requires:
.PHONY : accel-pppd/triton/CMakeFiles/triton.dir/timer.c.o.requires

accel-pppd/triton/CMakeFiles/triton.dir/timer.c.o.provides: accel-pppd/triton/CMakeFiles/triton.dir/timer.c.o.requires
	$(MAKE) -f accel-pppd/triton/CMakeFiles/triton.dir/build.make accel-pppd/triton/CMakeFiles/triton.dir/timer.c.o.provides.build
.PHONY : accel-pppd/triton/CMakeFiles/triton.dir/timer.c.o.provides

accel-pppd/triton/CMakeFiles/triton.dir/timer.c.o.provides.build: accel-pppd/triton/CMakeFiles/triton.dir/timer.c.o
.PHONY : accel-pppd/triton/CMakeFiles/triton.dir/timer.c.o.provides.build

accel-pppd/triton/CMakeFiles/triton.dir/triton.c.o: accel-pppd/triton/CMakeFiles/triton.dir/flags.make
accel-pppd/triton/CMakeFiles/triton.dir/triton.c.o: accel-pppd/triton/triton.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object accel-pppd/triton/CMakeFiles/triton.dir/triton.c.o"
	cd /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton && /home/igarashi/cortina/staging_dir/toolchain-arm_gcc-4.5.1+l_uClibc-0.9.32_eabi/bin/arm-openwrt-linux-uclibcgnueabi-gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/triton.dir/triton.c.o   -c /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton/triton.c

accel-pppd/triton/CMakeFiles/triton.dir/triton.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/triton.dir/triton.c.i"
	cd /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton && /home/igarashi/cortina/staging_dir/toolchain-arm_gcc-4.5.1+l_uClibc-0.9.32_eabi/bin/arm-openwrt-linux-uclibcgnueabi-gcc  $(C_DEFINES) $(C_FLAGS) -E /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton/triton.c > CMakeFiles/triton.dir/triton.c.i

accel-pppd/triton/CMakeFiles/triton.dir/triton.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/triton.dir/triton.c.s"
	cd /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton && /home/igarashi/cortina/staging_dir/toolchain-arm_gcc-4.5.1+l_uClibc-0.9.32_eabi/bin/arm-openwrt-linux-uclibcgnueabi-gcc  $(C_DEFINES) $(C_FLAGS) -S /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton/triton.c -o CMakeFiles/triton.dir/triton.c.s

accel-pppd/triton/CMakeFiles/triton.dir/triton.c.o.requires:
.PHONY : accel-pppd/triton/CMakeFiles/triton.dir/triton.c.o.requires

accel-pppd/triton/CMakeFiles/triton.dir/triton.c.o.provides: accel-pppd/triton/CMakeFiles/triton.dir/triton.c.o.requires
	$(MAKE) -f accel-pppd/triton/CMakeFiles/triton.dir/build.make accel-pppd/triton/CMakeFiles/triton.dir/triton.c.o.provides.build
.PHONY : accel-pppd/triton/CMakeFiles/triton.dir/triton.c.o.provides

accel-pppd/triton/CMakeFiles/triton.dir/triton.c.o.provides.build: accel-pppd/triton/CMakeFiles/triton.dir/triton.c.o
.PHONY : accel-pppd/triton/CMakeFiles/triton.dir/triton.c.o.provides.build

accel-pppd/triton/CMakeFiles/triton.dir/conf_file.c.o: accel-pppd/triton/CMakeFiles/triton.dir/flags.make
accel-pppd/triton/CMakeFiles/triton.dir/conf_file.c.o: accel-pppd/triton/conf_file.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/CMakeFiles $(CMAKE_PROGRESS_4)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object accel-pppd/triton/CMakeFiles/triton.dir/conf_file.c.o"
	cd /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton && /home/igarashi/cortina/staging_dir/toolchain-arm_gcc-4.5.1+l_uClibc-0.9.32_eabi/bin/arm-openwrt-linux-uclibcgnueabi-gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/triton.dir/conf_file.c.o   -c /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton/conf_file.c

accel-pppd/triton/CMakeFiles/triton.dir/conf_file.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/triton.dir/conf_file.c.i"
	cd /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton && /home/igarashi/cortina/staging_dir/toolchain-arm_gcc-4.5.1+l_uClibc-0.9.32_eabi/bin/arm-openwrt-linux-uclibcgnueabi-gcc  $(C_DEFINES) $(C_FLAGS) -E /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton/conf_file.c > CMakeFiles/triton.dir/conf_file.c.i

accel-pppd/triton/CMakeFiles/triton.dir/conf_file.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/triton.dir/conf_file.c.s"
	cd /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton && /home/igarashi/cortina/staging_dir/toolchain-arm_gcc-4.5.1+l_uClibc-0.9.32_eabi/bin/arm-openwrt-linux-uclibcgnueabi-gcc  $(C_DEFINES) $(C_FLAGS) -S /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton/conf_file.c -o CMakeFiles/triton.dir/conf_file.c.s

accel-pppd/triton/CMakeFiles/triton.dir/conf_file.c.o.requires:
.PHONY : accel-pppd/triton/CMakeFiles/triton.dir/conf_file.c.o.requires

accel-pppd/triton/CMakeFiles/triton.dir/conf_file.c.o.provides: accel-pppd/triton/CMakeFiles/triton.dir/conf_file.c.o.requires
	$(MAKE) -f accel-pppd/triton/CMakeFiles/triton.dir/build.make accel-pppd/triton/CMakeFiles/triton.dir/conf_file.c.o.provides.build
.PHONY : accel-pppd/triton/CMakeFiles/triton.dir/conf_file.c.o.provides

accel-pppd/triton/CMakeFiles/triton.dir/conf_file.c.o.provides.build: accel-pppd/triton/CMakeFiles/triton.dir/conf_file.c.o
.PHONY : accel-pppd/triton/CMakeFiles/triton.dir/conf_file.c.o.provides.build

accel-pppd/triton/CMakeFiles/triton.dir/loader.c.o: accel-pppd/triton/CMakeFiles/triton.dir/flags.make
accel-pppd/triton/CMakeFiles/triton.dir/loader.c.o: accel-pppd/triton/loader.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/CMakeFiles $(CMAKE_PROGRESS_5)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object accel-pppd/triton/CMakeFiles/triton.dir/loader.c.o"
	cd /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton && /home/igarashi/cortina/staging_dir/toolchain-arm_gcc-4.5.1+l_uClibc-0.9.32_eabi/bin/arm-openwrt-linux-uclibcgnueabi-gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/triton.dir/loader.c.o   -c /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton/loader.c

accel-pppd/triton/CMakeFiles/triton.dir/loader.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/triton.dir/loader.c.i"
	cd /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton && /home/igarashi/cortina/staging_dir/toolchain-arm_gcc-4.5.1+l_uClibc-0.9.32_eabi/bin/arm-openwrt-linux-uclibcgnueabi-gcc  $(C_DEFINES) $(C_FLAGS) -E /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton/loader.c > CMakeFiles/triton.dir/loader.c.i

accel-pppd/triton/CMakeFiles/triton.dir/loader.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/triton.dir/loader.c.s"
	cd /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton && /home/igarashi/cortina/staging_dir/toolchain-arm_gcc-4.5.1+l_uClibc-0.9.32_eabi/bin/arm-openwrt-linux-uclibcgnueabi-gcc  $(C_DEFINES) $(C_FLAGS) -S /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton/loader.c -o CMakeFiles/triton.dir/loader.c.s

accel-pppd/triton/CMakeFiles/triton.dir/loader.c.o.requires:
.PHONY : accel-pppd/triton/CMakeFiles/triton.dir/loader.c.o.requires

accel-pppd/triton/CMakeFiles/triton.dir/loader.c.o.provides: accel-pppd/triton/CMakeFiles/triton.dir/loader.c.o.requires
	$(MAKE) -f accel-pppd/triton/CMakeFiles/triton.dir/build.make accel-pppd/triton/CMakeFiles/triton.dir/loader.c.o.provides.build
.PHONY : accel-pppd/triton/CMakeFiles/triton.dir/loader.c.o.provides

accel-pppd/triton/CMakeFiles/triton.dir/loader.c.o.provides.build: accel-pppd/triton/CMakeFiles/triton.dir/loader.c.o
.PHONY : accel-pppd/triton/CMakeFiles/triton.dir/loader.c.o.provides.build

accel-pppd/triton/CMakeFiles/triton.dir/log.c.o: accel-pppd/triton/CMakeFiles/triton.dir/flags.make
accel-pppd/triton/CMakeFiles/triton.dir/log.c.o: accel-pppd/triton/log.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/CMakeFiles $(CMAKE_PROGRESS_6)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object accel-pppd/triton/CMakeFiles/triton.dir/log.c.o"
	cd /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton && /home/igarashi/cortina/staging_dir/toolchain-arm_gcc-4.5.1+l_uClibc-0.9.32_eabi/bin/arm-openwrt-linux-uclibcgnueabi-gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/triton.dir/log.c.o   -c /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton/log.c

accel-pppd/triton/CMakeFiles/triton.dir/log.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/triton.dir/log.c.i"
	cd /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton && /home/igarashi/cortina/staging_dir/toolchain-arm_gcc-4.5.1+l_uClibc-0.9.32_eabi/bin/arm-openwrt-linux-uclibcgnueabi-gcc  $(C_DEFINES) $(C_FLAGS) -E /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton/log.c > CMakeFiles/triton.dir/log.c.i

accel-pppd/triton/CMakeFiles/triton.dir/log.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/triton.dir/log.c.s"
	cd /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton && /home/igarashi/cortina/staging_dir/toolchain-arm_gcc-4.5.1+l_uClibc-0.9.32_eabi/bin/arm-openwrt-linux-uclibcgnueabi-gcc  $(C_DEFINES) $(C_FLAGS) -S /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton/log.c -o CMakeFiles/triton.dir/log.c.s

accel-pppd/triton/CMakeFiles/triton.dir/log.c.o.requires:
.PHONY : accel-pppd/triton/CMakeFiles/triton.dir/log.c.o.requires

accel-pppd/triton/CMakeFiles/triton.dir/log.c.o.provides: accel-pppd/triton/CMakeFiles/triton.dir/log.c.o.requires
	$(MAKE) -f accel-pppd/triton/CMakeFiles/triton.dir/build.make accel-pppd/triton/CMakeFiles/triton.dir/log.c.o.provides.build
.PHONY : accel-pppd/triton/CMakeFiles/triton.dir/log.c.o.provides

accel-pppd/triton/CMakeFiles/triton.dir/log.c.o.provides.build: accel-pppd/triton/CMakeFiles/triton.dir/log.c.o
.PHONY : accel-pppd/triton/CMakeFiles/triton.dir/log.c.o.provides.build

accel-pppd/triton/CMakeFiles/triton.dir/mempool.c.o: accel-pppd/triton/CMakeFiles/triton.dir/flags.make
accel-pppd/triton/CMakeFiles/triton.dir/mempool.c.o: accel-pppd/triton/mempool.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/CMakeFiles $(CMAKE_PROGRESS_7)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object accel-pppd/triton/CMakeFiles/triton.dir/mempool.c.o"
	cd /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton && /home/igarashi/cortina/staging_dir/toolchain-arm_gcc-4.5.1+l_uClibc-0.9.32_eabi/bin/arm-openwrt-linux-uclibcgnueabi-gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/triton.dir/mempool.c.o   -c /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton/mempool.c

accel-pppd/triton/CMakeFiles/triton.dir/mempool.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/triton.dir/mempool.c.i"
	cd /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton && /home/igarashi/cortina/staging_dir/toolchain-arm_gcc-4.5.1+l_uClibc-0.9.32_eabi/bin/arm-openwrt-linux-uclibcgnueabi-gcc  $(C_DEFINES) $(C_FLAGS) -E /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton/mempool.c > CMakeFiles/triton.dir/mempool.c.i

accel-pppd/triton/CMakeFiles/triton.dir/mempool.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/triton.dir/mempool.c.s"
	cd /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton && /home/igarashi/cortina/staging_dir/toolchain-arm_gcc-4.5.1+l_uClibc-0.9.32_eabi/bin/arm-openwrt-linux-uclibcgnueabi-gcc  $(C_DEFINES) $(C_FLAGS) -S /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton/mempool.c -o CMakeFiles/triton.dir/mempool.c.s

accel-pppd/triton/CMakeFiles/triton.dir/mempool.c.o.requires:
.PHONY : accel-pppd/triton/CMakeFiles/triton.dir/mempool.c.o.requires

accel-pppd/triton/CMakeFiles/triton.dir/mempool.c.o.provides: accel-pppd/triton/CMakeFiles/triton.dir/mempool.c.o.requires
	$(MAKE) -f accel-pppd/triton/CMakeFiles/triton.dir/build.make accel-pppd/triton/CMakeFiles/triton.dir/mempool.c.o.provides.build
.PHONY : accel-pppd/triton/CMakeFiles/triton.dir/mempool.c.o.provides

accel-pppd/triton/CMakeFiles/triton.dir/mempool.c.o.provides.build: accel-pppd/triton/CMakeFiles/triton.dir/mempool.c.o
.PHONY : accel-pppd/triton/CMakeFiles/triton.dir/mempool.c.o.provides.build

accel-pppd/triton/CMakeFiles/triton.dir/event.c.o: accel-pppd/triton/CMakeFiles/triton.dir/flags.make
accel-pppd/triton/CMakeFiles/triton.dir/event.c.o: accel-pppd/triton/event.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/CMakeFiles $(CMAKE_PROGRESS_8)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object accel-pppd/triton/CMakeFiles/triton.dir/event.c.o"
	cd /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton && /home/igarashi/cortina/staging_dir/toolchain-arm_gcc-4.5.1+l_uClibc-0.9.32_eabi/bin/arm-openwrt-linux-uclibcgnueabi-gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/triton.dir/event.c.o   -c /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton/event.c

accel-pppd/triton/CMakeFiles/triton.dir/event.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/triton.dir/event.c.i"
	cd /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton && /home/igarashi/cortina/staging_dir/toolchain-arm_gcc-4.5.1+l_uClibc-0.9.32_eabi/bin/arm-openwrt-linux-uclibcgnueabi-gcc  $(C_DEFINES) $(C_FLAGS) -E /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton/event.c > CMakeFiles/triton.dir/event.c.i

accel-pppd/triton/CMakeFiles/triton.dir/event.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/triton.dir/event.c.s"
	cd /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton && /home/igarashi/cortina/staging_dir/toolchain-arm_gcc-4.5.1+l_uClibc-0.9.32_eabi/bin/arm-openwrt-linux-uclibcgnueabi-gcc  $(C_DEFINES) $(C_FLAGS) -S /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton/event.c -o CMakeFiles/triton.dir/event.c.s

accel-pppd/triton/CMakeFiles/triton.dir/event.c.o.requires:
.PHONY : accel-pppd/triton/CMakeFiles/triton.dir/event.c.o.requires

accel-pppd/triton/CMakeFiles/triton.dir/event.c.o.provides: accel-pppd/triton/CMakeFiles/triton.dir/event.c.o.requires
	$(MAKE) -f accel-pppd/triton/CMakeFiles/triton.dir/build.make accel-pppd/triton/CMakeFiles/triton.dir/event.c.o.provides.build
.PHONY : accel-pppd/triton/CMakeFiles/triton.dir/event.c.o.provides

accel-pppd/triton/CMakeFiles/triton.dir/event.c.o.provides.build: accel-pppd/triton/CMakeFiles/triton.dir/event.c.o
.PHONY : accel-pppd/triton/CMakeFiles/triton.dir/event.c.o.provides.build

# Object files for target triton
triton_OBJECTS = \
"CMakeFiles/triton.dir/md.c.o" \
"CMakeFiles/triton.dir/timer.c.o" \
"CMakeFiles/triton.dir/triton.c.o" \
"CMakeFiles/triton.dir/conf_file.c.o" \
"CMakeFiles/triton.dir/loader.c.o" \
"CMakeFiles/triton.dir/log.c.o" \
"CMakeFiles/triton.dir/mempool.c.o" \
"CMakeFiles/triton.dir/event.c.o"

# External object files for target triton
triton_EXTERNAL_OBJECTS =

accel-pppd/triton/libtriton.so: accel-pppd/triton/CMakeFiles/triton.dir/md.c.o
accel-pppd/triton/libtriton.so: accel-pppd/triton/CMakeFiles/triton.dir/timer.c.o
accel-pppd/triton/libtriton.so: accel-pppd/triton/CMakeFiles/triton.dir/triton.c.o
accel-pppd/triton/libtriton.so: accel-pppd/triton/CMakeFiles/triton.dir/conf_file.c.o
accel-pppd/triton/libtriton.so: accel-pppd/triton/CMakeFiles/triton.dir/loader.c.o
accel-pppd/triton/libtriton.so: accel-pppd/triton/CMakeFiles/triton.dir/log.c.o
accel-pppd/triton/libtriton.so: accel-pppd/triton/CMakeFiles/triton.dir/mempool.c.o
accel-pppd/triton/libtriton.so: accel-pppd/triton/CMakeFiles/triton.dir/event.c.o
accel-pppd/triton/libtriton.so: accel-pppd/triton/CMakeFiles/triton.dir/build.make
accel-pppd/triton/libtriton.so: accel-pppd/triton/CMakeFiles/triton.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking C shared library libtriton.so"
	cd /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/triton.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
accel-pppd/triton/CMakeFiles/triton.dir/build: accel-pppd/triton/libtriton.so
.PHONY : accel-pppd/triton/CMakeFiles/triton.dir/build

accel-pppd/triton/CMakeFiles/triton.dir/requires: accel-pppd/triton/CMakeFiles/triton.dir/md.c.o.requires
accel-pppd/triton/CMakeFiles/triton.dir/requires: accel-pppd/triton/CMakeFiles/triton.dir/timer.c.o.requires
accel-pppd/triton/CMakeFiles/triton.dir/requires: accel-pppd/triton/CMakeFiles/triton.dir/triton.c.o.requires
accel-pppd/triton/CMakeFiles/triton.dir/requires: accel-pppd/triton/CMakeFiles/triton.dir/conf_file.c.o.requires
accel-pppd/triton/CMakeFiles/triton.dir/requires: accel-pppd/triton/CMakeFiles/triton.dir/loader.c.o.requires
accel-pppd/triton/CMakeFiles/triton.dir/requires: accel-pppd/triton/CMakeFiles/triton.dir/log.c.o.requires
accel-pppd/triton/CMakeFiles/triton.dir/requires: accel-pppd/triton/CMakeFiles/triton.dir/mempool.c.o.requires
accel-pppd/triton/CMakeFiles/triton.dir/requires: accel-pppd/triton/CMakeFiles/triton.dir/event.c.o.requires
.PHONY : accel-pppd/triton/CMakeFiles/triton.dir/requires

accel-pppd/triton/CMakeFiles/triton.dir/clean:
	cd /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton && $(CMAKE_COMMAND) -P CMakeFiles/triton.dir/cmake_clean.cmake
.PHONY : accel-pppd/triton/CMakeFiles/triton.dir/clean

accel-pppd/triton/CMakeFiles/triton.dir/depend:
	cd /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3 /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3 /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton /home/igarashi/cortina/build_dir/linux-g2_wan/accel-ppp-1.7.3/accel-pppd/triton/CMakeFiles/triton.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : accel-pppd/triton/CMakeFiles/triton.dir/depend
