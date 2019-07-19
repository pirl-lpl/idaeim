#	URL_redirection_test
#
#	CVS ID: URL_redirection_test.pro,v 1.1 2014/01/18 06:53:34 castalia Exp

TEMPLATE		=	app
TARGET			=	URL_redirection_test

CONFIG			+=	qt console thread
QT				+=	network

SOURCES			+=	URL_redirection_test.cc

INCLUDEPATH		+=	../..

LIBS			+=	../../libQt_Utility.a
