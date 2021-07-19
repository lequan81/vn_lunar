#ifndef vn_lunar_h
#define vn_lunar_h

#include <Arduino.h>

class vn_lunar
{
    public:
      long long getJulius(uint32_t dd, uint32_t mm, uint32_t yy);
      uint32_t getNewMoonDay(uint32_t k);
      uint32_t getSunLongitude(uint32_t jdn);
      uint32_t getLunarMonthll(uint32_t yy);
      uint32_t getLeapMonthOffset(double a11);
      uint32_t convertSolar2Lunar(uint32_t dd, uint32_t mm, uint32_t yy);
      uint32_t get_lunar_dd();
      uint32_t get_lunar_mm();
      uint32_t get_lunar_yy();
    private:
      uint32_t _dd;
      uint32_t _mm;
      uint32_t _yy;
      uint32_t k;
      uint32_t jdn;
      uint32_t a11;
};

#endif
