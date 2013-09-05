/***************************************************************************
$Id: main.cc,v 1.5 2004/08/24 20:28:23 glenn Exp $

                          main.cc - Fax Front End for Hylafax
                             -------------------
    begin                : Thu Jan 8, 2004
    copyright            : (C) 2004 by Glenn Burkhardt
    email                : gbburkhardt@verizon.net

    Usage:  faxfrontend [-debug] [file to fax]

            If invoked with no arguments, user is allowed to enter
            fax file or send cover page only.

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
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <pwd.h>
#include <qapplication.h>
#include <qfileinfo.h>
#include <qfont.h>
#include <qstring.h>
#include <qtextcodec.h>
#include <qtranslator.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qmessagebox.h>
#include <qbuttongroup.h>
#include <qmultilineedit.h>
#include <qspinbox.h>
#include <qlineedit.h>
#include "FaxFrontEnd.h"
#include "faxstat.h"

/* Scan the string for single quotes, escape them, and return a
   double quoted string suitable for passing the string as a shell argument.
*/
   
static QString quoted_string(QString str)
{
    for (int i=0; i<str.length(); i++) {
	if ((str[i] == '\\') || (str[i] == '`')
	    || (str[i] == '$') || (str[i] == '"')) {

	    str.insert(i, '\\');
	    i += 2;
	}
    }

    str.prepend('"');
    str.append('"');

    return str;
}

/* Check that the file exists, is a regular file, and we can read it.
 */
static bool file_OK(const QString& fn)
{
    QFileInfo info(fn);

    if (!info.exists()) {
	QMessageBox::critical(0, QObject::tr("File Access Error"),
			QObject::tr("Fax File %1 doesn't exist.").arg(fn));
	return false;
    }

    if (info.isDir()) {
	QMessageBox::critical(0, QObject::tr("File Access Error"),
			QObject::tr("%1 is a directory").arg(fn));
	return false;
    }

    if (!info.isReadable()) {
	QMessageBox::critical(0, QObject::tr("File Access Error"),
			QObject::tr("%1 is not readable").arg(fn));
	return false;
    }

    return true;
}

