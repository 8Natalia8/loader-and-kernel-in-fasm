__asm("jmp kmain");
#define PIC1_PORT (0x20)
#define ALG (0x9000) //çíà÷åíèå,ïåðåäàííîå èç çàãðóç÷èêà
#define VIDEO_BUF_PTR (0xb8000)
#define VIDEO_WIDTH (80)
#define INT_MAX 2147483648
#define INT_MIN -2147483647

#define INTEGER_OVERFLOW -3 
#define UNRECOGNIZED_CHARACTER -4
#define GDT_CS (0x8)//!!!!!!!!!!!!!
#define IDT_TYPE_INTR (0x0E)
#define IDT_TYPE_TRAP (0x0F)
#define CURSOR_PORT (0x3D4)
#define END_STR -1212143

#define INVALID_SYNTAX -1212142
#define END_STR -1212143
#define DIVISION_BY_ZERO -1212144
#define MAX_EXP_LEN 256
 

int line = 0, column = 0;//ñòðîêà è ñòîëáåö, â êîòîðûõ ìû íàõîäèìñÿ
int shiftFlag = 0;
char buf[80];
char prev_str[80];
char alg = *(char*)ALG;
char numbers[10];//ìàññèâ öèôð
char small[27];//ìàññèâ ñòðî÷íûõ áóêâ
char symbols[33];//ìàññèâ ñèìâîëîâ
int alg_color;


struct idt_entry {
	unsigned short base_lo;// Ìëàäøèå áèòû àäðåñà îáðàáîò÷èêà
	unsigned short segm_sel; // Ñåëåêòîð ñåãìåíòà êîäà
	unsigned char always0; // Ýòîò áàéò âñåãäà 0
	unsigned char flags; // Ôëàãè òèï. Ôëàãè: P, DPL
	unsigned short base_hi; // Ñòàðøèå áèòû àäðåñà îáðàáîò÷èêà
} __attribute__((packed)); // Âûðàâíèâàíèå çàïðåùåíî
						   // Ñòðóêòóðà, àäðåñ êîòîðîé ïåðåäàåòñÿ êàê àðãóìåíò êîìàíäû lidt
struct idt_ptr {
	unsigned short limit;
	unsigned int base;//void *base;
} __attribute__((packed)); // Âûðàâíèâàíèå çàïðåùåíî
struct idt_entry g_idt[256]; // Ðåàëüíàÿ òàáëèöà IDT
struct idt_ptr g_idtp; // Îïèñàòåëü òàáëèöû äëÿ êîìàíäû lidt
char scan_table_small[84]{
	'0', //0
	'0', //ESC -- 1
	'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
	'0', //BKSP -- 14
	'0', //TAB -- 15
	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
	'0', //ENTER -- 28
	'0', //LCTRL -- 29
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
	'0', //LSHIFT -- 42
	'\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
	'0', // RSHIFT -- 54
	'0', // LALT, RALT -- 56
	'0', // SPACE -- 57
	'0', // CAPS -- 58
	'0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', //F1--F10 -- 59 -- 68
	'0', // NUM Lock -- 69'0', // scrl lock -- 70
	'0', // home -- 71
	'0', // up arrow -- 72
};
char scan_table_symbols_shift[84]{
	'0', //0
	'0', //ESC -- 1
	'!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',
	'0', //BKSP -- 14
	'0', //TAB -- 15
	//'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}',
	'0', //ENTER -- 28
	'0', //LCTRL -- 29
	//'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
	'0', //LSHIFT -- 42
	//'|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',
	'0', // RSHIFT -- 54
	'0', // LALT, RALT -- 56
	'0', // SPACE -- 57
	'0', // CAPS -- 58
	'0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', //F1--F10 -- 59 -- 68
	'0', // NUM Lock -- 69
	'0', // scrl lock -- 70
	'0', // home -- 71
	'0', // up arrow -- 72
};
//Ôóíêöèÿ ðàáîòû ñ ïîðòàìè
static inline unsigned char inb(unsigned short port); //÷òåíèå áàéòà
static inline void outb(unsigned short port, unsigned char data); //çàïèñü áàéòà
static inline void outw(unsigned int port, unsigned int data); //çàïèñü ñëîâà
void cursor_moveto (unsigned int strnum, unsigned int pos);
															   //Ðàáîòà ñ ïðåðûâàíèÿìè
