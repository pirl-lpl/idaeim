#	Check_URL
#
#	CVS ID: Check_URL.pro,v 1.5 2014/01/09 00:13:37 castalia Exp

TEMPLATE		=	app
TARGET			=	Check_URL

CONFIG			+=	qt console thread
QT				+=	network

SOURCES			+=	Check_URL.cc

INCLUDEPATH		+=	../..

LIBS			+=	../../libQt_Utility.a
