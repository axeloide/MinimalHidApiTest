#include <stdio.h>
#include <stdlib.h>


#include "hidapi.h"


using namespace std;

int main(int argc, char* argv[])
{
	int res;
	unsigned char buf[65];
	#define MAX_STR 255
	wchar_t wstr[MAX_STR];
	hid_device *handle;
	int i;

	// Enumerate and print the HID devices on the system
	struct hid_device_info *devs, *cur_dev;

	devs = hid_enumerate(0x0, 0x0);
	cur_dev = devs;
	while (cur_dev) {
		printf("Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls",
			cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
		printf("\n");
		printf("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
		printf("  Product:      %ls\n", cur_dev->product_string);
		printf("\n");
		cur_dev = cur_dev->next;
	}
	hid_free_enumeration(devs);


	// Open the device using the VID, PID,
	// and optionally the Serial number.
	handle = hid_open(0x03eb, 0xaaaa, NULL);

	// Read the Manufacturer String
	res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
	printf("Manufacturer String: %ls\n", wstr);

	// Read the Product String
	res = hid_get_product_string(handle, wstr, MAX_STR);
	printf("Product String: %ls\n", wstr);

	// Read the Serial Number String
	res = hid_get_serial_number_string(handle, wstr, MAX_STR);
	printf("Serial Number String: %ls", wstr);
	printf("\n");


	// Send a Feature Report to the device: SetColours
	buf[0] = 0x06; // First byte is report number
	buf[1] = 0x07;
	buf[2] = 0x00;
	res = hid_send_feature_report(handle, buf, 3);


	// Send a Feature Report to the device
	buf[0] = 0x02; // First byte is report number
	buf[1] = 0x00;
	buf[2] = 0x01;
	buf[3] = 0x00;
	buf[4] = 0x00;
	buf[5] = 0x00;
	buf[6] = 0x01;
	res = hid_send_feature_report(handle, buf, 7);

	// Read a Feature Report from the device
	buf[0] = 0x1;
	res = hid_get_feature_report(handle, buf, sizeof(buf));

	// Print out the returned buffer.
	printf("Feature Report\n   ");
	for (i = 0; i < res; i++)
		printf("%02hx.", buf[i]);
	printf("\n");

	// Set the hid_read() function to be non-blocking.
	hid_set_nonblocking(handle, 1);

	// Read requested state
	res = hid_read(handle, buf, 46);
	if (res < 0)
		printf("Unable to read()\n");

	// Print out the returned buffer.
	for (i = 0; i < res; i++)
		printf("buf[%d]: %d\n", i, buf[i]);

	return 0;
}

