/**	PDS_Plugin

Copyright (C) 2013-2014 Bradford Castalia, idaeim studio.

This file is part of the idaeim libraries.

The idaeim libraries are free software; you can redistribute them and/or
modify them under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

The idaeim libraries are distributed in the hope that they will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.

CVS ID: PDS_Plugin.cc,v 1.8 2014/03/17 05:58:13 castalia Exp
*******************************************************************************/

#include "PDS_Plugin.hh"

#include "PDS_Handler.hh"
using idaeim::PDS_Handler;




/*==============================================================================
	Constants
*/
const char* const
	PDS_Plugin::ID =
		"idaeim::PDS_Plugin (1.8 2014/03/17 05:58:13)";

/*==============================================================================
	Constructors
*/
PDS_Plugin::PDS_Plugin
	(
	QObject*	parent
	)
	:	QImageIOPlugin (parent)
{
}


PDS_Plugin::~PDS_Plugin ()
{
}

/*==============================================================================
	QImageIOPlugin virtual methods implementations
*/
QStringList
PDS_Plugin::keys () const
{
return PDS_Handler::FORMATS;
}


QImageIOPlugin::Capabilities
PDS_Plugin::capabilities
	(
	QIODevice*			device,
	const QByteArray&	format
	)
	const
{
Capabilities
	capability;
if (PDS_Handler::FORMATS.contains (format.toUpper ()) ||
	PDS_Handler::canRead (device))
	capability |= CanRead;
return capability;
}


QImageIOHandler*
PDS_Plugin::create
	(
	QIODevice*			device,
	const QByteArray&	format
	)
	const
{
PDS_Handler
	*handler = NULL;
if (PDS_Handler::canRead (device))
	{
	handler = new PDS_Handler;
	handler->setDevice (device);
	QByteArray
		format_name (format);
	if (format.isEmpty ())
		format_name.append (PDS_Handler::FORMATS[0]);
	handler->setFormat (format_name);
	}
return handler;
}

Q_PLUGIN_METADATA(IID "idaeim::PDS_Plugin")
//Q_EXPORT_PLUGIN2 (PDS, PDS_Plugin);
