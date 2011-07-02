FIND_PATH(QWT_INCLUDE_DIR qwt.h
/usr/local/qwt/include
/usr/local/include
/usr/include/qwt
/usr/include
/usr/local/qwt-6.0.0/include
)

SET(QWT_NAMES ${QWT_NAMES} qwt libqwt)
FIND_LIBRARY(QWT_LIBRARY
    NAMES ${QWT_NAMES}
    PATHS /usr/local/qwt/lib /usr/local/lib /usr/lib /usr/local/qwt-6.0.0/lib
)

IF (QWT_LIBRARY)

    SET(QWT_LIBRARIES ${QWT_LIBRARY})
    SET(QWT_FOUND "YES")

ENDIF (QWT_LIBRARY)
