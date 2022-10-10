#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  panic("Not implemented");
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}


// assistance function :
static char *write_to_str(char *in, int num, int hex) 
{ 
  int len = 0;
  char buf[64];
  if(hex){
    *in = '0'; in ++; *in = 'x'; in ++;
    while(num) {
      char ch = (char) (num % 16 + 48);
      num = num / 16;
      buf[len] = ch;
      len ++;
    }
  }
  else 
    while(num) {
      char ch = (char) (num % 10 + 48);
      num = num / 10;
      buf[len] = ch;
      len ++;
    }

  for(int i = 1; i <= len ; i ++){
    *in = buf[len - i];
    in ++;
  }
  in --;
  return in;

}

// Write format formation of *fmt to dest string out;
int sprintf(char *out, const char *fmt, ...) {
  char *org = out;
  va_list ap;
  va_start(ap, fmt);
  int num; 
  uintptr_t ptr;
  char c, *s;
  bool flag = 0;
  while(*fmt) {
    if(flag) {
      switch (*fmt) {
      case 'd': //decimal
        num = va_arg(ap, int);
        out = write_to_str(out, num, 0);
        break;

      case 'x': //hexadecimal
        num = va_arg(ap, int);
        out = write_to_str(out, num, 1);
        break;

      case 's': //string  
        s = va_arg(ap, char *);
        out = strcat(out, s);
        out += strlen(s) - 1;
        break;

      case 'c': // char 
        c = (char) va_arg(ap, int);
        *out = c;
        break;

      case 'p': //pointer for riscv-32
        ptr = (uintptr_t)va_arg(ap, unsigned);
        out = write_to_str(out, ptr, 0);
        break;

      default: 

        break;
      }
    }

    switch(*fmt) {
      case '%': flag = 1; break;
      default: *out = *fmt; break;
    }
    fmt ++;
    out ++;
  }
va_end(ap);
return out - org;

}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