typedef void(*intr_handler)();
void default_intr_handler();
void intr_reg_handler(int num, unsigned short segm_sel, unsigned short flags, intr_handler hndlr);
void keyb_handler();
void intr_init();
void keyb_init();
void intr_start();
void intr_enable();
void intr_disable();

//Âûâîä íà ýêðàí
void check_lines();
void clear_screen();
void out_str(int color, char *ptr);
void out_ch(int color, char ptr);
void add_to_buff();
void basckspace();
void enter();
void shift();
void nonshift();
void space();
void key_on(int code);


void keyb_process_keys();
//Ðàáîòà ñî ñòðîêàìè
int strlen(char *ptr);
int strcmp(char *ptr1, char *ptr2);
int strcmp_len(char *ptr1, char *ptr2, int len);

//OS
int getNumber(char *strExpr, int *iter);
int getSign(char *strExpr, int *iter);
bool isDigit(char c);
//void calculate();
int parsing();
void info();
void stutdown();

//Expresion

int stakNumber[256], iterStakNumber = 0;
int stakSign[256], iterStakSign = 0;
bool error = false;

//Different
void parse_color(char* num);
char* itoa(int n);
extern "C" int kmain();
static inline unsigned char inb(unsigned short port) { // ×òåíèå èç ïîðòà
	unsigned char data;
	asm volatile ("inb %w1, %b0" : "=a" (data) : "Nd" (port));
	return data;
}
static inline void outb(unsigned short port, unsigned char data) { // Çàïèñü
	asm volatile ("outb %b0, %w1" : : "a" (data), "Nd" (port));
}
static inline void outw(unsigned int port, unsigned int data) {
	asm volatile ("outw %w0, %w1" : : "a" (data), "Nd" (port));
}


int strlen( char *str) {
	char *s = str;
	while (*str++);
	return (str - s -1);
	
}

int strcmp(char*str1, char *str2) {
	int i = 0;
	for (i = 0; i < 80 * 2; i++) {
		if (str1[i] == 0 || str2[i] == 0) {
			break;
		}
		if ((str1[i] - str2[i] != 0)) {
			return str1[i] - str2[i];
		}
	}
	return 0;
}

int strcmp_len(char *str1, char *str2, int len) {
	int i = 0;
	for (i = 0; i < len; i++) {
		if (str1[i] == 0 || str2[i] == 0) {
			break;
		}
		if ((str1[i] - str2[i] != 0)) {
			return str1[i] - str2[i];
		}
	}
	return 0;
}
// Ïóñòîé îáðàáîò÷èê ïðåðûâàíèé. 
void default_intr_handler() {
	asm("pusha");
	// ... (ðåàëèçàöèÿ îáðàáîòêè)
	asm("popa; leave; iret");
}
void intr_reg_handler(int num, unsigned short segm_sel, unsigned short flags, intr_handler hndlr) {
	unsigned int hndlr_addr = (unsigned int)hndlr;
	g_idt[num].base_lo = (unsigned short)(hndlr_addr & 0xFFFF); g_idt[num].segm_sel = segm_sel; g_idt[num].always0 = 0;
	g_idt[num].flags = flags;
	g_idt[num].base_hi = (unsigned short)(hndlr_addr >> 16);
}

