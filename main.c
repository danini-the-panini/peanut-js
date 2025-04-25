#include <stdint.h>
#include <stdio.h>
#include <emscripten/emscripten.h>

#define ENABLE_SOUND 1

#include "minigb_apu.h"

#define AUDIO_BUFFER_SIZE_BYTES (AUDIO_SAMPLES*4)

struct minigb_apu_ctx apu;
audio_sample_t *stream;

void audio_write(uint_fast32_t addr, uint8_t val)
{
  minigb_apu_audio_write(&apu, addr, val);
}

uint8_t audio_read(uint_fast32_t addr)
{
  return minigb_apu_audio_read(&apu, addr);
}

#include "peanut_gb.h"

#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN
#endif

EXTERN extern void js_init(const uint8_t width, const uint8_t height, const int audioSamples, const int audioSampleRate);
EXTERN extern void draw_line(const uint8_t *pixels, const uint8_t width, const uint_fast8_t line);
EXTERN extern void js_play_audio(const audio_sample_t *stream, const unsigned int len);

struct gb_s gb;
uint8_t *rom;

uint8_t rom_read(struct gb_s *gb, const uint_fast32_t addr)
{
  return rom[addr];
}

uint8_t cart_ram_read(struct gb_s *gb, const uint_fast32_t addr)
{
  return 0;
}

void cart_ram_write(struct gb_s *gb, const uint_fast32_t addr, const uint8_t value)
{
  
}

void on_error(struct gb_s *gb, const enum gb_error_e e, const uint16_t addr)
{
  printf("ERROR: %d at %d\n", e, addr);
}

void js_draw_line(struct gb_s *gb, const uint8_t *pixels, const uint_fast8_t line)
{
  draw_line(pixels, LCD_WIDTH, line);
}

int main() {
  stream=malloc(AUDIO_BUFFER_SIZE_BYTES);
  memset(stream, 0, AUDIO_BUFFER_SIZE_BYTES);
  js_init(LCD_WIDTH, LCD_HEIGHT, AUDIO_SAMPLES, AUDIO_SAMPLE_RATE);
  return 0;
}

EXTERN EMSCRIPTEN_KEEPALIVE void js_gb_init(int argc, char ** argv)
{
  enum gb_init_error_e e = gb_init(&gb,
    rom_read,
    cart_ram_read,
    cart_ram_write,
    on_error,
    NULL
  );

  if (e != GB_INIT_NO_ERROR) {
    printf("Error initializing gb: %d\n", e);
    return;
  }

  gb_init_lcd(&gb, js_draw_line);

  minigb_apu_audio_init(&apu);
}

EXTERN EMSCRIPTEN_KEEPALIVE void js_load_rom(uint8_t *data, uint_fast32_t len)
{
  printf("Loading ROM: %d bytes\n", len);
  rom = malloc(len);
  memcpy(rom, data, len);
}

EXTERN EMSCRIPTEN_KEEPALIVE void js_run_frame()
{
  gb_run_frame(&gb);
  minigb_apu_audio_callback(&apu, stream);
  js_play_audio(stream, AUDIO_BUFFER_SIZE_BYTES);
}

EXTERN EMSCRIPTEN_KEEPALIVE void js_set_joypad(
  bool a,
  bool b,
  bool select,
  bool start,
  bool right,
  bool left,
  bool up,
  bool down
)
{
  gb.direct.joypad_bits.a = !a;
  gb.direct.joypad_bits.b = !b;
  gb.direct.joypad_bits.select = !select;
  gb.direct.joypad_bits.start = !start;
  gb.direct.joypad_bits.right= !right;
  gb.direct.joypad_bits.left = !left;
  gb.direct.joypad_bits.up = !up;
  gb.direct.joypad_bits.down = !down;
}
