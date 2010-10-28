#include <stdio.h>
#include <stdlib.h>


#include "hidapi.h"
#include <pthread.h>

using namespace std;

pthread_t thread;
pthread_barrier_t barrier; /* Ensures correct startup sequence */
int shutdown_thread;


static void *read_thread(void *param)
{
	hid_device *dev = (hid_device *) param;
	unsigned char buf[255];


	// Notify the main thread that the read thread is up and running.
	pthread_barrier_wait(&barrier);

	/* Handle all the events. */
	while (!shutdown_thread) {
        if (hid_read(dev, buf , sizeof(buf)) < 0) break;
	}

	return NULL;
}

int main(int argc, char* argv[])
{
	int res;
	unsigned char buf[65];
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

    // Start a Thread that will poll for input reports
    shutdown_thread = 0;
    pthread_barrier_init(&barrier, NULL, 2);
    pthread_create(&thread, NULL, read_thread, handle);
    // Wait here for the read thread to be initialized.
    pthread_barrier_wait(&barrier);


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
        // Read a Feature Report from the device
        buf[0] = 0x1;
        res = hid_get_feature_report(handle, buf, sizeof(buf));

        if (res<0) {
            printf("Getting feature report failed with: %d\n",res);
            break;
        }

        if (res>0) {
            printf(".");
            fflush(stdout);

            // Print out the returned buffer.
//            printf("Feature Report\n   ");
//            for (int i = 0; i < res; i++)
//                printf("0x%02hx ", buf[i]);
//            printf("\n");
        }

    }

    // Signal thread to stop.
	shutdown_thread = 1;

	// Wait for thread to stop
	pthread_join(thread, NULL);

    hid_close(handle);

    getchar();

	return 0;
}

