#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Single Pipes
#define ASCII_PIPE_UP 179u
#define ASCII_PIPE_RIGHTDOWN 191u
#define ASCII_PIPE_RIGHTDOWN_BOT 217u
#define ASCII_PIPE_LEFTDOWN_BOT 192u
#define ASCII_PIPE_LEFTDOWN 218u
#define ASCII_PIPE_LINE 196u

// Double Pipes
#define ASCII_DOUBLEPIPE_UP 186u
#define ASCII_DOUBLEPIPE_RIGHTDOWN 187u
#define ASCII_DOUBLEPIPE_RIGHTDOWN_BOT 188u
#define ASCII_DOUBLEPIPE_LEFTDOWN_BOT 200u
#define ASCII_DOUBLEPIPE_LEFTDOWN 201u
#define ASCII_DOUBLEPIPE_LINE 205u

// Some Addional ASCII
#define ASCII_LINE_UP 45u
#define ASCII_LINE_SIDE 47u
#define ASCII_LINE_SIDEI 92u

// Idek what are theese lines
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

#if !defined(__i386__)
#endif

// Defined colors so u dont strugle
enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

// Idk some colors and stuff
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg)
{
	return fg | bg << 4;
}
static inline uint16_t vga_entry(unsigned char uc, uint8_t color)
{
	return (uint16_t)uc | (uint16_t)color << 8;
}
size_t strlen(const char* str)
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

// Vars dont touch or u brek stuf
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
size_t terminal_row = 0;
size_t terminal_column = 0;
uint8_t terminal_color;
uint16_t* terminal_buffer;
uint8_t fgc;
uint8_t bgc;
uint8_t fgcs;
uint8_t bgcs;

// Fancy terminal stuff
void terminal_fillcolor(uint8_t clr1, uint8_t clr2)
{
	fgc = clr1;
	bgc = clr2;
	terminal_color = vga_entry_color(fgc, bgc);
	terminal_buffer = (uint16_t*)0xB8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
	terminal_row = 0;
	terminal_column = 0;
}
void terminal_clear()
{
	terminal_buffer = (uint16_t*)0xB8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
	terminal_row = 0;
	terminal_column = 0;
}
void terminal_setcolor(uint8_t clr1, uint8_t clr2)
{
	fgc = clr1;
	bgc = clr2;
	terminal_color = vga_entry_color(fgc, bgc);
}

// Text Stuff
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}
void terminal_putchar(char c)
{
	terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
	if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT)
			terminal_row = 0;
	}
}
void terminal_write(const char* data, size_t size)
{
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}
void terminal_writestring(const char* data)
{
	terminal_write(data, strlen(data));
}
void terminal_newline() {
	terminal_row++;
	terminal_column = 0;

	return;
}