void keyb_handler() {
	asm("pusha");
	// Îáðàáîòêà ïîñòóïèâøèõ äàííûõ
	keyb_process_keys();
	// Îòïðàâêà êîíòðîëëåðó 8259 íîòèôèêàöèè î òîì, ÷òî ïðåðûâàíèå îáðàáîòàíî
	outb(PIC1_PORT, 0x20);
	asm("popa; leave; iret");
}
// Ôóíêöèÿ èíèöèàëèçàöèè ñèñòåìû ïðåðûâàíèé: çàïîëíåíèå ìàññèâà ñ àäðåñàìè îáðàáîò÷èêîâ
void intr_init() {
	int i;
	int idt_count = sizeof(g_idt) / sizeof(g_idt[0]);
	for (i = 0; i < idt_count; i++) {//öèêë ïî âñåé òàáëèöå
		intr_reg_handler(i, GDT_CS, 0x80 | IDT_TYPE_INTR, default_intr_handler); // segm_sel=0x8, P=1, DPL=0, Type=Intr
	}
}
void keyb_init() {
	// Ðåãèñòðàöèÿ îáðàáîò÷èêà ïðåðûâàíèÿ
	intr_reg_handler(0x09, GDT_CS, 0x80 | IDT_TYPE_INTR, keyb_handler);
	// segm_sel=0x8, P=1, DPL=0, Type=Intr
	// Ðàçðåøåíèå òîëüêî ïðåðûâàíèé êëàâèàòóðû îò êîíòðîëëåðà 8259
	outb(PIC1_PORT + 1, 0xFF ^ 0x02); // 0xFF - âñå ïðåðûâàíèÿ, 0x02 - áèò IRQ1 (êëàâèàòóðà).
									  // Ðàçðåøåíû áóäóò òîëüêî ïðåðûâàíèÿ, ÷üè áèòû óñòàíîâëåíû â 0
}
void intr_start() {
	int idt_count = sizeof(g_idt) / sizeof(g_idt[0]);
	g_idtp.base = (unsigned int)(&g_idt[0]);
	g_idtp.limit = (sizeof(struct idt_entry) * idt_count) - 1;
	asm("lidt %0" : : "m" (g_idtp));
}
void intr_enable() {
	asm("sti");
}
void intr_disable() {
	asm("cli");
}
//int column = 0;
void out_str(int color, char* str) {
	unsigned char* video_buf = (unsigned char*)VIDEO_BUF_PTR;
	video_buf += 80 * 2 * line;
	while (*str) {
		video_buf[0] = (unsigned char)*str; // Ñèìâîë (êîä)
		video_buf[1] = color; //àòðèáóò ñèìâîëà- öâåò 
		video_buf += 2;
		str++;
	}
	line++;
	column = 0;
	check_lines();
	cursor_moveto(line, column);//íóæåí îí íàì èëè íåò???
}
void out_ch(int color, char ptr) {
	unsigned char* video_buf = (unsigned char*)VIDEO_BUF_PTR + 2 * column;
	video_buf += 80 * 2 * line;
	video_buf[0] = (unsigned char)ptr;
	video_buf[1] = color;
	video_buf[2] = ' ';
	video_buf[3] = alg_color;
	column++;
	cursor_moveto(line, column);
	video_buf += 2;
	if (column == 80) { //ïå÷àòü â ñòðîêó, all ðàáîòàåò
		column = 0;
		line++;
		check_lines();
		cursor_moveto(line, column);
	}
}
void clear_screen() {
	unsigned char* video_buf = (unsigned char*)VIDEO_BUF_PTR;
	int i;
	for (i = 0; i < 80 * 2 * 40; i++) {
		video_buf[i] = 0;
	}
	line = 0;
	column = 0;
	cursor_moveto(line, column);
}
void backspace() {
	if (column) {
		column--;
		unsigned char* video_buf = (unsigned char*)VIDEO_BUF_PTR + 2 * column;
		video_buf += 80 * 2 * line;
		video_buf[0] = ' ';
		video_buf[1] = alg_color;
		cursor_moveto(line, column);
	}
	else return;
}
void space() {
	unsigned char* video_buf = (unsigned char*)VIDEO_BUF_PTR + 2 * column;
	video_buf += 80 * 2 * line;
	video_buf[0] = ' ';
	video_buf[1] = alg_color;
	video_buf[2] = ' ';
	video_buf[3] = alg_color;
	column++;
	cursor_moveto(line, column);
}
void enter() {
	unsigned char* video_buf = (unsigned char*)VIDEO_BUF_PTR;
	line++;
	video_buf += 80 * 2 * (line);
	video_buf[0] = ' ';
	video_buf[1] = alg_color;
	column = 0;
	check_lines();
	cursor_moveto(line, column);
}
void shift() {
	shiftFlag = 1;
}
void nonshift() {
	shiftFlag = 0;
}
void print_ch(int code) {
	if (!shiftFlag) {
		out_ch(alg_color, scan_table_small[code]);
	}
	else {
		out_ch(alg_color, scan_table_symbols_shift[code]);
	}
	cursor_moveto(line,column);
}
void add_to_buff() {
	unsigned char* video_buf = (unsigned char*)VIDEO_BUF_PTR;
	video_buf += 80 * 2 * (line - 1);
	for (int i = 0; i<80; i++) {
		buf[i] = 0;
		prev_str[i] = 0;
	}
	for (int i = 0, j = 0; i<80 * 2 - 2; i += 2, j++) {
		if (video_buf[i] >= ' ' && video_buf[i] <= '~') { 
			buf[j] = video_buf[i];
			prev_str[j] = video_buf[i];
			out_ch(alg_color, buf[i]);
		}
		else {
			buf[j] = '\0';
			break;
		}
	}
}
void info() {
	out_str(alg_color, "IBKS 2019");
	out_str(alg_color, "Author is Grigorieva Natalie");
	out_str(alg_color, "Asm interprete: FASM");
	out_str(alg_color, "GCC");
	switch (alg_color) {
	case (0x02):
		out_str(alg_color, "color is green");
		break;
	case (0x01):
		out_str(alg_color, "color is blue");
		break;
	case (0x04):
		out_str(alg_color, "color is red");
		break;
	case (0x0E):
		out_str(alg_color, "color is yellow");
		break;
	case (0x08):
		out_str(alg_color, "color is grey");
		break;
	case (0x07):
		out_str(alg_color, "color is white");
		break;
	default:
		out_str(alg_color, "color is undefined");
		break;
	}
}



