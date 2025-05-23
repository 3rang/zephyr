# SPDX-License-Identifier: Apache-2.0

include(ExternalProject)

if(CONFIG_HAS_RPI_PICO)
  zephyr_library()
  set_property(TARGET ${ZEPHYR_CURRENT_LIBRARY} PROPERTY C_STANDARD 11)

  set(rp2_common_dir ${ZEPHYR_HAL_RPI_PICO_MODULE_DIR}/src/rp2_common)
  set(rp2xxx_dir ${ZEPHYR_HAL_RPI_PICO_MODULE_DIR}/src/${CONFIG_SOC_SERIES})
  set(common_dir ${ZEPHYR_HAL_RPI_PICO_MODULE_DIR}/src/common)
  set(boot_stage_dir ${rp2xxx_dir}/boot_stage2)

  # The Second Stage Bootloader is only linked to the app that resides
  # at 0x100. Therefore, only if the app's offset is 0x100, the second
  # stage bootloader should be compiled.
  if(CONFIG_RP2_REQUIRES_SECOND_STAGE_BOOT)
    foreach(flash W25Q080 GENERIC_03H IS25LP080 W25X10CL AT25SF128A)
      if(CONFIG_RP2_FLASH_${flash})
        set(flash_type ${flash})
        break()
      endif()
    endforeach()

    set(rp2_bootloader_prefix ${CMAKE_BINARY_DIR}/bootloader)
    set(rp2_bootloader_asm ${rp2_bootloader_prefix}/boot_stage2.S)

    ExternalProject_Add(
      second_stage_bootloader
      SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/bootloader
      BINARY_DIR ${rp2_bootloader_prefix}
      CMAKE_ARGS
        -DCMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
        -DCMAKE_ASM_COMPILER=${CMAKE_ASM_COMPILER}
        -DZEPHYR_HAL_RPI_PICO_MODULE_DIR=${ZEPHYR_HAL_RPI_PICO_MODULE_DIR}
        -DZEPHYR_BASE=${ZEPHYR_BASE}
        -DFLASH_TYPE=${flash_type}
        -DPYTHON_EXECUTABLE=${Python3_EXECUTABLE}
        -DRP2_BOOTLOADER_BYPRODUCT=${rp2_bootloader_asm}
      INSTALL_COMMAND "" # No installation needed
      BUILD_BYPRODUCTS ${rp2_bootloader_asm}
      BUILD_ALWAYS TRUE
      )

      zephyr_library_add_dependencies(second_stage_bootloader)
      zephyr_library_sources(${rp2_bootloader_asm})
  endif()

  if(CONFIG_SOC_SERIES_RP2040)
    zephyr_compile_definitions(PICO_RP2040)
  elseif(CONFIG_SOC_SERIES_RP2350)
    zephyr_compile_definitions(PICO_RP2350)
  endif()

  # Pico sources and headers necessary for every build.
  # These contain definitions and implementation used mostly for
  # initializing the SoC, and therefore are always required.

  zephyr_library_sources(
    ${rp2_common_dir}/hardware_clocks/clocks.c
    ${rp2_common_dir}/hardware_pll/pll.c
    ${rp2_common_dir}/hardware_xosc/xosc.c
    ${rp2_common_dir}/hardware_sync_spin_lock/sync_spin_lock.c
    ${rp2_common_dir}/hardware_ticks/ticks.c
    ${rp2_common_dir}/pico_bootrom/bootrom.c
    ${rp2xxx_dir}/pico_platform/platform.c
  )

  zephyr_include_directories(
    ${common_dir}/pico_base_headers/include
    ${rp2_common_dir}/boot_bootrom_headers/include
    ${rp2_common_dir}/hardware_base/include
    ${rp2_common_dir}/hardware_clocks/include
    ${rp2_common_dir}/hardware_watchdog/include
    ${rp2_common_dir}/hardware_xosc/include
    ${rp2_common_dir}/hardware_pll/include
    ${rp2_common_dir}/hardware_irq/include
    ${rp2_common_dir}/hardware_sync/include
    ${rp2_common_dir}/hardware_timer/include
    ${rp2_common_dir}/hardware_resets/include
    ${rp2_common_dir}/hardware_boot_lock/include
    ${rp2_common_dir}/hardware_ticks/include
    ${rp2_common_dir}/hardware_sync_spin_lock/include
    ${rp2_common_dir}/pico_bootrom/include
    ${rp2_common_dir}/pico_flash/include
    ${rp2_common_dir}/pico_platform_compiler/include
    ${rp2_common_dir}/pico_platform_sections/include
    ${rp2_common_dir}/pico_platform_panic/include
    ${common_dir}/boot_picoboot_headers/include
    ${common_dir}/boot_picobin_headers/include
    ${rp2xxx_dir}/hardware_regs/include
    ${rp2xxx_dir}/hardware_structs/include
    ${rp2xxx_dir}/pico_platform/include
    ${CMAKE_CURRENT_LIST_DIR}
  )

  zephyr_library_sources_ifdef(CONFIG_PICOSDK_USE_GPIO
          ${rp2_common_dir}/hardware_gpio/gpio.c)
  zephyr_include_directories_ifdef(CONFIG_PICOSDK_USE_GPIO
          ${rp2_common_dir}/hardware_gpio/include)

  zephyr_include_directories_ifdef(CONFIG_PICOSDK_USE_UART
          ${rp2_common_dir}/hardware_uart/include)

  zephyr_library_sources_ifdef(CONFIG_PICOSDK_USE_FLASH
          ${rp2_common_dir}/hardware_flash/flash.c
          ${rp2_common_dir}/hardware_xip_cache/xip_cache.c)
  zephyr_include_directories_ifdef(CONFIG_PICOSDK_USE_FLASH
          ${rp2_common_dir}/hardware_flash/include
          ${rp2_common_dir}/hardware_xip_cache/include)

  zephyr_include_directories_ifdef(CONFIG_PICOSDK_USE_PWM
          ${rp2_common_dir}/hardware_pwm/include)

  zephyr_library_sources_ifdef(CONFIG_PICOSDK_USE_ADC
          ${rp2_common_dir}/hardware_adc/adc.c)
  zephyr_include_directories_ifdef(CONFIG_PICOSDK_USE_ADC
          ${rp2_common_dir}/hardware_adc/include)

  zephyr_library_sources_ifdef(CONFIG_PICOSDK_USE_DMA
          ${rp2_common_dir}/hardware_dma/dma.c)
  zephyr_include_directories_ifdef(CONFIG_PICOSDK_USE_DMA
          ${rp2_common_dir}/hardware_dma/include)

  zephyr_library_sources_ifdef(CONFIG_PICOSDK_USE_TIMER
          ${rp2_common_dir}/hardware_timer/timer.c)
  zephyr_include_directories_ifdef(CONFIG_PICOSDK_USE_TIMER
          ${rp2_common_dir}/hardware_timer/include)

  zephyr_library_sources_ifdef(CONFIG_PICOSDK_USE_RTC
          ${rp2_common_dir}/hardware_rtc/rtc.c)
  zephyr_include_directories_ifdef(CONFIG_PICOSDK_USE_RTC
          ${rp2_common_dir}/hardware_rtc/include)

  zephyr_library_sources_ifdef(CONFIG_PICOSDK_USE_PIO
          ${rp2_common_dir}/hardware_pio/pio.c)
  zephyr_include_directories_ifdef(CONFIG_PICOSDK_USE_PIO
          ${rp2_common_dir}/hardware_pio/include)

  # Some flash driver functions must be executed from the RAM.
  # Originally pico-sdk places them in the RW data section, so this
  # implementation does the same.
  zephyr_linker_sources_ifdef(CONFIG_PICOSDK_USE_FLASH RWDATA timecritical.ld)

  # A function in flash.c adds 1 to a function pointer, causing a warning
  set_source_files_properties(
    ${rp2_common_dir}/hardware_flash/flash.c
    PROPERTIES
    COMPILE_FLAGS $<TARGET_PROPERTY:compiler,warning_no_pointer_arithmetic>
  )

  zephyr_library_sources_ifdef(CONFIG_PICOSDK_USE_CLAIM
          ${common_dir}/hardware_claim/claim.c)
  zephyr_include_directories_ifdef(CONFIG_PICOSDK_USE_CLAIM
          ${common_dir}/hardware_claim/include)

  zephyr_library_sources_ifdef(CONFIG_SOC_SERIES_RP2350
          ${rp2_common_dir}/pico_runtime_init/runtime_init.c)
  zephyr_include_directories_ifdef(CONFIG_SOC_SERIES_RP2350
          ${rp2_common_dir}/pico_runtime/include
          ${rp2_common_dir}/pico_runtime_init/include)

endif()
