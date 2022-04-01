#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<stddef.h>
/*=====================================define_hex_struct===================================*/
#pragma pack(push,1)
struct _hex_struct
{
	uint8_t start_code;
	uint8_t byte_count;
	uint8_t adress[2];
	uint8_t record_type;
	uint8_t data[16];
	
};
typedef struct _hex_struct HEX_typedef;
HEX_typedef hex_demo = {0U,0U,{0U,0U},0U,{0U,0U,0U,0U,0U,0U,0U,0U,0U,0U,0U,0U,0U,0U,0U,0U},0U};
#pragma pack(pop)
union _byte
{
	uint8_t data;
	struct
	{
		uint8_t bit_l:4;
		uint8_t bit_h:4;
	};
};
typedef union _byte BYTE_typedef;
BYTE_typedef byte_demo = {0x00};
typedef enum
{
	START_CODE = 0U,
	BYTE_COUNT,
	ADRESS,
	RECORD_TYPE,
	DATA,
	CHECK_SUM,
	DONE,
}state_active; 
state_active state_current;
typedef enum
{
	HEX_OK = 0U,
	HEX_ERROR,
	IDE,
}hex_state;
/*=====================================define_hex_struct===================================*/
/*=====================================declaration_varriable_function===================================*/
FILE *fp;
char data[50];
uint8_t read_line(FILE *f,char *s);
hex_state allocate_hex_struct(HEX_typedef *hex,char *s);
uint8_t get_hex(char c_1,char c_2);
/*=====================================declaration_varriable_function===================================*/
/*=====================================define_varriable_function===================================*/
uint8_t get_hex(char c_1,char c_2)
{
	byte_demo.data = 0x00;
	if(c_1 <= '9')
	{
		byte_demo.bit_h = c_1 - '0'; // 48
	}
	else byte_demo.bit_h = c_1 - 87;//ASCI
	if(c_2 <= '9')
	{
		byte_demo.bit_l = c_2 - '0'; // 48
	}
	else byte_demo.bit_l = c_2 - 87;//ASCI
	return byte_demo.data;
}
uint8_t read_line(FILE *f,char *s)
{
	char c;
	uint8_t count = 0U;
	if(f == NULL) return 2;
	while((c = fgetc(f)) != '\n')
	{
		*(s + count) = c;
		count++;
	}
	if(c == '@') return 1;
	else return 0;
}
hex_state allocate_hex_struct(HEX_typedef *hex,char *s)
{
	state_current = START_CODE;
	uint8_t dem = 0U;
	uint8_t i = 0U;
	uint8_t element=0U;
	uint8_t check_sum_value = 0U;
	hex_state state = IDE;
	for(i = 0U ; i < 8U ; i++)
	{
		switch(state_current)
		{
			case START_CODE:
				if(s[dem] == ':')
				{
					hex->start_code = *(s + dem);
					dem++;
					state_current = BYTE_COUNT;
				}
				else state_current = START_CODE;
				break;
			case BYTE_COUNT:
				hex->byte_count = get_hex(s[dem],s[dem+1]);
				dem += 2;
				state_current = ADRESS;
				break;
			case ADRESS:
				hex->adress[0] = get_hex(s[dem],s[dem+1]);
				dem += 2;
				hex->adress[1] = get_hex(s[dem],s[dem+1]);
				dem += 2;
				state_current = RECORD_TYPE;
				break;
			case RECORD_TYPE:
				hex->record_type = get_hex(s[dem],s[dem+1]);
				dem += 2;
				state_current = DATA;
				break;
			case DATA:
				for(element=0;element < hex->byte_count;element++)
				{
					hex->data[element] = get_hex(s[dem],s[dem+1]);
					dem += 2;
				}
				state_current = CHECK_SUM;
				break;
			case CHECK_SUM:
				hex->check_sum = get_hex(s[dem],s[dem+1]);
				dem += 2;
			    state_current = DONE;
				break;
			case DONE:
				check_sum_value = hex->byte_count + hex->record_type + hex->adress[0] + hex->adress[1];
				element = 0U;
				for(element=0;element < hex->byte_count;element++)
				{
					check_sum_value += hex->data[element];
				}
				check_sum_value = ~(uint8_t)check_sum_value + 1;
				(check_sum_value == hex->check_sum) ? (state = HEX_OK) : (state = HEX_ERROR);
				break;
			default:
				break;
		}
	}
	return state;
}
/*=====================================define_varriable_function===================================*/
int main(void)
{
	fp = fopen("C:\\Users\\Admin\\Desktop\\STM32_TUTORIAL_SANGML\\HEX_FILE\\bai1.hex","r+");
	if(fp == NULL)
	{
		printf("open file fall!");
	}
	else
	{
		read_line(fp,&data[0]);//line 1
		read_line(fp,&data[0]);//line 2
		read_line(fp,&data[0]);//line 3
		allocate_hex_struct(&hex_demo,&data[0]);
		fclose(fp);//close file
	}
	printf("%x",hex_demo.check_sum);
	return 0;
}
