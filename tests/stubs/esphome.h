#pragma once
#include <cstdint>
#include <cmath>
#include <string>
#include <vector>
#include <deque>
#include <initializer_list>
#include <cstring>
#include <limits>
inline uint32_t fake_now=0;
inline uint32_t millis() { return fake_now; }
inline void yield() { ++fake_now; }
inline void delay(uint32_t ms) { fake_now+=ms; }
constexpr int INPUT=0, OUTPUT=1, HIGH=1, LOW=0;
inline void pinMode(int,int) {}
inline void digitalWrite(int,int) {}
#define ESP_LOGD(...) ((void)0)
#define ESP_LOGI(...) ((void)0)
#define ESP_LOGW(...) ((void)0)
struct FakeTime { int day_of_week=1,hour=1,minute=1; bool is_valid() const { return true; } };
struct FakeClock { FakeTime now() { return {}; } };
inline FakeClock my_time;
#define id(x) x
struct Sensor { float state=std::numeric_limits<float>::quiet_NaN(); unsigned publications=0; void publish_state(float v) { state=v; ++publications; } };
struct TextSensor { std::string state; void publish_state(const std::string &v) { state=v; } };
struct BinarySensor { bool state=false; void publish_state(bool v) { state=v; } };
struct PollingComponent { explicit PollingComponent(uint32_t) {} virtual void setup() {} virtual void update() {} virtual void loop() {} };
struct CustomAPIDevice { template<typename T> void register_service(T,const char*,std::initializer_list<const char*>) {} };
struct UARTComponent {
  struct Byte { uint32_t due; uint8_t value; };
  struct Response { std::vector<uint8_t> data; uint32_t spacing=0; };
  std::deque<Byte> rx;
  std::deque<Response> responses;
  unsigned queries=0;
  void enqueue(const std::vector<uint8_t> &data,uint32_t spacing=0) {
    uint32_t due=fake_now;
    for (auto b:data) { due+=spacing; rx.push_back({due,b}); }
  }
};
struct UARTDevice {
  UARTComponent *parent;
  explicit UARTDevice(UARTComponent *p):parent(p) {}
  int available() { int n=0; for (auto b:parent->rx) { if (int32_t(fake_now-b.due)<0) break; ++n; } return n; }
  int read() { uint8_t b=0; read_byte(&b); return b; }
  bool read_byte(uint8_t *b) { if (!available()) return false; *b=parent->rx.front().value; parent->rx.pop_front(); return true; }
  void write(uint8_t) {}
  void write_array(uint8_t*,uint8_t) {
    ++parent->queries;
    if (!parent->responses.empty()) { auto r=parent->responses.front(); parent->responses.pop_front(); parent->enqueue(r.data,r.spacing); }
  }
};
#define ESP_LOGE(...) ((void)0)
