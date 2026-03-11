# Walkthrough: CMake Build System cho ACE (STM32F746-DISCO)

## Các file đã tạo

| File | Mục đích |
|---|---|
| [cmake/arm-none-eabi.cmake](file:///e:/Project/Embedded/STM32/stm32f746-disco/normal/ACE/cmake/arm-none-eabi.cmake) | Toolchain file — đọc `TOOLCHAIN_TUNG` env var |
| [CMakeLists.txt](file:///e:/Project/Embedded/STM32/stm32f746-disco/normal/ACE/CMakeLists.txt) | Root — điều phối toàn bộ build |
| [SDK/CMakeLists.txt](file:///e:/Project/Embedded/STM32/stm32f746-disco/normal/ACE/SDK/CMakeLists.txt) | Static library `stm32_lib` |
| [OS/threadx/CMakeLists.txt](file:///e:/Project/Embedded/STM32/stm32f746-disco/normal/ACE/OS/threadx/CMakeLists.txt) | Static library `tx_m7` |
| [Middleware/CMakeLists.txt](file:///e:/Project/Embedded/STM32/stm32f746-disco/normal/ACE/Middleware/CMakeLists.txt) | Static library `middleware` |
| [Bootloader/CMakeLists.txt](file:///e:/Project/Embedded/STM32/stm32f746-disco/normal/ACE/Bootloader/CMakeLists.txt) | Executable `boot.elf` + `.bin` + `.hex` |
| [Application/CMakeLists.txt](file:///e:/Project/Embedded/STM32/stm32f746-disco/normal/ACE/Application/CMakeLists.txt) | Executable `app.elf` + `.bin` + `.hex` |

## Kết quả Build

Tất cả targets build thành công:

| Target | Output | Status |
|---|---|---|
| `stm32_lib` | `_cmake_build/SDK/libstm32_lib.a` | ✅ |
| `tx_m7` | `_cmake_build/OS/threadx/libtx_m7.a` | ✅ |
| `middleware` | `_cmake_build/Middleware/libmiddleware.a` | ✅ |
| `boot` | `_cmake_build/Bootloader/boot.elf` + `.bin` + `.hex` | ✅ |
| `app` | `_cmake_build/Application/app.elf` + `.bin` + `.hex` | ✅ |

## Cách sử dụng

### Configure lần đầu
```bash
cd e:\Project\Embedded\STM32\stm32f746-disco\normal\ACE
cmake -B _cmake_build -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi.cmake -G "MinGW Makefiles"
```

> [!NOTE]
> Phải dùng `-G "MinGW Makefiles"` trên Windows. Generator mặc định sẽ dùng Visual Studio.

### Build các targets
```bash
cmake --build _cmake_build --target stm32_lib -j4   # Build SDK
cmake --build _cmake_build --target tx_m7 -j4        # Build ThreadX
cmake --build _cmake_build --target boot -j4          # Build Bootloader
cmake --build _cmake_build --target app -j4           # Build Application
cmake --build _cmake_build -j4                         # Build tất cả
```

### Flash targets
```bash
cmake --build _cmake_build --target flash_boot   # Flash boot vào 0x08000000
cmake --build _cmake_build --target flash_bin    # Flash app vào 0x08040000
cmake --build _cmake_build --target flash_hex    # Flash app vào external QSPI 0x90000000
```

## Lưu ý

- Các **Makefile gốc không bị xóa** — bạn vẫn có thể dùng [make](file:///e:/Project/Embedded/STM32/stm32f746-disco/normal/ACE/cmake/arm-none-eabi.cmake) như cũ
- Folder build của CMake là `_cmake_build/` (khác với `_build/` của Makefile)
- `TOOLCHAIN_TUNG` phải được set trước khi chạy cmake (ví dụ: `C:\gcc-arm-none-eabi\bin\`)