int main(int ac, char *av[])
{
    char tmpname[] = "/tmp/FaxCoverXXXXXX";
    char *user, *faxfile;
    bool delete_tmpname=false, debug=false;
    int i, status;
    struct passwd *pw;

    // allocation is from heap, so child faxstat process can tag along
    // on the X connection.
    QApplication* a = new QApplication(ac, av);

    // set the location where your .qm files are in load() below as the 
    // last parameter instead of "."
    // for development, use "/" to use the english original as
    // .qm files are stored in the base project directory.
    QTranslator tor( 0 );
    QFileInfo fi(av[0]);
    tor.load( QString("faxfrontend.")
	      + QTextCodec::locale(), fi.dirPath(true) );
    a->installTranslator( &tor );

    /* Check for arguments: -d for debug, file to fax */
    faxfile = 0;
    for (int i=1; i<ac; i++) {
	if (av[i][0] == '-' && av[i][1] == 'd') {
	    debug = true;
	    openlog("faxfrontend", 0, LOG_DAEMON);
	} else {
	    faxfile = av[i];
	    if (!file_OK(faxfile)) faxfile = 0;
	}
    }

    FaxFrontEnd faxfrontend(faxfile);

    for (;;) {
	status = faxfrontend.exec();
	if (status == QDialog::Rejected) return 0;
	if (faxfile) break;	// already checked file status

	if (faxfrontend.fileLineEdit->text().isEmpty()) break;
	if (file_OK(faxfrontend.fileLineEdit->text())) break;
    }
    
    // send the fax

    QString cmd("sendfax");
  	
    /* Determine send mode:
       - have fax file as argument, or given in file line edit,
       use cover page option
       - No file given, offer to build cover page only and send.
       
       Must be careful - arguments to sendfax are order dependent.
    */
    
    if (faxfile || !faxfrontend.fileLineEdit->text().isEmpty()) {
	// Have a file to fax

	if (faxfrontend.noCoverPage->isChecked()) {
	    cmd += " -n";
	} else {
	    if (!faxfrontend.companyBox->text().isEmpty())
		cmd += " -x "
		    + quoted_string(faxfrontend.companyBox->text()
				    .stripWhiteSpace());
	    if (!faxfrontend.locationBox->text().isEmpty())
		cmd += " -y "
		    + quoted_string(faxfrontend.locationBox->text()
				    .stripWhiteSpace());
	    if (!faxfrontend.regardingBox->text().isEmpty())
		cmd += " -r "
		    + quoted_string(faxfrontend.regardingBox->text()
				    .stripWhiteSpace());
	    if (!faxfrontend.comments->text().isEmpty())
		cmd += " -c "
		    + quoted_string(faxfrontend.comments->text()
				    .stripWhiteSpace());
	}
    } else
	cmd += " -n";	// no file to fax; assume cover page generation
			// option (below), specify sendfax no cover page
    
    if (faxfrontend.notifyComplete->isChecked())
	cmd += " -D";
    else
	cmd += " -N";
    if (faxfrontend.notifyRetry->isChecked()) cmd += " -R";
    if (faxfrontend.resButtonGroup->selected()
	== (QButton*)faxfrontend.lowResButton) cmd += " -l";
    
    // transmit delay value
    if (faxfrontend.xmitSpinBox->value() > 0) {
	cmd += QString(" -a 'now + %1 minutes'")
	    .arg(faxfrontend.xmitSpinBox->value());
    }
    if (faxfrontend.retrySpinBox->value() > 1) {
	QString retry;
	retry.sprintf(" -t %d", faxfrontend.retrySpinBox->value());
	cmd += retry;
    }
    if (faxfrontend.pageSizeComboBox->currentItem())
	cmd += " -s a4";
    else
	cmd += " -s na-let";
    
    /* Destination is 'user@fax-number'; strip '@' from user name
       just to be sure.
    */
    QString qs(faxfrontend.recipientBox->text().stripWhiteSpace());
    i = 0;
    while ((i = qs.find('@', i)) >= 0) qs[i++] = '_';

    qs += "@" + faxfrontend.faxNumberBox->text().stripWhiteSpace();
    
    cmd += " -d " + quoted_string(qs);
    
    cmd += " ";	// to precede filename
    
    /* If no arguments were given, assume test mode */
    if (faxfile)
	cmd += faxfile;	// fax file name
    else if (!faxfrontend.fileLineEdit->text().isEmpty())
	cmd += faxfrontend.fileLineEdit->text();
    else {
	/* Send cover page only */
	if (QMessageBox::warning(0, QObject::tr("Warning"),
				 QObject::tr("No file to fax specified.\n"
					     "Send Cover Page only?"),
				 QMessageBox::Yes|QMessageBox::Default,
				 QMessageBox::No) == QMessageBox::No) {
	    return 0;
	}
	
	mktemp(tmpname);
	QString fcmd("faxcover");
	
	if (faxfrontend.pageSizeComboBox->currentItem())
	    fcmd += " -s a4";
	else
	    fcmd += " -s na-let";
	
	fcmd += " -t " + quoted_string(faxfrontend.recipientBox->text()
				       .stripWhiteSpace());
	
	if (!faxfrontend.companyBox->text().isEmpty())
	    fcmd += " -x " + quoted_string(faxfrontend.companyBox->text()
					   .stripWhiteSpace());
	
	if (!faxfrontend.locationBox->text().isEmpty())
	    fcmd += " -l " + quoted_string(faxfrontend.locationBox->text()
					   .stripWhiteSpace());
	
	if (!faxfrontend.regardingBox->text().isEmpty())
	    fcmd += " -r " + quoted_string(faxfrontend.regardingBox->text()
					   .stripWhiteSpace());
	
	if (!faxfrontend.comments->text().isEmpty())
	    fcmd += " -c " + quoted_string(faxfrontend.comments->text()
					   .stripWhiteSpace());
	fcmd += " -p 0";
	
	/* Get 'real user' name for "from" address */
	user = getenv("USER");
	if (!user) user = "root";
	pw = getpwnam(user);
	if (!pw) 
	    fcmd += " -f HylaFAX";
	else {		
	    fcmd += " -f '";
	    fcmd += pw->pw_gecos;
	    fcmd += "'";
	}
	
	fcmd += " -n '"
	    + faxfrontend.faxNumberBox->text().stripWhiteSpace() + "'";
	
	fcmd += " > ";
	fcmd += tmpname;
	
	if (debug) {
	    syslog(LOG_INFO, fcmd.latin1());
	}
	
	status = system(fcmd.latin1());	// generate cover page
	status = WEXITSTATUS(status);
	if (status) {
	    QMessageBox::critical(0, QObject::tr("Error"),
		QObject::tr("Can't create faxcover\ncommand: %1").arg(fcmd));
	    unlink(tmpname);
	    return -1;
	}
	
	delete_tmpname = true;
	
	cmd += tmpname;	// file to fax is cover page we generated
    }
    
    if (debug) syslog(LOG_INFO, cmd.latin1());
	
    status = system(cmd.latin1());	// send the fax
    status = WEXITSTATUS(status);
    if (status) {
	QMessageBox::critical(0, QObject::tr("Error"),
			      QObject::tr("Can't send fax\ncommand: %1")
			      .arg(cmd));
    }
    
    if (delete_tmpname) unlink(tmpname);

    // Now poll status from Hylafax. Fork, and let parent return
    // completion status to CUPS so the print queue isn't held up
    // until the user closes the status window.
    if (fork()) return status;

    // detach from parent

    ::close(0);
    ::close(1);
    ::close(2);
    int fd = open("/dev/null", O_RDWR);
    dup2(fd, fileno(stdout));
    dup2(fd, fileno(stderr));
    fd = open("/dev/tty", O_RDWR);
    if (fd > 0) {
	ioctl(fd, TIOCNOTTY, 0);
	::close(fd);
    }

    FaxStat fs;
    fs.exec();
    return 0;

}
