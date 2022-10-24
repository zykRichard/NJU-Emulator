#include <am.h>
#include <nemu.h>
#include <stdint.h>

uint64_t boot_time = 0;
void __am_timer_init() {
  boot_time = inq(RTC_ADDR);

}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  uint64_t new_time = inq(RTC_ADDR);
  uptime -> us = new_time - boot_time;
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