char *itoa (int n,char *s) {
///char s[100]={0};
 char *t=s, tmp; 
 int z = 0, len = 0, i = 0;
 if (n < 0) {
 	z=1;
 	n=-n;
 }
 do {
 	*s++ = n % 10 + '0';
 } while (n /= 10);
 
 if (z) {
 	*s++='-';
 }
 
 *s='\0';
 
 len = s - t;
 
 for(i = 0; i < len/2; i++) {
 	tmp = t[i];
 	t[i] = t[len - i - 1];
 	t[len - i - 1] = tmp;
 }
 
 return t;
}



bool isDigit(char c) {
    return '0' <= c && c <= '9';
}
 
int getNumber(char *strEcxp, int *iter) {
    int res = 0;
    int countNumber = 0;
    int i = 0;
    bool isMin = false;
    error = true;
 
    while (strEcxp[(*iter)] == ' ') {
        (*iter)++;
    }
 
    if (strEcxp[(*iter)] == '-') {
        isMin = true;
        (*iter)++;
    }
 
    if (strEcxp[(*iter)] == '\0') {
        return END_STR;
    }
 
    while (isDigit(strEcxp[(*iter)])) {
        countNumber++;
        (*iter)++;
        error = false;
    }
 
    if (error) {
        return INVALID_SYNTAX;
    }
 
    for (i = countNumber - 1; i >= 0; i--) {
        res += strEcxp[(*iter) - i - 1] - '0';
        if (countNumber > 1 && i > 0){
            res *= 10;
        }
    }
 
    return res * (isMin ? -1 : 1);
}
 
int getSign(char *strEcxp, int *iter) {
    while (strEcxp[(*iter)] == ' ') {
        (*iter)++;
    }
 
    if (strEcxp[(*iter)] == '\0') {
        return END_STR;
    }
 
 
    if (strEcxp[(*iter)] == '-' || strEcxp[(*iter)] == '+' ||
        strEcxp[(*iter)] == '*' || strEcxp[(*iter)] == '/' 
	|| strEcxp[(*iter)] == '^' || strEcxp[(*iter)] == '!') {
        return strEcxp[(*iter)++];
    }
 
    error = true;
    return INVALID_SYNTAX;
}
 
