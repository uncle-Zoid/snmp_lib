WARNINGS += -Wall

CONFIG += debug_and_release

CONFIG(release, debug|release){
    DESTDIR=release
}

CONFIG(debug, debug|release){
    DESTDIR=debug
}

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.ui
