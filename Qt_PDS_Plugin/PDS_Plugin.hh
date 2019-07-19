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

CVS ID: PDS_Plugin.hh,v 1.2 2014/03/17 05:58:13 castalia Exp
*******************************************************************************/

#ifndef PDS_PLUGIN_HH
#define PDS_PLUGIN_HH

#include <QImageIOPlugin>

//	Forward reference.
class QImageIOHandler;


/**	The <i>PDS_Plugin</i> implements the QImageIOPlugin interface for
	gaining access to PDS image data.

	The PDS_Plugin is capable of creating a PDS_Handler when one of the
	recognized format keys is specified or a device is specified that the
	PDS_Handler indicates that it can read.

	@author		Bradford Castalia, idaeim
	@version	1.2
*/
class PDS_Plugin
:	public QImageIOPlugin
{
public:
/*==============================================================================
	Constants
*/
//!	Class identification name with source code version and date.
static const char* const
	ID;

/*==============================================================================
	Constructors
*/
/**	Construct a PDS_Plugin.

	@param	parent	A pointer to an optional QObject that will take ownership
		of the constructed object which will be destroyed by the parent when
		the parent is destroyed.
*/
explicit PDS_Plugin (QObject* parent = NULL);

//!	PDS_Plugin destructor.
virtual ~PDS_Plugin ();

/*==============================================================================
	QImageIOPlugin methods implementations
*/
/**	Get the list of format name keys recognized by this plugin.

	The keys list is the {@link idaeim::PDS_Handler::FORMATS PDS format
	names}. Each format name corresponds to a filename extension that
	will be recognized as indicating a file likely to contain PDS
	formatted content:PDS, IMG, and IMQ; both all uppercase and
	lowercase.

	@return	A QStringList containing the names
*/
virtual QStringList keys () const;

/**	Get the read/write capability for a specific format or data source.

	If the format matches one of the {@link keys()const recognized
	format keys} the CanRead capability is returned. Otherwise the data
	source is {@link idaeim::PDS_Handler::canRead(QIODevice* device)
	tested for readability by the PDS handler} and if true the CanRead
	capability is returned. If these tests fail an empty capability is
	returned.

	<b>N.B.</b>: This is suggestive test of supportability. The test is
	weakest for a matching format name because in this case the data
	source is not examined.

	@param	device 	A pointer to a QIODevice for a data source. May
		be NULL.
	@param	format	A QByteArray naming a format name key. May be
		empty.
	@return	A QImageIOPlugin Capabilities bit flags map. The value
		will be either QImageIOPlugin::CanRead if a test succeeds,
		or zero if both tests fail.
*/
virtual Capabilities capabilities (QIODevice* device,
	const QByteArray& format) const;

/**	Create a QImageIOHandler that should be capable of handling a
	specific format or data source.

	If the PDS file handler associated with this plugin {@link
	idaeim::PDS_Handler::canRead(QIODevice* device) can read}
	the specified device a {@link idaeim::PDS_Handler} is constructed
	and bound to the device. If the specified format is not empty
	the handler's foramat is set to given name; otherwise the
	first {@link keys()const format key} is applied.

	@param	device 	A pointer to a QIODevice for a data source. May
		be NULL.
	@param	format	A QByteArray naming a format name key. May be
		empty.
	@return	A pointer to a QImageIOHandler. This will be NULL if
		the PDS handler is not capable of reading the device.
*/
virtual QImageIOHandler* create (QIODevice* device,
	const QByteArray& format = QByteArray ()) const;

};	//	PDS_Plugin

#endif	//	PDS_PLUGIN_HH
