Very simple, work in progress input driver for the SPI keyboard / trackpad found on 12" MacBooks, as well a simple touchbar driver for 2016 MacBook Pro's.

Using it:
---------
To get this driver to work on a 2016 12" MacBook or a 2016 MacBook Pro, you'll need to boot the kernel with `intremap=nosid`. Additionally, you need to make sure the `spi_pxa2xx_platform` and `intel_lpss_pci` modules are loaded. This should result in the intel-lpss driver attaching itself to the SPI controller.

The 2015 MacBook seems much more complicated, as the DMA controller isn't built in to the SPI controller. Unfortunately, I don't have a 2015 model to test.

What works:
-----------
* Basic Typing
* FN keys
* Driver unloading (no more hanging)
* Basic touchpad functionality (even right click, handled by libinput)
* MT touchpad functionality (two finger scroll, probably others)
* Interrupts!
* Suspend / resume

What doesn't work:
------------------
* Key rollover (properly)
* Wakeup on keypress / touchpad
 
Known bugs:
-----------
* Occasionally, the SPI device can get itself into a state where it causes an interrupt storm. There should be a way of resetting it, or better yet avoiding this state altogether.

Interrupts:
-----------
Interrupts are now working! This means that the driver is no longer polled, and should no longer be a massive battery drain. For more information on how the driver receives interrupts, see the discussion [here](https://github.com/cb22/macbook12-spi-driver/pull/1)

Touchpad:
---------
The touchpad protocol is the same as the bcm5974 driver. Perhaps there is a nice way of utilizing it? For now, bits of code have just been copy and pasted.

Touchbar:
---------
The touchbar driver is called `appletb`. It provides basic functionality, enabling the touchbar and switching between modes based on the FN key. Furthermore the touchbar is automatically dimmed and later switched off if no (internal) keyboard, touchpad, or touchbar input is received for a period of time; any (internal) keyboard, touchpad, or touchbar input switches it back on.

The timeouts till the touchbar is dimmed and turned off can be changed via the `idle_timeout` and `dim_timeout` module params or sysfs attributes (`/sys/class/input/input9/device/...`); they default to 3min and 2.5min, respectively. See also `modinfo appletb`.

Some useful threads:
--------------------
* https://bugzilla.kernel.org/show_bug.cgi?id=108331
* https://bugzilla.kernel.org/show_bug.cgi?id=99891
