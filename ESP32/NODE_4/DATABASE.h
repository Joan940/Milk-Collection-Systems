#ifndef DATABASE_H
#define DATABASE_H

#include <Arduino.h>

struct User {
  const char* uid;
  const char* name;
};

class databaseModule {
  private:
    static const int MAX_USERS = 33;
    
    User userList[MAX_USERS] = {
      {"60 A1 B1 55", "Test 4"},
      {"04 89 47 F2 12 5E 80", "Setiyo Budi"},
      {"04 44 41 2A 89 48 80", "Stephanus Maryono"},
      {"04 20 30 DA 7C 23 80", "Teguh Budiyono"},
      {"04 79 71 7A 94 2A 80", "Slamet Riyadi"},
      {"04 28 5C 72 19 2B 80", "Jajak"},
      {"04 5E 5B 0A C4 28 80", "Sujono"},
      {"05 86 79 3D 7D C1 00", "Nur Hidayat"},
      {"04 48 43 F2 73 2B 80", "Suroso"},
      {"04 87 29 12 C8 2C 80", "Kristanto"},
      {"04 31 84 6A EE 2C 80", "Trisiono"},
      {"05 8A 70 41 6D C1 00", "Gunawan"},
      {"04 3B 33 32 59 2A 80", "Joko Pramono"},
      {"04 46 2B 22 7F 29 80", "Cipto Suradi"},
      {"04 62 1A 5A 4B 5A 80", "Turut"},
      {"04 20 5C C2 E3 5C 80", "Sabar"},
      {"04 65 0B 92 8B 29 80", "Hartini"},
      {"05 83 06 25 D6 53 00", "Tira Widyaningsih"},
      {"05 8B 30 1B 02 32 00", "Parmiati"},
      {"04 75 32 3A FF 24 80", "Arman"},
      {"04 95 3F 12 7A 28 80", "Supriyo"},
      {"04 55 72 E2 30 25 80", "Marno"},
      {"18 82 35 E5 5F 16 0D", "Wiyanto"},
      {"04 0E 2D 9A FF 2A 80", "Sukardi Hosea"},
      {"04 58 4E 02 11 71 80", "Slamet Maryono"},
      {"05 88 CF 5A D9 91 00", "Triyanto"},
      {"04 59 40 F2 7F 61 80", "Budiyono"},
      {"04 38 67 02 1E 2A 80", "Yamsih"},
      {"05 83 2C B7 B1 72 00", "Sutris Subur"},
      {"05 8E 34 C8 A7 03 00", "Lana"},
      {"04 7A 5C 12 9E 53 80", "Eko"},
      {"04 67 40 5A 3A 29 80", "Warko"},
      {"04 8D 4E 9A B8 27 80", "Dokik"}
    };

  public:
    databaseModule() {}

    String getName(String targetUID) {
      targetUID.trim(); 
      
      // 2. Looping perbandingan
      for (int i = 0; i < MAX_USERS; i++) {
        if (targetUID.equalsIgnoreCase(userList[i].uid)) {
          return String(userList[i].name); 
        }
      }
      return "Tidak ada";
    }
};

#endif