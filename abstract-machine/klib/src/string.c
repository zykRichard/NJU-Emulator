#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

// Return the length of string
size_t strlen(const char *s) {
  size_t len = 0;
  char *ch = (char *)s;
  while(*ch != '\0'){
    len ++;
    ch ++;
  }
  return len;
}

// Copy src string to destination string
char *strcpy(char *dst, const char *src) {
  char *pos = dst;
  char *ch = (char *)src;
  while(*ch != '\0') {
    *dst = *ch;
    dst ++;
    ch ++;
  }
  *dst = '\0';
  return pos;
}

// Copy n bytes form src to dst
char *strncpy(char *dst, const char *src, size_t n) {
  char *pos = dst;
  char *ch = (char *)src;
  size_t i = 0;
  for(i = 0; i < n && *ch != '\0'; i++){
    *dst = *ch;
    dst ++;
    ch ++;
  }
  for( ; i < n; i++) {
    *dst = '\0';
  }
  return pos;
}

// catenate the src string to dst
char *strcat(char *dst, const char *src) {
  size_t len = strlen(dst);
  size_t n = strlen(src);
  int i;
  for(i = 0; i < n; i++) {
    dst[len + i] = src[i];
  } 
  dst[len + i] = '\0';
  return dst;
}

// Judge whether the two string are equal or not; 
// return value is :
// 0 for s1 == s2;
// -1 for s1 < s2;
// 1 for s1 > s2
int strcmp(const char *s1, const char *s2) {
  size_t len1 = strlen(s1);
  size_t len2 = strlen(s2);
  if(len1 < len2) return -1;
  else if(len1 > len2) return 1;
  else {
    for(int i = 0; i < len1; i++) {
      if(s1[i] < s2[i]) return -1;
      else if(s1[i] > s2[i]) return 1;
    }
    return 0;
  }
}

// Compare the first n bytes of s1 and s2
int strncmp(const char *s1, const char *s2, size_t n) {
  size_t len1 = strlen(s1);
  size_t len2 = strlen(s2);
  if(len1 <= n && len2 <= n) return strcmp(s1, s2);
  else if(s1 > s2) return 1;
  else if(s1 < s2) return -1;
  else {
    for(int i = 0; i < n; i++){
      if(s1[i] < s2[i]) return -1;
      else if(s1[i] > s2[i]) return 1;
    }
    return 0;
  }
}

// Set the first n bytes of s to value c(unsigned char)
void *memset(void *s, int c, size_t n) {
  char *p = (char *)s;
  unsigned char ch = (unsigned char)c;
  for(int i = 0; i < n; i++){
    *p = ch;
    p ++;
  }
  return s;
}

// Copy first n bytes from in to out which must not overlap;
void *memcpy(void *out, const void *in, size_t n) {
  // overlap judge:
  uintptr_t src = (uintptr_t)in;
  uintptr_t dst = (uintptr_t)out;
  if(dst >= src - n && dst <= src + n) panic("memcpy: forbidden for overlapping between src and dst");
  char *a = (char *)in;
  char *b = (char *)out;
  for(int i = 0; i < n; i++) 
    b[i] = a[i];
  
  return out;
}

// Move first n bytes from src to dst, overlap allowed;
void *memmove(void *dst, const void *src, size_t n) {
  char tmp[n];
  memcpy((void *)tmp, src, n);
  memcpy(dst, (const void *)tmp, n);
  return dst;
}

// Compare first n bytes between s1 and s2(unsigned char)
int memcmp(const void *s1, const void *s2, size_t n) {
  if(n == 0) return 0;
  unsigned char *src = (unsigned char *)s1;
  unsigned char *dst = (unsigned char *)s2;
  for(int i = 0; i < n; i++) {
    if(src[i] < dst[i]) return -1;
    else if(src[i] > dst[i]) return 1;
  }
  return 0;
}

#endif
