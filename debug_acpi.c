/**
 * A simple module to debug acpi matching issues.
 */

#define pr_fmt(fmt) "debug_acpi: " fmt

#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/acpi.h>


static const struct acpi_device_id keyb_ids[] = {
	{ "APP000D", 0 },
	{ },
};

static const struct acpi_device_id no_ids[] = {
	{ },
};

static struct acpi_driver dummy_acpi_driver = {
	.name		= "debug-acpi-dummy",
	.owner		= THIS_MODULE,
	.ids		= ACPI_PTR(no_ids),
};

static struct bus_type *acpi_bus;

static int init_acpi_bus_type(void)
{
	int ret;

	ret = acpi_bus_register_driver(&dummy_acpi_driver);
	acpi_bus = dummy_acpi_driver.drv.bus;
	if (ret == 0)
		acpi_bus_unregister_driver(&dummy_acpi_driver);

	return ret;
}

#define FLAGS_TO_INT(flags) (*((u32 *) &flags))

static int dev_iter(struct device *dev, void *data)
{
	struct acpi_device *adev = to_acpi_device(dev);
	struct acpi_hardware_id *hwid;

	pr_info("  dev-name='%s' status=0x%x flags=0x%x pnp-type=0x%x "
		"hid='%s' name='%s' uid='%s' adr=%lu bus-id='%.8s' "
		"dep-unment=%d phys-nodes=%d handler=%p\n",
		dev_name(dev),
		FLAGS_TO_INT(adev->status),
		FLAGS_TO_INT(adev->flags),
		FLAGS_TO_INT(adev->pnp.type),
		acpi_device_hid(adev),
		acpi_device_name(adev),
		acpi_device_uid(adev),
		acpi_device_adr(adev),
		acpi_device_bid(adev),
		adev->dep_unmet,
		adev->physical_node_count,
		adev->handler);

	list_for_each_entry(hwid, &adev->pnp.ids, list)
		pr_info("    id='%s'\n", hwid->id);

	return 0;
}


static int drv_iter(struct device_driver *drv, void *data)
{
	struct device *dev = data;

	pr_info("  drv-name='%s' matches-keyb-dev=%d\n", drv->name,
		dev && drv->bus->match ? drv->bus->match(dev, drv) : -1);

	return 0;
}


static void test_keyb_dev(struct device *dev)
{
	struct acpi_device *adev = to_acpi_device(dev);
	int ret;

	ret = acpi_match_device_ids(adev, keyb_ids);
	pr_info("  match=%d is-registered=%d driver=%p (name='%s')\n", ret,
		device_is_registered(dev), dev->driver,
		(dev->driver ? dev->driver->name : NULL));
}

static int __init debug_acpi_init(void)
{
	struct device *dev;
	int ret;

	ret = init_acpi_bus_type();
	if (ret) {
		pr_err("Error getting acpi-bus-type: %d\n", ret);
		return ret;
	}

	dev = bus_find_device_by_name(acpi_bus, NULL, "APP000D:00");
	if (!dev)
		pr_err("Failed to get device 'APP000D:00'\n");

	pr_info("Enumerating all ACPI devices:\n");
	bus_for_each_dev(acpi_bus, NULL, NULL, dev_iter);

	pr_info("Enumerating all ACPI drivers:\n");
	bus_for_each_drv(acpi_bus, NULL, dev, drv_iter);

	if (dev) {
		pr_info("APP000D dev:\n");
		test_keyb_dev(dev);
	}

	return 0;
}

static void __exit debug_acpi_exit(void)
{
}

module_init(debug_acpi_init);
module_exit(debug_acpi_exit);


MODULE_AUTHOR("Ronald Tschalär");
MODULE_DESCRIPTION("Dummy to debug acpi matching issues");
MODULE_LICENSE("GPL");
