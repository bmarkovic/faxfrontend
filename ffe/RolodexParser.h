/***************************************************************************
$Id: RolodexParser.h,v 1.2 2004/04/04 19:36:59 glenn Exp $

                          RolodexParser.h  -  Parse the rolodex file
                             -------------------
    begin                : Fri Apr 18 2003
    copyright            : (C) 2003 by Glenn Burkhardt
    email                : gbburkhardt@verizon.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <stdio.h>
#include <qobject.h>
#include <qxml.h>
#include <qstring.h>

class RolodexParser : public QObject, public QXmlDefaultHandler
{
	Q_OBJECT
public:
	RolodexParser();
	~RolodexParser(){}
  bool startElement( const QString&, const QString&, const QString& ,
  	                     const QXmlAttributes& );
  bool endElement( const QString&, const QString&, const QString& );
  bool characters( const QString& ch );
  QString errorString();
  bool fatalError( const QXmlParseException& exception );
  QString errorProtocol();
  static void writeEntry(FILE *, const QString&, const QString&,
			 									 const QString&, const QString&);
	static void writeStart(FILE*);
	static void writeEnd(FILE*);

signals:
	void entry(const QString&, const QString&, const QString&, const QString&);

private:
  	enum State {
			StateInit,
			StateEntry,
			StateFaxNumber,
			StateRecipient,
			StateCompany,
			StateLocation,
    };
    State state;
    QString faxnumber, recipient, company, location, errorProt;
};