int apply(int a, char s, int b) {
int rez1=1;
unsigned int fact=1;
    switch (s) {

        case '-':
if((a>0 && (b<(INT_MIN+a))) || (a<0 && b>INT_MAX+a))
return INTEGER_OVERFLOW;
else
            return a - b;
        case '+':
if((a>INT_MAX/2 && b>INT_MAX/2) || a>INT_MAX || b>INT_MAX )
return INTEGER_OVERFLOW;
else
            return a + b;
        case '*':
if((a>INT_MAX/2 && b>INT_MAX/2) || (a>INT_MAX || b>INT_MAX ))
return INTEGER_OVERFLOW;
else
            return a * b;
        case '/':

            if (b == 0) {
                return DIVISION_BY_ZERO;
            }
            return a / b;
	case '^':
	if (a==0) return 0;
	if (b == 0) return 1;
	else{
	for(;b>0;--b){
		rez1*=a;
		}
//a=rez;
	}
	return rez1;
	case '!':
	//b=1;
	//if(a<0) return INVALID_SYNTAX;
 	if(a==0) return 1;
	else{	
	for(unsigned int i=2;i<=a;i++) fact*=i;
	    }
	return fact;
        default:
            error = true;
            return INVALID_SYNTAX;
    }
}
 
int calk(char *strEcxp) {
    int a = END_STR, b = END_STR;
    int iter = 0;
    int signRet = INVALID_SYNTAX;
    char sign = '\0';
 
    a = getNumber(strEcxp, &iter);
    if (a == END_STR) {
        return a;
    } else if (a == INVALID_SYNTAX) {
        return INVALID_SYNTAX;
    } else if (a == DIVISION_BY_ZERO) {
        return DIVISION_BY_ZERO;
    } else if(a==INTEGER_OVERFLOW){
	return INTEGER_OVERFLOW;
    }else
    strEcxp += iter;
    iter = 0;
 
    signRet = getSign(strEcxp, &iter);
 
    if (signRet == END_STR) {
        return a;
    } else if (signRet == INVALID_SYNTAX) {
        error = true;
        return INVALID_SYNTAX;
    }
    sign = (char)signRet;
 
    strEcxp += iter;
    iter = 0;
 
    while (sign == '*' || sign == '/' || sign == '^'|| sign == '!') {
        b = getNumber(strEcxp, &iter);
        if (b == END_STR) {
            return INVALID_SYNTAX;
        } else if (b == INVALID_SYNTAX) {
            return INVALID_SYNTAX;
        }
        a = apply(a, sign, b);
        if (a == DIVISION_BY_ZERO) {
            return DIVISION_BY_ZERO;
        }else if(a==INTEGER_OVERFLOW){
	return INTEGER_OVERFLOW;
    }
 
        strEcxp += iter;
        iter = 0;
 
        signRet = getSign(strEcxp, &iter);
 
        if (signRet == END_STR) {
            return a;
        } else if (signRet == INVALID_SYNTAX) {
            error = true;
            return INVALID_SYNTAX;
        }
        sign = (char)signRet;
 
        strEcxp += iter;
        iter = 0;
    }
 
 
    b = calk(strEcxp + iter);
    if (b == END_STR) {
        return INVALID_SYNTAX;
    } else if (b == INVALID_SYNTAX) {
        error = true;
        return INVALID_SYNTAX;
    } else if (b == DIVISION_BY_ZERO) {
        return DIVISION_BY_ZERO;
    }else if(b == INTEGER_OVERFLOW){
	return INTEGER_OVERFLOW;
    }

    return apply(a, sign, b);
}
 
int parsing(char *buf) {
    char *strEcxp = buf + 5;
    int iter = 0;
    int countMinus = 0;
 
    while (strEcxp[iter] == '-') {
        countMinus++;
        iter++;
    }
 
    if (countMinus % 2 != 0 && countMinus > 0) {
        iter -= 1;
    }
 
    return calk(strEcxp + iter);
}

