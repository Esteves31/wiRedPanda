TARGET = wiredpanda

TEMPLATE = app

include(../config.pri)
include(install.pri)

win32 {
    QMAKE_TARGET_COMPANY = GIBIS-UNIFESP
    QMAKE_TARGET_PRODUCT = wiRedPanda - Logic Circuit Simulator
    QMAKE_TARGET_DESCRIPTION = wiRedPanda - Logic Circuit Simulator
    QMAKE_TARGET_COPYRIGHT = GIBIS-UNIFESP and the wiRedPanda contributors

    RC_ICONS = resources/wpanda.ico
}

SOURCES += main.cpp

DISTFILES += resources/postinst

TRANSLATIONS += \
    resources/translations/wpanda_en.ts \
    resources/translations/wpanda_pt_BR.ts \
    resources/translations/wpanda_es.ts
