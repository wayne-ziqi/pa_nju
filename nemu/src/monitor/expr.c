#include "nemu.h"
#include "cpu/reg.h"
#include "memory/memory.h"


#include <stdlib.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

extern CPU_STATE cpu;

enum
{
NOTYPE = 256,
    EQ = 0,
    NEQ = 1,
    AND = 2,
    OR = 3,
    NUM = 255,
    HEX = 254,
    ALPHA = 253,
    REG = 252,
    DEREF = 251,
    OPPO = 250,
    BE = 249,
    AE = 248,
    B = 247,
    A = 246,
    BOR = 245,
    BAND = 244,
    BXOR = 243
	/* TODO: Add more token types */
	

};

static struct rule
{
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	    {" +",                NOTYPE}, // white space
        {"\\+",               '+'},    // ascii 43
        {"\\-",               '-'},    // ascii 45
        {"\\*",               '*'},    // asc
        {"\\/",               '/'},    // ascii 47ii 42
        {"\\(",               '('},    // ascii 40
        {"\\)",               ')'},    // ascii 41
        {"==",                 EQ},
        {"&&",                AND},
        {"&",                BAND},
        {"\\|\\|",             OR},
        {"\\|",               BOR},
        {"\\^",               BXOR},
        {"!=",                NEQ},
        {"!",                 '!'},    // ascii 33
        {"~",                 '~'},
        {"0[xX][0-9a-fA-F]+", HEX},
        {"[0-9]{1,10}",       NUM},
        {"\\$[a-zA-Z]{2,3}",  REG},
        {"[a-zA-Z_]+",      ALPHA},
        {">=",                 AE},
        {"<=",                 BE},
        {">",                   A},
        {"<",                   B}

	
};

static void utoa(uint32_t num, char str[32], int i1);

static uint32_t hex2dec(char *hex);

uint32_t eval(int p, int q);

bool check_parentheses(int p, int q);

static int getDominPos(int p, int q);

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]))

static regex_t re[NR_REGEX];

/* Rules are used for more times.
 * Therefore we compile them only once before any usage.
 */
void init_regex()
{
	int i;
	char error_msg[128];
	int ret;

	for (i = 0; i < NR_REGEX; i++)
	{
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if (ret != 0)
		{
			regerror(ret, &re[i], error_msg, 128);
			assert(ret != 0);
		}
	}
}

typedef struct token
{
	int type;
	char str[32];
} Token;

Token tokens[32];
int nr_token;

uint32_t look_up_symtab(char *, bool *);