void shutdown() {
	outw(0xB004, 0x2000);//qemu <1.7
	outw(0x604, 0x2000);//qemu>=1.7
}
void parse() {
	if (strcmp(buf, "info") == 0) {
		info();
	}
	else if (strcmp_len(buf, "expr ", 5) == 0) {
bool error = false;
		int res=parsing(buf);
if(res==INVALID_SYNTAX) out_str(0x07, "INVALID_SYNTAX");
else if(res==DIVISION_BY_ZERO) out_str(0x07, "DIVISION_BY_ZERO");
else if(res==INTEGER_OVERFLOW) out_str(0x07,"INTEGER_OVERFLOW");
else{
char t[100]={0};
out_str(0x05,itoa(res,t));//magenta
	}}
	else if (strcmp(buf, "clear") == 0) {
		clear_screen();
	}
	else if (strcmp(buf, "shutdown") == 0) {
		shutdown();
	}
	else {
		out_str(0x03, "ERROR: UNRECOGNIZED COMAND");
	}
}
void check_lines(){
if (line >= 25) {
unsigned char* video_buf_in = (unsigned char*) VIDEO_BUF_PTR;
unsigned char* video_buf_out = (unsigned char*) VIDEO_BUF_PTR;
int tmpline = 2;
video_buf_in += 80*2*(tmpline-2);
video_buf_out += 80*2*(tmpline-1);
while (tmpline <= 25) {
for (int i = 0; i<80*2; i++){
video_buf_in[i] = video_buf_out[i];
}
tmpline++;
video_buf_in = (unsigned char*)VIDEO_BUF_PTR + 80*2*(tmpline-2);
video_buf_out = (unsigned char*)VIDEO_BUF_PTR + 80*2*(tmpline-1);
}
video_buf_in = (unsigned char*)VIDEO_BUF_PTR + 80*2*24;
for (int i = 0; video_buf_in[i] != 0, i<80*2; i++){
video_buf_in[i] = 0;
}
line--;
unsigned char* video_buf = (unsigned char*) VIDEO_BUF_PTR + 80*2*(line);
video_buf[0] = ' ';
video_buf[1] = alg_color;
column = 0;
cursor_moveto(line, column);
}
}
void cursor_moveto (unsigned int strnum, unsigned int pos) {
unsigned short new_pos = (strnum * VIDEO_WIDTH) + pos;
outb(CURSOR_PORT, 0x0F);
outb(CURSOR_PORT + 1, (unsigned char)(new_pos & 0xFF));
outb(CURSOR_PORT, 0x0E);
outb(CURSOR_PORT + 1, (unsigned char)( (new_pos>> 8) & 0xFF));
}
void key_on(int code) {
	switch (code) {
	case 14:
		backspace();
		break;
	case 28:
		enter();
		add_to_buff();
		parse();
		break;
	case 42:
		shift();
		break;
	case 54:
		shift();
		break;
	case 57:
		space();
		break;
	default:
		print_ch(code);
		break;
	}
}
void key_off(int code) {
	switch (code) {
	case 170: //ïîòîìó ÷òî äâà ðàçíûõ øèôòà
		nonshift();
		break;
	case 182:
		nonshift();
		break;
	}
}
void keyb_process_keys() {
	// Ïðîâåðêà ÷òî áóôåð PS/2 êëàâèàòóðû íå ïóñò (ìëàäøèé áèò ïðèñóòñòâóåò)
	if (inb(0x64) & 0x01) {
		unsigned char code;
		unsigned char state;
		code = inb(0x60); // Ñ÷èòûâàíèå ñèìâîëà ñ PS/2 êëàâèàòóðû
		if (code < 128) { // Ñêàí-êîäû âûøå 128 - ýòî îòïóñêàíèå êëàâèøè
			key_on(code);
		}
		else if (code >= 128) {
			key_off(code);
		}
	}
}
void parse_color(char* num) {
	switch (*num) {
	case '1':
		alg_color = 0x02; //green
		break;
	case '2':
		alg_color = 0x01; //blue
		break;
	case '3':
		alg_color = 0x04; //red
		break;
	case '4':
		alg_color = 0x0E; //yellow
		break;
	case '5':
		alg_color = 0x08; //grey
		break;
	case '6':
		alg_color = 0x07; //white
		break;
	default:
		alg_color = 0x07;
		break;
	}
}
extern "C" int kmain() {
	char *hello = "WELCOME TO CALCOS";
	char *ptr;
	*ptr = *(char*)ALG;
	parse_color(ptr);
	//clear_screen();
	out_str(alg_color, hello);
	clear_screen();
	out_str(alg_color, hello);
	intr_init();
	intr_start();
	intr_enable();
	keyb_init();
	while (1)
	{
		asm("hlt");
	}
	return 0;
}
