#pragma once
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <cstring>

namespace roomba {
constexpr size_t FRAME_SIZE = 22;
constexpr uint32_t STALE_MS = 30000;
constexpr uint32_t STOP_CONFIRM_MS = 20000;
constexpr uint32_t MAX_ACCOUNTING_GAP_MS = 15000;

inline uint16_t u16(const uint8_t *p) { return (uint16_t(p[0]) << 8) | p[1]; }
inline int16_t i16(const uint8_t *p) {
  const int32_t n = u16(p);
  return static_cast<int16_t>(n >= 32768 ? n - 65536 : n);
}
struct Frame {
  uint8_t charging{}, oi_mode{}, virtual_wall{}, charging_sources{}, buttons{};
  uint16_t voltage{}, charge{}, capacity{};
  int16_t current{}, temperature{}, right{}, left{}, main_brush{}, side_brush{};
};
// Physical plausibility limits are deliberately broad design limits, not OI specs.
// Query List has no framing/checksum: plausible corruption cannot be ruled out.
inline bool decode(const uint8_t *b, size_t size, Frame &out) {
  if (size != FRAME_SIZE) return false;
  Frame f;
  f.charging=b[0]; f.voltage=u16(b+1); f.current=i16(b+3);
  f.charge=u16(b+5); f.capacity=u16(b+7);
  f.temperature=b[9] < 128 ? b[9] : int(b[9])-256;
  f.oi_mode=b[10]; f.right=i16(b+11); f.left=i16(b+13);
  f.main_brush=i16(b+15); f.side_brush=i16(b+17);
  f.virtual_wall=b[19]; f.charging_sources=b[20]; f.buttons=b[21];
  if (f.charging>5 || f.oi_mode>3 || f.virtual_wall>1 || f.charging_sources>3 ||
      f.voltage<8000 || f.voltage>22000 || f.capacity<100 || f.capacity>20000 ||
      f.charge>f.capacity || f.temperature < -20 || f.temperature>85 ||
      f.current < -10000 || f.current>10000) return false;
  for (int16_t motor : {f.right,f.left,f.main_brush,f.side_brush})
    if (motor < -5000 || motor>5000) return false;
  out=f;
  return true;
}
inline bool motor_active(int16_t current, int threshold) {
  return current >= threshold || current <= -threshold;
}
inline bool cleaning_evidence(const Frame &f) {
  // Battery percentage is NOT a stop signal. Main brush + discharge are evidence.
  return f.charging_sources==0 && (f.charging==0 || f.charging==4) && f.oi_mode!=0 &&
         f.current < -300 && motor_active(f.main_brush,100);
}
inline bool motion_evidence(const Frame &f) {
  return motor_active(f.right,50) || motor_active(f.left,50) ||
         motor_active(f.main_brush,100) || motor_active(f.side_brush,30);
}
class Tracker {
 public:
  void accept(const Frame &f, uint32_t now) {
    const bool active=cleaning_evidence(f);
    const uint32_t gap=now-last_valid_;
    if (seen_ && previous_clean_ && active && gap<=MAX_ACCOUNTING_GAP_MS)
      confirmed_ms_+=gap;
    previous_clean_=active;
    seen_=true; last_valid_=now;
    if (motion_evidence(f)) { motion_seen_=true; last_motion_=now; }
    if (f.charging==5) { activity_="Lost"; stopping_=false; }
    else if (f.charging_sources!=0) {
      activity_=(f.charging>=1 && f.charging<=3) ? "Charging" : "Docked";
      stopping_=false;
    } else if (active) { activity_="Cleaning"; stopping_=false; }
    else if (std::strcmp(activity_, "Cleaning")==0) {
      if (!stopping_) { stopping_=true; stop_since_=now; }
      if (uint32_t(now-stop_since_)>=STOP_CONFIRM_MS) activity_="Lost";
    } else { activity_="Lost"; stopping_=false; }
  }
  void reject() { previous_clean_=false; }
  bool fresh(uint32_t now) const { return seen_ && uint32_t(now-last_valid_)<STALE_MS; }
  bool cleaning(uint32_t now) const { return fresh(now) && previous_clean_; }
  const char *activity(uint32_t now) const { return fresh(now) ? activity_ : "Lost"; }
  float age_seconds(uint32_t now) const { return (seen_ ? uint32_t(now-last_valid_) : now)/1000.0f; }
  float inactivity_seconds(uint32_t now) const { return (motion_seen_ ? uint32_t(now-last_motion_) : now)/1000.0f; }
  double confirmed_seconds() const { return confirmed_ms_/1000.0; }
 private:
  const char *activity_="Lost";
  bool seen_=false, motion_seen_=false, previous_clean_=false, stopping_=false;
  uint32_t last_valid_=0, last_motion_=0, stop_since_=0;
  uint64_t confirmed_ms_=0;
};
}  // namespace roomba
