#include <libusb-1.0/libusb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ID_VENDOR             0x03eb
#define ID_PRODUCT            0x0001
#define LED_INTERFACE         1
#define BUZZER_INTERFACE      2
#define LED_ENDPOINT_OUT      0x02
#define BUZZER_ENDPOINT_OUT   0x03

libusb_device_handle *handle = NULL;

int led_interface = -1;
const struct libusb_endpoint_descriptor *led_endpoint = NULL;

int buzzer_interface = -1;
const struct libusb_endpoint_descriptor *buzzer_endpoint = NULL;

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
      printf("Device Found (Bus:Address) %.3d:%.3d\n", bus, address);
      printf("\tVendor ID 0x%04x\n", desc.idVendor);
      printf("\tProduct ID 0x%04x\n", desc.idProduct);
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

  if (led_interface != -1) release_interface(LED_INTERFACE);
  if (buzzer_interface != -1) release_interface(BUZZER_INTERFACE);
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

    printf("\nInterface n°%d:\n", i);
    printf("\tInterface ID: %d\n", altsetting.iInterface);
    printf("\tInterface number: %d\n", altsetting.bInterfaceNumber);
    printf("\tNumber of endpoints: %d\n", altsetting.bNumEndpoints);
    
    for (int j = 0; j < altsetting.bNumEndpoints; j++) {
      printf("\t\tEndpoint n°%d:", j);
      printf(" 0x%02x", endpoint[j].bEndpointAddress);
      printf(" - EP %d", endpoint[j].bEndpointAddress & 0x7F);
      endpoint[j].bEndpointAddress & 0x80 ? printf(" IN\n") : printf(" OUT\n");
    }

    switch (i) {
    case LED_INTERFACE:
      claim_interface(altsetting.bInterfaceNumber);
      led_interface = altsetting.bInterfaceNumber;
      led_endpoint = endpoint;
      break;
    case BUZZER_INTERFACE:
      claim_interface(altsetting.bInterfaceNumber);
      buzzer_interface = altsetting.bInterfaceNumber;
      buzzer_endpoint = endpoint;
      break;
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
  char action;

  printf("Help: h\nQuit: q\n");
  printf("Type your command:\n");
  do {
    printf(" > ");
    scanf(" %c", &action); //action = getchar();

    switch (action) {
    case 'a':
      {
      unsigned char data[] = {0x00, 0x03};
      interrupt_request(LED_ENDPOINT_OUT, data);
      }
      sleep(1);
      printf("Processed command 'a'\n");
      break;
    case 'b':
      {
      unsigned char data[] = {0x00, 0x00};
      interrupt_request(LED_ENDPOINT_OUT, data);
      }
      sleep(1);
      printf("Processed command 'b'\n");
      break;
    case 'c':
      {
      unsigned char data = 0x01;
      interrupt_request(BUZZER_ENDPOINT_OUT, &data);
      }
      sleep(1);
      printf("Processed command 'c'\n");
      break;
    case 'd':
      {
      unsigned char data = 0x00;
      interrupt_request(BUZZER_ENDPOINT_OUT, &data);
      }
      sleep(1);
      printf("Processed command 'd'\n");
      break;
    case 'h':
      printf("Help:\n");
      printf("\ta -> Send 0x0003 to Led Interface\n");
      printf("\tb -> Send 0x0000 to Led Interface\n");
      printf("\tc -> Send 0x01 to Buzzer Interface\n");
      printf("\td -> Send 0x00 to Buzzer Interface\n");
      break;
    case 'q':
      printf("\nBye!\n");
      break;
    default:
      printf("Invalid command! Try again:\n");
      break;
    }
  } while (action != 'q');
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
