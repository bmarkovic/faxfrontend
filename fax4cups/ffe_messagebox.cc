/* $Id: messagebox.cc,v 1.3 2004/08/24 20:32:52 glenn Exp $
 */
/***************************************************************************
                          messagebox.cc - Display an error message string
                             -------------------
    begin                : Thu Jan 8, 2004
    copyright            : (C) 2004 by Glenn Burkhardt
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
#include <qapplication.h>
#include <qfileinfo.h>
#include <qmessagebox.h>
#include <qtextcodec.h>


/* Just display a message.
 */
int main(int ac, char **av)
{
    if (ac < 2) return -1;
    QApplication a(ac, av);
    QFileInfo fi(av[0]);
    
    QTranslator tor( 0 );
    // set the location where your .qm files are in load() below as the 
    // last parameter instead of "."
    // for development, use "/" to use the english original as
    // .qm files are stored in the base project directory.
    tor.load( QString("messagebox.")
	      + QTextCodec::locale(), fi.dirPath(true) );
    a.installTranslator( &tor );

    QMessageBox::critical(0, QObject::tr("Fax Frontend: Error"), av[1]);
    
    return 0;
}
