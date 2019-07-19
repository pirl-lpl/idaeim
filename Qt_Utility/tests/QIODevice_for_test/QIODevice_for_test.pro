#	QIODevice_for_test
#
#	CVS ID: QIODevice_for_test.pro,v 1.1 2014/01/19 05:12:28 castalia Exp

TEMPLATE		=	app
TARGET			=	QIODevice_for_test

CONFIG			+=	qt console thread
QT				+=	network

SOURCES			+=	QIODevice_for_test.cc

INCLUDEPATH		+=	../..

LIBS			+=	../../libQt_Utility.a
