#	Fetch_URL
#
#	CVS ID: Fetch_URL.pro,v 1.1 2014/01/15 05:25:18 castalia Exp

TEMPLATE		=	app
TARGET			=	Fetch_URL

CONFIG			+=	qt console thread
QT				+=	network

INCLUDEPATH		+=	. \
					../..

HEADERS			+=	Fetch_URL.hh
SOURCES			+=	Fetch_URL.cc

LIBS			+=	../../libQt_Utility.a
