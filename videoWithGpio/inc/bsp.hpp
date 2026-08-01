#ifndef BSP_HPP
#define BSP_HPP
/* ============================================================
 *  Includes
 * ============================================================*/
#include <cstdint>
#include <cstdio>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================
 *  Defines / Macros
 * ============================================================*/
#define GPIO_YELLOW_LED (18)
#define GPIO_BUTTON_1 (22)
#define LED_ON (1)
#define LED_OFF (0)
#define TIMEOUT_MS(t) ((t) * 1000)

/* ============================================================
 *  Typedefs / Enums / Structs
 * ============================================================*/
typedef enum {
    eLED_YELLOW,
} outputsName_t;

typedef enum {
    eBUTTON_1,
} inputsName_t;

/* ============================================================
 *  Function Prototypes
 * ============================================================*/
void bsp_init();
void bsp_closeGPIOs();
void bsp_OutputsHandler();
void bsp_SetOutputValue(outputsName_t output, bool value);
int bsp_GetInputValue();

#ifdef __cplusplus
}
#endif

#endif /* BSP_HPP */