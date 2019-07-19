#	PDS_Metadata_test
#
#	CVS ID: PDS_Metadata_test.pro,v 1.3 2013/12/20 06:14:18 castalia Exp

TEMPLATE		=	app
TARGET			=	PDS_Metadata_test

CONFIG			+=	qt console
QT				+=	network


SOURCES			+=	PDS_Metadata_test.cc

INCLUDEPATH		+=	. \
					../.. \
					/opt/idaeim/include

LIBS			+=	../../libQt_Utility.a \
					/opt/idaeim/lib/libPVL.a \
					/opt/idaeim/lib/libString.a \
					/opt/idaeim/lib/libidaeim.a 