static bool make_token(char *e)
{
    //printf("%s\n",e);
	int position = 0;
	int i;
	regmatch_t pmatch;
    //init_regex();
	nr_token = 0;

	while (e[position] != '\0')
	{
		/* Try all rules one by one. */
		for (i = 0; i < NR_REGEX; i++)
		{
			if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0)
			{
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				//printf("match regex[%d] at position %d with len %d: %.*s", i, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. 
				 * Add codes to perform some actions with this token.
				 */

				switch (rules[i].token_type) {
                    case NOTYPE:
                        break;
                    case NUM: {
                        tokens[nr_token].type = rules[i].token_type;
                        assert(substr_len < 32);
                        memcpy(tokens[nr_token].str, substr_start, substr_len * sizeof(char));
                        tokens[nr_token].str[substr_len] = '\0';
                        nr_token++;
                        break;
                    }
                    case HEX: {
                        assert(substr_len < 32);
                        char hex[32];
                        memcpy(hex, substr_start + 2, (substr_len - 2) * sizeof(char));
                        hex[substr_len - 2] = '\0';
                        uint32_t dec_val = hex2dec(hex);
                        tokens[nr_token].type = NUM;
                        utoa(dec_val, tokens[nr_token].str, 10);
                        nr_token++;
                        break;
                    }
                    case REG: {
                        assert(substr_len < 5);
                        char reg_name[4];
                        memcpy(reg_name, substr_start + 1, (substr_len - 1) * sizeof(char));
                        reg_name[substr_len - 1] = '\0';
                        //分情况比对，21/10/8
                        if (strcmp((const char *) reg_name, "eax") == 0) {
                            utoa(cpu.eax, tokens[nr_token].str, 10);
                        }
                        if (strcmp((const char *) reg_name, "ecx") == 0) {
                            utoa(cpu.ecx, tokens[nr_token].str, 10);
                        }
                        if (strcmp((const char *) reg_name, "edx") == 0) {
                            utoa(cpu.edx, tokens[nr_token].str, 10);
                        }
                        if (strcmp((const char *) reg_name, "ebx") == 0) {
                            utoa(cpu.ebx, tokens[nr_token].str, 10);
                        }
                        if (strcmp((const char *) reg_name, "esp") == 0) {
                            utoa(cpu.esp, tokens[nr_token].str, 10);
                        }
                        if (strcmp((const char *) reg_name, "ebp") == 0) {
                            utoa(cpu.ebp, tokens[nr_token].str, 10);
                        }
                        if (strcmp((const char *) reg_name, "esi") == 0) {
                            utoa(cpu.esi, tokens[nr_token].str, 10);
                        }
                        if (strcmp((const char *) reg_name, "edi") == 0) {
                            utoa(cpu.edi, tokens[nr_token].str, 10);
                        }
                        if (strcmp((const char *) reg_name, "eip") == 0) {
                            utoa(cpu.eip, tokens[nr_token].str, 10);
                        }

                        tokens[nr_token].type = NUM;
                        nr_token++;
                        break;
                    }
                    case ALPHA: {
                        assert(substr_len < 32);
                        char vname[32];
                        memcpy(vname, substr_start, substr_len * sizeof(char));
                        vname[substr_len] = '\0';
                        bool success = true;
                        uint32_t addr = look_up_symtab(vname,&success);
                        if (!success){
                            printf("No symbol named %s\n", vname);
                            return false;
                        }
                        
                        else {
                            utoa(addr, tokens[nr_token].str, 10);
                            tokens[nr_token].type = NUM;
                        }
                        nr_token++;
                        break;
                        // should  dereference later
                        /*
                        uint32_t val = -35697;//vaddr_read(addr,SREG_CS,4);

                        if (nr_token == 1 && tokens[0].type == '*') {
                            tokens[nr_token].type = NUM;
                            itoa((int) val, tokens[nr_token].str, 10);
                            tokens[0].type = DEREF;
                        } else if (
                                nr_token >= 2 &&
                                tokens[nr_token - 1].type == '*' &&
                                (tokens[nr_token - 2].type == '+' ||
                                 tokens[nr_token - 2].type == '-' ||
                                 tokens[nr_token - 2].type == '*' ||
                                 tokens[nr_token - 2].type == '/' ||
                                 tokens[nr_token - 2].type == '!' ||
                                 tokens[nr_token - 2].type == AND ||
                                 tokens[nr_token - 2].type == EQ ||
                                 tokens[nr_token - 2].type == NEQ)) {
                            tokens[nr_token].type = NUM;
                            tokens[nr_token - 1].type = DEREF;
                            itoa((int) val, tokens[nr_token].str, 10);
                        } else {
                            tokens[nr_token].type = NUM;
                            itoa((int) addr, tokens[nr_token].str, 10);
                        }
                        */

                    }
                    case '*':
                    case '-': {
                        if (nr_token == 0 ||
                            tokens[nr_token - 1].type == '+' ||
                            tokens[nr_token - 1].type == '-' ||
                            tokens[nr_token - 1].type == '*' ||
                            tokens[nr_token - 1].type == '/' ||
                            tokens[nr_token - 1].type == '!' ||
                            tokens[nr_token - 1].type == AND ||
                            tokens[nr_token - 1].type == EQ ||
                            tokens[nr_token - 1].type == NEQ ||
                            tokens[nr_token - 1].type == DEREF ||
                            tokens[nr_token - 1].type == OPPO
                                ) {
                            if (rules[i].token_type == '*')
                                tokens[nr_token].type = DEREF;
                            else
                                tokens[nr_token].type = OPPO;
                        } else {
                            if (rules[i].token_type == '*')
                                tokens[nr_token].type = '*';
                            else tokens[nr_token].type = '-';
                        }

                        nr_token++;
                        break;
                    }
                    default:
                        tokens[nr_token].type = rules[i].token_type;
                        nr_token++;
                }

				break;
			}
		}

		if (i == NR_REGEX)
		{
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}
    //printf("%d\n",nr_token);
    /*
    for(int i = 0;i<nr_token;i++){
        printf("%d",tokens[i].type);
        printf("\n");
    }
    for(int i = 0;i<nr_token;i++){
        printf("%s",tokens[i].str);
        printf("\n");
    }
    */
    
	return true;
}

static uint32_t hex2dec(char *hex) {
    uint32_t dec = 0;
    //hex to dec, except 0x
    for (int i = 0; i < strlen(hex); i++) {
        if (hex[i] >= 'a' && hex[i] <= 'z')
            dec = dec * 16 + 10 + hex[i] - 'a';
        else if (hex[i] >= 'A' && hex[i] <= 'Z')
            dec = dec * 16 + 10 + hex[i] - 'A';
        else
            dec = dec * 16 + hex[i] - '0';
    }
    return dec;
}

static uint32_t atou(char *s) {
    uint32_t res = 0;
    for (int i = 0; i < strlen(s); i++) {
        res = res * 10 + s[i] - '0';
    }
    return res;
}

static void utoa(uint32_t num, char str[32], int i1) {
    int len = 0;
    bool neg = 0;
    if (num < 0)neg = 1;
    while (num != 0) {
        str[len++] = (char) ('0' + (num % i1));
        num /= i1;
    }
    if (neg) str[len++] = '-';
    for (int i = 0, j = len - 1; i < j; i++, j--) {
        char tmp = str[i];
        str[i] = str[j];
        str[j] = tmp;
    }
    str[len] = '\0';
}


