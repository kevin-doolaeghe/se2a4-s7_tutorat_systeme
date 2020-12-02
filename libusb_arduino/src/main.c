#include <libusb-1.0/libusb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ID_VENDOR             0x2341
#define ID_PRODUCT            0x0043
#define SERIAL_INTERFACE      1
#define SERIAL_IN_ENDPOINT    0x83
#define SERIAL_OUT_ENDPOINT   0x04

libusb_device_handle *handle = NULL;

int serial_interface = -1;
const struct libusb_endpoint_descriptor *serial_endpoint = NULL;

// ----------------------------------------------------------------

/**
 * Functions to open/close device
 */

void open_device(libusb_device *device) {
  int status = libusb_open(device, &handle);
  if (status != 0) {
    perror("libusb_open");
    exit(-1);
  }
}

void close_device() { libusb_close(handle); }

void find_device(libusb_context *context) {
  libusb_device **list;

  ssize_t count = libusb_get_device_list(context, &list);
  if (count < 0) {
    perror("libusb_get_device_list");
    exit(-1);
  }

  ssize_t i = 0;
  for (i = 0; i < count; i++) {
    libusb_device *device = list[i];
    struct libusb_device_descriptor desc;

    int status = libusb_get_device_descriptor(device, &desc);
    if (status != 0)
      continue;

    uint8_t bus = libusb_get_bus_number(device);
    uint8_t address = libusb_get_device_address(device);

    if (desc.idVendor == ID_VENDOR && desc.idProduct == ID_PRODUCT) {
      printf("\n---------------------------------------\n\n");
      printf("Device Found (Bus:Address) %3d:%3d\n", bus, address);
      printf("\tVendor ID 0x%4x\n", desc.idVendor);
      printf("\tProduct ID 0x%4x\n", desc.idProduct);
      printf("\n---------------------------------------\n\n");

      open_device(device);
    }
  }

  libusb_free_device_list(list, 1);
}

// ----------------------------------------------------------------

/**
 * Functions to claim/release interfaces
 */

void claim_interface(int interface) {
  // Check if kernel uses driver
  if (libusb_kernel_driver_active(handle, interface)) {
    int status = libusb_detach_kernel_driver(handle, interface);
    if (status != 0) {
      perror("libusb_detach_kernel_driver");
      exit(-1);
    }
  }

  // Claim interface
  int status = libusb_claim_interface(handle, interface);
  if (status != 0) {
    perror("libusb_claim_interface");
    exit(-1);
  }
  printf("Interface n°%d has been claimed!\n", interface);
}

void release_interface(int interface) {
  // Release interface
  int status = libusb_release_interface(handle, interface);
  if (status != 0) {
    perror("libusb_release_interface");
    exit(-1);
  }
  printf("Interface n°%d has been released.\n", interface);
}

void release_claimed_interfaces() {
  printf("\n---------------------------------------\n\n");

  if (serial_interface != -1) release_interface(SERIAL_INTERFACE);
}

void get_configuration() {
  libusb_device *device = libusb_get_device(handle);
  struct libusb_config_descriptor *config;
  int status = libusb_get_active_config_descriptor(device, &config);
  if (status != 0) {
    perror("libusb_get_active_config");
    exit(-1);
  }

  printf("Number of interfaces of opened device: %d\n", config->bNumInterfaces);

  for (int i = 0; i < config->bNumInterfaces; i++) {
    const struct libusb_interface interface = config->interface[i];
    const struct libusb_interface_descriptor altsetting = interface.altsetting[0];
    const struct libusb_endpoint_descriptor *endpoint = altsetting.endpoint;

    printf("\nInterface n°%d has number %d and id %d.\n", i, altsetting.bInterfaceNumber, altsetting.iInterface);
    printf("It has %d endpoint(s):\n", altsetting.bNumEndpoints);
    
    for (int j = 0; j < altsetting.bNumEndpoints; j++) {
      printf("\tEndpoint n°%d has address %d.\n", j, endpoint[j].bEndpointAddress);
    }

    if (i == SERIAL_INTERFACE) {
      claim_interface(altsetting.bInterfaceNumber);
      serial_interface = altsetting.bInterfaceNumber;
      serial_endpoint = endpoint;
    }
  }
  printf("\n---------------------------------------\n\n");

  libusb_free_config_descriptor(config);
}

// ----------------------------------------------------------------

/**
 * Functions to control device
 */

void interrupt_request(int endpoint, unsigned char *data) {
  int len = 0;
  int status = libusb_interrupt_transfer(handle, endpoint, data,
                                         sizeof(data), &len, 0);
  if (status != 0) {
    perror("libusb_interrupt_transfer");
    exit(-1);
  }
}

// Not used
void bulk_request(int endpoint, unsigned char *data) {
  int len = 0;
  int status = libusb_bulk_transfer(handle, endpoint, data,
                                         sizeof(data), &len, 0);
  if (status != 0) {
    perror("libusb_interrupt_transfer");
    exit(-1);
  }
}

void control_board() {
  unsigned char data = 0x01;

  printf("Send interrupt request to endpoint %d.\n", SERIAL_OUT_ENDPOINT);
  bulk_request(SERIAL_OUT_ENDPOINT, &data);

  sleep(1);

  data = 0x02;
  printf("Send interrupt request to endpoint %d.\n", SERIAL_OUT_ENDPOINT);
  bulk_request(SERIAL_OUT_ENDPOINT, &data);
}

// ----------------------------------------------------------------

int main() {
  libusb_context *context;

  int status = libusb_init(&context);
  if (status != 0) {
    perror("libusb_init");
    exit(-1);
  }

  find_device(context);
  if (handle != NULL) {
    get_configuration();

    control_board();

    release_claimed_interfaces();
    close_device();
  } else {
    printf("Couldn't find device!\n");
  }
  
  libusb_exit(context);
  return 0;
}
