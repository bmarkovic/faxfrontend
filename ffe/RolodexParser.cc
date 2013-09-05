/***************************************************************************
$Id: RolodexParser.cc,v 1.2 2004/04/04 19:36:59 glenn Exp $

                          RolodexParser.cc  -  Parse the rolodex file
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
#include "RolodexParser.h"

/* This doesn't need to be very complicated - we only expect this program to
   creating the file.
 */
const char rolodexTag[]   = "rolodex";
const char entryTag[]     = "entry";
const char faxnumTag[]    = "faxnumber";
const char recipientTag[] = "recipient";
const char companyTag[]   = "company";
const char locationTag[]  = "location";

RolodexParser::RolodexParser()
{
	state = StateInit;
}

bool RolodexParser::startElement( const QString&, const QString&, const QString& qName,
  	                     const QXmlAttributes& )
{
	if (qName == rolodexTag) {
		state = StateInit;	
 	} else if (qName == entryTag) {
 		// new entry; just clear all fields
 		faxnumber = recipient = company = location = "";
 		state = StateEntry;
 	} else if (qName == faxnumTag) {
 		state = StateFaxNumber;
 	} else if (qName == recipientTag) {
		state = StateRecipient;
 	} else if (qName == companyTag) {
		state = StateCompany;
	} else if (qName == locationTag) {
		state = StateLocation;
 	} else return false;
 	
 	return true;
}

bool RolodexParser::endElement( const QString&, const QString&, const QString& )
{
	if (state == StateEntry) {
		emit entry(recipient, company, location, faxnumber);
		state = StateInit;
	} else
		state = StateEntry;
	
	return true;  	
}

bool RolodexParser::characters( const QString& ch )
{
	switch (state) {
		case StateFaxNumber:
			faxnumber += ch;
			break;
		case StateRecipient:
			recipient += ch;
			break;
		case StateCompany:
			company += ch;
			break;
		case StateLocation:
			location += ch;
			break;
		default:;
	}
	return true;  	
}

QString RolodexParser::errorProtocol()
{
	return errorProt;
}


QString RolodexParser::errorString()
{
	return tr("Rolodex file invalid.");  	
}

bool RolodexParser::fatalError( const QXmlParseException& exception )
{
	errorProt += tr("fatal parsing error: %1 in line %2, column %3\n" )
		.arg( exception.message() )
		.arg( exception.lineNumber() )
		.arg( exception.columnNumber() );
		
		return QXmlDefaultHandler::fatalError( exception );
}

/* Write functions for creating rolodex file
 */
void RolodexParser::writeEntry(FILE *fp, const QString& r, const QString& co,
			       const QString& loc, const QString& fn)
{
  fprintf(fp, "<%s>\n", entryTag);

  if (!r.isNull())
      fprintf(fp, "  <%s>%s</%s>\n", recipientTag, r.latin1(), recipientTag);
  if (!co.isNull())
      fprintf(fp, "  <%s>%s</%s>\n", companyTag, co.latin1(), companyTag);
  if (!loc.isNull())
      fprintf(fp, "  <%s>%s</%s>\n", locationTag, loc.latin1(),
	      locationTag);
   if (!fn.isNull())
      fprintf(fp, "  <%s>%s</%s>\n", faxnumTag, fn.latin1(), faxnumTag);

  fprintf(fp, "</%s>\n", entryTag);
}
void RolodexParser::writeStart(FILE *fp)
{
	fprintf(fp, "<%s>\n", rolodexTag);
}

void RolodexParser::writeEnd(FILE *fp)
{
	fprintf(fp, "</%s>\n", rolodexTag);
}
