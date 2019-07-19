/*	Qt_Version

	CVS ID: Qt_Version.cc,v 1.3 2013/12/06 03:27:48 castalia Exp
*/
#include	<QtGlobal>
#include	<QLibraryInfo>

#include	<string>
#include	<iostream>
#include	<iomanip>
using namespace std;


std::ostream& operator<< (std::ostream& stream, const QString& qstring)
	{return stream << qPrintable (qstring);}



int
main ()
{
cout << "Qt version -" << hex << endl
	 << "    compiled: " << QT_VERSION_STR << " (0x" << QT_VERSION << ')' << endl
	 << "     runtime: " << qVersion () << endl
	 << " build key -" << endl
	 << "    compiled: " << QT_BUILD_KEY << endl
	 << "     runtime: " << QLibraryInfo::buildKey () << endl
	 << " build date - " << QLibraryInfo::buildDate ().toString () << endl
	 << " Qt locations -" << endl
	 << "    documentation: "
	 	<< QLibraryInfo::location (QLibraryInfo::DocumentationPath) << endl
	 << "     header files: "
	 	<< QLibraryInfo::location (QLibraryInfo::HeadersPath) << endl
	 << "        libraries: "
	 	<< QLibraryInfo::location (QLibraryInfo::LibrariesPath) << endl
	 << "          plugins: "
	 	<< QLibraryInfo::location (QLibraryInfo::PluginsPath) << endl;
#if QT_VERSION >= 0x040700
cout << "   QML extensions: "
	 	<< QLibraryInfo::location (QLibraryInfo::ImportsPath) << endl;
#endif
cout << "            tools: "
	 	<< QLibraryInfo::location (QLibraryInfo::BinariesPath) << endl
	 << "     general data: "
	 	<< QLibraryInfo::location (QLibraryInfo::DataPath) << endl
	 << "     translations: "
	 	<< QLibraryInfo::location (QLibraryInfo::TranslationsPath) << endl
	 << "         settings: "
	 	<< QLibraryInfo::location (QLibraryInfo::SettingsPath) << endl;
}
