#include "appmain.h"

int main(int argc, char *argv[])
{
    return vinxAppMain(argc, argv, "vinx-calculator", "org.vinx.calculator", "qrc:/qt/qml/Vinx/Calculator/Main.qml", {}, {});
}
