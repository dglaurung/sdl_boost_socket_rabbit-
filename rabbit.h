#ifndef SONAR_DATA_RABBIT__RABBIT_H_
#define SONAR_DATA_RABBIT__RABBIT_H_

#include <stdint.h>

#pragma pack(push)
#pragma pack(1)

struct SonarStatus {
  uint16_t a1;
  int16_t a2;
  uint16_t a3;
  uint16_t a4;
  uint16_t a5;
  uint16_t depth;
  uint8_t a7[2000];
  uint32_t a6;
};

struct ReplyFromServer{
  uint8_t aaa[4];
  SonarStatus sonar_status;
};

//запрос на сервер
struct RequestFromTcpClient {
  uint32_t sof;
  uint8_t mode;
  uint8_t frequency;
};

#pragma pack(pop)

#endif //SONAR_DATA_RABBIT__RABBIT_H_