uint32_t expr(char *e, bool *success)
{
	if (!make_token(e)) {
        *success = false;
        return 0;
    }
    *success = true;
    return eval(0, nr_token - 1);
}


uint32_t eval(int p, int q) {
    //printf("p = %d, q = %d\n", p,q);
    uint32_t res = 0;
    if (p > q) {
        printf("bad expression\n");
        return -1;
    } else if (p == q) {
        res = atou(tokens[p].str);
        // printf("res = %u\n", res);
        return res;
    } else if (check_parentheses(p, q) == true) {
        res = eval(p + 1, q - 1);
    } else {
        int op = getDominPos(p, q);  //得到最低优先级下标
        uint32_t val1 = 0;
        if (!(tokens[op].type == DEREF || tokens[op].type == OPPO || tokens[op].type == '!' || tokens[op].type == '~')) {
            val1 = eval(p, op - 1);  //val1 can be evaluated only when the operator is between two operands
        }
        uint32_t val2 = eval(op + 1, q);
        switch (tokens[op].type) {
            case '+':
                {return val1 + val2;}
            case '-':
                return val1 - val2;
            case '*':
                return val1 * val2;
            case '/':
                return val1 / val2;
            case 'A':
                return val1 > val2;
            case AE:
                return val1 >= val2;
            case B:
                return val1 < val2;
            case BE:
                return val1 <= val2;
            case EQ:
                return val1 == val2;
            case NEQ:
                return val1 != val2;
            case AND:
                return val1 && val2;
            case OR:
                return val1 || val2;
            case BOR:
                return val1 | val2;
            case BXOR:
                return val1 ^ val2;
            case BAND:
                return val1 & val2;
            case '!':
                return !val2;
            case '~':
                return ~val2;
            case DEREF: {
                //printf("addr = %u\n", val2);
                uint32_t val = vaddr_read(val2, SREG_CS, 4);
                return val;
            }
            case OPPO:
                return -val2;

            default:
                assert(0);
        }
    }
    return res;
}

static int priority(int type);

static int getDominPos(int p, int q) {
    int pos = q;
    for (int i = q; i >= p; i--) {      //operators having the same priority should be calculated from right to left
        if (tokens[i].type == ')') {
            int cnt_r = 0;
            while (i >= p) {
                if (tokens[i].type == ')') cnt_r++;
                else if (tokens[i].type == '(') cnt_r--;
                if (cnt_r == 0)break;
                i--;
            }
            if (cnt_r != 0) {
                printf("parentheses mismatch\n");
                assert(0);
            }
        }
        pos = priority(tokens[pos].type) <= priority(tokens[i].type) ? pos : i;
    }
    assert(pos < q);
    return pos;
}

/*
 * 优先级	    分类	                运算符	                    结合性
    1	逗号运算符	                  ,	                        从左到右
    2	赋值运算符	=、+=、-=、*=、/=、 %=、 >=、 <<=、&=、^=、|=	从右到左
    3	逻辑或	                      ||	                    从左到右
    4	逻辑与	                      &&	                    从左到右
    5	按位或	                      |	                        从左到右
    6	按位异或	                      ^	                        从左到右
    7	按位与	                      &	                        从左到右
    8	相等/不等	                    ==、!=	                    从左到右
    9	关系运算符	              <、<=、>、>=	                从左到右
    10	位移运算符	                <<、>>	                    从左到右
    11	加法/减法               	    +、-	                    从左到右
    12	乘法/除法/取余	        *（乘号）、/、%	                从左到右
    13	单目运算符	!、*（指针）、& 、++、–、+（正号）、-（负号）	    从右到左
    14	后缀运算符	            ( )、[ ]、->	                    从左到右
*/
static int priority(int type) {
    if (type == OR) return 3;
    if (type == AND) return 4;
    if (type == BOR) return 5;
    if (type == BXOR) return 6;
    if (type == BAND) return 7;
    if (type == EQ || type == NEQ) return 8;
    if (type == A || type == AE || type == B || type == BE) return 9;
    // << >>

    if (type == '+' || type == '-') return 11;
    if (type == '*' || type == '/') return 12;
    if (type == '!' || type == '~' || type == DEREF || type == OPPO) return 13;
        if(type == '('||type == ')')return 14;
    if (type == NUM) return 255;
    return 254;
}

bool check_parentheses(int p, int q) {
    if (tokens[p].type == '(' && tokens[q].type == ')') return true;
    int cnt = 0;
    for (; p <= q; p++) {
        if (tokens[p].type == '(')cnt++;
        else if (tokens[p].type == ')')cnt--;
        if (cnt < 0) {
            printf("parentheses mismatch\n");
            return false;
        }
    }
    if (cnt != 0) {
        printf("parentheses mismatch\n");
        return false;
    }
    return false;
}