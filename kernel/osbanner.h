#include <stdint.h>
struct OsBannerPixels {
  uint32_t width;
  uint32_t height;
  uint32_t bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */ 
  uint8_t pixel_data[768 * 38 * 3 + 1];
};
extern const struct OsBannerPixels osbanner;