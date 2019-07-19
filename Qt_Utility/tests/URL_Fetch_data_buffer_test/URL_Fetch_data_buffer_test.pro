#	URL_Fetch_data_buffer_test
#
#	CVS ID: URL_Fetch_data_buffer_test.pro,v 1.1 2014/01/19 12:24:57 castalia Exp

TEMPLATE		=	app
TARGET			=	URL_Fetch_data_buffer_test

CONFIG			+=	qt console thread
QT				+=	network

SOURCES			+=	URL_Fetch_data_buffer_test.cc

INCLUDEPATH		+=	../..

LIBS			+=	../../libQt_Utility.a
