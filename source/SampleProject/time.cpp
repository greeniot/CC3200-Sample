#include "time.h"
#include <stdint.h>
#include <limits.h>

#define UDP_DST_PORT  (2390)
#define NTP_PKG_SIZE  (48)
#define NTP_PORT      (123)
#define SECS_PER_DAY  (86400)
#define DAYS_PER_WEEK (7)
#define DAYS_OF_YEAR  (365)
#define LEAPOCH       (946684800LL + SECS_PER_DAY * (31 + 29))
#define DAYS_PER_400Y (DAYS_OF_YEAR * 400 + 97)
#define DAYS_PER_100Y (DAYS_OF_YEAR * 100 + 24)
#define DAYS_PER_4Y   (DAYS_OF_YEAR * 4   + 1)
#define SEVENTY_YEARS (2208988800UL)

static const char days_in_month[] = { 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 29 };

uint32_t getOfficialTime() {
  IPAddress time_server(129, 6, 15, 29);
  byte packet_buffer[NTP_PKG_SIZE];
  uint32_t secs_since_1900 = 0;
  int retries = 100;
  WiFiUDP udp;
  udp.begin(UDP_DST_PORT);
  memset(packet_buffer, 0, NTP_PKG_SIZE);

  // Initialize values needed to form NTP request
  packet_buffer[0] = 0b11100011;   // LI, Version, Mode
  packet_buffer[1] = 0;            // Stratum, or type of clock
  packet_buffer[2] = 6;            // Polling Interval
  packet_buffer[3] = 0xEC;         // Peer Clock Precision

  // 8 bytes of zero for Root Delay & Root Dispersion
  packet_buffer[12] = 49;
  packet_buffer[13] = 0x4E;
  packet_buffer[14] = 49;
  packet_buffer[15] = 52;

  udp.beginPacket(time_server, NTP_PORT);
  udp.write(packet_buffer, NTP_PKG_SIZE);
  udp.endPacket();

  while (!udp.parsePacket() && retries > 0) {
  	delay(100);
  	retries--;
  }

  udp.read(packet_buffer, NTP_PKG_SIZE);
  uint32_t high_word = word(packet_buffer[40], packet_buffer[41]);
  uint32_t low_word = word(packet_buffer[42], packet_buffer[43]);
  secs_since_1900 = high_word << 16 | low_word;

  udp.stop();
  Serial.println(secs_since_1900);
  return secs_since_1900;
}

uint32_t getUnixTime() {
  return getOfficialTime() - SEVENTY_YEARS;
}

SlDateTime_t getCurrentTime() {
  long long secs = getUnixTime() - LEAPOCH;
  long long days = secs / SECS_PER_DAY;
  int remsecs = secs % SECS_PER_DAY;

  if (remsecs < 0) {
    remsecs += SECS_PER_DAY;
    days--;
  }

  int wday = (3 + days) % DAYS_PER_WEEK;

  if (wday < 0) {
    wday += DAYS_PER_WEEK;
  }

  int qc_cycles = days / DAYS_PER_400Y;
  int remdays = days % DAYS_PER_400Y;

  if (remdays < 0) {
    remdays += DAYS_PER_400Y;
    qc_cycles--;
  }

  int c_cycles = remdays / DAYS_PER_100Y;

  if (c_cycles == 4) {
    c_cycles--;
  }

  remdays -= c_cycles * DAYS_PER_100Y;
  int q_cycles = remdays / DAYS_PER_4Y;

  if (q_cycles == 25) {
    q_cycles--;
  }

  remdays -= q_cycles * DAYS_PER_4Y;
  int remyears = remdays / DAYS_OF_YEAR;

  if (remyears == 4) {
    remyears--;
  }

  remdays -= remyears * DAYS_OF_YEAR;

  int leap = !remyears && (q_cycles || !c_cycles);
  int yday = remdays + 31 + 28 + leap;

  if (yday >= DAYS_OF_YEAR + leap) {
    yday -= DAYS_OF_YEAR + leap;
  }

  int years = remyears + 4 * q_cycles + 100 * c_cycles + 400 * qc_cycles;
  int months = 0;

  while (days_in_month[months] <= remdays) {
    remdays -= days_in_month[months++];
  }

  months += 2;

  if (months >= 12) {
    months -= 12;
    years++;
  }

  return SlDateTime_t {
    .sl_tm_sec = remsecs % 60,
    .sl_tm_min = remsecs / 60 % 60,
    .sl_tm_hour = remsecs / 3600,
    .sl_tm_day = remdays + 1,
    .sl_tm_mon = months + 1,
    .sl_tm_year = years + 2000,
    .sl_tm_week_day = wday,
    .sl_tm_year_day = yday,
  };
}

void setCurrentTime() {
  SlDateTime_t current = getCurrentTime();
  sl_DevSet(SL_DEVICE_GENERAL_CONFIGURATION, SL_DEVICE_GENERAL_CONFIGURATION_DATE_TIME, sizeof(current), (uint8_t*)(&current));
}
