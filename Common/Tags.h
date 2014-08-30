#ifndef PACKETHEADERS_H
#define PACKETHEADERS_H

#include <QString>
namespace Packets {

const QString server_name_tag = QStringLiteral("FoxServer");
const QString header_tag      = QStringLiteral("header");
const QString extension_tag   = QStringLiteral("extension");
const QString content_tag     = QStringLiteral("content");
const QString client_tag      = QStringLiteral("client");
const QString server_tag      = QStringLiteral("server");
const QString handler_tag     = QStringLiteral("HandlePackets");

enum class Headers {
    PATH = 0, INJECT, EMBED, BOT, EXTENSION, DBG
};
}//end Packets

namespace API {

const QString Path = QStringLiteral("Path");
const QString GUI  = QStringLiteral("GUI");
const QString Socket = QStringLiteral("Socket");
const QString PostBypass_handler = QStringLiteral("postBypass");

const QString KeyBoard_Obj   = QStringLiteral("KeyBoard");
const QString Injector_Obj   = QStringLiteral("Injector");
const QString AoBScanner_Obj = QStringLiteral("AoBScanner");
const QString Player_Obj     = QStringLiteral("Player");
const QString Bot_Obj        = QStringLiteral("Bot");
const QString Addys_Obj      = QStringLiteral("Addys");
const QString Pointers_Obj   = QStringLiteral("Pointers");
const QString Memory_Obj     = QStringLiteral("Memory");

const QString Folder_func  = QStringLiteral("getFolder");
const QString Include_func = QStringLiteral("include");
const QString Print_func   = QStringLiteral("debug");

const QString Save_func = QStringLiteral("SaveSettings");
const QString Load_func = QStringLiteral("LoadSettings");

const QString Sync_func = QStringLiteral("Sync");
}
#endif // PACKETHEADERS_H
