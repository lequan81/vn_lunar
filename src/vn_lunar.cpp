#include "vn_lunar.h"
#include <Arduino.h>
#include <math.h>

uint32_t lunar_dd;
uint32_t lunar_mm;
uint32_t lunar_yy;

//=====================================================================================================//
//get_Julius
long long vn_lunar::getJulius(uint32_t _dd, uint32_t _mm, uint32_t _yy) {
  uint32_t a, y, m, jd;
  a = (uint32_t)((14 - _mm) / 12);
  y = _yy + 4800 - a;
  m = _mm + 12 * a - 3;
  jd = _dd + (uint32_t)((153 * m + 2) / 5) + 365 * y + (uint32_t)(y / 4) - (uint32_t)(y / 100) + (uint32_t)(y / 400) - 32045;
  if (jd < 2299161) {
    jd = _dd + (uint32_t)((153 * m + 2) / 5) + 365 * y + (uint32_t)(y / 4) - 32083;
  }
  return jd;
}
//=====================================================================================================//
//get_New_Moon_Day
uint32_t vn_lunar::getNewMoonDay(uint32_t k) {
  double T, T2, T3, dr, Jd1, M, Mpr, F, C1, deltat, JdNew;
  T = k / 1236.85;
  T2 = T * T;
  T3 = T2 * T;
  dr = PI / 180;
  double timeZone = 7.0;
  Jd1 = 2415020.75933 + 29.53058868 * k + 0.0001178 * T2 - 0.000000155 * T3;  // Mean new moon
  Jd1 = Jd1 + 0.00033 * sin((166.56 + 132.87 * T - 0.009173 * T2) * dr);  // Sun's mean anomaly
  M = 359.2242 + 29.10535608 * k - 0.0000333 * T2 - 0.00000347 * T3;  // Moon's mean anomaly
  Mpr = 306.0253 + 385.81691806 * k + 0.0107306 * T2 + 0.00001236 * T3;  // Moon's argument of latitude
  F = 21.2964 + 390.67050646 * k - 0.0016528 * T2 - 0.00000239 * T3;
  C1 = (0.1734 - 0.000393 * T) * sin(M * dr) + 0.0021 * sin(2 * dr * M);
  C1 = C1 - 0.4068 * sin(Mpr * dr) + 0.0161 * sin(dr * 2 * Mpr);
  C1 = C1 - 0.0004 * sin(dr * 3 * Mpr);
  C1 = C1 + 0.0104 * sin(dr * 2 * F) - 0.0051 * sin(dr * (M + Mpr));
  C1 = C1 - 0.0074 * sin(dr * (M - Mpr)) + 0.0004 * sin(dr * (2 * F + M));
  C1 = C1 - 0.0004 * sin(dr * (2 * F - M)) - 0.0006 * sin(dr * (2 * F + Mpr));
  C1 = C1 + 0.0010 * sin(dr * (2 * F - Mpr)) + 0.0005 * sin(dr * (2 * Mpr + M));
  if (T < -11) {
    deltat = 0.001 + 0.000839 * T + 0.0002261 * T2 - 0.00000845 * T3 - 0.000000081 * T * T3;
  } else {
    deltat = -0.000278 + 0.000265 * T + 0.000262 * T2;
  }
  JdNew = Jd1 + C1 - deltat;
  return (uint32_t)(JdNew + 0.5 + timeZone / 24);
}
//=====================================================================================================//
//get_Sun_Longitude
uint32_t vn_lunar::getSunLongitude(uint32_t jdn) {
  double timeZone = 7.0;
  double T, T2, dr, M, L0, DL, L;
  // Time in Julian centuries from 2000-01-01 12:00:00 GMT
  T = (jdn - 2451545.5 - timeZone / 24) / 36525;
  T2 = T * T;   // degree to radian
  dr = PI / 180;  // mean anomaly, degree
  M = 357.52910 + 35999.05030 * T - 0.0001559 * T2 - 0.00000048 * T * T2;  // mean longitude, degree
  L0 = 280.46645 + 36000.76983 * T + 0.0003032 * T2;
  DL = (1.914600 - 0.004817 * T - 0.000014 * T2) * sin(dr * M);
  DL = DL + (0.019993 - 0.000101 * T) * sin(dr * 2 * M) + 0.000290 * sin(dr * 3 * M);
  L = L0 + DL; // true longitude, degree
  L = L * dr;  // Normalize to (0, 2*PI)
  L = L - PI * 2 * (uint32_t)(L / (PI * 2));
  return (uint32_t)(L / PI * 6);
}
//=====================================================================================================//
//get_Lunar_Month_11
uint32_t vn_lunar::getLunarMonthll(uint32_t yy) {
  double k, off, nm, sunLong;
  off = getJulius(31, 12, yy) - 2415021;
  k = (uint32_t)(off / 29.530588853);
  nm = getNewMoonDay((uint32_t)k);
  // sun longitude at local midnight
  sunLong = getSunLongitude((uint32_t)nm);
  if (sunLong >= 9) {
    nm = getNewMoonDay((uint32_t)k - 1);
  }
  return (uint32_t)nm;
}
//=====================================================================================================//
//get_Leap_Month_Offset
uint32_t vn_lunar::getLeapMonthOffset(double a11) {
  double last, arc;
  uint32_t k, i;
  k = (uint32_t)((a11 - 2415021.076998695) / 29.530588853 + 0.5);
  last = 0;
  // We start with the month following lunar month 11
  i = 1;
  arc = getSunLongitude((uint32_t)getNewMoonDay((uint32_t)(k + i)));
  do {
    last = arc;
    i++;
    arc = getSunLongitude((uint32_t)getNewMoonDay((uint32_t)(k + i)));
  } while (arc != last && i < 14);
  return i - 1;
}
//=====================================================================================================//
//convert_Solar_to_Lunar
uint32_t vn_lunar::convertSolar2Lunar(uint32_t dd, uint32_t mm, uint32_t yy) {
  double dayNumber, monthStart, a11, b11, lunarDay, lunarMonth, lunarYear;
  //double lunarLeap;
  uint32_t k, diff;
  dayNumber = getJulius(_dd, _mm, _yy);
  k = (uint32_t)((dayNumber - 2415021.076998695) / 29.530588853);
  monthStart = getNewMoonDay(k + 1);
  if (monthStart > dayNumber) {
    monthStart = getNewMoonDay(k);
  }
  a11 = getLunarMonthll(_yy);
  b11 = a11;
  if (a11 >= monthStart) {
    lunarYear = _yy;
    a11 = getLunarMonthll(_yy - 1);
  } else {
    lunarYear = yy + 1;
    b11 = getLunarMonthll(_yy + 1);
  }
  lunarDay = dayNumber - monthStart + 1;
  diff = (uint32_t)((monthStart - a11) / 29);
  //lunarLeap = 0;
  lunarMonth = diff + 11;
  if (b11 - a11 > 365) {
    uint32_t leapMonthDiff = getLeapMonthOffset(a11);
    if (diff >= leapMonthDiff) {
      lunarMonth = diff + 10;
      if (diff == leapMonthDiff) {
        //lunarLeap = 1;
      }
    }
  }
  if (lunarMonth > 12) {
    lunarMonth = lunarMonth - 12;
  }
  if (lunarMonth >= 11 && diff < 4) {
    lunarYear -= 1;
  }

  lunar_dd = (uint32_t)lunarDay;
  lunar_mm = (uint32_t)lunarMonth;
  lunar_yy = (uint32_t)lunarYear;

//  String lunar_day = String((uint32_t)lunar_dd);
//  String lunar_mon = String((uint32_t)lunar_mm);
//  String lunar_year = String((uint32_t)lunar_yy);
//  String str_lunar = String(lunar_day + "/" + lunar_mon + "/" + lunar_year);

}

uint32_t vn_lunar::get_lunar_dd() {
  return lunar_dd;
}
uint32_t vn_lunar::get_lunar_mm() {
  return lunar_mm;
}
uint32_t vn_lunar::get_lunar_yy() {
  return lunar_yy;
}
