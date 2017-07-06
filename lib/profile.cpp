#include "profile.h"
#include <QStringList>

using namespace QSS;

Profile::Profile() :
    local_address("127.0.0.1"), server_port(8388), local_port(1080), timeout(300), debug(false)
{}