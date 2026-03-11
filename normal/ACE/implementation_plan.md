# CMake Build System cho ACE Project (STM32F746-DISCO)

Dự án ACE hiện dùng Makefile. Mục tiêu là thêm CMake song song để hỗ trợ IntelliSense trong VS Code/CLion và có build pipeline hiện đại hơn. Các Makefile hiện có **không bị xóa**.

## User Review Required

> [!IMPORTANT]
> **Toolchain path**: CMake sẽ đọc biến môi trường `TOOLCHAIN_TUNG` (giống Makefile.windows). Đảm bảo biến này đã được set trên máy: `TOOLCHAIN_TUNG=C:\path\to\arm-none-eabi\bin\`

> [!IMPORTANT]
> **Build chung hay riêng**: Tôi sẽ tạo một root `CMakeLists.txt` ở `ACE/` build **tất cả** (SDK + ThreadX + Middleware + Bootloader + Application) trong một lần. Nếu bạn muốn build riêng lẻ từng folder thì hãy cho tôi biết.

> [!NOTE]
> **Optimization flags**: SDK và ThreadX Makefile dùng `-O0 -g`, còn Application/Bootloader dùng `-O3 -g3`. CMake sẽ giữ nguyên sự phân biệt này theo từng target.

## Proposed Changes

---

### Toolchain & Root

#### [NEW] [arm-none-eabi.cmake](file:///e:/Project/Embedded/STM32/stm32f746-disco/normal/ACE/cmake/arm-none-eabi.cmake)

File khai báo cross-compiler. Đọc `TOOLCHAIN_TUNG` từ env var (nếu có) hoặc dùng PATH:
- `CMAKE_SYSTEM_NAME Generic` (bare-metal)
- `CMAKE_SYSTEM_PROCESSOR arm`  
- Trỏ `CMAKE_C_COMPILER`, `CMAKE_CXX_COMPILER`, `CMAKE_ASM_COMPILER`, `CMAKE_AR` đến `arm-none-eabi-*`
- Vô hiệu hóa compiler test (`CMAKE_TRY_COMPILE_TARGET_TYPE = STATIC_LIBRARY`)

#### [NEW] [CMakeLists.txt — Root](file:///e:/Project/Embedded/STM32/stm32f746-disco/normal/ACE/CMakeLists.txt)

- `cmake_minimum_required(VERSION 3.20)`
- Project name `ACE`, ngôn ngữ C ASM
- Định nghĩa cache variables: `CPU_FLAGS`, `FPU_FLAGS`, `MCU_DEFINES`
- `add_subdirectory` cho SDK, OS/threadx, Middleware, Bootloader, Application

---

### SDK — Static Library `stm32_lib`

#### [NEW] [CMakeLists.txt](file:///e:/Project/Embedded/STM32/stm32f746-disco/normal/ACE/SDK/CMakeLists.txt)

Tương đương [SDK/Makefile](file:///e:/Project/Embedded/STM32/stm32f746-disco/normal/ACE/SDK/Makefile):
- `add_library(stm32_lib STATIC)`
- Sources: `Device/Src/*.c`, `Driver/hal/Src/*.c`, `Driver/hal/Src/Legacy/*.c`
- Include dirs: `Core/Include`, `Device/Include`, `Device/Include/Legacy`, `Driver/hal`, `Driver/hal/Inc`
- Compile flags: `-mcpu=cortex-m7 -mthumb -mabi=aapcs -mfloat-abi=hard -mfpu=fpv5-sp-d16 -O0 -g -DSTM32F746xx`

---

### OS/ThreadX — Static Library `tx_m7`

#### [NEW] [CMakeLists.txt](file:///e:/Project/Embedded/STM32/stm32f746-disco/normal/ACE/OS/threadx/CMakeLists.txt)

Tương đương [OS/threadx/Makefile](file:///e:/Project/Embedded/STM32/stm32f746-disco/normal/ACE/OS/threadx/Makefile):
- `add_library(tx_m7 STATIC)`
- Sources: `common/src/*.c`, `ports/cortex_m7/gnu/src/*.c`, `ports/cortex_m7/gnu/src/*.S`
- Include dirs: `common/inc`, `ports/cortex_m7/gnu/inc`
- Compile flags: `-mcpu=cortex-m7 -mthumb -mabi=aapcs -mfloat-abi=hard -mfpu=fpv5-sp-d16 -O0 -g`

---

### Middleware — Static Library `middleware`

#### [NEW] [CMakeLists.txt](file:///e:/Project/Embedded/STM32/stm32f746-disco/normal/ACE/Middleware/CMakeLists.txt)

- `add_library(middleware STATIC)`
- Sources: `qspi_flash/*.c`, `qspi_flash/w25q128j/*.c`, `sdram/*.c`
- Include dirs: `qspi_flash`, `qspi_flash/w25q128j`, `sdram` + SDK includes
- Link: `stm32_lib`

---

### Bootloader — Executable `boot.elf`

#### [NEW] [CMakeLists.txt](file:///e:/Project/Embedded/STM32/stm32f746-disco/normal/ACE/Bootloader/CMakeLists.txt)

Tương đương [Bootloader/Makefile](file:///e:/Project/Embedded/STM32/stm32f746-disco/normal/ACE/Bootloader/Makefile):
- `add_executable(boot)`
- Sources: `*.c`, `*.s` (trừ [boot_startup.s](file:///e:/Project/Embedded/STM32/stm32f746-disco/normal/ACE/Bootloader/boot_startup.s)), `boot_process/*.c`, `boot_process/operation/*.c`
- Linker script: `${CMAKE_CURRENT_SOURCE_DIR}/boot.ld`
- Linker flags: `--specs=nano.specs -Wl,--gc-sections -T... -mthumb -mabi=aapcs`
- Link: `stm32_lib`
- Include dirs: `Common`, SDK, `boot_process`, `boot_process/operation`
- Post-build: tạo `boot.bin` và `boot.hex` bằng `arm-none-eabi-objcopy`
- Custom target `flash_boot`: `STM32_Programmer_CLI.exe -c port=SWD -w boot.bin 0x08000000 -v -rst`

---

### Application — Executable `app.elf`

#### [NEW] [CMakeLists.txt](file:///e:/Project/Embedded/STM32/stm32f746-disco/normal/ACE/Application/CMakeLists.txt)

Tương đương [Application/Makefile](file:///e:/Project/Embedded/STM32/stm32f746-disco/normal/ACE/Application/Makefile):
- `add_executable(app)`
- Sources: `*.c`, `*.S` (trừ `template_startup.s`), `jzip/*.c`
- Linker script: `${CMAKE_CURRENT_SOURCE_DIR}/app.ld`
- Linker flags: `--specs=nano.specs -Wl,--gc-sections -T... -mthumb -mabi=aapcs`
- Link: `stm32_lib`, `tx_m7`, `middleware`
- Include dirs: `Common`, SDK, Middleware, ThreadX, `jzip`
- Post-build: tạo `app.bin` và `app.hex`
- Custom targets `flash_bin` (0x08040000) và `flash_hex` (external flash 0x90000000)

---

## Verification Plan

### Build Verification (Manual — chạy trên PowerShell)

```powershell
# 1. Configure từ thư mục ACE
cd e:\Project\Embedded\STM32\stm32f746-disco\normal\ACE
cmake -B _cmake_build -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi.cmake

# 2. Build SDK
cmake --build _cmake_build --target stm32_lib -j4

# 3. Build ThreadX
cmake --build _cmake_build --target tx_m7 -j4

# 4. Build Bootloader
cmake --build _cmake_build --target boot -j4

# 5. Build Application
cmake --build _cmake_build --target app -j4
```

**Kết quả mong đợi:**
- `_cmake_build/SDK/libstm32_lib.a`
- `_cmake_build/OS/threadx/libtx_m7.a`
- `_cmake_build/Bootloader/boot.elf` + `boot.bin` + `boot.hex`
- `_cmake_build/Application/app.elf` + `app.bin` + `app.hex`
