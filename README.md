# Unofficial Evercade Firmware

Currently only the kernel has been verified as working on the Evercade. Retroarch and other parts of this repo are untested. 
Of course, everything is at your own risk.

## Compiling the kernel

We will use the `arm-linux-gnueabihf` GCC cross-compiler in this repo.

From the `kernel` folder run:

    make ARCH=arm CROSS_COMPILE=../prebuilts/gcc/linux-x86/arm/gcc-linaro-6.3.1-2017.05-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf- rk3128_evercade_defconfig

This will configure the kernel for the Evercade. Now we can compile it like this:

    make ARCH=arm CROSS_COMPILE=../prebuilts/gcc/linux-x86/arm/gcc-linaro-6.3.1-2017.05-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf- -j4 rk3128-evercade.img

This will result in `zboot.img` if compilation is successful.

## Prerequisites

`rkdeveloptool` from https://github.com/rockchip-linux/rkdeveloptool

You need to bring the Evercade into recovery mode:

1. Plug Evercade into USB on your computer.
2. Hold Menu button as you power Evercade on.
3. It should show up as a new USB device.

## Backup your current kernel on the Evercade

Use `rkdeveloptool` to back up your Evercade's firmware.

1. Run `sudo rkdeveloptool ppt`.
   It should output something similar to the following:

    ```
    **********Partition Info(GPT)**********
    NO  LBA       Name
    00  00002000  uboot
    01  00002800  trust
    02  00003800  boot
    03  00007800  rootfs
    04  00025800  userdata
    ```

2. Run `sudo rkdeveloptool rl 0x00003800 $((0x00007800 - 0x00003800)) original-boot.img`, replacing `00003800` with the LBA of the `boot` partition, and replacing `00007800` with the LBA of the partition after the `boot` partition (`rootfs` in the example above).
   This command will create a file on your computer called `original-boot.img`

## Flash the new kernel to your Evercade

Run `sudo rkdeveloptool wlx boot zboot.img` to flash the new kernel.

## Reverting to your orignal backup

Run `sudo rkdeveloptool wlx boot original-boot.img` to revert to the original kernel you backed up above.

## USB mode

USB mode can be set inside `kernel/arch/arm/boot/dts/rk3128-evercade.dts` file.

To make `adb` work, you need to use OTG mode:
    dr_mode = "otg";

If you want to connect USB devices such as controllers to the USB port, then you need Host mode:
    dr_mode = "host";

After this change, you need to re-compile the kernel. Note that the Evercade does not support power to the USB device in host mode and it needs to be injected with a proper cable.

## Notices

This project is not in any way affiliated with Evercade or Blaze.
