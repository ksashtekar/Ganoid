#include <acpi/actypes.h>
#include <acpi/actbl.h>

#ifdef CONFIG_ENABLE_ACPI
#define ACPI_MAX_INIT_TABLES    16
static ACPI_TABLE_DESC      TableArray[ACPI_MAX_INIT_TABLES];

int init_acpi()
{
	/* ACPI */
	ACPI_STATUS Status;
	Status = AcpiInitializeTables(TableArray, ACPI_MAX_INIT_TABLES, TRUE);
	if (Status != 0) {
		printk("Error: Unable to early init ACPI tables: %d\n", Status);
		return -1;
	}

	return 0;
}
#endif /* CONFIG_ENABLE_ACPI */