// Addional Stuff
void terminal_writestart() {
	terminal_putchar(ASCII_DOUBLEPIPE_LEFTDOWN);
	for (int i = 0; i < 8; i++) {
		terminal_putchar(ASCII_DOUBLEPIPE_LINE);
	}
	terminal_putchar(ASCII_DOUBLEPIPE_RIGHTDOWN);
	terminal_newline();
	terminal_putchar(ASCII_DOUBLEPIPE_UP);
	terminal_writestring("Mikicrep");
	terminal_putchar(ASCII_DOUBLEPIPE_UP);
	terminal_newline();
	terminal_putchar(ASCII_DOUBLEPIPE_UP);
	terminal_putchar(ASCII_LINE_UP);
	terminal_writestring("Kernel");
	terminal_putchar(ASCII_LINE_UP);
	terminal_putchar(ASCII_DOUBLEPIPE_UP);
	terminal_newline();
	terminal_putchar(ASCII_DOUBLEPIPE_LEFTDOWN_BOT);
	for (int i = 0; i < 8; i++) {
		terminal_putchar(ASCII_DOUBLEPIPE_LINE);
	}
	terminal_putchar(ASCII_DOUBLEPIPE_RIGHTDOWN_BOT);
	terminal_newline();
}
void terminal_writebox(int width, int height) {
	width -= 2;
	int spacesin = width;

	terminal_putchar(ASCII_DOUBLEPIPE_LEFTDOWN);
	for (int i = 0; i < width; i++) {
		terminal_putchar(ASCII_DOUBLEPIPE_LINE);
	}
	terminal_putchar(ASCII_DOUBLEPIPE_RIGHTDOWN);
	terminal_newline();

	if (height != 0) {
		for (int u = 0; u < height; u++) {
			terminal_putchar(ASCII_DOUBLEPIPE_UP);
			for (int i = 0; i < spacesin; i++) {
				terminal_writestring(" ");
			}
			terminal_putchar(ASCII_DOUBLEPIPE_UP);
			terminal_newline();
		}
	}

	terminal_putchar(ASCII_DOUBLEPIPE_LEFTDOWN_BOT);
	for (int i = 0; i < width; i++) {
		terminal_putchar(ASCII_DOUBLEPIPE_LINE);
	}
	terminal_putchar(ASCII_DOUBLEPIPE_RIGHTDOWN_BOT);
	terminal_newline();
}
void terminal_writetriangle(int size) {
	int spacesin = 0;
	int sizewee = size + 2;
	int tsize = size;

	for (int u = 0; u < size; u++) {

		for (int i = 0; i < tsize; i++) {
			terminal_writestring(" ");
			tsize--;
		}
		terminal_putchar(ASCII_LINE_SIDE);

		for (int t = 0; t < spacesin; t++) {
			terminal_writestring(" ");
			
		}
		spacesin++;
		terminal_putchar(ASCII_LINE_SIDEI);
		terminal_newline();
		
	}

	for (int o = 0; o < sizewee; o++) {
		terminal_putchar(ASCII_LINE_UP);
	}
	terminal_newline();
}
void terminal_writergb() {
	fgcs = fgc;
	bgcs = bgc;
	terminal_setcolor(VGA_COLOR_BLACK, VGA_COLOR_BLACK);
	terminal_writestring("  ");
	terminal_setcolor(VGA_COLOR_DARK_GREY, VGA_COLOR_DARK_GREY);
	terminal_writestring("  ");
	terminal_setcolor(VGA_COLOR_LIGHT_GREY, VGA_COLOR_LIGHT_GREY);
	terminal_writestring("  ");
	terminal_setcolor(VGA_COLOR_WHITE, VGA_COLOR_WHITE);
	terminal_writestring("  ");
	terminal_setcolor(VGA_COLOR_BLUE, VGA_COLOR_BLUE);
	terminal_writestring("  ");
	terminal_setcolor(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_LIGHT_BLUE);
	terminal_writestring("  ");
	terminal_setcolor(VGA_COLOR_CYAN, VGA_COLOR_CYAN);
	terminal_writestring("  ");
	terminal_setcolor(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_LIGHT_CYAN);
	terminal_writestring("  ");
	terminal_setcolor(VGA_COLOR_RED, VGA_COLOR_RED);
	terminal_writestring("  ");
	terminal_setcolor(VGA_COLOR_LIGHT_RED, VGA_COLOR_LIGHT_RED);
	terminal_writestring("  ");
	terminal_setcolor(VGA_COLOR_GREEN, VGA_COLOR_GREEN);
	terminal_writestring("  ");
	terminal_setcolor(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_LIGHT_GREEN);
	terminal_writestring("  ");
	terminal_setcolor(VGA_COLOR_BROWN, VGA_COLOR_BROWN);
	terminal_writestring("  ");
	terminal_setcolor(VGA_COLOR_LIGHT_BROWN, VGA_COLOR_LIGHT_BROWN);
	terminal_writestring("  ");
	terminal_setcolor(VGA_COLOR_MAGENTA, VGA_COLOR_MAGENTA);
	terminal_writestring("  ");
	terminal_setcolor(VGA_COLOR_LIGHT_MAGENTA, VGA_COLOR_LIGHT_MAGENTA);
	terminal_writestring("  ");
	fgc = fgcs;
	bgc = bgcs;
}
void doonstart() {
	terminal_fillcolor(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_writestart();
}

// Write your code here (main code will be executed here, dont change function name)
void kernel_main(void)
{ 
	doonstart();
	terminal_writestring("Thank you for tryping Mikicrep Kernel, please check our discord server if you need help https://discord.gg/52DNUEyeD7 also we got our site https://mikicrepstudios.github.io there will be some tutorials soon");
}


// terminal_writetriangle does not work, currently in dev

// Mikicrep Kernel Build 1