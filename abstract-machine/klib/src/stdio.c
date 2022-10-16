#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  assert(fmt);
  int len;
  va_list ap;
  char buf[2048];
  va_start(ap, fmt);
  len = vsprintf(buf, fmt, ap);
  int cnt = 0;
  while(cnt <= len) {
    putch(buf[cnt]);
    cnt ++;
  }
  va_end(ap);
  return len;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  char *str;
  char ch;
  int tmp;
  char buf[65]; int len = 0;
  char *wr = out;
  
  for( ; *fmt != '\0'; fmt ++) {
    /* d : decimal ;
       x : hexadecimal ;
       p : pointer ;
       c : char ;
       s : string ;
    */
    if(*fmt == '%')  {
      fmt ++;
      switch(*fmt) {
        case 'd' :
          tmp = va_arg(ap, int);
          while(tmp != 0) {
            buf[len] = (char) ( tmp % 10 + '0');
            len ++;
            tmp = tmp / 10;
          }
          len --;
          while(len --){ *wr = buf[len]; wr ++;  }
          break;

        case 'x' :
          tmp = va_arg(ap, int);
          while(tmp != 0) {
            buf[len] = (char) (tmp % 16 + '0');
            len ++;
            tmp = tmp / 16;
          }
          len --; 
          while(len--){*wr = buf[len]; wr++;}
          break;

        case 'p' :          
          tmp = va_arg(ap, unsigned);
          { *wr = '0'; wr ++; *wr = 'x'; wr++; }
          while(tmp != 0) {
            buf[len] = (char) (tmp % 16 + '0');
            len ++;
            tmp = tmp / 16;
          }
          len --;
          while(len --) {*wr = buf[len]; wr ++;}
          break;

        case 'c' :
          ch = (char)va_arg(ap, int);
          *wr++ = ch;
          break;

        case 's' :
          str = va_arg(ap, char *);
          while(*str != '\0') *wr ++ = *str ++;
          break;

        default : break;
      }

    }
    else *wr ++ = *fmt;
  }
  *wr = '\0';
  return wr - out;
}


// Write format formation of *fmt to dest string out;
int sprintf(char *out, const char *fmt, ...) { 
  va_list ap;
  int len;
  va_start(ap, fmt);
  len = vsprintf(out, fmt, ap);
  va_end(ap);
  return len;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
