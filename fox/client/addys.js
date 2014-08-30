print("importing addys");


//EMS 104:: 6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 83 EC 24 53 55 56 57 A1 ? ? ? ? 33 C4 50 8D 44 24 38 64 A3 ? ? ? ? 8B F1 A1
Addys.CUIStatusBar_SetNumberValue = 0x00DBC890;

//EMS 104:: 6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 83 EC 38 53 55 56 57 A1 ? ? ? ? 33 C4 50 8D 44 24 4C 64 A3 ? ? ? ? 8B D9 89 5C 24 20 33 F6
Addys.CUIStatusBar_SetStatusValue = 0x00DB0190;

//EMS 104:: 6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 81 EC ? ? ? ? 53 55 56 57 A1 ? ? ? ? 33 C4 50 8D 84 24 ? ? ? ? 64 A3 ? ? ? ? 8B D9 8B 84 24 ? ? ? ? 8B 94 24
Addys.CVecCtrl_SetActive = 0x01013F20;


//EMS 104:: 51 8B C4 89 64 24 1C 50 8D 4E 04 E8
Addys.CVecCtrl_SetActive_ret = 0x00F33D00;


Addys.SendFunction = 0x00470610;
Addys.SendPacketRet = 0x0047061A;
