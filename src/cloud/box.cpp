#include "box.h"
#include "application_connection.h"

using namespace aquabox;

Box::Box(ResultsMap<Command>& results)
    : BoxConnection(results)
{}
