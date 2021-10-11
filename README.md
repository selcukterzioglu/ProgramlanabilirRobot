# ProgramlanabilirRobot

 Program butonuna basılınca PROGRAMLAMA moduna girer ve 
 yön tuşlarına her basmayı kaydeder. Tekrar program butonu
 ile PROGRAMLAMA modundan çıkar. Start butonuna basıldığına
 hafızasındaki hareketleri uygular.
 Hareketleri EEPROM' kaydettiği için silinmez kapatıp açsanızda
 hafızada kalır. Yeniden PROGRAM moduna girdiğinizde eski programı 
 siler.
 
# Devre Bağlantıları
Butonların bir ucu Arduino diğer ucu GND'ye gelecek şekilde bağlanmalıdır. 
 
 START BUTONU => ARDUINO 2 NOLU PIN
 
 PROGRAM BUTONU => ARDUINO 3 NOLU PIN
 
 İLERİ BUTONU => ARDUINO 7 NOLU PIN
 
 GERİ BUTONU => ARDUINO 4 NOLU PIN
 
 SOL BUTONU  => ARDUINO 6 NOLU PIN
 
 SAĞ BUTONU  => ARDUINO 7 NOLU PIN

 MOTOR BAĞLANTILARI (MOTOR SÜRÜCÜSÜ L298 VEYA L293)

 IN1 => ARDUINO 8 NOLU PIN
 
 IN2 => ARDUINO 9 NOLU PIN
 
 ENA => ARDUINO 10 NOLU PIN
 
 IN3 => ARDUINO 12 NOLU PIN
 
 IN4 => ARDUINO 13 NOLU PIN
 
 ENB => ARDUINO 11 NOLU PIN
