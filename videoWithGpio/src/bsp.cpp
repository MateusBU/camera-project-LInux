#include "bsp.hpp"

#include <gpiod.h>
#include <iostream>
#include <cerrno>
#include <cstring>
#include <unistd.h>

/* ============================================================
 *  Static (private) variables
 * ============================================================*/
#define NUMBER_OF_OUTPUTS 1
#define NUMBER_OF_INPUTS 1

typedef enum {
    eCONFIG_GPIO_OK,
    eCONFIG_GPIOS_CHIP_ERROR,
    eCONFIG_GPIOS_LINE_ERROR,
} configGpios_t;

typedef struct {
    unsigned int gpioOutput;
    unsigned int valueOutput;
} outputs_t;

typedef struct {
    unsigned int gpioInput;
    int valueInput;
} inputs_t;

const char *chip_path = "/dev/gpiochip0";
struct gpiod_chip *chip = nullptr;
struct gpiod_line_request *reqLedYellow = nullptr;

outputs_t outputs[NUMBER_OF_OUTPUTS];

struct gpiod_line_request *reqButton1 = nullptr;
inputs_t inputs[NUMBER_OF_INPUTS];

/* ============================================================
 *  Static (private) functions
 * ============================================================*/
static configGpios_t bsp_initOutputs(void);
static configGpios_t bsp_initInputs(void);

/* ============================================================
 *  Public functions
 * ============================================================*/
void bsp_init(void) {

    chip = gpiod_chip_open(chip_path);
    if (!chip) {
        std::cerr << "Failed to open gpiochip: " << std::strerror(errno) << std::endl;
        return;
    }

    // OUTPUTS
    if (bsp_initOutputs() != eCONFIG_GPIO_OK) {
        std::cerr << "Error on outputs config" << std::endl;
    } else {
        std::cout << "Success on outputs config" << std::endl;
        for (int i = 0; i < NUMBER_OF_OUTPUTS; i++) {
            std::cout << "GPIO: " << outputs[i].gpioOutput
                       << ", VALUE: " << outputs[i].valueOutput << std::endl;
        }
    }

    // INPUTS
    if (bsp_initInputs() != eCONFIG_GPIO_OK) {
        std::cerr << "Error on inputs config" << std::endl;
    } else {
        std::cout << "Success on inputs config" << std::endl;
    }
}

void bsp_closeGPIOs() {
    if (reqLedYellow) gpiod_line_request_release(reqLedYellow);
    if (reqButton1) gpiod_line_request_release(reqButton1);
    if (chip) gpiod_chip_close(chip);
}

void bsp_OutputsHandler(void) {
    for (int i = 0; i < NUMBER_OF_OUTPUTS; i++) {
        gpiod_line_value value = outputs[i].valueOutput
            ? GPIOD_LINE_VALUE_ACTIVE
            : GPIOD_LINE_VALUE_INACTIVE;

        gpiod_line_request_set_value(reqLedYellow, outputs[i].gpioOutput, value);
        std::cout << "OUT GPIO: " << outputs[i].gpioOutput
                   << ", VALUE: " << outputs[i].valueOutput << std::endl;
    }
    for (int i = 0; i < NUMBER_OF_INPUTS; i++) {
        std::cout << "IN GPIO: " << inputs[i].gpioInput
                   << ", VALUE: " << inputs[i].valueInput << std::endl;
    }
}

void bsp_SetOutputValue(outputsName_t output, bool value) {
    if (output >= NUMBER_OF_OUTPUTS) {
        return;
    }
    outputs[output].valueOutput = value;
}

int bsp_GetInputValue() {
    inputs[eBUTTON_1].valueInput = gpiod_line_request_get_value(reqButton1, inputs[eBUTTON_1].gpioInput);
    return inputs[eBUTTON_1].valueInput;
}

/* ============================================================
 *  Private functions (static)
 * ============================================================*/
static configGpios_t bsp_initOutputs(void) {

    outputs[eLED_YELLOW].gpioOutput = GPIO_YELLOW_LED;

    struct gpiod_line_settings *settings = gpiod_line_settings_new();
    gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_OUTPUT);

    struct gpiod_line_config *line_cfg = gpiod_line_config_new();
    gpiod_line_config_add_line_settings(
        line_cfg,
        &outputs[eLED_YELLOW].gpioOutput,
        1,
        settings
    );

    reqLedYellow = gpiod_chip_request_lines(chip, nullptr, line_cfg);

    gpiod_line_settings_free(settings);
    gpiod_line_config_free(line_cfg);

    if (!reqLedYellow) {
        std::cerr << "Failed to request line: " << std::strerror(errno) << std::endl;
        return eCONFIG_GPIOS_LINE_ERROR;
    }

    return eCONFIG_GPIO_OK;
}

static configGpios_t bsp_initInputs(void) {

    inputs[eBUTTON_1].gpioInput = GPIO_BUTTON_1;

    struct gpiod_line_settings *settings = gpiod_line_settings_new();
    gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_INPUT);
    gpiod_line_settings_set_bias(settings, GPIOD_LINE_BIAS_PULL_UP);

    struct gpiod_line_config *line_cfg = gpiod_line_config_new();
    gpiod_line_config_add_line_settings(
        line_cfg,
        &inputs[eBUTTON_1].gpioInput,
        1,
        settings
    );

    reqButton1 = gpiod_chip_request_lines(chip, nullptr, line_cfg);

    gpiod_line_settings_free(settings);
    gpiod_line_config_free(line_cfg);

    if (!reqButton1) {
        std::cerr << "Failed to request input line: " << std::strerror(errno) << std::endl;
        return eCONFIG_GPIOS_LINE_ERROR;
    }

    return eCONFIG_GPIO_OK;
}