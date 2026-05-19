// توابع پورت I/O
static inline unsigned char inb(unsigned short port) {
    unsigned char result;
    __asm__ volatile("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

static inline void outb(unsigned short port, unsigned char data) {
    __asm__ volatile("outb %0, %1" : : "a"(data), "Nd"(port));
}

// توابع رشته‌ای
int strlen(const char* str) {
    int len = 0;
    while (str[len]) len++;
    return len;
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

void strcpy(char* dest, const char* src) {
    while (*src) {
        *dest++ = *src++;
    }
    *dest = 0;
}

// توابع صفحه نمایش
#define VIDEO_MEMORY 0xB8000
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
#define WHITE_ON_BLACK 0x0F

static int cursor_x = 0;
static int cursor_y = 0;

void clear_screen() {
    volatile char* video = (volatile char*)VIDEO_MEMORY;
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT * 2; i += 2) {
        video[i] = ' ';
        video[i+1] = WHITE_ON_BLACK;
    }
    cursor_x = 0;
    cursor_y = 0;
}

void scroll() {
    volatile char* video = (volatile char*)VIDEO_MEMORY;
    for (int i = 0; i < (SCREEN_HEIGHT-1)*SCREEN_WIDTH*2; i++) {
        video[i] = video[i + SCREEN_WIDTH*2];
    }
    for (int i = (SCREEN_HEIGHT-1)*SCREEN_WIDTH*2; i < SCREEN_HEIGHT*SCREEN_WIDTH*2; i += 2) {
        video[i] = ' ';
        video[i+1] = WHITE_ON_BLACK;
    }
    cursor_y = SCREEN_HEIGHT-1;
}

void print_char(char c) {
    volatile char* video = (volatile char*)VIDEO_MEMORY;
    
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
        if (cursor_y >= SCREEN_HEIGHT) scroll();
        return;
    }
    
    if (c == '\b') {
        if (cursor_x > 0) {
            cursor_x--;
            int pos = (cursor_y*SCREEN_WIDTH + cursor_x)*2;
            video[pos] = ' ';
            video[pos+1] = WHITE_ON_BLACK;
        }
        return;
    }
    
    int pos = (cursor_y*SCREEN_WIDTH + cursor_x)*2;
    video[pos] = c;
    video[pos+1] = WHITE_ON_BLACK;
    
    cursor_x++;
    if (cursor_x >= SCREEN_WIDTH) {
        cursor_x = 0;
        cursor_y++;
        if (cursor_y >= SCREEN_HEIGHT) scroll();
    }
}

void print(const char* str) {
    while (*str) {
        print_char(*str++);
    }
}

void print_line(const char* str) {
    print(str);
    print_char('\n');
}

// توابع ورودی کیبورد
char get_char() {
    unsigned char scancode;
    static const char map[] = {
        0,0,'1','2','3','4','5','6','7','8','9','0','-','=','\b',
        '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
        0,'a','s','d','f','g','h','j','k','l',';','\'','`',
        0,'\\','z','x','c','v','b','n','m',',','.','/',0,
        '*',0,' '
    };
    
    while (1) {
        while (!(inb(0x64) & 1));
        scancode = inb(0x60);
        if (scancode & 0x80) continue;
        if (scancode < sizeof(map) && map[scancode])
            return map[scancode];
    }
}

void read_line(char* buffer, int max) {
    int i = 0;
    while (1) {
        char c = get_char();
        if (c == '\n') {
            buffer[i] = 0;
            print_char('\n');
            return;
        }
        if (c == '\b') {
            if (i > 0) {
                i--;
                print_char('\b');
            }
            continue;
        }
        if (i < max-1) {
            buffer[i++] = c;
            print_char(c);
        }
    }
}

// شل
void shell() {
    char input[128];
    
    while (1) {
        print("> ");
        read_line(input, 128);
        
        if (strcmp(input, "help") == 0) {
            print_line("Commands:");
            print_line("  help  - show this help");
            print_line("  clear - clear screen");
        }
        else if (strcmp(input, "clear") == 0) {
            clear_screen();
        }
        else if (strlen(input) == 0) {
            // خالی، هیچ کاری نکن
        }
        else {
            print("Unknown command: ");
            print_line(input);
        }
    }
}


// نقطه ورود کرنل
void kernel_main() {

    clear_screen();

    print_line("Simple OS Kernel");
    print_line("Type 'help' for commands");

    shell();
}
