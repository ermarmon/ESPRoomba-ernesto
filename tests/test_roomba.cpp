#include "stubs/esphome.h"
#include <cassert>
#include <iostream>
#define private public
#include "../custom_components/Roomba.h"
#undef private

std::vector<uint8_t> packet() {
  return {0,0x38,0xA4,0xFA,0xEC,0x03,0xE8,0x07,0xD0,25,1,
          0,200,0,200,1,144,0,70,0,0,0};
}
roomba::Frame frame() { auto p=packet(); roomba::Frame f; assert(roomba::decode(p.data(),p.size(),f)); return f; }
int main() {
  auto p=packet(); auto original=frame(); roomba::Frame f=original;
  assert(f.voltage==14500 && f.current==-1300 && f.main_brush==400);
  for (size_t len=0;len<22;++len) assert(!roomba::decode(p.data(),len,f));
  p[9]=251; assert(roomba::decode(p.data(),p.size(),f) && f.temperature==-5);
  for (auto test:std::vector<std::pair<unsigned,uint8_t>>{{0,6},{10,4},{19,2},{20,4},{1,0},{7,0},{9,100},{3,127},{15,127}}) {
    auto bad=packet();bad[test.first]=test.second;f=original;
    assert(!roomba::decode(bad.data(),bad.size(),f));assert(f.voltage==original.voltage);
  }
  auto bad=packet();bad[7]=bad[8]=0;assert(!roomba::decode(bad.data(),bad.size(),f));
  bad=packet();bad[5]=0xFF;assert(!roomba::decode(bad.data(),bad.size(),f));
  roomba::Tracker t; f=frame();
  assert(!t.fresh(0) && !t.cleaning(0));
  t.accept(f,100);t.accept(f,10100);assert(t.confirmed_seconds()==10);
  // Low battery alone is NOT stopped cleaning.
  f.charge=10;t.accept(f,20100);assert(t.cleaning(20100));
  // Stopped main brush with persistent discharge: stops counting immediately,
  // legacy activity leaves Cleaning after bounded debounce.
  f.main_brush=0;f.side_brush=0;f.left=f.right=0;
  t.accept(f,30100);assert(!t.cleaning(30100));
  t.accept(f,50100);assert(std::string(t.activity(50100))=="Lost");
  assert(t.confirmed_seconds()==20);
  f=frame(); t.accept(f,60100);t.reject();t.accept(f,70100);
  assert(t.confirmed_seconds()==20); // no accounting over bad packet
  assert(!t.fresh(100100) && std::string(t.activity(100100))=="Lost");
  t.accept(f,120100);assert(t.confirmed_seconds()==20); // no stale gap
  f.charging_sources=2;f.charging=2;t.accept(f,130100);
  assert(std::string(t.activity(130100))=="Charging" && !t.cleaning(130100));
  f.charging=0;t.accept(f,140100);assert(std::string(t.activity(140100))=="Docked");
  f=frame();f.main_brush=0;f.current=-800;t.accept(f,150100);
  assert(!t.cleaning(150100)); // wheels only / driving
  roomba::Tracker wrap;f=frame();wrap.accept(f,UINT32_MAX-4999);wrap.accept(f,5000);
  assert(wrap.confirmed_seconds()==10 && wrap.fresh(5000));
  roomba::Tracker constant;constant.accept(f,0);constant.accept(f,10000);
  assert(constant.inactivity_seconds(10000)==0); // unchanged motors still active
  // Exercise the ACTUAL component UART implementation against timed fake bytes.
  fake_now=0;UARTComponent port;RoombaComponent component(10,&port,10000,false,&my_time);
  uint8_t dest[22];port.enqueue(packet(),5);assert(component.getData(dest,22));assert(fake_now==110);
  port.enqueue({1,2,3},50);auto start=fake_now;assert(!component.getData(dest,22));assert(fake_now-start==200);
  fake_now=UINT32_MAX-50;port.rx.clear();port.enqueue(packet(),5);assert(component.getData(dest,22));
  port.rx.clear();port.enqueue(std::vector<uint8_t>(23,0));assert(!component.getData(dest,22));component.flush();
  port.responses.push_back({{1,2,3},0});port.responses.push_back({packet(),0});
  component.update();assert(port.queries==2 && component.voltageSensor->publications==1);
  assert(component.activitySensor->state=="Cleaning");
  assert(component.valid_responses()==1 && component.invalid_responses()==1);
  auto publications=component.voltageSensor->publications;auto state=component.voltageSensor->state;
  port.responses.push_back({std::vector<uint8_t>(22,255),0});port.responses.push_back({{0},0});
  component.update();assert(port.queries==4 && component.voltageSensor->publications==publications);
  assert(component.voltageSensor->state==state && !component.confirmed_cleaning());
  assert(component.valid_responses()==1 && component.invalid_responses()==3);
  fake_now+=30000;component.loop();assert(component.activitySensor->state=="Lost");
  std::cout << "PASS: frame validation, activity, accounting, stale data, rollover, actual UART fragmented/partial/extra/retry\n";
}
