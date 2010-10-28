#include <stdio.h>
#include <stdlib.h>


#include "hidapi.h"
#include <pthread.h>

using namespace std;

int main(int argc, char* argv[]) {
    int res;
    unsigned char buf[255];
#define MAX_STR 255
    wchar_t wstr[MAX_STR];
    hid_device *handle;


    printf("Looking for a device with a certain VID/PID pair...");
    fflush(stdout);
    // Open the device using the VID, PID,
    // and optionally the Serial number.
    do {
        handle = hid_open(0x03eb, 0xaaaa, NULL);
    } while (handle==NULL);

    printf("Found!\n");

    if (hid_set_nonblocking(handle,1) != 0) {
        printf("Could not set device handle to non-blocking!");
        return -1;
    }

    // Read the Manufacturer String
    res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
    if (res<0) return -1;
    printf("Manufacturer String: %ls\n", wstr);

    // Read the Product String
    res = hid_get_product_string(handle, wstr, MAX_STR);
    if (res<0) return -1;
    printf("Product String: %ls\n", wstr);

    // Read the Serial Number String
    res = hid_get_serial_number_string(handle, wstr, MAX_STR);
    if (res<0) return -1;
    printf("Serial Number String: %ls\n", wstr);


    // Send a Feature Report to the device: SetColours
    // buf[0] = 0x06; // First byte is report number
    // buf[1] = 0x07;
    // buf[2] = 0x00;
    // res = hid_send_feature_report(handle, buf, 3);


    while (1) {

        /*
        // Read a Feature Report from the device
        buf[0] = 0x1;
        res = hid_get_feature_report(handle, buf, sizeof(buf));
        if (res<0) {
            printf("Getting feature report failed with: %d\n",res);
            break;
        }
        */

        // Read a Input Report from the device
        res = hid_read(handle, buf , sizeof(buf));
        if (res<0) {
            printf("Reading an input-report failed with: %d\n",res);
            break;
        }

    }

    hid_close(handle);

    //getchar();

    return 0;
}

