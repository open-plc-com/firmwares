#ifndef __mx_misc_H
#define __mx_misc_H

#ifdef __cplusplus
	extern "C" {
#endif

void delay_ms( unsigned int delay );
void delay_us( unsigned int delay );
void put_str( char* str );

#ifdef __cplusplus
}
#endif

#endif /* __mx_misc_H */
